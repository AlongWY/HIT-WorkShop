//
// Created by along on 19-4-15.
//

#include "cfg/CFG.h"
#include "parser/LR1.h"
#include "context/Context.h"
#include "scanner/Scanner.h"
#include "utils/Utility.h"

#include <cassert>
#include <algorithm>
#include <stack>
#include <sstream>

using namespace std;

namespace Romeo {

    bool ItemReal::operator==(const ItemReal &rhs) const {
        return pro_idx == rhs.pro_idx &&
               body_idx == rhs.body_idx &&
               point == rhs.point &&
               suffix == rhs.suffix;
    }

    bool ItemReal::operator<(const ItemReal &rhs) const {
        if (pro_idx < rhs.pro_idx)return true;
        if (rhs.pro_idx < pro_idx)return false;
        if (body_idx < rhs.body_idx)return true;
        if (rhs.body_idx < body_idx)return false;
        if (point < rhs.point)return true;
        if (rhs.point < point)return false;
        return suffix < rhs.suffix;
    }


    void LR1::display_table(std::ostream &os) {
        os << "----------------------cfg----------------------" << std::endl;
        os << *cfg << std::endl;
        os << "---------------------first---------------------" << std::endl;
        display_first(os);
        os << "---------------------Items---------------------" << std::endl;
        display_items(os);
        os << "---------------------Table---------------------" << std::endl;
        display_action(os);
        display_goto(os);
    }

    void LR1::init_grammer() {
        Parser::make_first();

        m_items.clear();
        m_ACTION.clear();
        m_GOTO.clear();
        cache_code_map.clear();
        cache_code.clear();

        cfg->TakeActions();
        CFG::Production pruduct(cfg->StartSymbol() + '\'');
        CFG::ProductionBody body;
        body.push_back(cfg->StartSymbol());
        pruduct.push_back(body);

        cfg->AppendProduction(pruduct);
        cfg->StartSymbol(pruduct.Header());

        ((BaseTokenizer *) tokenizer)->add_non_terminal(pruduct.Header());
        make_table();
    }

    void LR1::make_table() {
        std::map<Tokenizer::Code, size_t> SymbolMap;
        for (const auto &product:cfg->Productions())
            SymbolMap.insert({tokenizer->code(product.Header()), SymbolMap.size()});

        auto start = code(cfg->StartSymbol());
        items(start, SymbolMap);

//        display_table(cerr);

        // 修改对应的操作，是否进行归约之类的
        for (auto[state, items]:enumerate(m_items)) {
            for (const Item &item:items) {
                auto &[header, codes] = cache_code[Codes(item)];
                if (item->point == codes.size()) {
                    // 归约
                    for (auto &suffix:item->suffix) {
                        if (header == start && item->point == codes.size()) {
                            auto[iter, done]= m_ACTION[state].insert({suffix, make_tuple(ACCEPT, 0, 0)});
                            if (!done) {
                                display_item(item, cerr);
                                cerr << " (" << state << "): " << non_str(suffix) << endl;
                                throw ("Action 重定义，不是有效的LR文法!");
                            }
                            continue;
                        }
                        auto[iter, done]= m_ACTION[state].insert(
                                {suffix, make_tuple(REDUCE, item->pro_idx, item->body_idx)});

                        if (!done) {
                            display_item(item, cerr);
                            cerr << " (" << state << "): " << non_str(suffix) << endl;
                            throw ("Action 重定义，不是有效的LR文法!");
                        }
                    }
                }
            }
        }
    }

