#ifndef __SIMPLE_JSON__
#define __SIMPLE_JSON__

#include <exception>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

//------------------------------------- API -------------------------------------

namespace simpleJSON {
    class JSONString;
    class JSONNumber;
    class JSONBool;
    class JSONNull;
    class JSONArray;
    class JSONObject;

    using JSONFloating = double;
    using JSONUnsignedDecimal = unsigned long long int;
    using JSONSignedDecimal = long long int;

    JSONObject parseFromFile(const char* fileName);
    JSONObject parseFromString(std::string& jsonString);
    void dumpToFile(const char* fileName);

    class JSONException : public std::exception {
        public:
            JSONException(const char* msg);

            const char * what () const throw ();

        private:
            const char* message;
    };

    class JSONString {
        public:
            JSONString();
            JSONString(const char* str);
            JSONString(std::string str);

            friend bool operator<(const JSONString& lhs, const JSONString& rhs);

            std::string getValue() const;
            std::string toString() const;

        private:
            std::string value;
    };

    class JSONNumber {
        public:
            JSONNumber();
            JSONNumber(const JSONFloating val);
            JSONNumber(const JSONUnsignedDecimal val);
            JSONNumber(const JSONSignedDecimal val);

            std::string toString() const;

        private:
            std::variant<JSONFloating, JSONUnsignedDecimal, JSONSignedDecimal> value;
    };

    class JSONBool {
        public:
            JSONBool(); // true by default
            JSONBool(bool val);

            bool getValue() const;
            std::string toString() const;

        private :
            bool value;
    };

    class JSONNull {
        public:
            JSONNull();

            std::nullptr_t getValue() const;
            std::string toString() const;

        private:
            std::nullptr_t value;    
    };

    class JSONArray {
        public:
            JSONArray();

            // TODO: explore other solutions
            template <typename T>
            void emplace(T value);

            std::string toString() const;

        private:
            std::vector<std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray, JSONObject>> value;
    };

    class JSONObject {
        public:
            JSONObject();
            JSONObject(JSONString val);
            JSONObject(JSONNumber val);
            JSONObject(JSONBool val);
            JSONObject(JSONNull val);
            JSONObject(JSONArray val);
            JSONObject(std::map<JSONString, std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray, JSONObject>> val);

            void removeField(JSONString key);

            std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray, JSONObject>& operator[](JSONString key);
            const std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray, JSONObject>& operator[](JSONString key) const;

            std::string toString() const;

        private:
            std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray,
                    std::map<JSONString, std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray, JSONObject>>> value;
    };

}   // namespace simpleJSON 

//------------------------------------- IMPLEMENTATION -------------------------------------

namespace internal {
    enum class NextJsonType {
        JSON_STRING,
        JSON_NUMBER,
        JSON_BOOL,
        JSON_NULL,
        JSON_ARRAY,
        JSON_OBJECT,
        JSON_END_OF_STREAM,
        JSON_ERROR
    };

    simpleJSON::JSONObject beginParseFromStream__internal(std::istream& stream);

    NextJsonType detectNextType(char nextCharInStream);

    simpleJSON::JSONString parseString__internal(std::istream& stream);
    simpleJSON::JSONNumber parseNumber__internal(std::istream& stream);
    simpleJSON::JSONBool parseBool__internal(std::istream& stream);
    simpleJSON::JSONNull parseNull__internal(std::istream& stream);
    simpleJSON::JSONArray parseArray__internal(std::istream& stream);
    simpleJSON::JSONObject parseObject__internal(std::istream& stream);
} // namespace internal

namespace simpleJSON {
    JSONObject parseFromFile(const char* fileName) {
        std::ifstream stream(fileName);
        return internal::beginParseFromStream__internal(stream);
    }

    JSONObject parseFromString(std::string& jsonString) {
        std::stringstream stream(jsonString);
        return internal::beginParseFromStream__internal(stream);
    }

    // void dumpToFile(const char* fileName);

    // JSONexception

    JSONException::JSONException(const char* msg) : message(msg) {}

    const char* JSONException::what () const throw () {
        return message;
    }

    // JSONString

    bool operator<(const JSONString& lhs, const JSONString& rhs) { 
        return lhs.value < rhs.value; 
    }

    JSONString::JSONString() : value(std::string{}) {}
    JSONString::JSONString(const char* str) : value(std::string(str)) {}
    JSONString::JSONString(std::string str) : value(str) {}

