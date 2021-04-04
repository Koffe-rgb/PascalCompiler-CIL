#include "Scope.h"

AbstractType *Scope::add_type(const Types type) {
    AbstractType* new_type = nullptr;
    switch (type) {
        case UNKNOWN_TYPE: break;
        case SCALAR_TYPE:
            new_type = new ScalarType();
            types_.push_back(new_type);
            break;
        case ENUM_TYPE:
            new_type = new EnumType();
            types_.push_back(new_type);
            break;
        default: break;
    }
    return new_type;
}

bool Scope::add_identifier(Identifier* identifier) {
	auto name = identifier->get_ident_name();
    const auto contains = identifiers_.count(identifier->get_ident_name()) == 1;

    if (!contains) {
        identifiers_[identifier->get_ident_name()] = identifier;
        return true;
    }
    return false;
}

Identifier *Scope::retrieve_identifier(string& ident_name) {
	const auto contains = identifiers_.count(ident_name) == 1;
    if (contains) {
        return identifiers_[ident_name];
    }
    return nullptr;
}

const map<string, Identifier *> &Scope::get_identifiers() const {
    return identifiers_;
}

const vector<AbstractType *> &Scope::get_types() const {
    return types_;
}

Scope *Scope::get_outer_scope() const {
    return outer_scope_;
}

Scope::~Scope() {
    for (const auto& ident : identifiers_) {
        delete ident.second;
    }
    identifiers_.clear();

    for (auto & type : types_) {
        delete type;
        type = nullptr;
    }
    types_.clear();

    delete outer_scope_;
}


