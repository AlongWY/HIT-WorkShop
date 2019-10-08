/**
 * Copyright (c) 2019 along All rights reserved.
 **/

#ifndef MINICOMPILER_TOKEN_H
#define MINICOMPILER_TOKEN_H

#include <map>
#include <string>
#include <vector>
#include <set>
#include <ostream>

namespace Romeo {
    class Lex;

    class Tokenizer {
    public:
        typedef unsigned long Code;
        static const Code ACTION_FLAG = Code(1) << (std::numeric_limits<Code>::digits - 1);
        static const Code TERMINAL_FLAG = Code(1) << (std::numeric_limits<Code>::digits - 2);
        static const Code MASK = ~(ACTION_FLAG | TERMINAL_FLAG);

        Tokenizer() = default;

        virtual ~Tokenizer() = default;

        enum RESEVERD {
            UNKNOWN = 0,
            ENDFILE = 1u | TERMINAL_FLAG,
            EPSILON = 2u | TERMINAL_FLAG,

            STRING = 3u | TERMINAL_FLAG,
            INTEGER = 4u | TERMINAL_FLAG,
            REAL = 5u | TERMINAL_FLAG,
            IDENTIFIER = 6u | TERMINAL_FLAG,

            RESEVERD = 10
        };

        virtual bool reset() {
            return true;
        }

        virtual bool is_action(Code code) const {
            return ACTION_FLAG & code;
        }

        virtual bool is_terminal(Code code) const {
            return !is_action(code) && (TERMINAL_FLAG & code);
        }

        virtual bool is_non_terminal(Code code) {
            return !is_action(code) && !(TERMINAL_FLAG & code);
        }

        virtual Tokenizer::Code code(const Lex *lex) const = 0;

        virtual Tokenizer::Code code(const std::string &symbol) const = 0;

        virtual std::vector<Tokenizer::Code> code(const std::vector<std::string> &syms) const = 0;

        virtual std::string str(Tokenizer::Code code) const = 0;

        virtual std::string non_str(Tokenizer::Code code) const = 0;

        virtual std::vector<std::string> non_str(const std::vector<Tokenizer::Code> &codes) const {
            std::vector<std::string> res;
            for (const auto &code:codes)
                res.push_back(non_str(code));
            return res;
        }

        virtual std::set<Tokenizer::Code> Codes() const = 0;
    };

    class BaseTokenizer : public Tokenizer {
    public:
        friend std::ostream &operator<<(std::ostream &os, const BaseTokenizer &tokenizer) {
            os << "{" << std::endl;
            for (auto &[key, value]:tokenizer.sym_code) {
                os << key << " => " << std::hex << (value & MASK);
                auto lex = tokenizer.code_lex.find(value);
                if (lex != tokenizer.code_lex.end())
                    os << " <= " << lex->second;
                os << std::endl;
            }
            os << "}" << std::endl;

            os << "{" << std::endl;
            for (auto &[key, value]:tokenizer.lex_code) {
                os << key << " => " << std::hex << (value & MASK) << std::dec;
                auto lex = tokenizer.code_sym.find(value);
                if (lex != tokenizer.code_sym.end())
                    os << " <= " << lex->second;
                os << std::endl;
            }
            os << "}" << std::endl;

            return os;
        }

        BaseTokenizer() = default;

        bool reset() override {
            lex_code.clear();
            sym_code.clear();
            code_sym.clear();
            code_lex.clear();
            return true;
        }

        Tokenizer::Code add_action(const std::string &sym, Tokenizer::Code code = 0);

        Tokenizer::Code add_terminal(const std::string &sym,
                                     const std::string &lex, Tokenizer::Code code = 0);

        Tokenizer::Code add_non_terminal(const std::string &sym, Tokenizer::Code code = 0);

        Tokenizer::Code code(const Lex *lex) const override;

        Tokenizer::Code code(const std::string &symbol) const override;

        std::vector<Tokenizer::Code> code(const std::vector<std::string> &syms) const override;

        std::string str(Tokenizer::Code code) const override;

        std::string non_str(Tokenizer::Code code) const override;

        std::set<Tokenizer::Code> Codes() const override {
            std::set<Tokenizer::Code> res;
            for (auto &[code, sym]:code_sym)
                if (!is_action(code)) res.insert(code);
            return res;
        }

    private:
        Tokenizer::Code add_keywords(const std::string &sym, const std::string &lex,
                                     Tokenizer::Code code = 0, bool only_sym = false);

        std::map<std::string, Tokenizer::Code> lex_code;      // 关键字/操作符识别
        std::map<std::string, Tokenizer::Code> sym_code;

        std::map<Tokenizer::Code, std::string> code_sym;
        std::map<Tokenizer::Code, std::string> code_lex;
    };

}

#endif //MINICOMPILER_TOKEN_H
