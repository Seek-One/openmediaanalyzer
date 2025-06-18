#ifndef STRINGHELPERS_STRING_FORMATTER_H_
#define STRINGHELPERS_STRING_FORMATTER_H_

#include <string>

class StringFormatter {

public:
    static std::string formatString(const char* fmt, ...);

};

#endif // STRINGHELPERS_STRING_FORMATTER_H_