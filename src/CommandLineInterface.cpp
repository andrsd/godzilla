// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/CommandLineInterface.h"
#include "godzilla/App.h"
#include "godzilla/CallStack.h"

namespace godzilla {

CommandLineInterface::CommandLineInterface(App & app, int argc, const char * const * argv) :
    app(app)
{
    this->args.reserve(argc);
    for (int i = 0; i < argc; ++i)
        this->args.emplace_back(argv[i]);
}

CommandLineInterface::CommandLineInterface(App & app, const std::vector<std::string> & args) :
    app(app)
{
    this->args.reserve(args.size() + 1);
    this->args.emplace_back(app.get_name());
    for (auto & s : args)
        this->args.emplace_back(s);
}

std::string
CommandLineInterface::get_app_name() const
{
    CALL_STACK_MSG();
    return this->app.get_name();
}

cxxopts::Options
CommandLineInterface::create_command_line_options()
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
    return cmdln_opts;
}

cxxopts::ParseResult
CommandLineInterface::parse_command_line(cxxopts::Options & opts)
{
    CALL_STACK_MSG();
    auto argc = this->args.size();
    std::vector<const char *> argv;
    argv.reserve(argc);
    for (auto & a : this->args)
        argv.push_back(a.c_str());
    return opts.parse(argc, argv.data());
}

void
CommandLineInterface::process_command_line(cxxopts::Options & opts,
                                           const cxxopts::ParseResult & result)
{
    CALL_STACK_MSG();

    if (result.count("help")) {
        fmt::print("{}", opts.help());
    }
    else if (result.count("version")) {
        fmt::print("{}, version {}\n", this->app.get_name(), this->app.get_version());
    }
    else {
        if (result.count("no-colors"))
            Terminal::set_colors(false);

        if (result.count("verbose"))
            this->app.set_verbosity_level(result["verbose"].as<unsigned int>());

        if (result.count("restart-from"))
            this->app.set_restart_file_name(result["restart-from"].as<std::string>());

        if (result.count("perf-log"))
            this->app.set_perf_log_file_name(result["perf-log"].as<std::string>());

        if (result.count("redirect-stdout")) {
            auto fname = fmt::format("output.{}.txt", this->app.get_comm().rank());
            this->app.redirect_stdout(fname);
        }

        if (result.count("redirect-stderr")) {
            auto fname = fmt::format("error.{}.txt", this->app.get_comm().rank());
            this->app.redirect_stderr(fname);
        }
    }

    if (result.count("export-parameters"))
        this->app.export_parameters_yaml();
}

int
CommandLineInterface::run()
{
    auto cli_opt = create_command_line_options();
    try {
        auto result = parse_command_line(cli_opt);
        process_command_line(cli_opt, result);
        return 0;
    }
    catch (const cxxopts::exceptions::exception & e) {
        fmt::print(stderr, "Error: {}\n", e.what());
        fmt::print(stdout, "{}", cli_opt.help());
        return 1;
    }
}

} // namespace godzilla
