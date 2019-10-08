//
// Created by along on 19-4-6.
//

#include "context/BaseContext.h"
#include <iostream>

using namespace std;

namespace Romeo {
    void BaseContext::act(Tokenizer::Code action) {
        auto act = actions.find(action);

        if (act != actions.end())
            act->second(lexes);
    }

    void BaseContext::act(const std::string &action) {
        auto act = actions_str.find(action);

        if (act != actions_str.end())
            act->second(lexes);
    }

    void BaseContext::push_back(const Lex *lex) {
        lexes.push_back(lex);
    }

    void BaseContext::push_back(Tokenizer::Code code,
                                const std::function<void(std::vector<const Lex *> &)> &action) {
        actions.emplace(code, action);
    }

    void BaseContext::push_back(const std::string &code,
                                const std::function<void(std::vector<const Lex *> &)> &action) {
        actions_str.emplace(code, action);
    }
}