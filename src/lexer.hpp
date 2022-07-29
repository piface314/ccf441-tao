#ifndef LEXER_H
#define LEXER_H

#include <sstream>

class Lexer {
private:
public:
    std::stringstream buffer;

    Lexer() {};
    void update_yyloc();
    int id_token(int token);
    int char_int_val(char c);
    int int_val(int base, int i0);
    double real_val();
    char unescape(char c);
    int tri_hex_offset();
    int trig_token();
    int hex_token();
    int integer_token(int base, int i0);
    int real_token();
    int char_token();
    int char_esc_token();
    int char_hex_token();
    void str_lit_begin();
    int string_token();
    void str_lit_push();
    void str_lit_push_esc();
    void str_lit_push_hex();
    void error(char *msg);
};

#endif