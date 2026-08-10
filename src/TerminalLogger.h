#pragma once

#include "Logger.h"

class TerminalLogger : public Logger
{
public:
    void info(std::string_view message) override;
    void error(std::string_view message) override;
};
