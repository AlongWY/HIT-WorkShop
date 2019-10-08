//
// Created by along on 19-4-6.
//

#ifndef ROMEO_BASECONTEXT_H
#define ROMEO_BASECONTEXT_H

#include "Context.h"
#include <functional>
#include <iostream>

namespace Romeo {
    class BaseContext : public Context {
    public:
        ~BaseContext() override {
            clear();
        }

        virtual void clear() {
            for (auto &val:lexes) {
                std::cerr << "Lost: " << *val << std::endl;
                delete val;
            }
            lexes.clear();
        }

        void act(Tokenizer::Code action) override;

        void act(const std::string &action) override;

        void push_back(const Lex *lex) override;

        void put_reduce(const std::string &str) override { reduces.push_back(str); };

        const std::vector<std::string> &get_reduces() { return reduces; };

        virtual void push_back(Tokenizer::Code code,
                               const std::function<void(std::vector<const Lex *> &)> &action);

        virtual void push_back(const std::string &code,
                               const std::function<void(std::vector<const Lex *> &)> &action);

    private:
        std::vector<std::string> reduces;

        std::vector<const Lex *> lexes;
        std::map<Tokenizer::Code, std::function<void(std::vector<const Lex *> &)>> actions;
        std::map<std::string, std::function<void(std::vector<const Lex *> &)>> actions_str;
    };
}

#endif //ROMEO_BASECONTEXT_H
