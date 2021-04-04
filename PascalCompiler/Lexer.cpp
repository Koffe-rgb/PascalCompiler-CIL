#include "Lexer.h"

void to_lower_case(string& str) {
    transform(str.begin(), str.end(), str.begin(),[](const unsigned char c) { return std::tolower(c); });
}

AbstractToken *Lexer::scan_next_token() {
    cur_char_ = io_module_->get_char();

    while (cur_char_ == ' ' || cur_char_ == '\n') {
        cur_char_ = io_module_->get_char();
    }

    AbstractToken* token;

    switch (cur_char_) {
        case '\'': token = scan_string_constant(); break;
        case '<': token = scan_later(); break;
        case '>': token = scan_greater(); break;
        case ':': token = scan_colon(); break;
        case '.': token = scan_point(); break;
        case '*': token = scan_star(); break;
        case '(': token = scan_left_par(); break;
        case '{': token = scan_flpar(); break;
        case '}': token = scan_frpar(); break;
        case ')': token = new KeywordToken(rightpar); break;
        case ';': token = new KeywordToken(semicolon); break;
        case '/': token = new KeywordToken(slash); break;
        case '=': token = new KeywordToken(TokenCodes::equal); break;
        case ',': token = new KeywordToken(comma); break;
        case '^': token = new KeywordToken(arrow); break;
        case '[': token = new KeywordToken(lbracket); break;
        case ']': token = new KeywordToken(rbracket); break;
        case '+': token = new KeywordToken(TokenCodes::plus); break;
        case '-': token = new KeywordToken(TokenCodes::minus); break;
        case '\0': token = new KeywordToken(endoffile); break;
        default:

            if (isdigit(cur_char_)) {
                token = scan_number_constant();
            }
            else if (isalpha(cur_char_) || cur_char_ == '_') {
                token = scan_name();
            }
            else {
                io_module_->log_error(6); // ошибка - запрещенный символ
                token = scan_next_token();
            }

            break;
    }

    return token;
}

AbstractToken *Lexer::scan_later() const
{
	const auto next_ch = io_module_->peek_char();

    AbstractToken* token;

    switch (next_ch) {
        case '=':
            token = new KeywordToken(laterequal);
            io_module_->get_char();
            break;

        case '>':
            token = new KeywordToken(latergreater);
            io_module_->get_char();
            break;

        default:
            token = new KeywordToken(later);
            break;
    }

    return token;
}

AbstractToken *Lexer::scan_greater() const
{
	const auto next_ch = io_module_->peek_char();

    AbstractToken* token;

    if (next_ch == '=') {
        token = new KeywordToken(greaterequal);
        io_module_->get_char();
    }
    else {
        token = new KeywordToken(TokenCodes::greater);
    }

    return token;
}

AbstractToken *Lexer::scan_colon() const
{
	const auto next_ch = io_module_->peek_char();

    AbstractToken* token;

    if (next_ch == '=') {
        token = new KeywordToken(assign);
        io_module_->get_char();
    }
    else {
        token = new KeywordToken(colon);
    }

    return token;
}

AbstractToken *Lexer::scan_point() const
{
	const auto next_ch = io_module_->peek_char();

    AbstractToken* token;

    if (next_ch == '.') {
        token = new KeywordToken(twopoints);
        io_module_->get_char();
    }
    else {
        token = new KeywordToken(point);
    }

    return token;
}

AbstractToken *Lexer::scan_left_par() {
	const auto next_ch = io_module_->peek_char();

    AbstractToken* token;

    if (next_ch == '*') {
        char prev = cur_char_;
        cur_char_ = io_module_->get_char();

        // пропуск коммента
        while ((prev != '*' || cur_char_ != ')') && cur_char_ != '\0') {
            prev = cur_char_;
            cur_char_ = io_module_->get_char();
        }

        if (prev == '*' && cur_char_ == ')') {
            token = scan_next_token();
        }
        else {
            io_module_->log_error(86);		// коммент не закрыт
            token = new KeywordToken(endoffile);
        }
    }
    else {
        token = new KeywordToken(leftpar);
    }

    return token;
}

AbstractToken *Lexer::scan_star() {
	const auto next_ch = io_module_->peek_char();

    AbstractToken* token;

    if (next_ch == ')') {
        cur_char_ = io_module_->get_char();
        io_module_->log_error(85); // коммент не открыт
        token = new KeywordToken(rcomment);
    }
    else {
        token = new KeywordToken(star);
    }

    return token;
}

AbstractToken *Lexer::scan_flpar() {
	io_module_->peek_char();

    AbstractToken* token;

    while (!(cur_char_ == '}' || cur_char_ == '\0')) {
        cur_char_ = io_module_->get_char();
    }

    if (cur_char_ == '}') {
        token = scan_next_token();
    }
    else {
        io_module_->log_error(86);	// коммент не закрыт
        token = new KeywordToken(endoffile);
    }

    return token;
}

