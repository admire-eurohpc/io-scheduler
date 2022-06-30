/******************************************************************************
 * Copyright 2021, Barcelona Supercomputing Center (BSC), Spain
 *
 * This software was partially supported by the EuroHPC-funded project ADMIRE
 *   (Project ID: 956748, https://www.admire-eurohpc.eu).
 *
 * This file is part of scord.
 *
 * scord is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * scord is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with scord.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <filesystem>
#include <string>
#include <system_error>
#include <fmt/format.h>

#include <server.hpp>
#include <config/settings.hpp>
#include <logger/logger.hpp>
#include <network/engine.hpp>
#include <utils/signal_listener.hpp>

namespace scord {

server::server() : m_settings(std::make_unique<config::settings>()) {}

server::~server() = default;

pid_t
server::daemonize() {
    /*
     * --- Daemonize structure ---
     *  Check if this is already a daemon
     *  Fork off praent process
     *  Obtain new process group
     *  Close all descriptors
     *  Handle standard IO
     *  Change file mode mask
     *  Change the current working directory
     *  Check if daemon already exists
     *  Manage signals
     */

    pid_t pid, sid;

    /* Check if this is already a daemon */
    if(::getppid() == 1) {
        return 0;
    }

    // We need to destroy the global logger before calling fork. Otherwise the
    // logger will not function properly since its internal thread will not
    // be duplicated by fork(). Furthermore, if we don't destroy pre-fork()
    // and attempt to replace it post-fork(), the logger destructor will attempt
    // to join the (now invalid) thread and end up blocking forever. To avoid
    // this (and since we want to be able to output messages from all
    // processes), we destroy it now and recreate it post-fork() both in the
    // parent process and in the child.
    logger::destroy_global_logger();

    /* Fork off the parent process */
    pid = fork();

    // re-initialize logging facilities (post-fork)
    init_logger();

    if(pid < 0) {
        LOGGER_ERRNO("Failed to create child process");
        exit(EXIT_FAILURE);
    }

    /* Parent returns to caller */
    if(pid != 0) {
        return pid;
    }

    /* Become a session and process group leader with no controlling tty */
    if((sid = setsid()) < 0) {
        /* Log failure */
        LOGGER_ERRNO("Failed to disassociate controlling tty");
        exit(EXIT_FAILURE);
    }

    /* Handle standard IO: discard data to/from stdin, stdout and stderr */
    int dev_null;

    if((dev_null = ::open("/dev/null", O_RDWR)) == -1) {
        LOGGER_ERRNO("Failed to open \"/dev/null\"");
        exit(EXIT_FAILURE);
    }

    if(::dup2(dev_null, STDIN_FILENO) == -1) {
        LOGGER_ERRNO("Failed to dup \"/dev/null\" onto stdin");
        exit(EXIT_FAILURE);
    }

    if(::dup2(dev_null, STDOUT_FILENO) == -1) {
        LOGGER_ERRNO("Failed to dup \"/dev/null\" onto stdout");
        exit(EXIT_FAILURE);
    }

    if(::dup2(dev_null, STDERR_FILENO) == -1) {
        LOGGER_ERRNO("Failed to dup \"/dev/null\" onto stderr");
        exit(EXIT_FAILURE);
    }

    /* Change the file mode creation mask */
    ::umask(0);

    /* ensure the process does not keep a directory in use,
     * avoid relative paths beyond this point! */
    if(::chdir("/") < 0) {
        LOGGER_ERRNO("Failed to change working directory to root directory");
        exit(EXIT_FAILURE);
    }

    /* Check if daemon already exists:
     * First instance of the daemon will lock the file so that other
     * instances understand that an instance is already running.
     */
    int pfd;

    if((pfd = ::open(m_settings->pidfile().c_str(), O_RDWR | O_CREAT, 0640)) ==
       -1) {
        LOGGER_ERRNO("Failed to create daemon lock file");
        exit(EXIT_FAILURE);
    }

    if(::lockf(pfd, F_TLOCK, 0) < 0) {
        LOGGER_ERRNO("Failed to acquire lock on pidfile");
        LOGGER_ERROR("Another instance of this daemon may already be running");
        exit(EXIT_FAILURE);
    }

    /* record pid in lockfile */
    std::string pidstr(std::to_string(getpid()));

    if(::write(pfd, pidstr.c_str(), pidstr.length()) !=
       static_cast<ssize_t>(pidstr.length())) {
        LOGGER_ERRNO("Failed to write pidfile");
        exit(EXIT_FAILURE);
    }

    ::close(pfd);
    ::close(dev_null);

    /* Manage signals */
    ::signal(SIGCHLD, SIG_IGN); /* ignore child */
    ::signal(SIGTSTP, SIG_IGN); /* ignore tty signals */
    ::signal(SIGTTOU, SIG_IGN);
    ::signal(SIGTTIN, SIG_IGN);
    //  signal(SIGHUP, signal_handler); /* catch hangup signal */
    //  signal(SIGTERM, signal_handler); /* catch kill signal */

    return 0;
}

void
server::configure(const config::settings& settings) {
    m_settings = std::make_unique<config::settings>(settings);
}

config::settings
server::get_configuration() const {
    return *m_settings;
}

void
server::signal_handler(int signum) {

    switch(signum) {

        case SIGINT:
            LOGGER_WARN("A signal (SIGINT) occurred.");
            shutdown();
            break;

        case SIGTERM:
            LOGGER_WARN("A signal (SIGTERM) occurred.");
            shutdown();
            break;

        case SIGHUP:
            LOGGER_WARN("A signal (SIGHUP) occurred.");
            break;

        default:
            break;
    }
}

