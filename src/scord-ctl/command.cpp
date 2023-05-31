#include <ranges>
#include <regex>
#include <cassert>
#include <unistd.h>
#include <sys/wait.h>
#include <logger/logger.hpp>
#include "command.hpp"


namespace {
/**
 * @brief Convert a vector of strings into a vector of C strings.
 * The returned vector is null-terminated.
 *
 * @note The const char* stored into the resulting vector are valid only as
 * long as the input vector is not modified or destroyed. The caller is
 * responsible for ensuring this.
 *
 * @param v Vector of strings.
 *
 * @return Vector of C strings.
 */
[[nodiscard]] std::unique_ptr<const char*[]>
as_char_array(const std::vector<std::string>& v) {

    auto tmp = std::make_unique<const char*[]>(v.size() + 1);
    tmp[v.size()] = nullptr;

    for(const auto i : std::views::iota(0u, v.size())) {
        tmp[i] = v[i].c_str();
    }

    return tmp;
}

} // namespace

namespace scord_ctl {

void
environment::set(const std::string& key, const std::string& value) {
    m_env[key] = value;
}

std::string
environment::get(const std::string& key) const {
    return m_env.count(key) == 0 ? std::string{} : m_env.at(key);
}

std::vector<std::string>
environment::as_vector() const {

    std::vector<std::string> tmp;
    tmp.reserve(m_env.size());
    for(const auto& [key, value] : m_env) {
        tmp.emplace_back(fmt::format("{}={}", key, value));
    }

    return tmp;
}

std::size_t
environment::size() const {
    return m_env.size();
}

std::unordered_map<std::string, std::string>::const_iterator
environment::begin() const {
    return m_env.begin();
}

std::unordered_map<std::string, std::string>::const_iterator
environment::end() const {
    return m_env.end();
}

command::command(std::string cmdline, std::optional<environment> env)
    : m_cmdline(std::move(cmdline)), m_env(std::move(env)) {}

const std::string&
command::cmdline() const {
    return m_cmdline;
}

const std::optional<environment>&
command::env() const {
    return m_env;
}

command
command::eval(const std::string& adhoc_id,
              const std::filesystem::path& adhoc_directory,
              const std::vector<std::string>& adhoc_nodes) const {

    // generate a regex from a map of key/value pairs
    constexpr auto regex_from_map =
            [](const std::map<std::string, std::string>& m) -> std::regex {
        std::string result;
        for(const auto& [key, value] : m) {
            const auto escaped_key =
                    std::regex_replace(key, std::regex{R"([{}])"}, R"(\$&)");
            result += fmt::format("{}|", escaped_key);
        }
        result.pop_back();
        return std::regex{result};
    };

    const std::map<std::string, std::string> replacements{
            {std::string{keywords.at(0)}, adhoc_id},
            {std::string{keywords.at(1)}, adhoc_directory.string()},
            {std::string{keywords.at(2)},
             fmt::format("\"{}\"", fmt::join(adhoc_nodes, ","))}};

    // make sure that we fail if we ever add a new keyword and forget to add
    // a replacement for it
    assert(replacements.size() == keywords.size());

    std::string result;

    const auto re = regex_from_map(replacements);
    auto it = std::sregex_iterator(m_cmdline.begin(), m_cmdline.end(), re);
    auto end = std::sregex_iterator{};

    std::string::size_type last_pos = 0;

    for(; it != end; ++it) {
        const auto& match = *it;
        result += m_cmdline.substr(last_pos, match.position() - last_pos);
        result += replacements.at(match.str());
        last_pos = match.position() + match.length();
    }

    result += m_cmdline.substr(last_pos, m_cmdline.length() - last_pos);

    return command{result, m_env};
}

std::vector<std::string>
command::as_vector() const {
    std::vector<std::string> tmp;

    for(auto&& r : std::views::split(m_cmdline, ' ') |
                           std::views::transform([](auto&& v) -> std::string {
                               auto c = v | std::views::common;
                               return std::string{c.begin(), c.end()};
                           })) {
        tmp.emplace_back(std::move(r));
    }

    return tmp;
}

void
command::exec() const {

    const auto envs = m_env.value_or(environment{}).as_vector();
    const auto args = this->as_vector();

    const auto argv = ::as_char_array(args);
    const auto envp = ::as_char_array(envs);

    switch(const auto pid = ::fork()) {
        case 0: {
            ::execvpe(argv[0], const_cast<char* const*>(argv.get()),
                      const_cast<char* const*>(envp.get()));
            // We cannot use the default logger in the child process because it
            // is not fork-safe, and even though we received a copy of the
            // global logger, it is not valid because the child process does
            // not have the same threads as the parent process.
            // Instead, we create a new logger with the same configuration as
            // the global logger, and use it to log the error. This new logger
            // must be synchronous to avoid issues with accessing the
            // (existing but invalid) internal thread pool of the global logger.
            const auto msg = fmt::format("Failed to execute command: {}",
                                         ::strerror(errno));
            if(const auto logger = logger::get_default_logger(); logger) {
                auto cfg = logger->config();
                logger::sync_logger lg{cfg};
                lg.error("{}", msg);
            } else {
                fmt::print(stderr, "{}", msg);
            }

            // We cannot call ::exit() here because it will attempt to destruct
            // the global logger, and as we mentioned the global logger in
            // the child process contains the same information as the global
            // logger of the parent process, but without the proper threads
            // (since fork() only copies the calling thread). Thus, attempting
            // to call its destructor will end up hanging the process while it
            // tries to join() a non-existing thread. Instead, we call ::_exit()
            // which will terminate the process without calling any functions
            // registered with atexit() or similar. This is safe because we know
            // the process is about to be destroyed anyway...
            ::_exit(EXIT_FAILURE);
        }
        case -1:
            throw std::runtime_error{fmt::format(
                    "Failed to create subprocess: {}", ::strerror(errno))};
        default: {
            int wstatus = 0;
            do {
                if(const auto retwait = ::waitpid(pid, &wstatus, 0);
                   retwait == -1) {
                    throw std::runtime_error{
                            fmt::format("Failed to wait for subprocess: {}",
                                        ::strerror(errno))};
                }

                if(!WIFEXITED(wstatus)) {
                    throw std::runtime_error{
                            "Subprocess did not exit normally"};
                }

                if(WEXITSTATUS(wstatus) != 0) {
                    throw std::runtime_error{
                            fmt::format("Subprocess exited with status {}",
                                        WEXITSTATUS(wstatus))};
                }
            } while(!WIFEXITED(wstatus));
        }
    }
}

} // namespace scord_ctl
