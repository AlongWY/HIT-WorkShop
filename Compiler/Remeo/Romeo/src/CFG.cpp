/**
 * Copyright (c) 2019 along All rights reserved.
 **/

#include "cfg/CFG.h"
#include "utils/Utility.h"

#include <typeinfo>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <regex>
#include <cfg/CFG.h>

using namespace std;

namespace CFG {
    std::ostream &operator<<(std::ostream &os, const Production &production) {
        os << production.header << " ->";
        auto &bodys = production.bodies;
        for (auto iter = bodys.begin(); iter != bodys.end();) {
            for (auto &str:*iter) os << " " << str;
            iter++;
            if (iter != bodys.end()) os << endl << "\t| ";
        }
        return os;
    }

    bool Production::operator==(const std::string &rhs) const {
        return header == rhs;
    }

    bool Production::operator!=(const std::string &rhs) const {
        return header != rhs;
    }

    Production &Production::operator+=(const Production &rhs) {
        // 产生式头部相同才可以合并
        if (header != rhs.header) {
            return *this;
        }

        for (const auto &body : rhs.bodies) {
            this->bodies.push_back(body);
        }

        return *this;
    }

    std::ostream &operator<<(std::ostream &os, const CFG &cfg) {
        auto display_set = [&os](const std::set<std::string> &mset) {
            for (auto iter = mset.begin(); iter != mset.end();) {
                os << *iter;
                iter++;
                if (iter != mset.end()) os << ", ";
            }
        };

        os << "NonTerminals: ";
        display_set(cfg.non_terminals);

        os << endl << "Terminals: ";
        display_set(cfg.terminals);

        os << endl << "Actions: ";
        display_set(cfg.actions);

        for (const auto &production:cfg.productions)
            os << endl << production;

        for (const auto&[key, action]:cfg.actions_table) {
            auto[pro_idx, body_idx, act_pos] = key;
            os << "(" << pro_idx << "," << body_idx
               << "," << act_pos << ") : " << action << endl;
        }
        return os;
    }

    bool CFG::IsEpsilon(const std::string &symbolic) {
        return symbolic == epsilon;
    }

    bool CFG::IsTerminal(const std::string &symbolic) {
        return terminals.find(symbolic) != terminals.end();
    }

    bool CFG::IsNTerminal(const std::string &symbolic) {
        return non_terminals.find(symbolic) != non_terminals.end();
    }

    bool CFG::IsAction(const std::string &symbolic) {
        return actions.find(symbolic) != actions.end();
    }

    bool CFG::IsCFGSymbolic(const std::string &symbolic) {
        return IsEpsilon(symbolic) || IsTerminal(symbolic) || IsNTerminal(symbolic) || IsAction(symbolic);
    }

    CFG::CFG(std::string Epsilon, std::istream &is) : epsilon(std::move(Epsilon)) {
        build(is);
    }

    CFG::CFG(std::string Epsilon) : epsilon(std::move(Epsilon)) {}

    SymbolicType CFG::GetSymbolicType(const std::string &symbolic) {
        if (terminals.find(symbolic) != terminals.end())
            return TERMINAL;
        else if (non_terminals.find(symbolic) != non_terminals.end())
            return NON_TERMINAL;
        else if (actions.find(symbolic) != actions.end())
            return ACTION;
        return UNKNOW;
    }

