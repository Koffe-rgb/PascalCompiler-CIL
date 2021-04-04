#include "Syntaxer.h"
#include "Sets.h"

using namespace std;

void Syntaxer::list_error(const int code) const {
    translator_->IsWorking = false;
    io_module_->log_error(code, cur_token_->to_string().size());
}

void Syntaxer::scan_next_token() {
    delete cur_token_;
    cur_token_ = lexer_->scan_next_token();
}

void Syntaxer::accept(const TokenCodes token_code) {
    if (cur_token_->getCode() == token_code) {
        scan_next_token();
    } else {
        translator_->IsWorking = false;
        io_module_->log_error(token_code, cur_token_->to_string().size());
    }
}

bool Syntaxer::is_symbol_belong_to(const set<TokenCodes>& symbols) const {
    return symbols.count(cur_token_->getCode()) > 0;
}

set<TokenCodes> Syntaxer::union_of(const set<TokenCodes>& first, const set<TokenCodes>& second) {
    set<TokenCodes> res = first;
    res.insert(second.begin(), second.end());
    return res;
}

void Syntaxer::skip_to(const set<TokenCodes> &symbols, const set<TokenCodes> &followers) {
	auto contains1 = symbols.count(cur_token_->getCode()) > 0;
	auto contains2 = followers.count(cur_token_->getCode()) > 0;
	
    while(!contains1 && !contains2) {
        scan_next_token();
        contains1 = symbols.count(cur_token_->getCode()) > 0;
        contains2 = followers.count(cur_token_->getCode()) > 0;
    }
}

void Syntaxer::program() {
    semancer_->init_global_scope();
    semancer_->open_scope();

    scan_next_token();
    accept(programsy);
    accept(ident);
    accept(semicolon);
    block(follow_block);
    accept(point);

    translator_->save();
}