    std::string JSONString::getValue() const {
        return value;
    }

    std::string JSONString::toString() const {
        return "\"" + value + "\"";
    }

    // JSONNumber

    JSONNumber::JSONNumber() : value(JSONSignedDecimal(0)) {}
    JSONNumber::JSONNumber(const JSONFloating val) : value(val) {}
    JSONNumber::JSONNumber(const JSONUnsignedDecimal val) : value(val) {}
    JSONNumber::JSONNumber(const JSONSignedDecimal val) : value(val) {}

    std::string JSONNumber::toString() const {
        std::string res;

        auto toStringLambda = [&](auto&& arg) {
            res += std::to_string(arg);
        };
        
        std::visit(toStringLambda, value);

        return res;
    }

    // JSONBool

    JSONBool::JSONBool() : value(false) {}
    JSONBool::JSONBool(bool val) : value(val) {}

    bool JSONBool::getValue() const {
        return value;
    }

    std::string JSONBool::toString() const {
        return value ? std::string("true") : std::string("false");
    }

    // JSONNull

    JSONNull::JSONNull() : value(nullptr) {}

    
    std::nullptr_t JSONNull::getValue() const {
        return value;
    }

    std::string JSONNull::toString() const {
        return std::string("null");
    }

    // JSONArray

    JSONArray::JSONArray() : value(std::vector<std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray, JSONObject>>{}) {}

    template <typename T>
    void JSONArray::emplace(T val) {
        value.emplace_back(val);
    }

    std::string JSONArray::toString() const {
        std::string res = "[";

        auto appendToResultLambda = [&](auto&& arg) {
            res += arg.toString() + ",";
        };

        for(auto& elem : value) {
            std::visit(appendToResultLambda, elem);
        }

        if (res.length() > 1) {
            res.back() = ']';
        }
        else {
            res += "]";
        }
        return res;
    }

    // JSONObject

    JSONObject::JSONObject() : value(std::map<JSONString, std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray, JSONObject>>{}) {}
    JSONObject::JSONObject(JSONString val) : value(val) {}
    JSONObject::JSONObject(JSONNumber val) : value(val) {}
    JSONObject::JSONObject(JSONBool val) : value(val) {}
    JSONObject::JSONObject(JSONNull val) : value(val) {}
    JSONObject::JSONObject(JSONArray val) : value(val) {}
    JSONObject::JSONObject(std::map<JSONString, std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray, JSONObject>> val) : value(val) {}

    void JSONObject::removeField(JSONString key) {
        if (std::holds_alternative<std::map<JSONString, std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray, JSONObject>>>(value)) {
            auto& map = std::get<std::map<JSONString, std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray, JSONObject>>>(value);

            auto it = map.find(key);
            if (it != std::end(map)) {
                map.erase(it);
            }
        }
        else {
            throw JSONException("Removing field failed, this JSONObject is not a map");
        }
    }

    std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray, JSONObject>& JSONObject::operator[](JSONString key) {
        if (std::holds_alternative<std::map<JSONString, std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray, JSONObject>>>(value)) {
           auto& map = std::get<std::map<JSONString, std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray, JSONObject>>>(value);

            return map[key];
        }
        else {
            throw JSONException("Operator[] failed, this JSONObject is not a map");
        }
    }

    const std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray, JSONObject>& JSONObject::operator[](JSONString key) const {
        if (std::holds_alternative<std::map<JSONString, std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray, JSONObject>>>(value)) {
            auto& map = std::get<std::map<JSONString, std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray, JSONObject>>>(value);

            return map.at(key);
        }
        else {
            throw JSONException("Operator[] failed, this JSONObject is not a map");
        }
    }   
    