    CFG &CFG::RemoveRecursive() {
        for (unsigned long i = 0; i < productions.size(); i++) {
            vector<ProductionBody> cont;

            // 消除间接左递归
            for (int j = 0; j < i; j++) {
                const auto &i_header = productions[i].Header();
                auto &i_body = productions[i].Body();

                const auto j_header = productions[j].Header();
                auto &j_body = productions[j].Body();

                auto it_i = i_body.begin();
                decltype(j_body.begin()) it_j;

                // 查阅 i 的产生式起始 是否为 j.header
                for (; it_i != i_body.end(); it_i++)
                    if (!it_i->empty() && it_i->at(0) == j_header) { // 发现存在左递归
                        for (it_j = j_body.begin(); it_j != j_body.end(); it_j++) {
                            // 构造新的产生式
                            vector<Symbol> temp = *it_j;
                            temp.insert(temp.end(), it_i->begin() + 1, it_i->end());
                            cont.push_back(temp);
                        }
                    }

                long nn = i_body.size();
                while (nn--) {
                    if (i_body.begin()->at(0) == j_header)
                        i_body.erase(i_body.begin());
                    else {
                        cont.push_back(*i_body.begin());
                        i_body.erase(i_body.begin());
                    }
                }
                swap(i_body, cont);
            }

            // 消除直接左递归
            auto it = productions[i].Body().begin();
            string tt;

            bool no_direct = true;
            for (; it != productions[i].Body().end(); it++) {
                // 发现直接左递归 P -> Pa...
                if (it->at(0) == productions[i].Header()) {
                    // 构造 P'
                    string temp(productions[i].Header());
                    temp.insert(temp.end(), '\'');

                    auto[iter, done] = non_terminals.insert(temp);
                    productions.emplace_back(*iter);
                    tt = *iter;
                    no_direct = false;
                    break;
                }
            }

            if (no_direct) continue;

            // 将 i_body 转存到 cont
            swap(productions[i].Body(), cont);

            // tt  <P'>
            vector<ProductionBody> &i_body_ = find(productions.begin(), productions.end(), tt)->Body();
            i_body_.emplace_back(vector{epsilon});

            // P -> Pa | b
            while (!cont.empty()) {
                auto body_inner_iter = cont.begin();

                // P -> aP'
                if (body_inner_iter->at(0) == productions[i].Header()) {
                    // 存入新的表达式中
                    body_inner_iter->emplace_back(tt);
                    body_inner_iter->erase(body_inner_iter->begin());
                    i_body_.emplace_back(*body_inner_iter);
                } else {
                    // P -> bP'
                    // 存入当前表达式中
                    body_inner_iter->emplace_back(tt);
                    productions[i].emplace_back(*body_inner_iter);
                }
                cont.erase(cont.begin());
            }
        }

        return *this;
    }

    CFG &CFG::TakeLeft() {
        for (auto m = 0; m != productions.size(); m++) {
            sort(productions[m].Body().begin(), productions[m].Body().end(),
                 [](const ProductionBody &p1, const ProductionBody &p2) {
                     return p1 < p2;
                 });
            auto header = productions[m].Header();
            auto iter = productions[m].Body().begin();

            while (iter != productions[m].Body().end() && iter + 1 != productions[m].Body().end()) {
                auto preffix = iter->at(0); // 可能的前缀

                // 确定有前缀
                if (preffix == (iter + 1)->at(0)) {
                    auto[insed, done] = non_terminals.insert(string(header) + '_' + string(preffix));
                    Production new_production(*insed);

                    // 将补充一个 [preffix]<new_production>
                    {
                        ProductionBody trans;
                        trans.push_back(preffix);
                        trans.push_back(new_production.Header());
                        iter = productions[m].Body().insert(iter--, trans);
                        iter++;
                    }

                    while (iter != productions[m].Body().end() && preffix == iter->at(0)) {
                        ProductionBody new_body;
                        if (iter->size() == 1)
                            new_body.emplace_back(epsilon);
                        else
                            new_body.insert(new_body.end(), iter->begin() + 1, iter->end());
                        new_production.Body().emplace_back(new_body);
                        iter = productions[m].Body().erase(iter);
                    }
                    productions.push_back(new_production);
                } else
                    iter++;
            }
        }

        return *this;
    }

    CFG &CFG::Simplify() {
        set<string> used_term;
        vector<bool> used_non_term(productions.size(), false);
        map<string, long> dict;

        for (auto i = 0; i != productions.size(); ++i)
            dict[productions[i].Header()] = i;
        Simplify(dict, used_term, used_non_term, dict[start]);

        // 消除不需要的表达式
        for (auto[i, iter] =make_pair(0, productions.begin()); iter != productions.end(); i++) {
            std::string header(iter->Header());
            if (used_non_term[i]) iter++;
            else {
                iter = productions.erase(iter);
                non_terminals.erase(find(non_terminals.begin(), non_terminals.end(), header));
            }
        }

        for (auto iter = terminals.begin(); iter != terminals.end();) {
            auto terminal = *iter;
            auto found = used_term.find(terminal);
            if (found == used_term.end()) {
                iter = terminals.erase(iter);
            } else iter++;
        }

        return *this;
    }

