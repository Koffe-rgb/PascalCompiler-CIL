#pragma once
#include <map>
#include <vector>
#include <string>

#include "Identifier.h"
#include "Type.h"



using namespace std;

class Scope {
private:
    map<string, Identifier*> identifiers_;
    vector<AbstractType*> types_;
    Scope* outer_scope_;

public:
    explicit Scope(Scope* outerScope = nullptr) {
        this->outer_scope_ = outerScope;
    };
    ~Scope();

    AbstractType* add_type(Types type);
    bool add_identifier(Identifier* identifier);
    Identifier* retrieve_identifier(string& ident_name);

    const map<string, Identifier *> &get_identifiers() const;

    const vector<AbstractType *> &get_types() const;

    Scope *get_outer_scope() const;
};