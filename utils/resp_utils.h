#ifndef RESP_UTILS_H
#define RESP_UTILS_H

#include <map>
#include <functional>
#include <algorithm>
#include "utils.h"

#define int64 int64_t

enum RESPType {
    enumRESPSimpleString = 0,           // simple string "+"
    enumRESPErrors = 1,                 // error         "-"
    enumRESPIntegers = 2,               // int           ":"
    enumRESPBulkStrings = 3,            // bulk strings  "$"
    enumRESPArrays = 4,                 // array         "*"
};

enum EnumInnerError {
    enumArgumentInvalid = 1,
    enumRedisTypeInvalid = 2,
    enumSocketSendMsgFailed = 3,
};

enum EnumOuterError {

};

struct RESPReply {
    std::string str;
    std::vector<std::string> bulkStrs;
    int64 integerResp;

    // 返回类型
    RESPType type;

    EnumInnerError innerError;
    // -1 innerError -2 outerError
    int errorCode;

    RESPReply() {
        Reset();
    }

    RESPReply(const RESPReply& another) {
        str = another.str;
        bulkStrs = another.bulkStrs;
        type = another.type;
        innerError = another.innerError;
        errorCode = another.errorCode;
    }

    ~RESPReply() {

    }

    void Reset() {
        type = enumRESPSimpleString;
        errorCode = 0;
        str = "";
        integerResp = 0;
        bulkStrs.clear();

    }
};

typedef std::map<RESPType, std::function<RESPReply(const std::string&)> > Convert;

class RESPUtils {
public:
    static RESPReply ConvertToRESPReply(const std::string& respString) {
        RESPReply reply;
        if (respString.size() == 0) {
            reply.errorCode = -1;
            reply.innerError = enumArgumentInvalid;
            return reply;
        }

        RESPType type;
        if (!IsValidRedisType(respString[0], type)) {
            reply.errorCode = -1;
            reply.innerError = enumRedisTypeInvalid;
            return reply;
        }
        return RESPUtils::converter[type](respString);
    }

    static bool IsValidRedisType(const char& type, RESPType& resType) {
        switch (type) {
            case '+':
                resType = enumRESPSimpleString;
                break;
            case '-':
                resType = enumRESPErrors;
                break;
            case '*':
                resType = enumRESPArrays;
                break;
            case ':':
                resType = enumRESPIntegers;
                break;
            case '$':
                resType = enumRESPBulkStrings;
                break;
            default:
                return false;
        }
        return true;
    }

    static RESPReply ConvertToError(const std::string& respError);

    static RESPReply ConvertToSimpleString(const std::string& respSimpleString);

    static RESPReply ConvertToInteger(const std::string& respInteger);

    static RESPReply ConvertToBulkStrings(const std::string& respBulkStrings);

private:
    
    static Convert  converter;
};

RESPReply RESPUtils::ConvertToError(const std::string& respError) {
    RESPReply reply;
    reply.errorCode = 0;
    reply.type = enumRESPErrors;
    size_t crlf = respError.find("\r\n");
    if (crlf != std::string::npos) {
        reply.str = respError.substr(1, crlf - 1);
    }
    return reply;
}

RESPReply RESPUtils::ConvertToSimpleString(const std::string& respSimpleString) {
    RESPReply reply;
    reply.errorCode = 0;
    reply.type = enumRESPSimpleString;
    size_t crlf = respSimpleString.find("\r\n");
    if (crlf != std::string::npos) {
        reply.str = respSimpleString.substr(1, crlf - 1);
    }
    
    return reply;
}

RESPReply RESPUtils::ConvertToInteger(const std::string& respInteger) {
    RESPReply reply;
    reply.errorCode = 0;
    reply.type = enumRESPSimpleString;
    size_t crlf = respInteger.find("\r\n");
    if (crlf != std::string::npos) {
        reply.integerResp = stol(respInteger.substr(1, crlf - 1));
    }
    
    return reply;
}

RESPReply RESPUtils::ConvertToBulkStrings(const std::string& respBulkStrings) {
    RESPReply reply;
    reply.errorCode = 0;
    reply.type = enumRESPBulkStrings;
    reply.bulkStrs = MyUtils::SplitString(respBulkStrings.substr(1), "\r\n");

    // for (const auto& bulkStr : reply.bulkStrs) {
    //     std::cout << bulkStr << std::endl;
    // }
    return reply;
}

Convert RESPUtils::converter = {
    {enumRESPSimpleString, &ConvertToSimpleString},
    {enumRESPErrors, &ConvertToError},
    {enumRESPIntegers, &ConvertToInteger},
    {enumRESPBulkStrings, &ConvertToBulkStrings},
};

#endif