    CFG &CFG::Simplify(const map<string, long> &dict,
                       set<string> &used_terminals, vector<bool> &used, long pos) {
        if (used[pos]) return *this;
        used[pos] = true;

        auto curr = productions[pos].Body();
        for (auto &body:curr) {
            for (auto &sym:body) {
                if (IsNTerminal(sym)) {
                    auto iter = dict.find(sym);
                    if (iter == dict.end()) {
                        return *this;
                    }
                    auto[symbolic, the_pos] = *iter;
                    Simplify(dict, used_terminals, used, the_pos);
                } else if (IsTerminal(sym) | IsAction(sym)) {
                    used_terminals.insert(string(sym));
                }
            }
        }
        return *this;
    }

    void CFG::build(std::istream &is) {
        productions.clear();                // 产生式列表
        non_terminals.clear();              // 非终结符号集合
        terminals.clear();                  // 终结符号集合
        actions.clear();                    // 动作集合

        string line;

        // 第一行为起始符号
        if (std::getline(is, line)) {
            auto commit_pos = line.find_first_of("//");
            line = line.substr(0, commit_pos);
            auto[iter, done] = non_terminals.insert(line.substr(1, line.length() - 2));
            start = *iter;
        }

        if (is.eof()) throw runtime_error("NO T_CFG BODY!!!");

        while (std::getline(is, line)) {
            // 去除注释
            auto commit_pos = line.find_first_of("//");
            line = line.substr(0, commit_pos);

            // 去除空白符号
            line.erase(remove_if(line.begin(), line.end(), static_cast<int (*)(int)>(&isspace)), line.end());
            std::vector<std::string> strs;
            split(line, strs, "->");

            if (strs.empty()) {
                continue;
            } else if (strs.size() < 2) {
                cerr << "GRAMMER ERROR : " << line << std::endl;
                continue;
            }

            // 产生式的左部
            auto[l_iter, l_done] = non_terminals.insert(strs[0].substr(1, strs[0].length() - 2));
            if (find(productions.begin(), productions.end(), *l_iter) == productions.end())
                productions.emplace_back(*l_iter);
            Production &production = *find(productions.begin(), productions.end(), *l_iter);

            string rights = strs[1];
            strs.clear();
            split(rights, strs, "|");

            for (auto &str:strs) {
                std::regex expression(R"((<[\w']+>)|(\[([\w']+)\])|(\{[\w']+}))");
                std::smatch searched;
                ProductionBody productionBody;
                string::const_iterator searchStart(str.cbegin());
                while (regex_search(searchStart, str.cend(), searched, expression)) {
                    string expr = searched[0];
                    string symbol = expr.substr(1, expr.length() - 2);
                    productionBody.emplace_back(symbol);

                    if (symbol != epsilon) {
                        switch (expr.at(0)) {
                            case '<':
                                non_terminals.insert(symbol);
                                break;
                            case '[':
                                terminals.insert(symbol);
                                break;
                            case '{':
                                actions.insert(symbol);
                                break;
                            default:
                                throw runtime_error("WRONG BODY!!!");
                        }
                    }
                    searchStart = searched.suffix().first;
                }
                production.push_back(productionBody);
            }
        }
    }

    CFG &CFG::TakeActions() {
        for (auto pro_idx = 0; pro_idx != productions.size(); ++pro_idx) {
            for (auto body_idx = 0; body_idx != productions[pro_idx].size(); ++body_idx) {
                auto iter = productions[pro_idx][body_idx].begin();
                while (iter != productions[pro_idx][body_idx].end()) {
                    if (IsAction(*iter)) {
                        auto acc_pos = iter - productions[pro_idx][body_idx].begin();
                        if (acc_pos <= 0) throw std::runtime_error("不应该存的在语义动作!!!");
                        actions_table.insert({make_tuple(pro_idx, body_idx, acc_pos), *iter});
                        iter = productions[pro_idx][body_idx].erase(iter);
                    } else if (IsEpsilon(*iter)) {
                        if (productions[pro_idx][body_idx].size() == 1) {
                            // 确认无误 空产生式
                            iter = productions[pro_idx][body_idx].erase(iter);
                        } else {
                            throw runtime_error("产生式内部含有空匹配!!!");
                        }
                    } else {
                        iter++;
                    }

                }
            }
        }
        return *this;
    }
}