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
        std::string token;
        while ((pos = temp.find(delimiter)) != std::string::npos) {
            token = temp.substr(0, pos);
            res.push_back(token);
            temp.erase(0, pos + delimiter.length());
        }
        if (temp.length() > 0) {
            res.push_back(temp);
        }
        
        return res;
    }

    static unsigned short CRC16_XMODEM(char *puchMsg, unsigned int usDataLen)
    {
        unsigned short wCRCin = 0x0000;
        unsigned short wCPoly = 0x1021;
        unsigned char wChar = 0;
    
        while (usDataLen--) 
        {
            wChar = *(puchMsg++);
            wCRCin ^= (wChar << 8);
            for (int i = 0; i < 8; i++) 
            {
                if (wCRCin & 0x8000)
                    wCRCin = (wCRCin << 1) ^ wCPoly;
                else
                    wCRCin = wCRCin << 1;
            }
        }
        return (wCRCin);
    }

    // static unsigned short GetSlotValue(const std::string& key) {
    //     std::string res;
    //     int begin = 0;
    //     int end = 0;
    //     for (begin = 0; begin < key.length(); ++begin) {
    //         if (key[begin] == '{') {
    //             break;
    //         }
    //     }

    //     if (begin == key.length()) {
    //         res = key;
    //         return CRC16_XMODEM(&res[0], res.length()) % 16384;
    //     }

    //     for (end = begin + 1; end < key.length(); ++end) {
    //         if (key[end] == '}') {
    //             break;
    //         }
    //     }

    //     if (end == begin + 1 || end == key.length()) {
    //         res = key;
    //         return CRC16_XMODEM(&res[0], key.length()) % 16384;
    //     }
    //     return CRC16_XMODEM(&(key.substr(begin + 1, end - begin - 1))[0], end - begin - 1) % 16384;
    // }

    // static std::string connacateStringVector(const std::vector<std::string>& vs) {
    //     std::string res = "";

    //     for_each(vs.begin(), vs.end(), [&](const auto& str) {
    //         res += str + " ";
    //     });

    //     return res;
    // }

    // static std::string connacateStringPairHashMap(const std::map<std::string, std::string>& pairs) {
    //     std::string res = "";

    //     for (const auto& pair : pairs) {
    //         res += pair.first + " " + pair.second + " ";
    //     }

    //     return res;
    // }
};

#endif