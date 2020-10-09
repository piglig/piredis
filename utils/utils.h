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

    static std::string GetRealKey(const std::string& key) {
        std::string res;
        int begin = 0;
        int end = 0;
        for (int i = 0; i < key.length(); ++i) {
            if (key[i] == '{') {
                begin = i;
                break;
            }
        }

        if (begin == 0) {
            res = key;
            return res;
        }

        for (int i = begin + 1; i < key.length(); ++i) {
            if (key[i] == '}') {
                end = i;
                break;
            }
        }

        if (end == 0 || end == key.length() - 1) {
            res = key;
            return res;
        }

    }
};

#endif