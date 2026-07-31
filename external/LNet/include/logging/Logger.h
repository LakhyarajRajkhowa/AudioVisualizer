#pragma once

#include <string>

class Logger
{
public:
    static void Success(const std::string& category,
                        const std::string& message);

    static void Warning(const std::string& category,
                        const std::string& message);

    static void Error(const std::string& category,
                      const std::string& message);

    static void Info(const std::string& category,
                     const std::string& message);
};