    std::string JSONObject::toString() const {
        if (std::holds_alternative<JSONString>(value)) {
            return std::get<JSONString>(value).toString();
        }
        else if (std::holds_alternative<JSONNumber>(value)) {
            return std::get<JSONNumber>(value).toString();
        }
        else if (std::holds_alternative<JSONBool>(value)) {
            return std::get<JSONBool>(value).toString();
        }
        else if (std::holds_alternative<JSONNull>(value)) {
            return std::get<JSONNull>(value).toString();
        }
        else if (std::holds_alternative<JSONArray>(value)) {
            return std::get<JSONArray>(value).toString();
        }
        else if (std::holds_alternative<std::map<JSONString, std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray, JSONObject>>>(value)) {
            auto& map = std::get<std::map<JSONString, std::variant<JSONString, JSONNumber, JSONBool, JSONNull, JSONArray, JSONObject>>>(value);
            
            std::string res = "{";

            for (auto& [key, val] : map) {
                // lambda cannot capture key from structured binding so a copy must be made
                auto keyCpy = key;

                auto appendToResultLambda = [&](auto&& arg) {
                    res += keyCpy.toString() + ":" + arg.toString() + ",";
                };

                std::visit(appendToResultLambda, val);
            }

            if (res.length() > 1) {
                res.back() = '}';
            }
            else {
                res += "}";
            }
            
            return res;
        }
        
        // should not get here
        throw JSONException("Error when converting JSONObject to string");
    }

} // namespace simpleJSON 

namespace internal {
    simpleJSON::JSONObject beginParseFromStream__internal(std::istream& stream) {
        simpleJSON::JSONObject result;

        char next = stream.peek();

        if (isspace(next)) {
            stream.get();
            next = stream.peek();
        }

        internal::NextJsonType nextType = internal::detectNextType(next);

        switch (nextType) {
            case internal::NextJsonType::JSON_STRING:
                result = internal::parseString__internal(stream);
                break;
            case internal::NextJsonType::JSON_NUMBER:
                result = internal::parseNumber__internal(stream);
                break;
            case internal::NextJsonType::JSON_BOOL:
                result = internal::parseBool__internal(stream);
                break;
            case internal::NextJsonType::JSON_NULL:
                result = internal::parseNull__internal(stream);
                break;
            case internal::NextJsonType::JSON_ARRAY:
                result = internal::parseArray__internal(stream);
                break;
            case internal::NextJsonType::JSON_OBJECT:
                result = internal::parseObject__internal(stream);
                break;
            case internal::NextJsonType::JSON_END_OF_STREAM:    
                // next read will fail and function will end
                break;
            default: 
                std::string errorMessage = "Error while parsing object, unexpected next character '";
                errorMessage += next;
                errorMessage += '\''; 
                throw simpleJSON::JSONException(errorMessage.c_str());
                break;
        }
   
        next = stream.peek();

        if (isspace(next)) {
            stream.get();
            next = stream.peek();
        }

        if (next != std::istream::traits_type::eof()) {
            std::string errorMessage = "Error after reading a valid json object. Expected EOF but found '";
            errorMessage += next;
            errorMessage += '\''; 
            throw simpleJSON::JSONException(errorMessage.c_str());
        }
        
        return result;
    }


    NextJsonType detectNextType(char nextCharInStream) {
        switch (nextCharInStream) {
            case '"':
                return NextJsonType::JSON_STRING;
            case '-': [[fallthrough]];
            case '0': [[fallthrough]];
            case '1': [[fallthrough]];
            case '2': [[fallthrough]];
            case '3': [[fallthrough]];
            case '4': [[fallthrough]];
            case '5': [[fallthrough]];
            case '6': [[fallthrough]];
            case '7': [[fallthrough]];
            case '8': [[fallthrough]];
            case '9':
                return NextJsonType::JSON_NUMBER;
            case 't': [[fallthrough]];
            case 'f':
                return NextJsonType::JSON_BOOL;
            case 'n':
                return NextJsonType::JSON_NULL;
            case '[':
                return NextJsonType::JSON_ARRAY;
            case '{':
                return NextJsonType::JSON_OBJECT;
            case std::istream::traits_type::eof():
                return NextJsonType::JSON_END_OF_STREAM;
            default:
                return NextJsonType::JSON_ERROR;
        }
    }

