#include <string>
#include <iostream>
#include <locale.h>

#include "Syntaxer.h"


using namespace std;

void test() {
    //string path = "ten_to_two.txt";
    string path = "guess_number.txt";
    //string path = "pascal.txt";

    auto* compiler = new Syntaxer(path);

    compiler->start();
    if (compiler->has_errors()) {
        compiler->print_errors();
    } else {
        cout << "Ошибок не обнаружено\n";
    }

    delete compiler;
}

int main() {
    setlocale(LC_ALL, "Russian");
    test();
    system("pause");
    return 0;
}