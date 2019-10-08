//
// Created by along on 19-4-6.
//

#include "cfg/CFG.h"
#include "parser/LL1.h"
#include "context/Context.h"
#include "scanner/Lex.h"
#include "scanner/Token.h"
#include "scanner/Scanner.h"

#include <stack>

using namespace std;

namespace Romeo {
    void LL1::init_grammer() {
        table_pos.clear();
        predict_table.clear();
        PARSER::init_grammer();
        make_table();
    }

    void LL1::display_table(std::ostream &os) {
        PARSER::display_table(os);
        os << "--------------------table---------------------";
        for (int index = 0; index != predict_table.size(); ++index) {
            os << std::endl << PARSER::cfg->Productions()[index].Header() << " -> {";
            auto line = predict_table[index];

            for (auto iter = line.begin(); iter != line.end();) {
                const auto&[term, rights] = *iter;
                os << std::endl << '\t' << str(term) << " => ";
                for (auto &right:rights) os << " " << str(right);
                iter++;
                if (iter != line.end()) os << " , ";
            }
            os << std::endl << "}";
        }
    }

    void LL1::make_table() {
        std::map<Tokenizer::Code, std::vector<Tokenizer::Code>> table_line;

        long pos = 0;
        for (const auto &grammer:PARSER::cfg->Productions()) {
            table_line.clear();
            const auto &left = grammer.Header();
            const auto &alphas = grammer.Body();

            table_pos[code(left)] = pos++;

            // A -> \alpha
            for (const auto &alpha_i:alphas) {
                bool nil = false;

                for (const auto &terminal:alpha_i) {
                    nil = false;

                    // A -> \alpha(B...)
                    // \forall b \in First(B)
                    //     A --b-> B....
                    if (cfg->IsNTerminal(terminal)) {
                        auto first_set = PARSER::first[code(terminal)];

                        for (const auto &terminal_symbolic : first_set) {
                            if (terminal_symbolic == Tokenizer::EPSILON) {
                                nil = true;
                                continue;
                            }

                            assert(table_line[terminal_symbolic].empty());
                            table_line[terminal_symbolic] = tokenizer->code(alpha_i);
                        }

                    } else if (cfg->IsTerminal(terminal) || cfg->IsEpsilon(terminal)) {
                        // A -> \alpha(b...) / \alpha(~~b...)
                        if (code(terminal) == Tokenizer::EPSILON) {
                            nil = true;
                            continue;
                        }
                        assert(table_line[code(terminal)].empty());
                        table_line[code(terminal)] = tokenizer->code(alpha_i);
                    }

                    if (!nil) {
                        break;
                    }
                }

                // 产生式右部所有的符号都可以为空
                if (nil) {
                    auto follow_set = PARSER::follow[code(left)];
                    for (auto terminal_symbolic : follow_set) {
                        assert(table_line[terminal_symbolic].empty());
                        table_line[terminal_symbolic].push_back(Tokenizer::EPSILON);
                    }
                }
            }
            predict_table.push_back(table_line);
        }
    }

    void LL1::parse(Context *context) {
        return parse(context, false);
    }

    const std::vector<std::map<Tokenizer::Code, std::vector<Tokenizer::Code>>> *LL1::PredictTable() const {
        return &predict_table;
    }

    const std::map<Tokenizer::Code, Tokenizer::Code> *LL1::PredictTablePos() const {
        return &table_pos;
    }

    void LL1::parse(Context *context, bool out_log, std::ostream &stack_log, bool Eps) {
        std::stack<Tokenizer::Code> ll1_stack;
        ll1_stack.emplace(code(PARSER::cfg->StartSymbol()));

        // 工具lambda
        auto display_stack = [&](auto X, auto input_token) {
            stack_log << "\n---------------------------\n" << std::endl;
            stack_log << "INPUT: " << *input_token << std::endl;

            stack_log << "STACK PEEK: " << str(X) << std::endl
                      << "STACK:" << std::endl;
            while (!ll1_stack.empty()) {
                stack_log << "\t" << str(ll1_stack.top()) << std::endl;
                ll1_stack.pop();
            }
            stack_log << "\n---------------------------\n" << std::endl;
        };

        if (scanner == nullptr) return;
        auto input_token = (*scanner)();

        if (code(input_token) == Tokenizer::UNKNOWN) {
            throw std::runtime_error("词法分析失败");
        } else if (code(input_token) == Tokenizer::ENDFILE) {
            stack_log << "The FIle Is Empty!!!" << endl;
            return;
        }

        while (!ll1_stack.empty()) {
            auto X = ll1_stack.top();
            if (tokenizer->is_terminal(X)) {
                if (code(input_token) == X) {
                    if (input_token == nullptr) {
                        ll1_stack.pop();             // 当前符号出栈
                        if (!ll1_stack.empty())
                            throw std::runtime_error("语法分析错误(EOF)");
                        continue;
                    }

                    if (out_log) stack_log << *input_token << " 匹配成功" << std::endl;
                    ll1_stack.pop();             // 当前符号出栈

                    if (context != nullptr) context->push_back(input_token);
                    else delete input_token;
                    input_token = scanner->next(); // 获取下一个输入符号
                } else if (X == Tokenizer::EPSILON) {
                    if (out_log) stack_log << "空匹配" << std::endl;
                    ll1_stack.pop();
                } else {
                    display_stack(X, input_token);
                    throw std::runtime_error("语法分析错误");
                }
            } else if (tokenizer->is_non_terminal(X)) {
                // 获取当前非终结符在不同终结符的转移情况
                auto X_pos = table_pos[X];
                auto X_line = predict_table[X_pos];
                auto iter = X_line.find(code(input_token));

                if (iter == X_line.end())       // 也有可能进行空转移
                    iter = X_line.find(Tokenizer::EPSILON);
                if (iter == X_line.end()) {
                    display_stack(X, input_token);
                    throw std::runtime_error("语法分析错误");
                }

                // 弹出当前符号
                if (out_log) stack_log << str(ll1_stack.top()) << " => ";
                ll1_stack.pop();

                // 对于当前非终结符 应用的产生式体
                const auto &product_body = iter->second;
                for (auto item = product_body.crbegin(); item != product_body.crend(); item++) {
                    if (out_log) stack_log << " " << str(*item);
                    ll1_stack.push(*item);
                }
                if (out_log) stack_log << std::endl;
            } else if (tokenizer->is_action(X)) {
                if (context != nullptr)
                    context->act(X);
                ll1_stack.pop();
            } else {
                display_stack(X, input_token);
                throw std::runtime_error("语法错误");
            }
        }
    }
}