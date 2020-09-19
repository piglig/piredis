#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>

class MyUtils {
public:
    static std::vector<std::string> SplitString(const std::string& str, const std::string& delimiter) {
        size_t pos = 0;
        std::string temp = str;
        std::vector<std::string> res;
        while ((pos = temp.find(delimiter)) != std::string::npos) {
            res.push_back(temp.substr(0, pos));
            temp.erase(0, pos + delimiter.length());
        }
        return res;
    }
};

#endif