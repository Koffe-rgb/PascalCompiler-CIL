#include "IoModule.h"

void IoModule::read_line() {
    if (code_source_.is_open()) {
        line_idx_++;
        getline(code_source_, cur_line_);
        if (code_source_.eof()) {
            code_source_.close();
        }
    } else {
        end_of_file_reached_ = true;
    }
}

char IoModule::get_char() {
    char chr;
    if (char_idx_ < cur_line_.size()) {
        chr = cur_line_[char_idx_++];
    } else {
        read_line();

        if (!end_of_file_reached_) {
            char_idx_ = 0;
            chr = '\n';
        } else {
            chr = '\0';
        }
    }

    return chr;
}

char IoModule::peek_char(int offset) {
    return char_idx_ + offset < cur_line_.size() ?
           cur_line_[char_idx_ + offset] :
           '\n';
}

void IoModule::log_error(int err_code) {
	auto pos = pair<int, int>(char_idx_, line_idx_);
	const auto ecp = pair<int, pair<int, int>>(err_code, pos);
    err_codes_and_pos_.push_back(ecp);
}

void IoModule::log_error(int err_code, int len) {
	auto pos = pair<int, int>(char_idx_ - len, line_idx_);
	const auto ecp = pair<int, pair<int, int>>(err_code, pos);
    err_codes_and_pos_.push_back(ecp);
}


const vector<pair<int, pair<int, int>>> &IoModule::getErrCodesAndPos() const {
    return err_codes_and_pos_;
}