void
server::init_logger() {

    if(m_settings->use_console()) {
        logger::create_global_logger(m_settings->progname(), "console color");
        return;
        ;
    }

    if(m_settings->use_syslog()) {
        logger::create_global_logger(m_settings->progname(), "syslog");

        if(!m_settings->daemonize()) {
            fmt::print(stderr, "PSA: Output sent to syslog while in "
                               "non-daemon mode\n");
        }

        return;
    }

    if(!m_settings->log_file().empty()) {
        logger::create_global_logger(m_settings->progname(), "file",
                                     m_settings->log_file());
        return;
    }

    logger::create_global_logger(m_settings->progname(), "console color");
}

void
server::install_signal_handlers() {

    LOGGER_INFO(" * Installing signal handlers...");

    m_signal_listener = std::make_unique<utils::signal_listener>();

    m_signal_listener->set_handler(std::bind(&server::signal_handler, // NOLINT
                                             this, std::placeholders::_1),
                                   SIGHUP, SIGTERM, SIGINT);

    // This call does not block. Instead, it starts an internal std::thread
    // responsible for processing incoming signals
    m_signal_listener->run();
}

void
server::install_rpc_handlers() {

    LOGGER_INFO(" * Creating RPC listener...");

    // create (but not start) the API listener
    // and register handlers for each request type
    m_network_engine = std::make_unique<network::rpc_acceptor>(
            m_settings->transport_protocol(), m_settings->bind_address(),
            m_settings->remote_port());

    if(m_rpc_registration_callback) {
        m_rpc_registration_callback(m_network_engine);
    }
}

void
server::check_configuration() {

    //    // check that the staging directory exists and that we can write to it
    //    if(!fs::exists(m_settings->staging_directory())) {
    //        LOGGER_ERROR("Staging directory {} does not exist",
    //                     m_settings->staging_directory());
    //        teardown_and_exit();
    //    }

    //    auto s = fs::status(m_settings->staging_directory());
    //
    //    auto expected_perms = fs::perms::owner_read | fs::perms::owner_write;
    //
    //    if((s.permissions() & expected_perms) != expected_perms) {
    //        LOGGER_ERROR("Unable to read from/write to staging directory {}",
    //                     m_settings->staging_directory());
    //        teardown_and_exit();
    //    }
}

void
server::print_greeting() {
    const char greeting[] = "Starting {} daemon (pid {})";
    const auto gsep =
            std::string(sizeof(greeting) - 4 + m_settings->progname().size() +
                                std::to_string(getpid()).size(),
                        '=');

    LOGGER_INFO("{}", gsep);
    LOGGER_INFO(greeting, m_settings->progname(), getpid());
    LOGGER_INFO("{}", gsep);
}

void
server::print_configuration() {
    LOGGER_INFO("");
    LOGGER_INFO("[[ Configuration ]]");
    LOGGER_INFO("  - running as daemon?: {}",
                (m_settings->daemonize() ? "yes" : "no"));

    if(!m_settings->log_file().empty()) {
        LOGGER_INFO("  - log file: {}", m_settings->log_file());
        LOGGER_INFO("  - log file maximum size: {}",
                    m_settings->log_file_max_size());
    } else {
        LOGGER_INFO("  - log file: none");
    }

    LOGGER_INFO("  - pidfile: {}", m_settings->pidfile());
    //    LOGGER_INFO("  - staging directory: {}",
    //    m_settings->staging_directory());
    LOGGER_INFO("  - port for remote requests: {}", m_settings->remote_port());
    LOGGER_INFO("  - workers: {}", m_settings->workers_in_pool());
    LOGGER_INFO("");
}

void
server::print_farewell() {
    const char farewell[] = "Stopping {} daemon (pid {})";
    const auto fsep =
            std::string(sizeof(farewell) - 4 + m_settings->progname().size() +
                                std::to_string(getpid()).size(),
                        '=');

    LOGGER_INFO("{}", fsep);
    LOGGER_INFO(farewell, m_settings->progname(), getpid());
    LOGGER_INFO("{}", fsep);
}

int
server::run() {

    // initialize logging facilities (pre-fork)
    init_logger();

    // validate settings
    check_configuration();

#ifdef __LOGGER_ENABLE_DEBUG__
    if(::prctl(PR_SET_DUMPABLE, 1) != 0) {
        LOGGER_WARN("Failed to set PR_SET_DUMPABLE flag for process. "
                    "Daemon will not produce core dumps.");
    }
#endif

    // daemonize if needed
    if(m_settings->daemonize() && daemonize() != 0) {
        /* parent clean ups and exits, child continues */
        teardown();
        return EXIT_SUCCESS;
    }

    // print useful information
    print_greeting();
    print_configuration();

    LOGGER_INFO("[[ Starting up ]]");

    install_signal_handlers();
    install_rpc_handlers();

    LOGGER_INFO("");
    LOGGER_INFO("[[ Start up successful, awaiting requests... ]]");

    // N.B. This call blocks here, which means that everything after it
    // will only run when a shutdown command is received
    m_network_engine->listen();

    print_farewell();
    teardown();

    LOGGER_INFO("");
    LOGGER_INFO("[Stopped]");

    return EXIT_SUCCESS;
}

void
server::teardown() {

    if(m_signal_listener) {
        LOGGER_INFO("* Stopping signal listener...");
        m_signal_listener->stop();
    }

    if(m_settings) {
        std::error_code ec;

        fs::remove(m_settings->pidfile(), ec);

        if(ec) {
            LOGGER_ERROR("Failed to remove pidfile {}: {}",
                         m_settings->pidfile(), ec.message());
        }

        m_settings.reset();
    }
}

void
server::teardown_and_exit() {
    teardown();
    exit(EXIT_FAILURE);
}

void
server::shutdown() {
    m_network_engine->stop();
}

} // namespace scord
