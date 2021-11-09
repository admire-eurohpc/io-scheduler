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

namespace scord {

server::server()
    : m_is_paused(false), m_settings(std::make_unique<config::settings>()) {}

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
server::init_event_handlers() {

#if 0
    LOGGER_INFO(" * Creating event listener...");

    // create (but not start) the API listener
    // and register handlers for each request type
    try {
        m_ipc_service = std::make_unique<api_listener>();
    } catch(const std::exception& e) {
        LOGGER_ERROR("Failed to create the event listener. This should "
                     "not happen under normal conditions.");
        exit(EXIT_FAILURE);
    }

    boost::system::error_code ec;
    mode_t old_mask = ::umask(0);

    // setup socket for control API
    // (note that if we find any socket files at this point, where no pidfile
    // was found during initialization, they must be stale sockets from
    // another run. Just remove them).
    if(fs::exists(m_settings->control_socket())) {
        fs::remove(m_settings->control_socket(), ec);

        if(ec) {
            LOGGER_ERROR("Failed to remove stale control API socket: {}",
                         ec.message());
            teardown();
            exit(EXIT_FAILURE);
        }
    }

    ::umask(S_IXUSR | S_IRWXG | S_IRWXO); // u=rw-, g=---, o=---

    try {
        m_ipc_service->register_endpoint(m_settings->control_socket());
    } catch(const std::exception& e) {
        LOGGER_ERROR("Failed to create control API socket: {}", e.what());
        teardown();
        exit(EXIT_FAILURE);
    }

    // setup socket for user API
    if(fs::exists(m_settings->global_socket())) {
        fs::remove(m_settings->global_socket(), ec);

        if(ec) {
            LOGGER_ERROR("Failed to remove stale user API socket: {}",
                         ec.message());
            teardown();
            exit(EXIT_FAILURE);
        }
    }

    ::umask(S_IXUSR | S_IXGRP | S_IXOTH); // u=rw-, g=rw-, o=rw-

    try {
        m_ipc_service->register_endpoint(m_settings->global_socket());
    } catch(const std::exception& e) {
        LOGGER_ERROR("Failed to create user API socket: {}", e.what());
        teardown();
        exit(EXIT_FAILURE);
    }

    // restore the umask
    ::umask(old_mask);

    try {

        const std::string bind_address =
                m_settings->bind_address() + ":" +
                std::to_string(m_settings->remote_port());

        m_network_service = std::make_shared<hermes::async_engine>(
                hermes::transport::ofi_tcp, bind_address, true);
    } catch(const std::exception& e) {
        LOGGER_ERROR("Failed to create remote listener: {}", e.what());
        teardown();
        exit(EXIT_FAILURE);
    }

#if 0
    // setup socket for remote connections
    try {
        m_ipc_service->register_endpoint(m_settings->remote_port());
    }
    catch(const std::exception& e) {
        LOGGER_ERROR("Failed to create socket for remote connections: {}",
                e.what());
        teardown();
        exit(EXIT_FAILURE);
    }
#endif


    LOGGER_INFO(" * Installing message handlers...");

    /* user-level functionalities */
    m_ipc_service->register_callback(api::request_type::iotask_create,
                                     std::bind(&server::iotask_create_handler,
                                               this, std::placeholders::_1));

    m_ipc_service->register_callback(api::request_type::iotask_status,
                                     std::bind(&server::iotask_status_handler,
                                               this, std::placeholders::_1));

    m_ipc_service->register_callback(
            api::request_type::ping,
            std::bind(&server::ping_handler, this, std::placeholders::_1));

    /* admin-level functionalities */
    m_ipc_service->register_callback(
            api::request_type::job_register,
            std::bind(&server::job_register_handler, this, std::placeholders::_1));

    m_ipc_service->register_callback(
            api::request_type::job_update,
            std::bind(&server::job_update_handler, this, std::placeholders::_1));

    m_ipc_service->register_callback(
            api::request_type::job_unregister,
            std::bind(&server::job_remove_handler, this, std::placeholders::_1));

    m_ipc_service->register_callback(
            api::request_type::process_register,
            std::bind(&server::process_add_handler, this, std::placeholders::_1));

    m_ipc_service->register_callback(api::request_type::process_unregister,
                                     std::bind(&server::process_remove_handler,
                                               this, std::placeholders::_1));

    m_ipc_service->register_callback(api::request_type::backend_register,
                                     std::bind(&server::namespace_register_handler,
                                               this, std::placeholders::_1));

    /*    m_ipc_service->register_callback(
                api::request_type::backend_update,
                std::bind(&server::namespace_update_handler, this,
       std::placeholders::_1));*/

    m_ipc_service->register_callback(api::request_type::backend_unregister,
                                     std::bind(&server::namespace_remove_handler,
                                               this, std::placeholders::_1));

    m_ipc_service->register_callback(api::request_type::global_status,
                                     std::bind(&server::global_status_handler,
                                               this, std::placeholders::_1));

    m_ipc_service->register_callback(
            api::request_type::command,
            std::bind(&server::command_handler, this, std::placeholders::_1));

    m_ipc_service->register_callback(api::request_type::bad_request,
                                     std::bind(&server::unknown_request_handler,
                                               this, std::placeholders::_1));

    /* remote event handlers */
    m_network_service->register_handler<rpc::push_resource>(std::bind(
            &server::push_resource_handler, this, std::placeholders::_1));

    m_network_service->register_handler<rpc::pull_resource>(std::bind(
            &server::pull_resource_handler, this, std::placeholders::_1));

    m_network_service->register_handler<rpc::stat_resource>(std::bind(
            &server::stat_resource_handler, this, std::placeholders::_1));


    // signal handlers must be installed AFTER daemonizing
    LOGGER_INFO(" * Installing signal handlers...");

    m_ipc_service->set_signal_handler(
            std::bind(&server::signal_handler, this, std::placeholders::_1),
            SIGHUP, SIGTERM, SIGINT);
#endif
} // namespace scord

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
    LOGGER_INFO("  - control socket: {}", m_settings->control_socket());
    LOGGER_INFO("  - global socket: {}", m_settings->global_socket());
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

    //    init_task_manager();
    //    init_event_handlers();
    //    init_namespace_manager();
    //    load_backend_plugins();
    //    load_default_namespaces();

    // load plugins now so that when we propagate the daemon context to them
    // everything is set up
    //    load_transfer_plugins();

    // start the listener for remote transfers
    // N.B. This call returns immediately
    //    m_network_service->run();

    LOGGER_INFO("");
    LOGGER_INFO("[[ Start up successful, awaiting requests... ]]");

    // N.B. This call blocks here, which means that everything after it
    // will only run when a shutdown command is received
    //    m_ipc_service->run();

    print_farewell();
    teardown();

    LOGGER_INFO("");
    LOGGER_INFO("[Stopped]");

    return EXIT_SUCCESS;
}

void
server::teardown() {

    // XXX deprecated, signals_are now managed by api_listener
    //     if(m_signal_listener) {
    //         LOGGER_INFO("* Stopping signal listener...");
    //         m_signal_listener->stop();
    //         //m_signal_listener.reset();
    //     }

    //    if(m_ipc_service) {
    //        LOGGER_INFO("* Stopping API listener...");
    //        m_ipc_service->stop();
    //        m_ipc_service.reset();
    //    }

    //    api_listener::cleanup();

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
server::shutdown() {}

} // namespace scord
