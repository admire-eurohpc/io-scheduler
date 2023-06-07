#include <ranges>
#include <regex>
#include <cassert>
#include "command.hpp"

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

} // namespace scord_ctl
