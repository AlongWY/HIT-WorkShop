//
// Created by along on 19-4-6.
//

#ifndef ROMEO_CONTEXT_H
#define ROMEO_CONTEXT_H

#include <vector>
#include <scanner/Lex.h>
#include <scanner/Token.h>

namespace Romeo {
    class Context {
    public:
        virtual void act(Tokenizer::Code action) = 0;

        virtual void act(const std::string &action) = 0;

        virtual void push_back(const Lex *lex) = 0;

        virtual void put_reduce(const std::string &str) = 0;

        virtual ~Context() = default;;
    };

}
#endif //ROMEO_CONTEXT_H
