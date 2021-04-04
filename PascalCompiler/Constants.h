#pragma once
#include <string>
#include <utility>

using namespace std;

class AbstractConstant {
public:
    enum class ConstTypes {
        INT,
        FLOAT,
        CHAR,
        STRING
    };

protected:
    ConstTypes type_;

public:
    explicit AbstractConstant(const ConstTypes type) { this -> type_ = type; }
    ConstTypes get_type() const { return this -> type_; }
    virtual ~AbstractConstant() = default;;
    virtual string to_string() = 0;
};

class IntConstant : public AbstractConstant {
private:
    int value_;

public:
    explicit IntConstant(const int value) : AbstractConstant(ConstTypes::INT) {
        this -> value_ = value;
    }
    int getValue() const { return this -> value_; }
    ~IntConstant() override = default;;
    string to_string() override {
        return std::to_string(value_);
    }
};

class FloatConstant : public AbstractConstant {
private:
    float value_;

public:
    explicit FloatConstant(const float value) : AbstractConstant(ConstTypes::FLOAT) {
        this -> value_ = value;
    }
    float get_value() const { return this -> value_; }
    ~FloatConstant() override = default;;
    string to_string() override {
        return std::to_string(value_);
    }
};

class CharConstant : public AbstractConstant {
private:
    char value_;

public:
    explicit CharConstant(const char value) : AbstractConstant(ConstTypes::CHAR) {
        this -> value_ = value;
    }
    char get_value() const { return this -> value_; }
    ~CharConstant() override = default;;
    string to_string() override {
        return string(1, value_);
    }
};

class StringConstant : public AbstractConstant {
private:
    string value_;

public:
    explicit StringConstant(string value) : AbstractConstant(ConstTypes::STRING) {
        this -> value_ = std::move(value);
    }
    string get_value() const { return this -> value_; }
    ~StringConstant() override = default;;
    string to_string() override {
        return value_;
    }
};