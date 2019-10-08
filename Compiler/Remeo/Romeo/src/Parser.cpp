//
// Created by along on 19-4-6.
//

#include <iostream>
#include "parser/Parser.h"
#include "cfg/CFG.h"
#include "scanner/Token.h"

namespace Romeo {
    void Parser::set_cfg(CFG::CFG *cfg_) {
        this->cfg = cfg_;
    }

    void Parser::set_tokenizer(Tokenizer *tokenizer_) {
        this->tokenizer = tokenizer_;
    }

    void Parser::init_grammer() {
        make_first();
        make_follow();
    }

    std::string Parser::str(Tokenizer::Code code) const {
        return tokenizer->str(code);
    }

    std::string Parser::non_str(Tokenizer::Code code) const {
        return tokenizer->non_str(code);
    }

    std::vector<std::string> Parser::non_str(const std::vector<Tokenizer::Code> &code) {
        return tokenizer->non_str(code);
    }

    void Parser::display_follow(std::ostream &os) {
        for (auto &[item, follow]:this->follow) {
            os << "FOLLOW(" << tokenizer->str(item) << ") = {";

            for (auto iter = follow.begin(); iter != follow.end();) {
                os << " " << tokenizer->str(*iter);
                iter++;
                if (iter != follow.end()) os << " , ";
            }

            os << " }" << std::endl;
        }
    }

    void Parser::display_first(std::ostream &os) {
        for (auto &[item, first]:this->first) {
            os << "FIRST(" << str(item) << ") = {";

            for (auto iter = first.begin(); iter != first.end();) {
                os << " " << str(*iter);
                iter++;
                if (iter != first.end()) os << " , ";
            }

            os << " }" << std::endl;
        }
    }

    void Parser::display_table(std::ostream &os) {
        os << "----------------------cfg----------------------" << std::endl;
        os << *cfg << std::endl;
        os << "---------------------first---------------------" << std::endl;
        display_first(os);
        os << "---------------------follow---------------------" << std::endl;
        display_follow(os);
    }

    const CFG::CFG *Parser::CFG() const {
        return cfg;
    }

    void Parser::first_dfs(std::vector<bool> &used, long x) {
        if (used[x]) return;
        used[x] = true;
        std::vector<CFG::Production> &products = cfg->Productions();
        for (auto i = 0; i != products[x].size(); ++i) {
            for (auto j = 0; j != products[x][i].size(); ++j) {
                auto a_item = products[x][i][j];

                // 终结符号 直接插入
                if (cfg->IsTerminal(a_item) || cfg->IsEpsilon(a_item) || cfg->IsAction(a_item)) {
                    first[code(products[x].Header())].insert(
                            cfg->IsAction(a_item) ? Tokenizer::EPSILON : code(a_item));
                    break;
                }
                // 非终结符号，看该终结符号的First集
                if (cfg->IsNTerminal(a_item)) {
                    auto b_iter = find_if(products.cbegin(), products.cend(),
                                          [&](const CFG::Production &production) -> bool {
                                              return production.Header() == a_item;
                                          });
                    if (b_iter == products.cend())
                        throw std::runtime_error("No Body Of " + a_item);
                    long y = b_iter - cfg->Productions().begin();

                    CFG::Symbol b_left = b_iter->Header();
                    first_dfs(used, y);

                    auto b_first = first[code(b_left)];

                    bool flag = false; // 是否包含空串
                    for (auto &b_first_item:b_first) {
                        if (b_first_item == Tokenizer::EPSILON) flag = true;
                        first[code(cfg->Productions()[x].Header())].insert(b_first_item);
                    }
                    if (!flag) break; // 如果不包含空串可以处理下一个表达式了
                } else continue;
            }
        }
    }

    void Parser::make_first() {
        first.clear();
        std::vector<bool> used(cfg->Productions().size(), false);
        for (int i = 0; i < cfg->Productions().size(); i++)
            first_dfs(used, i);
    }

