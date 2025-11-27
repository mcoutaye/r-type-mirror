/*
** EPITECH PROJECT, 2025
** r-type-mirror
** File description:
** Args
*/

#pragma once
#include <string>

class Args {
    public:
        enum class Mode { SERVER, CLIENT };

        class Config {
            public:
                Config() = default;
                ~Config() = default;
                std::string machine = "127.0.0.1";
                int port = 55002;
                void reset();
        };

        Args(int argc, char *argv[], Mode mode);
        ~Args() = default;
        Config parse();
        [[noreturn]] static void printUsageAndExit(const std::string &programName, Mode mode);
    private:
        int _argc;
        char **_argv;
        std::string _programName;
        Config _config;
        Mode _mode;
        void checkPort(const std::string &str);
};
