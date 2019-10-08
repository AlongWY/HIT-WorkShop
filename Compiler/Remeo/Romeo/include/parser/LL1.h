#include <utility>
#include <ostream>

/**
 * Copyright (c) 2019 along All rights reserved.
 **/

#ifndef MINICOMPILER_PARSER_LL1_H
#define MINICOMPILER_PARSER_LL1_H

#include <stack>
#include <functional>
#include <cassert>
#include <iostream>
#include "Parser.h"

namespace CFG {
    typedef std::string Symbol;
}

namespace Romeo {
    class Scanner;

    class LL1 : public Parser {
        using PARSER = Parser;
    public:
        template<class... T>
        explicit LL1(Scanner *scanner, T &&... t) :
                scanner(scanner), PARSER(std::forward<T>(t)...) {}

        void init_grammer() override;

        void display_table(std::ostream &os) override;

        void parse(Context *context) override;

        const std::vector<std::map<Tokenizer::Code, std::vector<Tokenizer::Code>>> *PredictTable() const;

        const std::map<Tokenizer::Code, unsigned long> *PredictTablePos() const;

        void parse(Context *context, bool out_log, std::ostream &stack_log = std::cerr, bool Eps = false);

    private:
        void make_table();

    protected:
        Scanner *scanner;

        std::map<Tokenizer::Code, Tokenizer::Code> table_pos;
        std::vector<std::map<Tokenizer::Code, std::vector<Tokenizer::Code>>> predict_table;
    };

}
#endif //MINICOMPILER_PARSER_LL1_H
