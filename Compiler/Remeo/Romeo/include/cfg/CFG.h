/**
 * Copyright (c) 2019 along All rights reserved.
 **/

#ifndef ROMEO_CFG_H
#define ROMEO_CFG_H

#include <string>
#include <vector>
#include <utility>
#include <ostream>
#include <set>
#include <map>
#include <memory>

namespace CFG {

    enum SymbolicType {
        TERMINAL = 0,        // 终结符号
        NON_TERMINAL,        // 非终结符号
        ACTION,              // 语法制导动作
        UNKNOW               // 未知类型
    };

    // 文法符号
    typedef std::string Symbol;

    // 产生式体
    typedef std::vector<Symbol> ProductionBody;

    // 产生式类
    class Production {
    public:
        template<class... T>
        explicit Production(T &&... t) : header(std::forward<T>(t)...) {}

        Symbol &Header() { return header; }

        const Symbol &Header() const { return header; }

        std::vector<ProductionBody> &Body() { return bodies; }

        const std::vector<ProductionBody> &Body() const { return bodies; }

        friend std::ostream &operator<<(std::ostream &os, const Production &production);

        template<class... T>
        auto push_back(T &&... t) {
            return bodies.push_back(std::forward<T>(t)...);
        }

        template<class... T>
        auto emplace_back(T &&... t) {
            return bodies.emplace_back(std::forward<T>(t)...);
        }

        Production &operator+=(const Production &rhs);

        auto &operator[](size_t pos) {
            return bodies[pos];
        }

        auto size() {
            return bodies.size();
        }

        bool operator==(const std::string &rhs) const;

        bool operator!=(const std::string &rhs) const;

        operator std::string() const {
            return header;
        }

    private:
        Symbol header; // 产生式头
        std::vector<ProductionBody> bodies; // 产生式体
    };

    // 文法类
    class CFG {
    public:
        friend std::ostream &operator<<(std::ostream &os, const CFG &cfg);

        explicit CFG(std::string Epsilon, std::istream &is);

        explicit CFG(std::string Epsilon);

        CFG() = default;

        ~CFG() = default;

        void build(std::istream &is);

        std::set<std::string> &Terminals() { return terminals; }

        const std::set<std::string> &Terminals() const { return terminals; }

        std::set<std::string> &NonTerminals() { return non_terminals; }

        const std::set<std::string> &NonTerminals() const { return non_terminals; }

        std::set<std::string> &Actions() { return actions; }

        const std::set<std::string> &Actions() const { return actions; }

        const std::map<std::tuple<size_t, size_t, size_t>, std::string> &ActionsTable() const { return actions_table; }

        const std::string &StartView() const { return start; };

        Symbol StartSymbol() const { return start; };

        Symbol StartSymbol(const std::string &start_) {
            if (non_terminals.count(start_)) {
                start = start_;
            }
            return start;
        };

        /// 是否是空符号
        bool IsEpsilon(const std::string &symbolic);

        /// 是否是终结符号
        bool IsTerminal(const std::string &symbolic);

        /// 是否是非终结符号
        bool IsNTerminal(const std::string &symbolic);

        /// 是否是制导行为
        bool IsAction(const std::string &symbolic);

        /// 是否是文法符号
        bool IsCFGSymbolic(const std::string &symbolic);

        /// 获取文法符号类型
        SymbolicType GetSymbolicType(const std::string &symbolic);

        /// 获取产生式列表
        std::vector<Production> &Productions() { return productions; }

        const std::vector<Production> &Productions() const { return productions; }

        /// 移除左递归
        CFG &RemoveRecursive();

        /// 化简产生式
        CFG &Simplify();

        /// 提取左公因子
        CFG &TakeLeft();

        /// 提取Action表
        CFG &TakeActions();

        /// 添加一个产生式
        void AppendProduction(const Production &production) {
            auto[iter, done] = non_terminals.insert(production.Header());
            if (!done)
                for (auto &product:productions) {
                    if (product.Header() == production.Header()) {
                        product += production;
                        return;
                    }
                }
            productions.push_back(production);
        }

    private:
        // 化简产生式
        CFG &Simplify(const std::map<std::string, long> &dict,
                      std::set<std::string> &used_terminals,
                      std::vector<bool> &used, long pos);

        std::string start;                                // 文法开始符号
        std::set<std::string> non_terminals;              // 非终结符号集合
        std::set<std::string> terminals;                  // 终结符号集合
        std::set<std::string> actions;                    // 动作集合

        std::vector<Production> productions;              // 产生式列表
        std::map<std::tuple<size_t, size_t, size_t>, std::string> actions_table;
    public:
        const std::string epsilon;                        // 空匹配符号
    };
}

#endif //ROMEO_CFG_H
