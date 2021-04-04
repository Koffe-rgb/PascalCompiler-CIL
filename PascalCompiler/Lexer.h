#pragma once
#include "IoModule.h"
#include "Token.h"
#include "Codes.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <set>

using namespace std;

class Lexer {
private:
    IoModule* io_module_;		// модуль ввода-вывода
    char cur_char_;			// текущая литера, полученная от модуля ввода-вывода
    int len_max_ = 40;		// максимальная длина строки / имени
    int max_int_ = INT_MAX;

private:

    AbstractToken* scan_later() const;
    AbstractToken* scan_greater() const;
    AbstractToken* scan_left_par();
    AbstractToken* scan_flpar();
    AbstractToken* scan_frpar() const;
    AbstractToken* scan_star();
    AbstractToken* scan_colon() const;
    AbstractToken* scan_point() const;

    AbstractToken* scan_name() const;
    AbstractToken* scan_number_constant();
    AbstractToken* scan_string_constant();

public:

    explicit Lexer(string& path) {
        this->io_module_ = new IoModule(path);
        cur_char_ = '\0';
    }
    ~Lexer() {
        delete io_module_;
    }

    AbstractToken* scan_next_token();

    IoModule* get_io_module() const;
};