void Syntaxer::block(const set<TokenCodes> &followers) {
    if (!is_symbol_belong_to(start_block)) {
        list_error(18);
        skip_to(start_block, followers);
    }
    if (is_symbol_belong_to(start_block)) {
        auto symbols = union_of(follow_constPart, followers);
        const_part(symbols);
        symbols = union_of(follow_typePart, followers);
        type_part(symbols);
        symbols = union_of(follow_varPart, followers);
        var_part(symbols);
        operator_part(followers);

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
}

void Syntaxer::const_part(const set<TokenCodes> &followers) {
    if (!is_symbol_belong_to(start_constPart)) {
        list_error(18);
        skip_to(start_constPart, followers);
    }
    if (cur_token_->getCode() == constsy) {
        accept(constsy);
        const auto symbols = union_of(follow_constDeclaration, followers);
        do {
            const_declaration(symbols);
            accept(semicolon);
        } while (cur_token_->getCode() == ident);

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
}

void Syntaxer::const_declaration(const set<TokenCodes> &followers) {
    if (!is_symbol_belong_to(start_constDeclaration)) {
        list_error(18);
        skip_to(start_constDeclaration, followers);
    }
    if (is_symbol_belong_to(start_constDeclaration)) {
	    auto ident_name = cur_token_->to_string();
        auto identifier = new Identifier(ident_name, CONST_CLASS, nullptr, true);

        accept(ident);
        accept(TokenCodes::equal);
        const auto type = constant(followers);
        identifier->set_type(type);

    	
        if (semancer_->get_last_scope()->get_identifiers().count(ident_name) > 0)
            list_error(101);
        else
            semancer_->get_last_scope()->add_identifier(identifier);

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
}

AbstractType* Syntaxer::constant(const set<TokenCodes> &followers) {
    AbstractType* type = nullptr;
    Scope* scope;
    Identifier* identifier;
    string name;

    if (!is_symbol_belong_to(start_constant)) {
        list_error(18);
        skip_to(start_constant, followers);
    }
    if (is_symbol_belong_to(start_constant)) {
        switch (cur_token_->getCode()) {
            case intconst:
                type = semancer_->integer_type;

                accept(intconst);
                break;
            case floatconst:
                type = semancer_->real_type;

                accept(floatconst);
                break;
            case stringconst:
                type = semancer_->string_type;

                accept(stringconst);
                break;
            case charconst:
                type = semancer_->char_type;

                accept(charconst);
                break;
            case ident:
                scope = semancer_->get_last_scope();
                name = cur_token_->to_string();
                identifier = semancer_->search_identifier(scope, name);

                if (identifier == nullptr) {
                    list_error(104);
                    identifier = new Identifier(name, VAR_CLASS, semancer_->abstract_type);
                    semancer_->get_last_scope()->add_identifier(identifier);
                }
                else {
                    type = identifier->get_type();
                }

                accept(ident);
                break;
            default:
                if (cur_token_->getCode() == TokenCodes::plus ||
                    cur_token_->getCode() == TokenCodes::minus)
                {
                    scan_next_token();
                    const auto code = cur_token_->getCode();
                    switch (code) {
                        case intconst:
                            type = semancer_->integer_type;
                            break;
                        case floatconst:
                            type = semancer_->real_type;
                            break;
                        case ident:
                            scope = semancer_->get_last_scope();
                            name = cur_token_->to_string();
                            identifier = semancer_->search_identifier(scope, name);

                            if (identifier == nullptr) {
                                list_error(104);
                                identifier = new Identifier(name, VAR_CLASS, semancer_->abstract_type);
                                semancer_->get_last_scope()->add_identifier(identifier);
                            }
                            else {
                                type = identifier->get_type();
                            }
                            break;
                    }
                    scan_next_token();
                }
                break;
        }
        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
    return type;
}

void Syntaxer::type_part(const set<TokenCodes> &followers) {
    if (!is_symbol_belong_to(start_typePart)) {
        list_error(18);
        skip_to(start_typePart, followers);
    }
    if (cur_token_->getCode() == typesy) {
        accept(typesy);
        const auto symbols = union_of(follow_typeDeclaration, followers);

        do {
            type_declaration(symbols);
            accept(semicolon);
        } while (cur_token_->getCode() == ident);

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
}

void Syntaxer::type_declaration(const set<TokenCodes> &followers) {
    if (!is_symbol_belong_to(start_typeDeclaration)) {
        list_error(18);
        skip_to(start_typeDeclaration, followers);
    }
    if (is_symbol_belong_to(start_typeDeclaration)) {
	    auto ident_name = cur_token_->to_string();
        auto identifier = new Identifier(ident_name, TYPE_CLASS, nullptr);

        accept(ident);
        accept(TokenCodes::equal);
        const auto t = type(followers);

        identifier->set_type(t);
        semancer_->get_last_scope()->add_identifier(identifier);

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
}

AbstractType* Syntaxer::type(const set<TokenCodes> &followers) {
    AbstractType* t = nullptr;
    if (!is_symbol_belong_to(start_type)) {
        list_error(10);
        skip_to(start_type, followers);
    }
    if (is_symbol_belong_to(start_type)) {
        if (cur_token_->getCode() == arrow) {
            t = reference_type(followers);
        }
        else {
            t = simple_type(followers);
        }

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
    return t;
}

AbstractType* Syntaxer::simple_type(const set<TokenCodes> &followers) {
    AbstractType* type = nullptr;
    if (!is_symbol_belong_to(start_simpleType)) {
        list_error(10);
        skip_to(start_simpleType, followers);
    }
    if (is_symbol_belong_to(start_simpleType)) {
        if (cur_token_->getCode() == ident) {
            string name = cur_token_->to_string();
            type = semancer_->search_type(semancer_->get_last_scope(), name);
            if (type == nullptr) {
                auto identifier = semancer_->search_identifier(semancer_->get_last_scope(), name);
                if (identifier == nullptr) {
                    list_error(104);
                    identifier = new Identifier(name, VAR_CLASS, semancer_->abstract_type);
                    semancer_->get_last_scope()->add_identifier(identifier);

                    // TODO ?????????
                    accept(ident);
                }
            }

            accept(ident);
        }

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
    return type;
}

AbstractType* Syntaxer::reference_type(const set<TokenCodes> &followers) {
    // TODO REFERENCE TYPE
    AbstractType* type = nullptr;
    if (!is_symbol_belong_to(start_linkType)) {
        list_error(22);
        skip_to(start_linkType, followers);
    }
    if (is_symbol_belong_to(start_linkType)) {
        accept(arrow);
        accept(ident);

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
    return type;
}

void Syntaxer::var_part(const set<TokenCodes> &followers) {
    if (!is_symbol_belong_to(start_varPart)) {
        list_error(18);
        skip_to(start_varPart, followers);
    }
    if (is_symbol_belong_to(start_varPart)) {
        accept(varsy);
        const auto symbols = union_of(follow_varDeclaration, followers);
        do {
            var_declaration(symbols);
            accept(semicolon);
        } while (cur_token_->getCode() == ident);

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
}

void insert_var_to(vector<Identifier*>& variables, AbstractToken* token) {
	auto name = token->to_string();
    const auto identifier = new Identifier(name, VAR_CLASS, nullptr);
    variables.push_back(identifier);
}

void Syntaxer::var_declaration(const set<TokenCodes> &followers) {
    if (!is_symbol_belong_to(start_varDeclaration)) {
        list_error(2);
        skip_to(start_varDeclaration, followers);
    }
    if (is_symbol_belong_to(start_varDeclaration)) {
        vector<Identifier*> variables;
        insert_var_to(variables, cur_token_);

        accept(ident);

        while (cur_token_->getCode() == comma) {
            accept(comma);
            insert_var_to(variables, cur_token_);
            accept(ident);
        }
        accept(colon);
        const auto t = type(followers);

        for (auto identifier : variables) {
            identifier->set_type(t);
            auto ident_name = identifier->get_ident_name();

            if (t == semancer_->integer_type)
                translator_->add_local_var(ident_name, int::typeid);
            else if (t == semancer_->real_type)
                translator_->add_local_var(ident_name, double::typeid);
            else if (t == semancer_->char_type)
                translator_->add_local_var(ident_name, Char::typeid);
            else if (t == semancer_->string_type)
                translator_->add_local_var(ident_name, String::typeid);
            else if (t == semancer_->boolean_type)
                translator_->add_local_var(ident_name, bool::typeid);
        	
            if (semancer_->get_last_scope()->get_identifiers().count(identifier->get_ident_name()) > 0)
                list_error(101);
            else
                semancer_->get_last_scope()->add_identifier(identifier);
        }

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
}

void Syntaxer::operator_part(const set<TokenCodes> &followers) {
    compound_operator(followers);
}

void Syntaxer::compound_operator(const set<TokenCodes> &followers) {
    if(!is_symbol_belong_to(start_compoundOperator)) {
        list_error(22);
        skip_to(start_compoundOperator, followers);
    }
    if (is_symbol_belong_to(start_compoundOperator)) {
        accept(beginsy);
        const auto symbols = union_of(follow_operator, followers);
        while(is_symbol_belong_to(start_operator)) {
            oper(symbols);
            accept(semicolon);
        }
        accept(endsy);

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
}

void Syntaxer::write_line(const set<TokenCodes>& followers)
{
    AbstractType* type = nullptr;
    accept(ident);
    accept(leftpar);
    if (cur_token_->getCode() != rightpar) {
        const auto symbols = union_of(follow_write_line, followers);
        type = expression(symbols);
    }
    accept(rightpar);

    if (type == nullptr)
        translator_->write_line();
    if (type == semancer_->integer_type)
        translator_->write_line(int::typeid);
	if (type == semancer_->real_type)
        translator_->write_line(double::typeid);
    if (type == semancer_->char_type)
        translator_->write_line(Char::typeid);
    if (type == semancer_->string_type)
        translator_->write_line(String::typeid);
    if (type == semancer_->boolean_type)
        translator_->write_line(bool::typeid);
}

void Syntaxer::read_line(const set<TokenCodes>& followers)
{
    AbstractType* type = nullptr;
    accept(ident);
    accept(leftpar);
    if (cur_token_->getCode() != rightpar) {
        const auto symbols = union_of(follow_read_line, followers);
        type = variable(symbols);
    }
    accept(rightpar);

    if (type == nullptr)
        translator_->read_line();
    if (type == semancer_->integer_type)
        translator_->read_line(last_name_, int::typeid);
    if (type == semancer_->real_type)
        translator_->read_line(last_name_, double::typeid);
    if (type == semancer_->char_type)
        translator_->read_line(last_name_, Char::typeid);
    if (type == semancer_->string_type)
        translator_->read_line(last_name_, String::typeid);
    if (type == semancer_->boolean_type)
        translator_->read_line(last_name_, bool::typeid);
}

void Syntaxer::oper(const set<TokenCodes> &followers) {
    if (!is_symbol_belong_to(start_operator)) {
        list_error(22);
        skip_to(start_operator, followers);
    }
    if (is_symbol_belong_to(start_operator)) {
        switch (cur_token_->getCode()) {
            case ident:
                if (cur_token_->to_string() == "writeln") {
                    write_line(followers);
                }
                else if (cur_token_->to_string() == "readln")
                {
                    read_line(followers);
                }
                else
					assignment_operator(followers);	
        	break;

            case beginsy: compound_operator(followers); break;
        	
            case ifsy: 
                if_operator(followers);
        	break;
        	
            case casesy: case_operator(followers); break;
            case whilesy: while_operator(followers); break;

        	case semicolon:
            case endsy:
            case elsesy: break;
        }
    }
}

void Syntaxer::assignment_operator(const set<TokenCodes> &followers) {
    if (!is_symbol_belong_to(start_assignmentOperator)) {
        list_error(22);
        skip_to(start_assignmentOperator, followers);
    }
    if (is_symbol_belong_to(start_assignmentOperator)) {
	    auto var_name = cur_token_->to_string();
        const auto symbols = union_of(follow_assignmentOperator, followers);
        const auto var_type = variable(symbols);

        if (is_last_const_) {
            list_error(333);
            is_last_const_ = false;
        }

        accept(assign);
        const auto expr_type = expression(followers);

        if (!semancer_->check_assignment_types(var_type, expr_type))
            list_error(328);

        if (var_type == semancer_->real_type && expr_type == semancer_->integer_type)
            translator_->cast_to_real();
        translator_->assign(var_name);

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
}

AbstractType* Syntaxer::variable(const set<TokenCodes> &followers) {
    AbstractType* type = nullptr;
    if (!is_symbol_belong_to(start_variable)) {
        list_error(22);
        skip_to(start_variable, followers);
    }
    if (is_symbol_belong_to(start_variable)) {
	    auto name = cur_token_->to_string();
        last_name_ = name;
        auto identifier = semancer_->get_last_scope()->retrieve_identifier(name);
        if (identifier == nullptr) {
            list_error(104);
            identifier = new Identifier(name, VAR_CLASS, semancer_->abstract_type);
            semancer_->get_last_scope()->add_identifier(identifier);
        }
        else {
            type = identifier->get_type();
            is_last_const_ = identifier->get_ident_class() == CONST_CLASS;
        }

        accept(ident);

        if (cur_token_->getCode() == arrow) {
            accept(arrow);
        }

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
    return type;
}

AbstractType* Syntaxer::expression(const set<TokenCodes> &followers) {
    AbstractType* type = nullptr;
    if (!is_symbol_belong_to(start_expression)) {
        list_error(23);
        skip_to(start_expression, followers);
    }
    if (is_symbol_belong_to(start_expression)) {
	    const auto symbols = union_of(follow_simpleExpression, followers);
        type = simple_expression(symbols);

	    const auto operation_code = cur_token_->getCode();
        if (operation_code == TokenCodes::equal || operation_code == latergreater ||
            operation_code == later || operation_code == laterequal ||
            operation_code == greaterequal || operation_code == TokenCodes::greater)
        {
            scan_next_token();
            const auto snd_type = simple_expression(followers);
            type = semancer_->check_relation_operation(type, snd_type);
            if (type == nullptr)
                list_error(186);
            else
                translator_->operation(operation_code);
        }

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
    return type;
}

AbstractType* Syntaxer::simple_expression(const set<TokenCodes> &followers) {
    AbstractType* type = nullptr;
    if (!is_symbol_belong_to(start_simpleExpression)) {
        list_error(22);
        skip_to(start_simpleExpression, followers);
    }
    if (is_symbol_belong_to(start_simpleExpression)) {
	    auto first_term_has_sign = false;
        need_real_cast_ = false;
        was_first_ = false;

        if (cur_token_->getCode() == TokenCodes::plus || cur_token_->getCode() == TokenCodes::minus) {
            scan_next_token();
            first_term_has_sign = true;
        }

	    const auto symbols = union_of(start_additive, followers);
        type = term(symbols);

        if (first_term_has_sign)
            semancer_->check_right_sign(type);

        while (is_symbol_belong_to(start_additive)) {
	        const auto operation_code = cur_token_->getCode();
            scan_next_token();
	        const auto snd_type = term(symbols);
            type = semancer_->check_additive(type, snd_type, operation_code, cur_token_->to_string().size());

            if (type != nullptr)
                translator_->operation(operation_code);
        }

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
    return type;
}

AbstractType* Syntaxer::term(const set<TokenCodes> &followers) {
    AbstractType* type = nullptr;
    if (!is_symbol_belong_to(start_term)) {
        list_error(22);
        skip_to(start_term, followers);
    }
    if (is_symbol_belong_to(start_term)) {
	    const auto symbols = union_of(start_multiplicative, followers);
        type = factor(symbols);

        while (is_symbol_belong_to(start_multiplicative)) {
	        const auto operation_code = cur_token_->getCode();
            scan_next_token();
	        const auto snd_type = factor(symbols);
            type = semancer_->check_multiplicative(type, snd_type, operation_code, cur_token_->to_string().size());

            if (type != nullptr)
                translator_->operation(operation_code);
        }

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
    return type;
}

AbstractType* Syntaxer::factor(const set<TokenCodes> &followers) {
    AbstractType* type = nullptr;
    if (!is_symbol_belong_to(start_factor)) {
        list_error(22);
        skip_to(start_factor, followers);
    }
    if (is_symbol_belong_to(start_factor)) {
        switch (cur_token_->getCode()) {
            case leftpar: {
                accept(leftpar);
                const auto symbols = union_of(follow_factorExpression, followers);
                type = expression(symbols);
                accept(rightpar);
            } break;
            case notsy:
                accept(notsy);
                type = factor(followers);
                type = semancer_->check_boolean(type);
                if (type == nullptr)
                    list_error(210);
                else
                    translator_->operation(notsy);
                break;
            case intconst:
                type = semancer_->integer_type;
                translator_->load_const(stoi(cur_token_->to_string()));
                if (need_real_cast_)
                    translator_->cast_to_real();
                accept(intconst);
                break;
            case floatconst:
                type = semancer_->real_type;
                translator_->load_const(stod(cur_token_->to_string()));
                if (!need_real_cast_ && was_first_)
                    translator_->cast_to_real();
                need_real_cast_ = true;
                accept(floatconst);
                break;
            case charconst:
                type = semancer_->char_type;
                translator_->load_const(cur_token_->to_string()[0]);
                accept(charconst);
                break;
            case stringconst:
                type = semancer_->string_type;
                translator_->load_const(cur_token_->to_string());
                accept(stringconst);
                break;
            case nilsy:
                type = semancer_->nil_type;
                accept(nilsy);
                break;
            case ident:
	            auto name = cur_token_->to_string();
                auto identifier = semancer_->search_identifier(semancer_->get_last_scope(), name);
                if (identifier != nullptr) {
                    switch (identifier->get_ident_class()) {
                        case VAR_CLASS:
                            if (identifier->get_type() == semancer_->real_type)
                            {
                                if (!need_real_cast_ && was_first_)
                                    translator_->cast_to_real();
                                need_real_cast_ = true;
                            }
                            if (identifier->get_type() == semancer_->integer_type && need_real_cast_)
                                translator_->cast_to_real();
                            translator_->load_local_var(cur_token_->to_string());
                            type = variable(followers);
                            break;
                        case CONST_CLASS:
                            type = identifier->get_type();
                            if (type == semancer_->boolean_type)
                                translator_->load_const(cur_token_->to_string() == "true" ? 1 : 0);
                            accept(ident);
                            break;
//                        case TYPE_CLASS:
//                            // TODO ????
//                            t = type(followers);
//                            break;
                    }
                }
                else {
                    identifier = new Identifier(name, VAR_CLASS, nullptr);
                    if (semancer_->get_last_scope()->get_identifiers().count(name) > 0)
                        list_error(101);
                    else
                        semancer_->get_last_scope()->add_identifier(identifier);
                    list_error(104);
                    accept(ident);
                }
                break;
        }

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
    return type;
}

void Syntaxer::if_operator(const set<TokenCodes> &followers) {
    if (!is_symbol_belong_to(start_ifOperator)) {
        list_error(22);
        skip_to(start_ifOperator, followers);
    }
    if (is_symbol_belong_to(start_ifOperator)) {
        accept(ifsy);
        translator_->create_else_ifless_labels();

        const auto else_cnt = translator_->else_labs.Count;
        const auto ifless_cnt = translator_->ifless_labs.Count;
    	
        auto symbols = union_of(follow_ifOperatorThen, followers);
        const auto type = expression(symbols);
        if (type == nullptr)
            list_error(328);

        accept(thensy);
        translator_->branch_false(Label(translator_->else_labs.Peek()));
    	
        symbols = union_of(follow_ifOperatorElse, followers);
        oper(symbols);

        translator_->branch(Label(translator_->ifless_labs.Peek()));
    	
        if (cur_token_->getCode() == elsesy) {
            translator_->pop_else_and_mark();
            accept(elsesy);
            oper(symbols);
            translator_->pop_ifless_and_mark();
        }

        const auto else_cnt_2 = translator_->else_labs.Count;
        const auto ifless_cnt_2 = translator_->ifless_labs.Count;

        if (ifless_cnt - 1 != ifless_cnt_2)
            translator_->pop_ifless_and_mark();
        if (else_cnt - 1 != else_cnt_2)
            translator_->pop_else_and_mark();
    	

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
}

void Syntaxer::while_operator(const set<TokenCodes> &followers) {
    if (!is_symbol_belong_to(start_whileOperator)) {
        list_error(22);
        skip_to(start_whileOperator, followers);
    }
    if (is_symbol_belong_to(start_whileOperator)) {
        accept(whilesy);
        translator_->create_while_loopless_labels();

        Label lab;
    	if (translator_->IsWorking)
			lab = Label(translator_->while_labs.Peek());
    	
        translator_->pop_while_and_mark();
    	
        const auto symbols = union_of(follow_whileDo, followers);
        const auto type = expression(symbols);
        if (type == nullptr)
            list_error(328);

        translator_->branch_false(Label(translator_->loopless_labs.Peek()));
        accept(dosy);
        oper(followers);

    	if (translator_->IsWorking)
			translator_->branch(Label(lab));
        translator_->pop_loopless_and_mark();

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
}

void Syntaxer::case_operator(const set<TokenCodes> &followers) {
    if (!is_symbol_belong_to(start_caseOperator)) {
        list_error(22);
        skip_to(start_caseOperator, followers);
    }
    if (is_symbol_belong_to(start_caseOperator)) {
        accept(casesy);
        auto symbols = union_of(follow_caseOf, followers);

        last_case_type_ = expression(symbols);
        if (last_case_type_ == nullptr)
            list_error(328);

        accept(ofsy);
        symbols = union_of(follow_caseEnd, followers);
        element_of_variants(symbols);

        while (cur_token_->getCode() == semicolon) {
            accept(semicolon);
            element_of_variants(symbols);
        }

        accept(endsy);

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
}

void Syntaxer::element_of_variants(const set<TokenCodes> &followers) {
    if (!is_symbol_belong_to(start_elementOfVariants)) {
        list_error(22);
        skip_to(start_elementOfVariants, followers);
    }
    if (is_symbol_belong_to(start_elementOfVariants)) {
	    const auto symbols = union_of(follow_elementOfVariants, followers);
        list_of_marks(symbols);
        accept(colon);
        oper(followers);

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
}

void Syntaxer::list_of_marks(const set<TokenCodes> &followers) {
    if (!is_symbol_belong_to(start_listOfMarks)) {
        list_error(22);
        skip_to(start_listOfMarks, followers);
    }
    if (is_symbol_belong_to(start_listOfMarks)) {
	    const auto type = constant(followers);
        if (type == nullptr || type != last_case_type_)
            list_error(328);

        while (cur_token_->getCode() == comma) {
            accept(comma);
            constant(followers);
        }

        if (!is_symbol_belong_to(followers)) {
            list_error(6);
            skip_to(followers, followers);
        }
    }
}