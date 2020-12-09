#ifndef RESP_UTILS_H
#define RESP_UTILS_H

#include <map>
#include <functional>

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
};

enum EnumOuterError {

};

struct RESPReply {
    std::string str;
    std::vector<std::string> bulkStrs;

    // 返回类型
    RESPType type;

    EnumInnerError innerError;
    EnumOuterError outerError;
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
        outerError = another.outerError;
        errorCode = another.errorCode;
    }

    ~RESPReply() {

    }

    void Reset() {
        type = enumRESPSimpleString;
        errorCode = 0;
    }
};



class RESPUtils {
public:
    static RESPReply ConvertToRESPReply(const std::string& respSimpleString) {
        RESPReply reply;
        if (respSimpleString.size() == 0) {
            reply.errorCode = -1;
            reply.innerError = enumArgumentInvalid;
            return reply;
        }

        RESPType type;
        if (!IsValidRedisType(respSimpleString[0], type)) {
            reply.errorCode = -1;
            reply.innerError = enumRedisTypeInvalid;
            return reply;
        }
        
        
        return converter[type](respSimpleString);
    }

    static RESPReply ConvertToSimpleString(const std::string& respSimpleString) {
        RESPReply reply;
        reply.errorCode = 0;
        reply.type = enumRESPSimpleString;
        reply.str = respSimpleString.substr(1, respSimpleString.size() - 2);
        return reply;
    }

    static RESPReply ConvertToError(const std::string& respError) {
        RESPReply reply;
        reply.errorCode = 0;
        reply.type = enumRESPErrors;
        reply.str = respError.substr(1);
        return reply;
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

private:
    typedef std::map<RESPType, std::function<RESPReply(const std::string&)> > Convert;
    static Convert CreateConverter();
    static Convert  converter;
};

RESPUtils::Convert RESPUtils::CreateConverter() {
    converter = {
        {enumRESPSimpleString, &ConvertToSimpleString},
    };

    return converter;
}

RESPUtils::Convert RESPUtils::converter = RESPUtils::CreateConverter();


// std::map<RESPType, std::function<RESPReply(const std::string&)> > RESPUtils::converter = {
//     
// }


#endif