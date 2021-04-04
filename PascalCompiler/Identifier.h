#pragma once
#include <string>

#include "Type.h"

using namespace std;

enum IdentClass {
    PROG_CLASS = 300,
    TYPE_CLASS,
    CONST_CLASS,
    VAR_CLASS,
    PROC_CLASS,
    FUNC_CLASS
};


class Identifier {
private:
    string ident_name_;
    IdentClass ident_class_;
    AbstractType* type_;
    bool is_const_;

public:
    Identifier(string& ident_name, const IdentClass ident_class, AbstractType* type) {
        this->ident_class_ = ident_class;
        this->ident_name_ = ident_name;
        this->type_ = type;
        this->is_const_ = false;
    };

    Identifier(string& ident_name, const IdentClass ident_class, AbstractType* type, const bool is_const) {
        this->ident_class_ = ident_class;
        this->ident_name_ = ident_name;
        this->type_ = type;
        this->is_const_ = is_const;
    };

    virtual ~Identifier() = default;


    const string &get_ident_name() const {
        return ident_name_;
    }

    IdentClass get_ident_class() const {
        return ident_class_;
    }

    AbstractType *get_type() const {
        return type_;
    }

    bool is_constant() const {
        return is_const_;
    }

    void set_type(AbstractType *type) {
        Identifier::type_ = type;
    }

    void set_is_const(bool isConst) {
        is_const_ = isConst;
    }
};