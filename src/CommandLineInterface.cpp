// SPDX-FileCopyrightText: 2025 David Andrs <andrsd@gmail.com>
// SPDX-License-Identifier: MIT

#include "godzilla/CommandLineInterface.h"
#include "godzilla/Exception.h"
#include "godzilla/InputFile.h"
#include "godzilla/GYMLFile.h"

namespace godzilla {

CommandLineInterface::CommandLineInterface(int argc,
                                           const char * const * argv,
                                           const std::string & app_name) :
    cmdln_opts(app_name)
{
    this->args.resize(argc);
    for (int i = 0; i < argc; ++i)
        this->args.emplace_back(argv[i]);
}

CommandLineInterface::CommandLineInterface(const std::vector<std::string> & args,
                                           const std::string & app_name) :
    args(args),
    cmdln_opts(app_name)
{
}

CommandLineInterface::~CommandLineInterface() {}

const InputFile &
CommandLineInterface::get_input_file() const
{
    CALL_STACK_MSG();
    return *this->yml.get();
}

cxxopts::Options
CommandLineInterface::create_command_line_options()
{
    cxxopts::Options cmdln_opts("");
    cmdln_opts.add_option("", "h", "help", "Show this help page", cxxopts::value<bool>(), "");
    cmdln_opts.add_option("",
                          "i",
                          "input-file",
                          "Input file to execute",
                          cxxopts::value<std::string>(),
                          "");
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
    argv.reserve(argc + 2);
    argv.push_back(opts.program().c_str());
    for (auto & a : this->args)
        argv.push_back(a.c_str());
    argv.push_back(nullptr);
    return opts.parse(argc + 1, argv.data());
}

const std::string &
CommandLineInterface::get_input_file_name() const
{
    CALL_STACK_MSG();
    static std::string empty_file_name;
    if (this->yml == nullptr)
        return empty_file_name;
    else
        return this->yml->get_file_name();
}

cxxopts::Options &
CommandLineInterface::get_command_line_opts()
{
    return this->cmdln_opts;
}

void
CommandLineInterface::process_command_line(cxxopts::Options & opts,
                                           const cxxopts::ParseResult & result)
{
    CALL_STACK_MSG();

    if (result.count("help")) {
        fmt::print("{}", this->cmdln_opts.help());
    }
    else if (result.count("version")) {
        // fmt::print("{}, version {}\n", get_name(), get_version());
    }
    else {
        if (result.count("no-colors"))
            Terminal::set_colors(false);

        // if (result.count("verbose"))
        //     set_verbosity_level(result["verbose"].as<unsigned int>());

        // if (result.count("restart-from"))
        //     this->restart_file_name = result["restart-from"].as<std::string>();
        // if (result.count("perf-log"))
        //     this->perf_log_file_name = result["perf-log"].as<std::string>();
    }

    if (result.count("input-file")) {
        auto input_file_name = result["input-file"].as<std::string>();
        run_input_file(input_file_name);
    }

    // if (result.count("export-parameters"))
    //     export_parameters_yaml();
}

Qtr<InputFile>
CommandLineInterface::create_input_file()
{
    CALL_STACK_MSG();
    return nullptr;
    // return Qtr<GYMLFile>::alloc(this);
}

void
CommandLineInterface::run_input_file(const std::string & input_file_name)
{
    CALL_STACK_MSG();
    if (!utils::path_exists(input_file_name))
        throw Exception(
            "Unable to open '{}' for reading. Make sure it exists and you have read permissions.",
            input_file_name);

    this->yml = create_input_file();
    if (this->yml == nullptr)
        throw InternalError("App::yaml is null");

#if 0
    this->yml->parse(input_file_name);
    this->yml->build();
    this->problem = this->yml->get_problem();

    if (this->logger->get_num_errors() == 0)
        this->yml->create_objects();

    if (!check_integrity()) {
        this->logger->print();
        throw Exception("");
    }

    run_problem();
#endif
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
