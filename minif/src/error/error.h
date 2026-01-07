#pragma once
#include <string>
#include <iostream>
#include <cstdlib>

const std::string ANSI_RED = "\033[31m";
const std::string ANSI_RESET = "\033[0m";

inline void throw_error(const std::string& msg) {
    std::cerr << ANSI_RED << "[Error] "  << msg << ANSI_RESET << "\n";
    std::exit(1);
}
