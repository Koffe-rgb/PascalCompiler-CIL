#pragma once
#include <string>
#include <vector>

using namespace std;

enum Types {
    UNKNOWN_TYPE,
    SCALAR_TYPE = 401,
    LIMITED_TYPE,
    ENUM_TYPE,
    ARRAY_TYPE,
    REFERENCE_TYPE,
    SET_TYPE,
    FILE_TYPE,
    RECORD_TYPE
};

class AbstractType {
protected:
    Types type_code_ = UNKNOWN_TYPE;
public:
    Types get_type_code() const {
        return type_code_;
    }
    virtual ~AbstractType() = default;
};

class ScalarType : public AbstractType {
public:
    ScalarType() {
        this->type_code_ = SCALAR_TYPE;
    }
};

class EnumType : public AbstractType {
private:
    vector<string> symbols_;
public:
    EnumType() {
        this->type_code_ = ENUM_TYPE;
    }
    ~EnumType() override {
        symbols_.clear();
    }
    void add_symbol(const string& symbol) {
        symbols_.push_back(symbol);
    }
};