#include <cstdio>
#include <cstdarg>

#include "StringFormatter.h"

std::string StringFormatter::formatString(const char* fmt, ...){
    va_list args;
    va_start(args, fmt);

    va_list args_copy;
    va_copy(args_copy, args);
    int size = std::vsnprintf(nullptr, 0, fmt, args_copy);
    va_end(args_copy);

    if (size < 0) {
        va_end(args);
        return "String formatting error";
    }

    std::string result(size, '\0');

    std::vsnprintf(&result[0], size + 1, fmt, args); 
    va_end(args);

    return result;
}