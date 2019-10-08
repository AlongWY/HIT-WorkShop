/**
 * Copyright (c) 2019 along All rights reserved.
 **/

#ifndef REMEO_PARSER_H
#define REMEO_PARSER_H

#include <set>
#include <map>
#include <string>
#include <vector>
#include <ostream>
#include <functional>

#include "scanner/Token.h"

namespace CFG {
    class CFG;
}

namespace Romeo {
    class Lex;

    class Tokenizer;

    class Context;

    class Parser {
    public:
        Parser() : cfg(nullptr), tokenizer(nullptr) {};

        explicit Parser(Tokenizer *tokenizer, CFG::CFG *cfg) :
                tokenizer(tokenizer), cfg(cfg) {};

        virtual ~Parser() = default;

        void set_cfg(CFG::CFG *cfg);

        void set_tokenizer(Tokenizer *tokenizer);

        const Tokenizer *get_tokenizer() { return tokenizer; };

        virtual void init_grammer();

        virtual void display_table(std::ostream &os);

        virtual void parse(Context *context) = 0;

        const CFG::CFG *CFG() const;

        const std::map<Tokenizer::Code, std::set<Tokenizer::Code>> *First() const;

        const std::map<Tokenizer::Code, std::set<Tokenizer::Code>> *Follow() const;

        std::string str(Tokenizer::Code code) const;

        std::string non_str(Tokenizer::Code code) const ;

        std::vector<std::string> non_str(const std::vector<Tokenizer::Code> &code);

        Tokenizer::Code code(const Lex *lex) const;

        Tokenizer::Code code(const std::string &symbol) const;

    private:
        void first_dfs(std::vector<bool> &used, long x);

    protected:
        void make_first();

        void make_follow();

        void display_first(std::ostream &os);

        void display_follow(std::ostream &os);

    protected:
        CFG::CFG *cfg;
        Tokenizer *tokenizer;

        std::map<Tokenizer::Code, std::set<Tokenizer::Code>> first;
        std::map<Tokenizer::Code, std::set<Tokenizer::Code>> follow;
    };
}

#endif //REMEO_PARSER_H