AbstractToken *Lexer::scan_frpar() const
{
    io_module_->log_error(85); // коммент не открыт
    return new KeywordToken(frpar);
}

AbstractToken *Lexer::scan_number_constant() {

    // ------------------INTEGER------------------
    auto int_const = cur_char_ - '0';
    auto is_int_error = false;

    auto next_char = io_module_->peek_char();
    while (isdigit(next_char)) {
        cur_char_ = io_module_->get_char();
        const auto digit = cur_char_ - '0';

        if (!is_int_error && (int_const < max_int_ / 10 ||
            int_const < max_int_ / 10 && digit <= max_int_ % 10))
        {
            int_const = 10 * int_const + digit;
        }
        else {
            is_int_error = true;
        }
        next_char = io_module_->peek_char();
    }

    if (next_char != '.') {
        if (is_int_error) {
            io_module_->log_error(203);	// переполнение int
        }
        auto* ic = new IntConstant(int_const);
        return new ConstantToken(ic, TokenCodes::intconst);
    }

    next_char = io_module_->peek_char(1);
    if (!isdigit(next_char)) {
        if (isalpha(next_char)) {
            io_module_->log_error(201);
        }
        auto* ic = new IntConstant(int_const);
        return new ConstantToken(ic, TokenCodes::intconst);
    }


    // ------------------FLOAT------------------
    io_module_->get_char();	// пропуск точки
    next_char = io_module_->peek_char();

    if (!isdigit(next_char)) {
        io_module_->log_error(201); // должна идти цифра
        auto* fc = new FloatConstant(int_const);
        return new ConstantToken(fc, floatconst);
    }

    auto float_part_const = 0;
    auto radix = 1;
    auto is_float_error = false;

    while (isdigit(next_char)) {
        cur_char_ = io_module_->get_char();
        const auto digit = cur_char_ - '0';

        if (!is_float_error && (float_part_const < max_int_ / 10 ||
            float_part_const < max_int_ / 10 && digit <= max_int_ % 10))
        {
            float_part_const = 10 * float_part_const + digit;
            radix *= 10;
        }
        else {
            is_float_error = true;
        }
        next_char = io_module_->peek_char();
    }

    if (is_int_error || is_float_error) {
        io_module_->log_error(207);
    }

    auto* fc = new FloatConstant(int_const + float_part_const / static_cast<float>(radix));
    return new ConstantToken(fc, floatconst);
}

AbstractToken *Lexer::scan_string_constant() {
    // -----------------CHAR-----------------
    const auto char_const = io_module_->get_char();
    if (char_const == '\'' || char_const == '\n') {
        io_module_->log_error(75); // ошибка в символьной константе

        auto* cc = new CharConstant(char_const);
        return new ConstantToken(cc, charconst);
    }

    cur_char_ = io_module_->get_char();
    if (cur_char_ == '\'') {
        auto* cc = new CharConstant(char_const);
        return new ConstantToken(cc, charconst);
    }

    if (cur_char_ == '\n') {
        io_module_->log_error(75); // ошибка в символьной константе

        auto* cc = new CharConstant(char_const);
        return new ConstantToken(cc, charconst);
    }

    // -----------------STRING-----------------
    string str_const(1, char_const);
    auto len = 1;
    auto is_str_error = false;

    while (cur_char_ != '\'') {
        str_const += cur_char_;
        len++;

        if (len > len_max_) {
            is_str_error = true;
        }

        cur_char_ = io_module_->get_char();
        if (cur_char_ == '\n') {
            io_module_->log_error(75); // ошибка в символьной константе

            auto* sc = new StringConstant(str_const);
            return new ConstantToken(sc, stringconst);
        }
    }

    if (is_str_error) {
        io_module_->log_error(76); // слишком длинная строковая константа
    }

    auto* sc = new StringConstant(str_const);
    return new ConstantToken(sc, stringconst);
}

AbstractToken *Lexer::scan_name() const
{
	auto len = 1;
    string name(1, cur_char_);

	auto next_char = io_module_->peek_char();
    while ((isalpha(next_char) || isdigit(next_char) || next_char == '_')
        && len <= len_max_)
    {
        name += io_module_->get_char();
        len++;
        next_char = io_module_->peek_char();
    }

    to_lower_case(name);

    if (KeywordsMap.count(name) == 1) {
	    const auto code = KeywordsMap.at(name);
        return new KeywordToken(code);
    }
	return new IdentifierToken(name);
}

IoModule* Lexer::get_io_module() const {
    return io_module_;
}