    void LR1::parse(Context *context) {
        assert(scanner != nullptr);

        std::stack<size_t> state_stack;
        state_stack.push(0);

        const string default_act("DEFAULT");
        auto input = (*scanner)();
        auto unknown_action = make_tuple(UNKNOWN, size_t(), size_t());
        do {
            auto X = code(input);
            auto next = GetWithDef(m_ACTION[state_stack.top()], X, unknown_action);

            auto &[action, state, body] = next;
            switch (action) {
                case STACK: { // 移进
                    if (context != nullptr) context->push_back(input);
                    state_stack.push(state);
                    input = scanner->next();
                    break;
                }
                case REDUCE: { // 归约 (state, body) -> (product, body)
                    auto product = cfg->Productions()[state];
                    auto pro_body = product.Body()[body];

                    auto act = GetWithDef(cfg->ActionsTable(),
                                          make_tuple(state, body, pro_body.size()),
                                          default_act);

                    if (context != nullptr) {
                        ostringstream out;
                        out << product.Header() << " ==归约==> ";
                        for (auto &str:pro_body) out << " " << str;
                        context->put_reduce(out.str());
                    }

                    if (act != default_act && context != nullptr) {
                        context->act(code(act));
                    }
                    for (auto pops = pro_body.size(); pops != 0; pops--)
                        state_stack.pop();

                    auto go_state = GetWithDef(m_GOTO[state_stack.top()], code(product), -1);
                    if (go_state == -1) throw std::runtime_error("The Code Is Wrong!!!");
                    state_stack.push(go_state);

                    break;
                }
                case ACCEPT: { // 接受
                    return;
                }
                default: {// 遇到错误
                    cerr << non_str(X) << " : " << *input << endl;
                    throw std::runtime_error("The Code Is Wrong!!!");
                    break;
                }
            }
        } while (true);
    }

    size_t LR1::Codes(const Item &item) {
        auto cache_pos = GetWithDef(cache_code_map[item->pro_idx], item->body_idx, -1);

        if (cache_pos != -1) {
            return cache_pos;
        } else {
            auto product = cfg->Productions()[item->pro_idx];
            auto body = product.Body()[item->body_idx];
            auto codes = tokenizer->code(body);
            cache_code_map[item->pro_idx][item->body_idx] = cache_code.size();
            cache_code.emplace_back(tokenizer->code(product.Header()), codes);
            return cache_code.size() - 1;
        }
    }

    std::set<Item> LR1::items(Tokenizer::Code sym,
                              const std::map<Tokenizer::Code, size_t> &SymbolMap) {
        auto start = SymbolMap.find(sym);
        if (start == SymbolMap.end()) throw std::runtime_error("CFG NO START!!!");

        Item C(start->second, 0, 0);
        C->suffix.insert(Tokenizer::ENDFILE);

        std::set<Item> C_closure;
        C_closure.insert(C);

        m_items.push_back(closure(C_closure, SymbolMap));

        while (true) {
            auto pre_size = m_items.size();
            for (auto state = 0; state < m_items.size(); ++state) {
                for (auto &terminal:tokenizer->Codes()) {
                    auto goto_t = GOTO(m_items[state], terminal, SymbolMap);
                    if (goto_t.empty()) continue;
                    int wana = -1;
                    for (auto i = 0; m_items.size() != i; ++i) {
                        if (m_items[i] == goto_t) {
                            wana = i;
                            break;
                        }
                    }
                    if (wana == -1) {
                        wana = m_items.size();
                        m_items.push_back(goto_t);
                    }
                    if (tokenizer->is_terminal(terminal)) {
                        m_ACTION[state][terminal] = make_tuple(STACK, wana, 0);
                    } else if (tokenizer->is_non_terminal(terminal)) {
                        m_GOTO[state][terminal] = wana;
                    }
                }
            }
            if (m_items.size() == pre_size) break;
        }
        return C_closure;
    }

    std::set<Item> LR1::closure(std::set<Item> &items,
                                const std::map<Tokenizer::Code, size_t> &SymbolMap) {
        for (;;) {
            auto pre_size = items.size();
            for (const auto &item:items) {
                auto &[header, codes] = cache_code[Codes(item)];
                auto next_non_terminal = codes.begin() + item->point;
                if (next_non_terminal != codes.end() && tokenizer->is_non_terminal(*next_non_terminal)) {
                    auto suffix(std::move(First(next_non_terminal + 1, codes.end(), item->suffix)));
                    auto product_next_pos = SymbolMap.find(*(codes.begin() + item->point));
                    if (product_next_pos == SymbolMap.end()) throw runtime_error(non_str(codes[item->point]) + "未定义");
                    auto product_next = cfg->Productions()[product_next_pos->second];

                    for (auto i = 0; i != product_next.Body().size(); ++i) {
                        auto found = find_if(items.begin(), items.end(), [&](const Item &rhs) -> bool {
                            return product_next_pos->second == rhs->pro_idx &&
                                   i == rhs->body_idx &&
                                   0 == rhs->point;
                        });
                        if (found != items.end()) {
                            auto ext = std::move(items.extract(found).value());
                            ext->suffix.insert(suffix.begin(), suffix.end());
                            items.insert(ext);
                        } else {
                            Item new_item(product_next_pos->second, i, 0, suffix);
                            items.insert(new_item);
                        }
                    }
                }
            }
            if (items.size() == pre_size) break;
        }

        return items;
    }

