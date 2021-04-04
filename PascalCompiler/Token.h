#pragma once
#include <string>
#include <utility>
#include "Codes.h"
#include "Constants.h"

using namespace std;

class AbstractToken {
public:
    enum class TokenTypes {
        IDENTIFIER,
        CONSTANT,
        KEYWORD
    };

protected:
    TokenTypes type_;
    TokenCodes code_;

public:
    AbstractToken(const TokenTypes type, const TokenCodes code) {
        this -> type_ = type;
        this -> code_ = code;
    }
    TokenTypes getType() const { return this->type_; }
    TokenCodes getCode() const { return this->code_; }

    virtual string to_string() {
        return toStringMap.at(code_);
    }

    virtual ~AbstractToken() = default;;
};


class IdentifierToken : public AbstractToken {
private:
    string ident_name_;

public:
    explicit IdentifierToken(string ident_name) : AbstractToken(TokenTypes::IDENTIFIER, ident)
    {
        this -> ident_name_ = std::move(ident_name);
    }

    string get_ident_name() const { return this -> ident_name_; }

    string to_string() override {
        return ident_name_;
    }

    ~IdentifierToken() override = default;;
};


class ConstantToken : public AbstractToken {
private:
    AbstractConstant* constant_;

public:
    ConstantToken(AbstractConstant* constant, const TokenCodes code) : AbstractToken(TokenTypes::CONSTANT, code)
    {
        this -> constant_ = constant;
    }

    AbstractConstant* get_constant() const
    {
        return this -> constant_;
    }

    string to_string() override {
        return constant_->to_string();
    }

    ~ConstantToken() override {
        delete constant_;
    }
};


class KeywordToken : public AbstractToken {

public:
    explicit KeywordToken(const TokenCodes token_code) : AbstractToken(TokenTypes::KEYWORD, token_code)
    {
    }

    ~KeywordToken() override = default;
};