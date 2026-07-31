#pragma once

#include "Logger.h"

#define LOG_SUCCESS(category, msg) Logger::Success(category, msg)
#define LOG_WARNING(category, msg) Logger::Warning(category, msg)
#define LOG_ERROR(category, msg)   Logger::Error(category, msg)
#define LOG_INFO(category, msg)    Logger::Info(category, msg)
