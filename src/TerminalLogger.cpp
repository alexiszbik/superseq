#include "TerminalLogger.h"

#include <iostream>

void TerminalLogger::info(std::string_view message)
{
    std::cout << message;
}

void TerminalLogger::error(std::string_view message)
{
    std::cerr << message;
}