    void Parser::make_follow() {
        follow.clear();
        cfg->Terminals().insert(str(Tokenizer::ENDFILE));
        follow[code(cfg->StartView())].insert(Tokenizer::ENDFILE);

        while (true) {
            bool goon = false;
            for (const auto &production:cfg->Productions()) {
                const auto &header = production.Header();
                const auto &alphas = production.Body();

                for (const auto &alpha_i:alphas) { // alpha_i 是一个序列式
                    // iter 是指向 Symbol 的迭代式
                    for (auto iter_B = alpha_i.cbegin(); iter_B != alpha_i.cend(); iter_B++) {

                        if (cfg->IsTerminal(*iter_B) || cfg->IsEpsilon(*iter_B) || cfg->IsAction(*iter_B)) continue;
                        auto iter_C = iter_B + 1;

                        // 对于产生式：A ->.....aBC.... ,将除去空集e的First（C）加入Follow（B）中;
                        // A -> ........aBcD.......
                        if (iter_C != alpha_i.cend()) {
                            if (cfg->IsNTerminal(*iter_C)) {                             // 加入非终结符的First集合
                                for (const auto &C_first:first[code(*iter_C)]) {
                                    if (C_first != Tokenizer::EPSILON) {
                                        auto[pos_iter, done] = follow[code(*iter_B)].insert(C_first);
                                        if (done) { goon = true; }
                                    }
                                }
                            } else if (!cfg->IsEpsilon(*iter_C) && !cfg->IsAction(*iter_C)) {             // 将终结符直接插入
                                auto[pos_iter, done] = follow[code(*iter_B)].insert(code(*iter_C));
                                if (done) { goon = true; }
                            } else if (cfg->IsEpsilon(*iter_C) || cfg->IsAction(*iter_C)) {
                                // A -> ...aBeeeeeeeeeeeeeeD
                                // A -> ...aBeeeeeeeeeeeeeee
                                // 只要下一个能导出空串，全加进来
                                auto iter_nil = iter_C;
                                auto iter_D = iter_C + 1;

                                while ((cfg->IsEpsilon(*iter_nil) || cfg->IsAction(*iter_nil))
                                       && iter_D != alpha_i.end()) {

                                    if (cfg->IsNTerminal(*iter_D))
                                        for (const auto &D_follow:follow[code(*iter_D)]) {
                                            auto[pos_iter, done] = follow[code(*iter_B)].insert(D_follow);
                                            if (done) { goon = true; }
                                        }
                                    iter_nil++;
                                    iter_D++;
                                }

                                if (iter_D == alpha_i.end()) {
                                    for (const auto &A_follow:follow[code(header)]) {
                                        auto[pos_iter, done] = follow[code(*iter_B)].insert(A_follow);
                                        if (done) { goon = true; }
                                    }
                                }
                            }
                        }

                        // 当前符号最后一个符号 并且是 非终结符
                        // A -> ....aB,则将Follow（A）加入Follow（B）中。
                        if (iter_B + 1 == alpha_i.cend()) {
                            for (const auto &A_follow:follow[code(header)]) {
                                auto[pos_iter, done] = follow[code(*iter_B)].insert(A_follow);
                                if (done) { goon = true; }
                            }
                        } else if (iter_C + 1 == alpha_i.cend() && cfg->IsNTerminal(*iter_C)) {
                            // A -> ......aBC,(其中C可以推导出空串，C => Epsilon),则将Follow（A）加入Follow（B）中
                            const auto C_production = find(cfg->Productions().begin(),
                                                           cfg->Productions().end(),
                                                           *iter_C);

                            for (const auto &C_items: C_production->Body()) {
                                bool nil = true;

                                for (const auto &C_item : C_items)
                                    if (!(cfg->IsEpsilon(C_item) || cfg->IsAction(C_item)))
                                        nil = false;

                                if (nil) {
                                    for (const auto &A_follow:follow[code(header)]) {
                                        auto[pos_iter, done] = follow[code(*iter_B)].insert(A_follow);
                                        if (done) { goon = true; }
                                    }
                                }
                            }
                        } else if (cfg->IsNTerminal(*iter_C)) {
                            // A -> ......aBC.....,(其中C可以推导出空串，C => Epsilon),则将Follow（C）加入Follow（B）中
                            const auto C_production = find(cfg->Productions().begin(),
                                                           cfg->Productions().end(),
                                                           *iter_C);

                            for (const auto &C_items: C_production->Body()) {
                                bool nil = true;

                                for (const auto &C_item : C_items)
                                    if (!(cfg->IsEpsilon(C_item) || cfg->IsAction(C_item)))
                                        nil = false;

                                if (nil) {
                                    for (const auto &C_follow:follow[code(*iter_C)]) {
                                        auto[pos_iter, done] = follow[code(*iter_B)].insert(C_follow);
                                        if (done) { goon = true; }
                                    }
                                }
                            }
                        }
                    }
                }
            }
//            std::cerr << "---------------------follow(Debug)---------------------" << std::endl;
//            display_follow(std::cerr);
            if (!goon) break;
        }
    }

    const std::map<Tokenizer::Code, std::set<Tokenizer::Code>> *Parser::First() const {
        return &first;
    }

    const std::map<Tokenizer::Code, std::set<Tokenizer::Code>> *Parser::Follow() const {
        return &follow;
    }

    Tokenizer::Code Parser::code(const Lex *lex) const {
        auto res = tokenizer->code(lex);
        return res;
    }

    Tokenizer::Code Parser::code(const std::string &symbol) const {
        auto res = tokenizer->code(symbol);
        return res;
    }
}