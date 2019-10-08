/**
 * Copyright (c) 2019 along All rights reserved.
 **/

#ifndef MINICOMPILER_SCANNER_H
#define MINICOMPILER_SCANNER_H

#include <istream>
#include <memory>
#include <set>
#include <ostream>
#include <variant>
#include <stack>


namespace Romeo {
    class Lex;

    class Scanner {
    public:
        explicit Scanner() : line_num(1), curr_lex(nullptr),
                             i_stream(nullptr),
                             curr_pos(0),
                             curr_line_start() {
        }

        explicit Scanner(std::istream *stream);

        Scanner(const Scanner &scanner) = default;

        ~Scanner();

        virtual bool is_valid();

        virtual void reset_stream(std::istream *stream);

        virtual void reset();

        virtual const Lex *operator()() {
            return curr_lex;
        }

        virtual Lex *next();

        template<typename ...Args>
        auto add_op(Args &&...args) {
            return ops.insert(std::forward<Args>(args)...);
        }

    private:

        Lex *NextToken();

        Lex *ParseString();    // 字符串

        Lex *ParseUnit();      // 关键字 和 标识符

        Lex *ParseNumber();    // 数字

        int peek() {
            return i_stream->peek();
        };

        int get() {
            curr_pos += 1;
            auto res = i_stream->get();

            if (res == '\n') {
                line_num++;
                curr_line_start.push(curr_pos);
            }

            return res;
        };

        int unget() {
            curr_pos -= 1;
            i_stream->unget();
            if (peek() == '\n') {
                line_num--;
                curr_line_start.pop();
            }
            return peek();
        };

        static bool isdigit(int c, int base = 10);

    private:
        size_t line_num;
        std::streampos curr_pos;
        std::stack<std::streampos> curr_line_start;

        Lex *curr_lex;

    private:
        std::istream *i_stream;

        std::set<std::string> ops;
    };


}
#endif //MINICOMPILER_SCANNER_H
