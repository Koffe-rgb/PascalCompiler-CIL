#pragma once
#include <set>
#include <gcroot.h>

#include "Lexer.h"
#include "Semancer.h"
#include "Translator.h"


class Syntaxer {
private:
    Lexer* lexer_;
    IoModule* io_module_;
    Semancer* semancer_;
	
    gcroot<Translator^> translator_;
	
    AbstractToken* cur_token_;
    AbstractType* last_case_type_;

    bool is_last_const_ = false;
    bool need_real_cast_ = false;
    bool was_first_ = false;

    string last_name_ = "";
public:
    explicit Syntaxer(string& path) {
        lexer_ = new Lexer(path);
        io_module_ = lexer_->get_io_module();
        semancer_ = new Semancer(io_module_);

        translator_ = gcnew Translator();

        cur_token_ = nullptr;
        last_case_type_ = nullptr;
    };
    ~Syntaxer() {
        delete lexer_;
        delete semancer_;
        delete translator_;
    };

    void start() {
        program();
    };

    bool has_errors() const
    {
        return !io_module_->getErrCodesAndPos().empty();
    }

    void print_errors() const
    {
        for (auto err : io_module_->getErrCodesAndPos()) {
	        const auto pos = err.second;
            const auto& a = ErrorTable.at(err.first);
            printf("Ошибка (%d, %d): %s\n", pos.second, pos.first, a.c_str());
        }
        printf("В процессе анализа найдено ошибок: %d\n\n", io_module_->getErrCodesAndPos().size());
    }
private:
    void list_error(int code) const;

    void scan_next_token();

    void accept(TokenCodes token_code);
    bool is_symbol_belong_to(const set<TokenCodes>& symbols) const;
    static set<TokenCodes> union_of(const set<TokenCodes>& first, const set<TokenCodes>& second);

    void skip_to(const set<TokenCodes>& symbols, const set<TokenCodes>& followers);

    // Грамматики
    void program();
    void block(const set<TokenCodes>& followers);
    void const_part(const set<TokenCodes>& followers);
    void const_declaration(const set<TokenCodes>& followers);
    AbstractType* constant(const set<TokenCodes>& followers);
    void type_part(const set<TokenCodes>& followers);
    void type_declaration(const set<TokenCodes>& followers);
    AbstractType* type(const set<TokenCodes>& followers);
    AbstractType* simple_type(const set<TokenCodes>& followers);
    AbstractType* reference_type(const set<TokenCodes>& followers);
    void var_part(const set<TokenCodes>& followers);
    void var_declaration(const set<TokenCodes>& followers);
    void operator_part(const set<TokenCodes>& followers);
    void compound_operator(const set<TokenCodes>& followers);
    void write_line(const set<TokenCodes>& followers);
    void read_line(const set<TokenCodes>& followers);
    void oper(const set<TokenCodes>& followers);
    void assignment_operator(const set<TokenCodes>& followers);
    AbstractType* variable(const set<TokenCodes>& followers);
    AbstractType* expression(const set<TokenCodes>& followers);
    AbstractType* simple_expression(const set<TokenCodes>& followers);
    AbstractType* term(const set<TokenCodes>& followers);
    AbstractType* factor(const set<TokenCodes>& followers);
    void if_operator(const set<TokenCodes>& followers);
    void while_operator(const set<TokenCodes>& followers);
    void case_operator(const set<TokenCodes>& followers);
    void element_of_variants(const set<TokenCodes>& followers);
    void list_of_marks(const set<TokenCodes>& followers);
};