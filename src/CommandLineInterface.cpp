// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/CommandLineInterface.h"
#include "godzilla/App.h"
#include "godzilla/CallStack.h"

namespace godzilla {

cxxopts::Options
CommandLineInterface::default_command_line_options()
{
    cxxopts::Options cmdln_opts("");
    cmdln_opts.add_option("", "h", "help", "Show this help page", cxxopts::value<bool>(), "");
    cmdln_opts
        .add_option("", "", "restart-from", "Restart file name", cxxopts::value<std::string>(), "");
    cmdln_opts.add_option("", "v", "version", "Show the version", cxxopts::value<bool>(), "");
    cmdln_opts.add_option("", "", "verbose", "Verbosity level", cxxopts::value<unsigned int>(), "");
    cmdln_opts
        .add_option("", "", "no-colors", "Do not use terminal colors", cxxopts::value<bool>(), "");
    cmdln_opts.add_option("",
                          "",
                          "export-parameters",
                          "Export parameters for all registered objects into a YAML file",
                          cxxopts::value<bool>(),
                          "");
    cmdln_opts.add_option("",
                          "",
                          "perf-log",
                          "Save performance log into a file",
                          cxxopts::value<std::string>(),
                          "");
    cmdln_opts
        .add_option("", "", "log-file", "Save into a log file", cxxopts::value<std::string>(), "");
    return cmdln_opts;
}

CommandLineInterface::CommandLineInterface(App & app, int argc, const char * const * argv) :
    app_(app)
{
    this->args_.reserve(argc);
    for (int i = 0; i < argc; ++i)
        this->args_.emplace_back(argv[i]);
}

CommandLineInterface::CommandLineInterface(App & app, const std::vector<String> & args) : app_(app)
{
    this->args_.reserve(args.size() + 1);
    this->args_.emplace_back(app.get_name());
    for (auto & s : args)
        this->args_.emplace_back(s);
}

String
CommandLineInterface::get_app_name() const
{
    CALL_STACK_MSG();
    return this->app_.get_name();
}

cxxopts::ParseResult
CommandLineInterface::parse(cxxopts::Options & opts)
{
    CALL_STACK_MSG();
    try {
        auto argc = this->args_.size();
        std::vector<const char *> argv;
        argv.reserve(argc);
        for (auto & a : this->args_)
            argv.push_back(a.c_str());
        return opts.parse(argc, argv.data());
    }
    catch (const cxxopts::exceptions::exception & e) {
        fmt::print(stderr, "Error: {}\n", e.what());
        fmt::print(stdout, "{}", opts.help());
        godzilla::abort();
    }
}

void
CommandLineInterface::process_command_line(const cxxopts::ParseResult & result)
{
    CALL_STACK_MSG();

    if (result.count("no-colors"))
        Terminal::set_colors(false);

    if (result.count("verbose"))
        this->app_.set_verbosity_level(result["verbose"].as<unsigned int>());

    if (result.count("restart-from"))
        this->app_.set_restart_file_name(result["restart-from"].as<std::string>());

    if (result.count("perf-log"))
        this->app_.set_perf_log_file_name(result["perf-log"].as<std::string>());

    if (result.count("log-file"))
        this->app_.get_logger()->set_log_file_name(result["log-file"].as<std::string>());

    if (result.count("redirect-stdout")) {
        auto fname = fmt::format("output.{}.txt", this->app_.get_comm().rank());
        this->app_.redirect_stdout(fname);
    }

    if (result.count("redirect-stderr")) {
        auto fname = fmt::format("error.{}.txt", this->app_.get_comm().rank());
        this->app_.redirect_stderr(fname);
    }
}

int
CommandLineInterface::run(const cxxopts::Options & opts, const cxxopts::ParseResult & result)
{
    if (result.count("help")) {
        fmt::print("{}", opts.help());
    }
    else if (result.count("version")) {
        fmt::print("{}, version {}\n", this->app_.get_name(), this->app_.get_version());
    }
    else if (result.count("export-parameters"))
        this->app_.export_parameters_yaml();

    return 0;
}

} // namespace godzilla
