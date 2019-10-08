//
// Created by along on 19-4-6.
//

#ifndef ROMEO_LL1CONTEXT_H
#define ROMEO_LL1CONTEXT_H

#include <utils/Reflection.h>
#include <context/BaseContext.h>

namespace Romeo {
    class Lex;

}

namespace RmoLang {

    class LL1Context : public Romeo::BaseContext {
    public:
        using Lex = Romeo::Lex;
        using Lexs = std::vector<const Romeo::Lex *>;
        using REF_TYPE = std::function<void(std::vector<const Romeo::Lex *> &)>;

        ~LL1Context() override {
            for (auto &val:m_lexes) {
                delete val;
            }
            m_lexes.clear();
        }

        void push_back(const Lex *lex) override {
            BaseContext::push_back(lex);
        }

        void push_back(Romeo::Tokenizer::Code code,
                       const std::function<void(std::vector<const Lex *> &)> &action) override {
            BaseContext::push_back(code, action);
        }

        void Accept(std::vector<const Romeo::Lex *> &arg) {
            std::cout << "Accept" << std::endl;
        };

        void IntVal(Lexs &lexs) {
            auto back = lexs.back();
            lexs.pop_back();
            std::cout << *back << std::endl;
            delete back;
        };

        void RealVal(Lexs &lexs) {
            auto back = lexs.back();
            lexs.pop_back();
            std::cout << *back << std::endl;
            delete back;
        };

        void IdVal(Lexs &lexs) {
            auto back = lexs.back();
            lexs.pop_back();
            std::cout << *back << std::endl;
            delete back;
        }

        void BinOp(Lexs &lexs) {
            auto back = lexs.back();
            lexs.pop_back();
            std::cout << *back << " (Binary Operator)" << std::endl;
            delete back;
        };

        void Def(Lexs &lexs) {
            auto back = lexs.back();
            lexs.pop_back();
            std::cout << *back << " (Function)" << std::endl;
            delete back;
        };

        void Call(Lexs &lexs) {
            auto back = lexs.back();
            lexs.pop_back();
            std::cout << *back << " (Call)" << std::endl;
            delete back;
        };

        void UnaryOp(Lexs &lexs) {
            auto back = lexs.back();
            lexs.pop_back();
            std::cout << *back << " (Unary  Operator)" << std::endl;
            delete back;
        };

        void Assign(Lexs &lexs) {
            auto back = lexs.back();
            lexs.pop_back();
            delete back;
            back = lexs.back();
            lexs.pop_back();
            std::cout << "Assign to " << (std::string) *back << std::endl;
            delete back;
        };

        void Skip(Lexs &lexs) {
            auto back = lexs.back();
            lexs.pop_back();
            delete back;
        }

    private:
        std::vector<const Lex *> m_lexes;
    };

    REGISTER1st(LL1Context, LL1Context::REF_TYPE, Accept);
    REGISTER1st(LL1Context, LL1Context::REF_TYPE, IntVal);
    REGISTER1st(LL1Context, LL1Context::REF_TYPE, RealVal);
    REGISTER1st(LL1Context, LL1Context::REF_TYPE, IdVal);
    REGISTER1st(LL1Context, LL1Context::REF_TYPE, BinOp);
    REGISTER1st(LL1Context, LL1Context::REF_TYPE, UnaryOp);
    REGISTER1st(LL1Context, LL1Context::REF_TYPE, Skip);
    REGISTER1st(LL1Context, LL1Context::REF_TYPE, Assign);
    REGISTER1st(LL1Context, LL1Context::REF_TYPE, Def);
    REGISTER1st(LL1Context, LL1Context::REF_TYPE, Call);
}


#endif //ROMEO_LL1CONTEXT_H