    std::set<Item> LR1::GOTO(std::set<Item> &items, Tokenizer::Code X,
                             const std::map<Tokenizer::Code, size_t> &SymbolMap) {
        std::set<Item> J;
        for (const auto &item:items) {
            auto &[header, codes] = cache_code[Codes(item)];
            auto next_non_terminal = codes.begin() + item->point;

            if (next_non_terminal != codes.end() && *next_non_terminal == X) {
                Item new_item(item->pro_idx, item->body_idx, item->point + 1);
                new_item->suffix.insert(item->suffix.begin(), item->suffix.end());
                J.insert(new_item);
            }
        }
        return closure(J, SymbolMap);
    }

    std::set<Tokenizer::Code> LR1::First(std::vector<Tokenizer::Code>::iterator begin,
                                         std::vector<Tokenizer::Code>::iterator end,
                                         const std::set<Tokenizer::Code> &suffix) {
        std::set<Tokenizer::Code> res;
        auto first = PARSER::First();

        while (begin != end) {
            if (tokenizer->is_action(*begin) || *begin == Tokenizer::EPSILON) {
                begin++;
                continue;
            }
            bool nil = false;
            auto maybe = first->find(*begin);
            if (maybe != first->end()) {
                for (auto &code:maybe->second) {
                    if (code == Tokenizer::EPSILON) {
                        nil = true;
                        continue;
                    }
                    res.insert(code);
                }
            } else if (tokenizer->is_terminal(*begin)) {
                res.insert(*begin);
                break;
            }
            if (!nil) break;
            begin++;
        }

        if (begin == end) {
            auto merge = suffix;
            res.merge(merge);
        }

        return res;
    }

    void LR1::display_item(const Item &item, std::ostream &os) {
        auto &[header, codes] = cache_code[Codes(item)];
        auto list = non_str(codes);
        os << "\t" << non_str(header) << "=>";
        for (auto index = 0; index != list.size(); ++index) {
            if (index == item->point) os << ".";
            os << list[index] << ' ';
        }
        if (item->point == list.size()) os << ".";
        auto iter = item->suffix.cbegin();
        os << " ," << non_str(*iter++);
        for (; iter != item->suffix.cend(); ++iter) {
            os << "/" << non_str(*iter);
        }
    }

    void LR1::display_items(std::ostream &os) {
        int state = 0;
        for (const auto &item_set:m_items) {
            os << state << ": {" << endl;
            for (const auto &item:item_set) {
                display_item(item, os);
                os << endl;
            }
            os << "}" << endl;
            state++;
        }
    }

    void LR1::display_action(std::ostream &os) {
        for (const auto &[state, trans]:m_ACTION) {
            os << state << " -> {" << endl;

            for (const auto &[get, next]:trans) {
                auto[action, state, body] = next;

                switch (action) {
                    case STACK:
                        os << '\t' << non_str(get) << " ==移进==> " << state << endl;
                        break;
                    case REDUCE:
                        os << '\t' << non_str(get) << " ==归约==> (" << state << ',' << body << ")" << endl;
                        break;
                    case ACCEPT:
                        os << '\t' << non_str(get) << " ==接受==> " << endl;
                        break;
                    default:
                        break;
                }

            }

            os << "}" << endl;
        }
    }

    void LR1::display_goto(std::ostream &os) {
        for (const auto &[state, trans]:m_GOTO) {
            os << state << " -> {" << endl;

            for (const auto &[get, next]:trans) {
                os << '\t' << non_str(get) << " => " << next << endl;
            }

            os << "}" << endl;
        }
    }
}