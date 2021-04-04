#pragma once
#include "Scope.h"
#include "Codes.h"
#include "IoModule.h"

class Semancer {
private:
    Scope* last_scope_;
    IoModule* io_module_;

public:
    AbstractType* boolean_type = nullptr;
    AbstractType* integer_type = nullptr;
    AbstractType* real_type = nullptr;
    AbstractType* char_type = nullptr;
    AbstractType* string_type = nullptr;
    AbstractType* nil_type = nullptr;
    AbstractType* abstract_type = new AbstractType();

    explicit Semancer(IoModule* io_module) {
        this->io_module_ = io_module;
        last_scope_ = new Scope();
    };

    ~Semancer() {
        close_scopes(last_scope_);
        delete abstract_type;
    };

    Scope* open_scope();
    static void close_scopes(Scope* scope);
    void init_global_scope();
    static Identifier* search_identifier(Scope* scope, string& ident_name);
    static AbstractType* search_type(Scope* scope, string& name);
    bool check_assignment_types(AbstractType* var_type, AbstractType* expr_type) const;
    AbstractType* check_relation_operation(AbstractType* fst_type, AbstractType* snd_type) const;
    void check_right_sign(AbstractType* type) const;
    AbstractType* check_additive(AbstractType* fst_type, AbstractType* snd_type, TokenCodes operation, int len);
    AbstractType* check_multiplicative(AbstractType* fst_type, AbstractType* snd_type, TokenCodes operation, int len);
    AbstractType* check_boolean(AbstractType* type) const;

    Scope *get_last_scope() const;
};