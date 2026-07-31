#include "logging/Logger.h"

#include <iostream>

namespace
{
    constexpr const char* RESET  = "\033[0m";

    constexpr const char* RED    = "\033[31m";
    constexpr const char* GREEN  = "\033[32m";
    constexpr const char* YELLOW = "\033[33m";
    constexpr const char* CYAN   = "\033[36m";
}

void Logger::Success(const std::string& category,
                     const std::string& message)
{
    std::cout
        << "[" << category << "]"
        << GREEN << "[SUCCESS] " << RESET
        << message
        << '\n';
}

void Logger::Warning(const std::string& category,
                     const std::string& message)
{
    std::cout
        << "[" << category << "]"
        << YELLOW << "[WARNING] " << RESET
        << message
        << '\n';
}

void Logger::Error(const std::string& category,
                   const std::string& message)
{
    std::cerr
        << "[" << category << "]"
        << RED << "[ERROR] " << RESET
        << message
        << '\n';
}

void Logger::Info(const std::string& category,
                  const std::string& message)
{
    std::cout
        << "[" << category << "]"
        << CYAN << "[INFO] " << RESET
        << message
        << '\n';
}
