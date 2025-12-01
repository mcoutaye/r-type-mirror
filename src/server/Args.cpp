/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** Args
*/

#include "Args.hpp"
#include <iostream>
#include <cstdlib>
#include <cctype>

void Args::Config::reset()
{
    port = 55002;
    machine = "127.0.0.1";
}

Args::Args(int argc, char *argv[])
    : _argc(argc), _argv(argv)
{
    if (argc > 0) {
        _programName = argv[0];
        std::size_t pos = _programName.rfind('/');
        if (pos != std::string::npos)
            _programName = _programName.substr(pos + 1);
    }
}

void Args::printUsageAndExit(const std::string &programName)
{
    std::cerr << "USAGE: " << programName << " -p port [-h machine]\n"
              << "\n"
              << "    -p port     port number of the server\n"
              << "    -h machine  name of the machine (default: 127.0.0.1)\n"
              << std::endl;
    std::exit(84);
}

void Args::checkPort(const std::string &str)
{
    if (str.empty()) {
        std::cerr << "Error: the port cannot be empty." << std::endl;
        printUsageAndExit(_programName);
    }
    for (char c : str)
        if (!std::isdigit(c)) {
            std::cerr << "Error: the port must contain only digits." << std::endl;
            printUsageAndExit(_programName);
        }
    int p = std::stoi(str);
    if (p < 1 || p > 65535) {
        std::cerr << "Error: port must be between 1 and 65535." << std::endl;
        printUsageAndExit(_programName);
    }
    _config.port = p;
}

Args::Config Args::parse()
{
    int opt;
    optind = 0;
    while ((opt = getopt(_argc, _argv, ":p:h:")) != -1) {
        switch (opt) {
            case 'p':
                checkPort(optarg);
                break;
            case 'h':
                _config.machine = optarg;
                break;
            case ':':
                std::cerr << "Option -" << static_cast<char>(optopt)
                          << " requires an argument." << std::endl;
                printUsageAndExit(_programName);
                break;
            case '?':
                std::cerr << "Unknown option: -" << static_cast<char>(optopt) << std::endl;
                printUsageAndExit(_programName);
                break;
            default:
                printUsageAndExit(_programName);
        }
    }
    if (optind < _argc) {
        std::cerr << "Error: unexpected argument." << std::endl;
        printUsageAndExit(_programName);
    }
    std::cout << "[Server] Listening on port " << _config.port << std::endl;
    if (_config.machine != "127.0.0.1")
        std::cout << "[Info] -h " << _config.machine << " ignored in server mode" << std::endl;
    return _config;
}
