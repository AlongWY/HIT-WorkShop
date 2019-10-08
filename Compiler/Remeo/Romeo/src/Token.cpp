/**
 * Copyright (c) 2019 along All rights reserved.
 **/

#include <scanner/Token.h>
#include <scanner/Scanner.h>
#include <iostream>
#include "scanner/Lex.h"

namespace Romeo {

    Tokenizer::Code BaseTokenizer::add_keywords(const std::string &sym, const std::string &lex,
                                                Tokenizer::Code code, bool only_sym) {
        sym_code[sym] = code;
        code_sym[code] = sym;

        if (!only_sym) {
            lex_code[lex] = code;
            code_lex[code] = lex;
        }
        return code;
    }

    Tokenizer::Code BaseTokenizer::add_action(const std::string &sym, Tokenizer::Code code) {
        auto sym_ = sym_code.find(sym);
        auto before = sym_ == sym_code.end() ? (sym_code.size() + RESEVERD) | ACTION_FLAG : sym_->second;
        return add_keywords(sym, sym, code == 0 ? before : code, true);
    }

    Tokenizer::Code BaseTokenizer::add_terminal(const std::string &sym, const std::string &lex, unsigned long code) {
        auto sym_ = sym_code.find(sym);
        auto before = sym_ == sym_code.end() ? ((sym_code.size() + RESEVERD) | TERMINAL_FLAG) : sym_->second;
        return add_keywords(sym, lex, code == 0 ? before : code);
    }

    Tokenizer::Code BaseTokenizer::add_non_terminal(const std::string &sym, Tokenizer::Code code) {
        auto sym_ = sym_code.find(sym);
        auto before = sym_ == sym_code.end() ? (sym_code.size() + RESEVERD) : sym_->second;
        return add_keywords(sym, sym, code == 0 ? before : code, true);
    }

    Tokenizer::Code BaseTokenizer::code(const Lex *lex) const {
        if (lex == nullptr) return Tokenizer::ENDFILE;
        if (!lex->is_valid) {
            if (lex->is_integer()) return Tokenizer::ENDFILE;
            return Tokenizer::UNKNOWN;
        }
        if (lex->is_string()) return Tokenizer::STRING;
        if (lex->is_integer())return Tokenizer::INTEGER;
        if (lex->is_real()) return Tokenizer::REAL;

        auto res = lex_code.find((std::string) *lex);

        if (res == lex_code.end() && lex->is_punctuation()) {
            return UNKNOWN;
        } else if (res == lex_code.end()) {
            return IDENTIFIER;
        }
        return res->second;
    }

    Tokenizer::Code BaseTokenizer::code(const std::string &symbol) const {
        auto res = lex_code.find(symbol);
        if (res == lex_code.end())
            res = sym_code.find(symbol);
        else return res->second;
        return res == sym_code.end() ? UNKNOWN : res->second;
    }

    std::string BaseTokenizer::str(Tokenizer::Code code) const {
        auto lex = code_lex.find(code);
        if (lex != code_lex.end()) return lex->second;

        auto sym = code_sym.find(code);
        return sym == code_sym.end() ? "UNKNOW" : sym->second;
    }

    std::string BaseTokenizer::non_str(Tokenizer::Code code) const {
        auto sym = code_sym.find(code);
        return sym == code_sym.end() ? "UNKNOW" : sym->second;
    }

    std::vector<unsigned long> BaseTokenizer::code(const std::vector<std::string> &syms) const {
        std::vector<Tokenizer::Code> res;
        for (const auto &sym:syms) res.push_back(code(sym));
        return res;
    }
}
