//
// Created by along on 19-4-15.
//

#ifndef ROMEO_LR1_H
#define ROMEO_LR1_H

#include <iostream>
#include <memory>
#include "Parser.h"

namespace CFG {
    typedef std::string Symbol;
}

namespace Romeo {
    class Scanner;

    struct ItemReal {
        template<class... T>
        ItemReal(size_t production_idx, size_t body_idx, size_t pt, T &&... t) :
                pro_idx(production_idx), body_idx(body_idx), point(pt), suffix(std::forward<T>(t)...) {}

        bool operator==(const ItemReal &rhs) const;

        bool operator<(const ItemReal &rhs) const;

        const size_t pro_idx;
        const size_t body_idx;
        const size_t point;               // 点的位置
        std::set<Tokenizer::Code> suffix;
    };

    struct Item {
        template<class... T>
        explicit Item(T &&... t) :real(std::make_shared<ItemReal>(std::forward<T>(t)...)) {}

        bool operator==(const Item &rhs) const {
            return *real == *rhs.real;
        }

        bool operator<(const Item &rhs) const {
            return *real < *rhs.real;
        }

        std::shared_ptr<ItemReal> operator->() {      //重载->操作符
            return real;
        }

        std::shared_ptr<ItemReal> operator->() const {      //重载->操作符
            return real;
        }

        std::shared_ptr<ItemReal> real;
    };

    class LR1 : public Parser {
        using PARSER = Parser;
    public:
        template<class... T>
        explicit LR1(Scanner *scanner, T &&... t) :
                scanner(scanner), PARSER(std::forward<T>(t)...) {}

        void init_grammer() override;

        void display_table(std::ostream &os) override;

        void parse(Context *context) override;

        enum Action {
            UNKNOWN,
            STACK,
            REDUCE,
            ACCEPT
        };
    private:
        void make_table();

        std::set<Tokenizer::Code> First(std::vector<Tokenizer::Code>::iterator begin,
                                        std::vector<Tokenizer::Code>::iterator end,
                                        const std::set<Tokenizer::Code> &suffix);

        std::set<Item> items(Tokenizer::Code sym, const std::map<Tokenizer::Code, size_t> &table);

        // 求闭包
        std::set<Item> closure(std::set<Item> &items, const std::map<Tokenizer::Code, size_t> &table);

        // GOTO 函数
        std::set<Item> GOTO(std::set<Item> &items, Tokenizer::Code x, const std::map<Tokenizer::Code, size_t> &table);

        // ITEM -> CODES
        size_t Codes(const Item &item);

    protected:
        void display_item(const Item &item, std::ostream &os);

        void display_items(std::ostream &os);

        void display_action(std::ostream &os);

        void display_goto(std::ostream &os);

    public:
        // 项集
        const std::vector<std::set<Item>> &Items() const { return m_items;};

        // state,char,action
        const std::map<size_t, std::map<Tokenizer::Code, std::tuple<Action, size_t, size_t>>> &Actions() const { return m_ACTION; };

        // state,char,goto
        const std::map<size_t, std::map<Tokenizer::Code, int>> &Gotos() const { return m_GOTO; };

    protected:
        Scanner *scanner;
        std::vector<std::set<Item>> m_items; // 项集

        // state,char,action
        std::map<size_t, std::map<Tokenizer::Code, std::tuple<Action, size_t, size_t>>> m_ACTION;
        // state,char,goto
        std::map<size_t, std::map<Tokenizer::Code, int>> m_GOTO;

        // cache code
        std::map<size_t, std::map<size_t, int>> cache_code_map;
        std::vector<std::pair<Tokenizer::Code, std::vector<Tokenizer::Code>>> cache_code;
    };
}
#endif //ROMEO_LR1_H
