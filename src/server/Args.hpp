/*
** EPITECH PROJECT, 2025
** Teck3
** File description:
** Args
*/

#pragma once
#include <string>

class Args {
    public:
        class Config {
        public:
            Config() = default;
            ~Config() = default;
            int port = 55002;
            std::string machine = "127.0.0.1";
            void reset();
        };
        explicit Args(int argc, char *argv[]);
        ~Args() = default;
        Config parse();
        [[noreturn]] static void printUsageAndExit(const std::string &programName);
    private:
        int _argc;
        char **_argv;
        std::string _programName;
        Config _config;
        void checkPort(const std::string &str);
};
