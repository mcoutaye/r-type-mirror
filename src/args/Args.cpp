/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** Args
*/

#include "../../include/args/Args.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cctype>

void Args::Config::reset()
{
    port = 55002;
    machine = "127.0.0.1";
}

std::string Args::getProgramName(const std::string& fullPath)
{
    const std::size_t pos = fullPath.find_last_of("/\\");
    return (pos == std::string::npos) ? fullPath : fullPath.substr(pos + 1);
}

Args::Args(int argc, char* argv[], Mode mode)
    : _argc(argc), _argv(argv), _mode(mode)
{
    if (argc > 0 && argv[0] != nullptr)
        _programName = getProgramName(argv[0]);
}

void Args::printUsageAndExit(const std::string& programName, Mode mode)
{
    if (mode == Mode::SERVER) {
        std::cerr << "USAGE: " << programName << " -p port [-h machine]\n"
                  << "\n"
                  << "  -p port     port number of the server\n"
                  << "  -h machine  name of the machine (default: 127.0.0.1)\n"
                  << std::endl;
    } else {
        std::cerr << "USAGE: " << programName << " -h machine [-p port]\n"
                  << "\n"
                  << "  -h machine  IP or hostname of the server (default: 127.0.0.1)\n"
                  << "  -p port     port number of the server (default: 55002)\n"
                  << std::endl;
    }
    std::exit(84);
}

void Args::checkPort(const std::string& str)
{
    if (str.empty()) {
        std::cerr << "Error: the port cannot be empty." << std::endl;
        printUsageAndExit(_programName, _mode);
    }
    for (char c : str) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            std::cerr << "Error: the port must contain only digits." << std::endl;
            printUsageAndExit(_programName, _mode);
        }
    }
    const int p = std::stoi(str);
    if (p < 1 || p > 65535) {
        std::cerr << "Error: port must be between 1 and 65535." << std::endl;
        printUsageAndExit(_programName, _mode);
    }
    _config.port = static_cast<unsigned short>(p);
}

Args::Config Args::parse()
{
    _config.reset();
    for (int i = 1; i < _argc; ++i) {
        const std::string arg = _argv[i];
        if (arg == "-h") {
            if (++i >= _argc) {
                std::cerr << "Option -h requires an argument." << std::endl;
                printUsageAndExit(_programName, _mode);
            }
            _config.machine = _argv[i];
        }
        else if (arg == "-p") {
            if (++i >= _argc) {
                std::cerr << "Option -p requires an argument." << std::endl;
                printUsageAndExit(_programName, _mode);
            }
            checkPort(_argv[i]);
        }
        else {
            std::cerr << "Unknown option or unexpected argument: " << arg << std::endl;
            printUsageAndExit(_programName, _mode);
        }
    }
    if (_mode == Mode::SERVER)
        std::cout << "[Server] Listening on port " << _config.port << std::endl;
    else
        std::cout << "[Client] Connecting to " << _config.machine << ":" << _config.port << std::endl;
    return _config;
}