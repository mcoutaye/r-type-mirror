/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** Args
*/

#include "../../include/args/Args.hpp"
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cctype>

void Args::Config::reset()
{
    port = 55002;
    machine = "127.0.0.1";
}

Args::Args(int argc, char *argv[], Mode mode)
: _argc(argc), _argv(argv), _mode(mode)
{
    if (argc > 0) {
        _programName = argv[0];
        std::size_t pos = _programName.rfind('/');
        if (pos != std::string::npos)
            _programName = _programName.substr(pos + 1);
    }
}

void Args::printUsageAndExit(const std::string &programName, Mode mode)
{
    if (mode == Mode::SERVER) {
        std::cerr << "USAGE: " << programName << " -p port [-h machine]\n"
                  << "\n"
                  << "    -p port     port number of the server\n"
                  << "    -h machine  name of the machine (default: 127.0.0.1)\n"
                  << std::endl;
    } else {
        std::cerr << "USAGE: " << programName << " -h machine [-p port]\n"
                  << "\n"
                  << "    -h machine  IP or hostname of the server (default: 127.0.0.1)\n"
                  << "    -p port     port number of the server (default: 55002)\n"
                  << std::endl;
    }
    std::exit(84);
}

void Args::checkPort(const std::string &str)
{
    if (str.empty()) {
        std::cerr << "Error: the port cannot be empty." << std::endl;
        printUsageAndExit(_programName, _mode);
    }
    for (char c : str)
        if (!std::isdigit(c)) {
            std::cerr << "Error: the port must contain only digits." << std::endl;
            printUsageAndExit(_programName, _mode);
        }
    int p = std::stoi(str);
    if (p < 1 || p > 65535) {
        std::cerr << "Error: port must be between 1 and 65535." << std::endl;
        printUsageAndExit(_programName, _mode);
    }
    _config.port = p;
}

Args::Config Args::parse()
{
    int opt;
    optind = 0;
    std::string optstring = (_mode == Mode::SERVER) ? ":p:h:" : ":h:p:";
    while ((opt = getopt(_argc, _argv, optstring.c_str())) != -1) {
        switch (opt) {
            case 'h':
                _config.machine = optarg;
                break;
            case 'p':
                checkPort(optarg);
                break;
            case ':':
                std::cerr << "Option -" << static_cast<char>(optopt)
                          << " requires an argument." << std::endl;
                printUsageAndExit(_programName, _mode);
                break;
            case '?':
                std::cerr << "Unknown option: -" << static_cast<char>(optopt) << std::endl;
                printUsageAndExit(_programName, _mode);
                break;
            default:
                printUsageAndExit(_programName, _mode);
        }
    }
    if (optind < _argc) {
        std::cerr << "Error: unexpected argument." << std::endl;
        printUsageAndExit(_programName, _mode);
    }
    if (_mode == Mode::SERVER) {
        std::cout << "[Server] Listening on port " << _config.port << std::endl;
    } else {
        std::cout << "[Client] Connecting to " << _config.machine << ":" << _config.port << std::endl;
    }
    return _config;
}
