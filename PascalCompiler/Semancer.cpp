#include "Semancer.h"

Scope *Semancer::open_scope() {
	const auto scope = new Scope(last_scope_);
    last_scope_ = scope;
    return scope;
}

void Semancer::init_global_scope() {
    this->boolean_type = last_scope_->add_type(ENUM_TYPE);
    string boolean_name = "boolean";
    const auto boolean_ident = new Identifier(boolean_name, TYPE_CLASS, boolean_type);
    last_scope_->add_identifier(boolean_ident);

    string false_name = "false";
    const auto false_ident = new Identifier(false_name, CONST_CLASS, boolean_type);
    last_scope_->add_identifier(false_ident);
    static_cast<EnumType*>(boolean_type)->add_symbol(false_name);

    string true_name = "true";
    const auto true_ident = new Identifier(true_name, CONST_CLASS, boolean_type);
    last_scope_->add_identifier(true_ident);
    static_cast<EnumType*>(boolean_type)->add_symbol(true_name);


    this->integer_type = last_scope_->add_type(SCALAR_TYPE);
    string integer_name = "integer";
    const auto integer_ident = new Identifier(integer_name, TYPE_CLASS, integer_type);
    last_scope_->add_identifier(integer_ident);

    this->real_type = last_scope_->add_type(SCALAR_TYPE);
    string real_name = "real";
    const auto read_ident = new Identifier(real_name, TYPE_CLASS, real_type);
    last_scope_->add_identifier(read_ident);

    this->char_type = last_scope_->add_type(SCALAR_TYPE);
    string char_name = "char";
    const auto char_ident = new Identifier(char_name, TYPE_CLASS, char_type);
    last_scope_->add_identifier(char_ident);

    this->string_type = last_scope_->add_type(SCALAR_TYPE);
    string string_name = "string";
    const auto string_ident = new Identifier(string_name, TYPE_CLASS, string_type);
    last_scope_->add_identifier(string_ident);

    this->nil_type = last_scope_->add_type(SCALAR_TYPE);
    string nil_name = "nil";
    const auto nil_ident = new Identifier(nil_name, TYPE_CLASS, nil_type);
    last_scope_->add_identifier(nil_ident);
}


void Semancer::close_scopes(Scope* scope) {
//    if (scope->getOuterScope() != nullptr)
//        closeScopes(scope->getOuterScope());

    delete scope;
}

Identifier *Semancer::search_identifier(Scope *scope, string &ident_name) {
	const auto ident = scope->retrieve_identifier(ident_name);
    if (ident != nullptr)
        return ident;
    if (scope->get_outer_scope() != nullptr)
        return search_identifier(scope->get_outer_scope(), ident_name);
    return nullptr;
}

AbstractType *Semancer::search_type(Scope *scope, string &name) {
	const auto ident = scope->retrieve_identifier(name);

    if (ident != nullptr && (ident->get_ident_class() == TYPE_CLASS ||
        ident->get_ident_class() == VAR_CLASS))
    {
        return ident->get_type();
    }
    if (scope->get_outer_scope() != nullptr)
        return search_type(scope->get_outer_scope(), name);

    return nullptr;
}

bool Semancer::check_assignment_types(AbstractType *var_type, AbstractType *expr_type) const {
    if (var_type == nullptr || expr_type == nullptr)
        return false;
    if (var_type == abstract_type) {
        //ioModule->logError(328);
        return true;
    }
    if (var_type == real_type && (expr_type == real_type || expr_type == integer_type))
        return true;
    if (var_type == string_type && (expr_type == string_type || expr_type == char_type))
        return true;
    if (var_type == expr_type)
        return true;
    return false;
}

AbstractType* Semancer::check_relation_operation(AbstractType *fst_type, AbstractType *snd_type) const {
    if (fst_type == integer_type && snd_type == real_type
        || fst_type == real_type && snd_type == integer_type
        || fst_type == snd_type)
    {
        return boolean_type;
    }
    return nullptr;
}

void Semancer::check_right_sign(AbstractType *type) const
{
    if (type == nullptr || type == integer_type || type == real_type)
        return;
    io_module_->log_error(211);
}

AbstractType *Semancer::check_additive(AbstractType *fst_type, AbstractType *snd_type, const TokenCodes operation, const int len) {
    if (operation == TokenCodes::plus || operation == TokenCodes::minus) {
        if ((fst_type == integer_type || fst_type == real_type)
            && (snd_type == integer_type || snd_type == real_type))
        {
            return fst_type == real_type
                    ? real_type
                    : snd_type == real_type
                        ? real_type
                        : integer_type;
        }
        io_module_->log_error(211, len);
    }
    if (operation == orsy) {
        if (fst_type == boolean_type && snd_type == boolean_type)
            return boolean_type;
        io_module_->log_error(210, len);
    }
    return nullptr;
}

AbstractType *Semancer::check_multiplicative(AbstractType *fst_type, AbstractType *snd_type, const TokenCodes operation, const int len) {
    if (operation == divsy || operation == modsy) {
        if (fst_type == integer_type && snd_type == integer_type)
            return integer_type;
        io_module_->log_error(212);
    }
    if (operation == slash || operation == star) {
        if ((fst_type == integer_type || fst_type == real_type)
            && (snd_type == integer_type || snd_type == real_type))
        {
            return fst_type == real_type
                    ? real_type
                    : snd_type == real_type
                        ? real_type
                        : integer_type;
        }
        if (operation == slash) io_module_->log_error(214);
        if (operation == star) io_module_->log_error(213);
    }
    if (operation == andsy) {
        if (fst_type == boolean_type && snd_type == boolean_type)
            return boolean_type;
        io_module_->log_error(210, len);
    }
    return nullptr;
}

AbstractType *Semancer::check_boolean(AbstractType *type) const {
    return type == boolean_type ? boolean_type : nullptr;
}

Scope *Semancer::get_last_scope() const {
    return last_scope_;
}