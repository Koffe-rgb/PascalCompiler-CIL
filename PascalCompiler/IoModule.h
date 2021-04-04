#pragma once
#include <string>
#include <fstream>
#include <map>
#include <vector>

using namespace std;

class IoModule {
private:
    int line_idx_;			// номер текущей строки
    int char_idx_;			// номер текущей литеры
    ifstream code_source_;	// поток ввода
    string cur_line_;			// текущая строка
    vector<pair<int, pair<int, int>>> err_codes_and_pos_; // список возникших ошибок

    bool end_of_file_reached_ = false;

private:
    void read_line();		// прочитать следующую строку в curLine

public:
    explicit IoModule(const string& file_path) {
        line_idx_ = 0;
        char_idx_ = 0;
        cur_line_ = "";
        code_source_.open(file_path, ios::in);
    }

    virtual ~IoModule() = default;

    const vector<pair<int, pair<int, int>>> &getErrCodesAndPos() const;

    char peek_char(int offset = 0);	// посмотреть на n-ную литеру, от текущей, без изменения charIdx

    char get_char();	// получить текущую литеру и изменить charIdx, и lineIdx, если требуется

    void log_error(int err_code);	// записать в лог информацию об ошибке, по коду
    void log_error(int err_code, int len);
};
