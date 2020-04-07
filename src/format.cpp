#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"

using std::string;

string Format::ElapsedTime(const long& seconds) {
    int hour = 0, minute = 0, second = 0;
    hour = seconds / 3600;
    minute = (seconds % 3600) / 60;
    second = seconds % 60;
    return FormatNumber(hour) + ":" + FormatNumber(minute) + ":" + FormatNumber(second);
}

string Format::FormatNumber(const int& number){
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << number;
    return ss.str();  
}