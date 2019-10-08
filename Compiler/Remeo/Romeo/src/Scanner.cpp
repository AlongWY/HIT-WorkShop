/**
 * Copyright (c) 2019 along All rights reserved.
 **/

#include "scanner/Scanner.h"
#include "scanner/Lex.h"
#include "scanner/Token.h"

#include <vector>
#include <string>
#include <tuple>
#include <cstring>
#include <cassert>
#include <iostream>

namespace Romeo {
    using namespace std;

    Scanner::Scanner(std::istream *stream) :
            line_num(1), curr_lex(nullptr), curr_pos(0),
            curr_line_start(), i_stream(stream) {
        reset_stream(stream);
    }

    Scanner::~Scanner() = default;;

    Lex *Scanner::NextToken() {
        if (!i_stream || i_stream->eof()) { // EOF
            return new Lex(line_num, curr_pos - curr_line_start.top(), false, 0);
        }
        if ((get() && i_stream->eof())) {    // EOF
            return new Lex(line_num, curr_pos - curr_line_start.top(), false, 0);
        } else unget();

        while (i_stream && !i_stream->eof()) {
            if (isspace(peek())) {
                get();
            } else if (peek() == '/') {
                get();
                if (peek() == '/') {
                    while (i_stream && !i_stream->eof() && get() != '\n');
                    continue;
                } else if (peek() == '*') {
                    bool state = false;

                    while (i_stream && !i_stream->eof() && peek() != -1) {
                        while (i_stream && !i_stream->eof() && peek() != -1)
                            if (get() == '*') {
                                state = true;
                                break;
                            }
                        if (get() != '/')
                            state = false;
                        if (state) break;
                    }

                } else {
                    unget();
                    break;
                }
            } else
                break;
        }

        if (!i_stream || i_stream->eof() || peek() == -1) return nullptr;

        if (isalpha(peek()) || (peek() == '_')) {
            return ParseUnit();
        } else if (isdigit(peek())) {
            return ParseNumber();
        } else if (peek() == '"') {
            return ParseString();
        } else if (i_stream) {
            // 处理操作符
            string m_op;
            m_op += get();

            while (ispunct(peek()))
                m_op += get();

            while (m_op.length() > 1 && !ops.count(m_op)) {
                m_op.pop_back();
                i_stream->unget();
            }
            return new Lex(line_num, curr_pos - curr_line_start.top() - m_op.length(), ops.count(m_op), m_op);
        } else {
            return nullptr;
        }
    }

    Lex *Scanner::ParseString() {
        assert(i_stream->peek() == '"');
        string lex;

        lex += get();

        for (; i_stream && i_stream->peek() != '"'; lex += get()) {
            if (i_stream->peek() == '\\') {
                lex += get();
                lex += get();
            }
        }
        if (i_stream->peek() == '"')
            lex += get();

        return new Lex(line_num, curr_pos - curr_line_start.top() - lex.length(), true, lex);
    }

    Lex *Scanner::ParseUnit() {
        string unit;
        unit += get();

        while (isdigit(peek()) || isalpha(peek()) || peek() == '_')
            unit += get();

        if (unit.length() > 255)
            cerr << "Too Long(>255)" << endl;
        else if (unit.length() > 63)
            cerr << "Too Long(>63)" << endl;

        return new Lex(line_num, curr_pos - curr_line_start.top() - unit.length(), true, unit);
    }

    Lex *Scanner::ParseNumber() {
        string unit;
        int base = 10;
        if (get() == '0') {
            base = 8;
            switch (get()) {
                case 'x':
                    unit = "0x";
                    base = 16;
                    break;
                case 'X':
                    unit = "0X";
                    base = 16;
                    break;
                default:
                    unget();
                    unget();
                    break;
            }
        } else
            unget();

        bool valid = true;

        // 至少把数字都录进来
        if (isdigit(peek(), base < 10 ? 10 : base)) {
            while (isdigit(peek(), base < 10 ? 10 : base)) {
                if (!isdigit(peek(), base))
                    valid = false;
                unit += get();
            }
        } else {
            valid = false;
        }

        if (i_stream && (peek() == '.' || peek() == 'e' || peek() == 'E') && (base == 10 || base == 8)) {
            unit += get();
            if (peek() == '+' || peek() == '-') unit += get();
            while (isdigit(peek())) unit += get();
        } else if (i_stream && (peek() == '.' || peek() == 'p' || peek() == 'P') && base == 16) {
            unit += get();
            if (peek() == '+' || peek() == '-') unit += get();
            while (isdigit(peek())) unit += get();
        } else if (valid) {
            return new Lex(line_num, curr_pos - curr_line_start.top(), valid, stoi(unit, nullptr, base));
        } else {
            return new Lex(line_num, curr_pos - curr_line_start.top(), valid, unit);
        }

        // 小数
        return new Lex(line_num, curr_pos - curr_line_start.top() - unit.length(), valid, stod(unit));
    }

    void Scanner::reset_stream(std::istream *stream) {
        stream->clear();
        this->i_stream = stream;
        reset();
    }

    void Scanner::reset() {
        line_num = 1;
        curr_pos = 0;
        while (!curr_line_start.empty())
            curr_line_start.pop();
        curr_line_start.emplace(curr_pos);
        curr_lex = nullptr;
        next();
    }

    bool Scanner::is_valid() {
        return !i_stream || peek() != -1;
    }

    bool Scanner::isdigit(int c, int base) {
        if (base <= 10) return '0' <= c && c < '0' + base;
        else return ('0' <= c && c < '0' + base) || ('a' <= c && c < 'a' + base - 10);
    }

    Lex *Scanner::next() {
        return curr_lex = NextToken();
    }
}