    simpleJSON::JSONString parseString__internal(std::istream& stream) {
        char currentChar;
        stream.get(currentChar);

        if (currentChar != '"') {
            std::string errorMessage = "Error while parsing string, expected '\"'";
                throw simpleJSON::JSONException(errorMessage.c_str());
        }

        std::string result;
        bool currentCharIsEscaped = false;

        while (stream) {
            stream.get(currentChar);

            if (currentChar == '"' && !currentCharIsEscaped) {
                return result;
            }
            
            if (currentChar == '\\' && !currentCharIsEscaped) {
                currentCharIsEscaped = true;
            }
            else if (currentCharIsEscaped) {
                currentCharIsEscaped = false;
            }
            
            result += currentChar;
        }

        // should not get here
        std::string errorMessage = "Error while parsing string, unexpected end of stream";
        throw simpleJSON::JSONException(errorMessage.c_str());
    }

    
    simpleJSON::JSONNumber parseNumber__internal(std::istream& stream) {
        char c = stream.peek();
        std::string numberAsString;

        while (isdigit(c) || c == '.' || c == '-' || c == 'e' || c == 'E') {
            stream.get(c);
            numberAsString += c;

            c = stream.peek();
        }

        short dotCount = 0;
        short eCount = 0;
        short minusCount = 0;

        for (char c : numberAsString) {
            switch (c) {
                case '-':
                    ++minusCount;
                    break;
                case 'e': [[fallthrough]];
                case 'E':
                    ++eCount;
                    break;
                case '.':
                    ++dotCount;
                    break;
                default:
                    break;
            }
        }
        
        char* afterEnd;

        // flating type
        if (dotCount != 0 || eCount != 0) {
            simpleJSON::JSONFloating result = std::strtod(numberAsString.c_str(), &afterEnd);

            if (*afterEnd != '\0') {
                std::string errorMessage = "Error while parsing number, invalid floating point number";
                throw simpleJSON::JSONException(errorMessage.c_str());
            }

            return simpleJSON::JSONNumber(result);
        }

        // signed integer type
        if (numberAsString[0] == '-') {
            simpleJSON::JSONSignedDecimal result = std::strtoll(numberAsString.c_str(), &afterEnd, 10);
            
            if (*afterEnd != '\0') {
                std::string errorMessage = "Error while parsing number, invalid signed integer";
                throw simpleJSON::JSONException(errorMessage.c_str());
            }

            return simpleJSON::JSONNumber(result);
        }
        // unsigned integer type
        else {
            simpleJSON::JSONUnsignedDecimal result = std::strtoull(numberAsString.c_str(), &afterEnd, 10);
            
            if (*afterEnd != '\0') {
                std::string errorMessage = "Error while parsing number, invalid unsigned integer";
                throw simpleJSON::JSONException(errorMessage.c_str());
            }

            return simpleJSON::JSONNumber(result);
        }

        return simpleJSON::JSONNumber(simpleJSON::JSONUnsignedDecimal(1));
    }
    
    simpleJSON::JSONBool parseBool__internal(std::istream& stream) {
        char c1, c2, c3, c4;
        stream.get(c1);
        stream.get(c2);
        stream.get(c3);
        stream.get(c4);

        std::string out = {c1, c2, c3, c4};

        if (out == "true") {
            return simpleJSON::JSONBool(true);
        }
        else if (out[0] == 'f') {
            char c5;
            stream.get(c5);

            out += c5;

            if (out == "false") {
                return simpleJSON::JSONBool(false);
            } 
        }

        std::string errorMessage = "Error while parsing bool, expected \"true\" or \"false\", got \"";
        errorMessage += out;
        errorMessage += "\"";
        throw simpleJSON::JSONException(errorMessage.c_str());
    }
    
    simpleJSON::JSONNull parseNull__internal(std::istream& stream) {
        char c1, c2, c3, c4;
        stream.get(c1);
        stream.get(c2);
        stream.get(c3);
        stream.get(c4);

        std::string out = {c1, c2, c3, c4};

        if (out != "null") {
            std::string errorMessage = "Error while parsing null, expected \"null\", got \"";
            errorMessage += out; 
            errorMessage += "\"";
            throw simpleJSON::JSONException(errorMessage.c_str());
        }

        return simpleJSON::JSONNull{};
    }

