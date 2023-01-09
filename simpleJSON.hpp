#ifndef __SIMPLE_JSON__
#define __SIMPLE_JSON__

#include <exception>
#include <fstream>
#include <map>
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

namespace simpleJSON {
    // JSONexception

    JSONException::JSONException(const char* msg) : message(msg) {};

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

#endif //__SIMPLE_JSON__