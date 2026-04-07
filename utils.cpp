#include "utils.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

void printResult(double val) {
    if (std::isinf(val)) {
        std::cout << (val > 0 ? "inf\n" : "-inf\n");
        return;
    }
    if (std::isnan(val)) {
        std::cout << "nan\n";
        return;
    }
    
    if (val == 0.0 && std::signbit(val)) {
        std::cout << "-0\n";
        return;
    }
    
    std::ostringstream oss;
    oss << std::setprecision(6) << val;
    std::string s = oss.str();
    
    if (s.find('.') != std::string::npos) {
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (!s.empty() && s.back() == '.') s.pop_back();
    }
    std::cout << s << "\n";
}