    simpleJSON::JSONArray parseArray__internal(std::istream& stream) {
        char c;
        stream.get(c);

        if (c != '[') {
            std::string errorMessage = "Error while parsing array, expected '[', got '";
            errorMessage += c;
            errorMessage += "'";
            throw simpleJSON::JSONException(errorMessage.c_str());
        }

        simpleJSON::JSONArray result;

        while (stream) {
            c = stream.peek();

            if (isspace(c) || c == ',') {
                stream.get(c);
                continue;
            }

            if (c == ']') {
                stream.get(c);
                return result;
            }

            NextJsonType nextType = detectNextType(c);
            
            switch (nextType) {
                case NextJsonType::JSON_STRING: 
                    result.emplace(parseString__internal(stream));
                    break;
                case NextJsonType::JSON_NUMBER:
                    result.emplace(parseNumber__internal(stream));
                    break;
                case NextJsonType::JSON_BOOL:
                    result.emplace(parseBool__internal(stream));
                    break;
                case NextJsonType::JSON_NULL:
                    result.emplace(parseNull__internal(stream));
                    break;
                case NextJsonType::JSON_ARRAY:
                    result.emplace(parseArray__internal(stream));
                    break;
                case NextJsonType::JSON_OBJECT:
                    result.emplace(parseObject__internal(stream));
                    break;
                case NextJsonType::JSON_END_OF_STREAM:
                    // next read will fail and function will end
                    break;
                default: 
                    std::string errorMessage = "Error while parsing array, unexpected next character '";
                    errorMessage += c;
                    errorMessage += '\''; 
                    throw simpleJSON::JSONException(errorMessage.c_str());
                    break;
            }
        }

        // should not get here
        std::string errorMessage = "Error while parsing array, unexpected end of stream";
        throw simpleJSON::JSONException(errorMessage.c_str());
    }

    simpleJSON::JSONObject parseObject__internal(std::istream& stream) {
        char c;
        stream.get(c);

        if (c != '{') {
            std::string errorMessage = "Error while parsing object, expected '{', got '";
            errorMessage += c;
            errorMessage += "'";
            throw simpleJSON::JSONException(errorMessage.c_str());
        }

        simpleJSON::JSONObject result;

        while (stream) {
            char next = stream.peek();

            while (isspace(next)) {
                stream.get();
                next = stream.peek();
            }

            // empty object
            if (next == '}') {
                stream.get();
                return result;
            }

            // must read string as map key if object is not empty
            if (next != '"') {
                std::string errorMessage = "Error while parsing object, expected '\"', got '";
                errorMessage += next;
                errorMessage += "'";
                throw simpleJSON::JSONException(errorMessage.c_str());
            }

            simpleJSON::JSONString key = parseString__internal(stream);

            // possible white space between map key and :
            next = stream.peek();
            while (isspace(next)) {
                stream.get();
                next = stream.peek();
            }

            stream.get(c);

            // must read separator
            if (c != ':') {
                std::string errorMessage = "Error while parsing object, expected ':', got '";
                errorMessage += c;
                errorMessage += "'";
                throw simpleJSON::JSONException(errorMessage.c_str());
            }

            // possible white space between : and map value
            next = stream.peek();
            while (isspace(next)) {
                stream.get();
                next = stream.peek();
            }

            NextJsonType nextType = detectNextType(next);
            
            switch (nextType) {
                case NextJsonType::JSON_STRING:
                    // auto val = parseString__internal(stream);
                    // result[key] = val;
                    result[key] = parseString__internal(stream);
                    break;
                case NextJsonType::JSON_NUMBER:
                    result[key] = parseNumber__internal(stream);
                    break;
                case NextJsonType::JSON_BOOL:
                    result[key] = parseBool__internal(stream);
                    break;
                case NextJsonType::JSON_NULL:
                    result[key] = parseNull__internal(stream);
                    break;
                case NextJsonType::JSON_ARRAY:
                    result[key] = parseArray__internal(stream);
                    break;
                case NextJsonType::JSON_OBJECT:
                    result[key] = parseObject__internal(stream);
                    break;
                case NextJsonType::JSON_END_OF_STREAM:
                    // next read will fail and function will end
                    break;
                default: 
                    std::string errorMessage = "Error while parsing object, unexpected next character '";
                    errorMessage += next;
                    errorMessage += '\''; 
                    throw simpleJSON::JSONException(errorMessage.c_str());
                    break;
            }

            //  possible white space between map value and , or }
            next = stream.peek();
            while (isspace(next)) {
                stream.get();
                next = stream.peek();
            }

            stream.get(c);
            
            if (c == ',') {
                continue;
            }
            else if (c == '}') {
                break;
            }
            else {
                std::string errorMessage = "Error while parsing object, unexpected next character '";
                errorMessage += c;
                errorMessage += "'"; 
                throw simpleJSON::JSONException(errorMessage.c_str());
            }
        }
        
        return result;
    }
} // namespace internal

#endif //__SIMPLE_JSON__