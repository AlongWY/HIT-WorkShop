//
// Created by along on 19-4-17.
//

#include "LR1Context.h"
#include <iomanip>
#include <iostream>
#include <utils/Reflection.h>

using namespace std;
using namespace Romeo;


namespace RmoLang {
    LR1Context::~LR1Context() = default;

    /// 值相关
    void LR1Context::IntVal(LR1Context::Lexs &lexs) {
        auto val = Pop(lexs);
        auto val_meta = make_shared<ValueMeta>(statements.size() + 1, Integer, to_string(val->to_integer()));
        meta_stack.push(val_meta);
        delete val;
    }

    void LR1Context::RealVal(LR1Context::Lexs &lexs) {
        auto val = Pop(lexs);
        auto val_meta = make_shared<ValueMeta>(statements.size() + 1, Real, to_string(val->to_real()));
        meta_stack.push(val_meta);
        delete val;
    }

    void LR1Context::BoolVal(LR1Context::Lexs &lexs) {
        auto val = Pop(lexs);
        auto val_meta = make_shared<ValueMeta>(statements.size() + 1, Bool, val->to_string());
        meta_stack.push(val_meta);
        delete val;
    }

    void LR1Context::UnitVal(LR1Context::Lexs &lexs) {
        auto val = Pop(lexs);
        auto unit = metas->find(val->to_string());
        if (unit == nullptr) {
            display_statements(cerr);
            display_symbol_table(cerr);
            throw runtime_error(val->to_string() + " Is Not Found!!!");
        }
        auto val_meta = make_shared<ValueMeta>(statements.size() + 1, (MetaType) *unit, val->to_string());
        meta_stack.push(val_meta);
        delete val;
    }

    void LR1Context::StringVal(LR1Context::Lexs &lexs) {
        auto val = Pop(lexs);
        auto val_meta = make_shared<ValueMeta>(statements.size() + 1, String, val->to_string());
        meta_stack.push(val_meta);
        delete val;
    }

    /// 声明相关
    void LR1Context::Type(LR1Context::Lexs &lexs) {
        auto type = Pop(lexs);
        auto meta = Unknown;
        auto width = 0;
        if (type->to_string() == "int") {
            meta = Integer;
            width = 4;
        } else if (type->to_string() == "real") {
            meta = Real;
            width = 8;
        } else if (type->to_string() == "bool") {
            meta = Bool;
            width = 4;
        } else if (type->to_string() == "string") {
            meta = String;
            width = 4;
        }
        // 不生成语句
        meta_stack.push(make_shared<TypeMeta>(statements.size() + 1, meta, width));
        delete type;
    }

    void LR1Context::PopVarDecl(LR1Context::Lexs &lexs) {
        Skip(2, lexs);
        meta_stack.pop();
    }

    void LR1Context::VarDeclare(LR1Context::Lexs &lexs) {
        auto var = lexs.back();
        auto meta_ = meta_stack.top();

        // 当前作用域存在该变量
        if (metas->check(var->to_string())) {
            display_statements(cerr);
            display_symbol_table(cerr);
            throw runtime_error(var->to_string() + " redefined!!!");
        }

        // 不生成语句
        metas->metas.emplace_back(var->to_string(), meta_);
        meta_stack.pop();
    }

    void LR1Context::FuncDeclMark(LR1Context::Lexs &lexs) {
        Skip(1, lexs);          // DEF
        auto func = Pop(lexs);  // MAIN
        Skip(1, lexs);          // (

        // 使用新的符号表取代旧的
        auto func_meta = make_shared<FunctionMeta>(statements.size() + 1);
        metas->metas.emplace_back(func->to_string(), func_meta);
        meta_stack.push(func_meta);

        delete func;
    }

    void LR1Context::FuncDeclare(LR1Context::Lexs &lexs) {
        Skip(2, lexs);
        auto res = meta_stack.top();
        meta_stack.pop();
        auto func_meta = dynamic_pointer_cast<FunctionMeta>(meta_stack.top());
        func_meta->res = res;
    }

    void LR1Context::FuncDefine(LR1Context::Lexs &lexs) {

    }

    void LR1Context::FuncArgsLeft(LR1Context::Lexs &lexs) {
        Skip(1, lexs);
        auto func_meta = dynamic_pointer_cast<FunctionMeta>(meta_stack.top());
        auto last = metas->back();
        metas->metas.pop_back();
        func_meta->args_push_front(last);
    }

    void LR1Context::FuncArgsRight(LR1Context::Lexs &lexs) {
        Skip(2, lexs);
        auto func_meta = dynamic_pointer_cast<FunctionMeta>(meta_stack.top());
        auto last = metas->back();
        metas->metas.pop_back();
        func_meta->args_push_front(last);
    }

    void LR1Context::ArrayDeclare(LR1Context::Lexs &lexs) {
        auto size = Pop(lexs);

        Skip(2, lexs);
        delete size;
    }

    void LR1Context::ArrayExpend(LR1Context::Lexs &lexs) {
        auto size = Pop(lexs);

        delete size;
        Skip(2, lexs);
    }

    void LR1Context::ArrayExpendEnd(LR1Context::Lexs &lexs) {
        Skip(1, lexs);
    }

    /// 调用相关
    void LR1Context::Import(LR1Context::Lexs &lexs) {
        Skip(1, lexs);
        auto module = Pop(lexs);
        cerr << "导入" << module->to_string() << endl;

        /// todo
        delete module;
        Skip(1, lexs);
    }


    void LR1Context::FuncCall(LR1Context::Lexs &lexs) {

    }

    /// 逻辑判断结束
    void LR1Context::CtrlOver(LR1Context::Lexs &lexs) {
        Skip(1, lexs);

        auto last = dynamic_pointer_cast<ValueMeta>(meta_stack.top());
        auto jmp = make_shared<JmpMeta>(statements.size() + 1);
        statements.emplace_back(metas->id, "j=", "false", "", jmp);
        /// 弹出之前的逻辑语句
        meta_stack.pop();
    }

    /// 开启一个IF,同时接下来的Expression需要进行逻辑处理了
    void LR1Context::IfMark(LR1Context::Lexs &lexs) {
        Skip(2, lexs);
        auto if_ctl = make_shared<CtrlMeta>(statements.size() + 1, CtrlMeta::IF);

        metas->emplace_back("#IF", if_ctl);
        meta_stack.push(if_ctl);
    }

    /// 回填If的各种LIST
    void LR1Context::IfStatement(LR1Context::Lexs &lexs) {
        /// 判断上一个Meta是什么 IF/ELSE不同
        auto else_ctl = dynamic_pointer_cast<CtrlMeta>(meta_stack.top());
        auto if_ctl = else_ctl;
        if (if_ctl->label == CtrlMeta::ELSE) {
            meta_stack.pop();
            if_ctl = dynamic_pointer_cast<CtrlMeta>(meta_stack.top());
        }
        auto if_end_jmp = if_ctl->block->next;
        dynamic_pointer_cast<JmpMeta>(get<4>(statements[if_end_jmp - 1]))->jmp_to = else_ctl->block->next;
    }

    /// 填上IF的JMP
    void LR1Context::ElseMark(LR1Context::Lexs &lexs) {
        // 删除else
        Skip(1, lexs);

        // 进入新的作用域
        auto else_ctl = make_shared<CtrlMeta>(statements.size() + 1, CtrlMeta::ELSE);

        // if的结束jmp 站位
        auto jmp = make_shared<JmpMeta>(statements.size() + 1);
        statements.emplace_back(metas->id, "j", "", "", jmp);

        // if 判断条件为假跳转
        auto if_ctl = dynamic_pointer_cast<CtrlMeta>(meta_stack.top());
        auto if_false_jmp = if_ctl->block->pos - 1;
        dynamic_pointer_cast<JmpMeta>(get<4>(statements[if_false_jmp - 1]))->jmp_to = else_ctl->pos + 1;

        metas->emplace_back("#ELSE", else_ctl);
        meta_stack.push(else_ctl);
    }

    /// 终结Else的作用域
    void LR1Context::ElseStatement(LR1Context::Lexs &lexs) {
        ///
    }

    /// 开启While
    void LR1Context::WhileMark(LR1Context::Lexs &lexs) {
        Skip(2, lexs);
        auto while_ctl = make_shared<CtrlMeta>(statements.size() + 1, CtrlMeta::WHILE);

        metas->emplace_back("#While", while_ctl);
        meta_stack.push(while_ctl);
    }

    /// 终结While的作用域，同时回填While的False JMP
    void LR1Context::WhileStatement(LR1Context::Lexs &lexs) {
        auto while_ctl = dynamic_pointer_cast<CtrlMeta>(meta_stack.top());
        // 结束跳转
        auto jmp = make_shared<JmpMeta>(statements.size() + 1);
        jmp->jmp_to = while_ctl->pos;//11
        statements.emplace_back(metas->id, "j", "", "", jmp);

        // 失败跳转
        auto while_false_jmp = while_ctl->block->pos - 1;//9
        dynamic_pointer_cast<JmpMeta>(get<4>(statements[while_false_jmp - 1]))->jmp_to = while_ctl->block->next + 1;
        meta_stack.pop();
    }

    /// 元素操作相关
    void LR1Context::Assign(LR1Context::Lexs &lexs) {
        Skip(2, lexs);
        auto var = Pop(lexs);

        auto unit = metas->find(var->to_string());
        if (unit == nullptr || unit->getMeta() == Function) {
            display_statements(cerr);
            display_symbol_table(cerr);
            throw runtime_error(var->to_string() + " Is Not Found!!!");
        }
        auto statement = meta_stack.top();
        string cast_it;
        if (dynamic_pointer_cast<TypeMeta>(unit)->type < dynamic_pointer_cast<ValueMeta>(statement)->type) {
            cast_it = "*cast*";
        }
        auto next_statement = make_shared<ValueMeta>(statements.size() + 1, (MetaType) *unit, var->to_string());
        statements.emplace_back(metas->id, "=", dynamic_pointer_cast<ValueMeta>(statement)->value, cast_it,
                                next_statement);
        meta_stack.pop();
        delete var;
    }

    static const set<string> &BINMAP() {
        const static std::set<std::string> binmap = {
                "!=", "!", "&&",
                "<=", "<", "==",
                "=", ">>", ">="
        };
        return binmap;
    }

    /// 应该记录之前运算的一些状态，比如结算结果的类型之类的
    void LR1Context::UnaryOp(LR1Context::Lexs &lexs) {
        auto op = Pop(lexs);
        auto op_str = op->to_string();
        auto last = dynamic_pointer_cast<ValueMeta>(meta_stack.top());
        meta_stack.pop();

        /// 分析运算符，进行逻辑判断
        bool logical = BINMAP().count(op_str) > 0;
        auto next_statement = make_shared<ValueMeta>(statements.size() + 1,
                                                     logical ? Bool : last->type, metas->getTmp());
        statements.emplace_back(metas->id, op_str, last->value, "", next_statement);
        meta_stack.push(next_statement);

        delete op;
    }

    /// 应该记录之前运算的一些状态，比如结算结果的类型之类的
    void LR1Context::BinOp(LR1Context::Lexs &lexs) {
        auto op = Pop(lexs);
        auto op_str = op->to_string();
        auto last = dynamic_pointer_cast<ValueMeta>(meta_stack.top());
        meta_stack.pop();
        auto last_forward = dynamic_pointer_cast<ValueMeta>(meta_stack.top());
        meta_stack.pop();

        /// 分析运算符，进行逻辑判断
        bool logical = BINMAP().count(op_str) > 0;
        auto type = last_forward->type > last->type ? last_forward->type : last->type;
        auto next_statement = make_shared<ValueMeta>(statements.size() + 1, logical ? Bool : type, metas->getTmp());
        statements.emplace_back(metas->id, op_str, last_forward->value, last->value, next_statement);
        meta_stack.push(next_statement);

        delete op;
    }

    void LR1Context::Def(LR1Context::Lexs &lexs) {

    }

    void LR1Context::Call(LR1Context::Lexs &lexs) {

    }

    /// 工具函数
    void LR1Context::Skip(int pops, LR1Context::Lexs &lexs) {
        for (auto i = 0; i != pops; ++i) {
            auto back = lexs.back();
            lexs.pop_back();
            delete back;
        }
    }

    const LR1Context::Lex *LR1Context::Pop(LR1Context::Lexs &lexs) {
        auto res = lexs.back();
        lexs.pop_back();
        return res;
    }

    void LR1Context::BlockStart(LR1Context::Lexs &lexs) {
        Skip(1, lexs);
        auto block = make_shared<BlockMeta>(statements.size() + 1, metas);
        if (meta_stack.top()->getMeta() == Function) {
            auto func = dynamic_pointer_cast<FunctionMeta>(meta_stack.top());
            func->block = block;
            for (auto arg:func->args) {
                block->push_back(arg);
            }
        } else if (meta_stack.top()->getMeta() == Logic) {
            dynamic_pointer_cast<CtrlMeta>(meta_stack.top())->block = block;
        }
        metas = block;
    }

    void LR1Context::BlockEnd(LR1Context::Lexs &lexs) {
        Skip(1, lexs);
        metas->next = statements.size() + 1;
        metas = metas->parent;
    }

    void LR1Context::display_statements(ostream &ostream) {
        for (int i = 0; !statements.empty(); ++i) {
            auto[block_id, op, arg1, arg2, res] = statements.front();
            if (res != nullptr && typeid(*res.get()) == typeid(Romeo::ValueMeta)) {
                ostream << setw(5) << left << res->pos
                        << "(" << setw(5) << right << block_id << "): ("
                        << setw(10) << right << op << ", "
                        << setw(10) << right << arg1 << ", "
                        << setw(10) << right << arg2 << ", "
                        << setw(10) << right << std::dynamic_pointer_cast<Romeo::ValueMeta>(res)->value
                        << ")" << std::endl;
            } else if (res != nullptr && typeid(*res.get()) == typeid(Romeo::JmpMeta)) {
                /// 可能要处理跳转逻辑
                ostream << setw(5) << left << res->pos
                        << "(" << setw(5) << right << block_id << "): ("
                        << setw(10) << right << op << ", "
                        << setw(10) << right << arg1 << ", "
                        << setw(10) << right << arg2 << ", "
                        << setw(10) << right << std::dynamic_pointer_cast<Romeo::JmpMeta>(res)->jmp_to
                        << ")" << std::endl;
            } else
                ostream << setw(5) << left << res->pos
                        << "(" << setw(5) << right << block_id << "): ("
                        << setw(10) << right << op << ", "
                        << setw(10) << right << arg1 << ", "
                        << setw(10) << right << arg2 << ", "
                        << setw(10) << right
                        << ")" << std::endl;
            statements.pop_front();
        }
    }


    REGISTER1st(LR1Context, LR1Context::REF_TYPE, Type);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, IntVal);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, RealVal);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, BoolVal);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, UnitVal);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, StringVal);

    REGISTER1st(LR1Context, LR1Context::REF_TYPE, VarDeclare);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, PopVarDecl);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, FuncDeclMark);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, FuncDeclare);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, FuncDefine);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, FuncArgsLeft);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, FuncArgsRight);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, ArrayDeclare);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, ArrayExpend);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, ArrayExpendEnd);

    REGISTER1st(LR1Context, LR1Context::REF_TYPE, BlockStart);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, BlockEnd);

    REGISTER1st(LR1Context, LR1Context::REF_TYPE, FuncCall);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, Import);

    REGISTER1st(LR1Context, LR1Context::REF_TYPE, CtrlOver);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, IfMark);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, IfStatement);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, ElseMark);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, ElseStatement);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, WhileMark);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, WhileStatement);

    REGISTER1st(LR1Context, LR1Context::REF_TYPE, BinOp);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, UnaryOp);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, Assign);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, Def);
    REGISTER1st(LR1Context, LR1Context::REF_TYPE, Call);

    REGISTER(LR1Context, LR1Context::REF_TYPE, Skip, Skip, 1, std::placeholders::_1);
    REGISTER(LR1Context, LR1Context::REF_TYPE, Skip, Skip1, 1, std::placeholders::_1);
    REGISTER(LR1Context, LR1Context::REF_TYPE, Skip, Skip2, 2, std::placeholders::_1);
    REGISTER(LR1Context, LR1Context::REF_TYPE, Skip, Skip3, 3, std::placeholders::_1);
    REGISTER(LR1Context, LR1Context::REF_TYPE, Skip, Skip4, 4, std::placeholders::_1);
    REGISTER(LR1Context, LR1Context::REF_TYPE, Skip, Skip5, 5, std::placeholders::_1);
    REGISTER(LR1Context, LR1Context::REF_TYPE, Skip, Skip6, 6, std::placeholders::_1);
    REGISTER(LR1Context, LR1Context::REF_TYPE, Skip, Skip7, 7, std::placeholders::_1);
    REGISTER(LR1Context, LR1Context::REF_TYPE, Skip, Skip8, 8, std::placeholders::_1);
    REGISTER(LR1Context, LR1Context::REF_TYPE, Skip, Skip9, 9, std::placeholders::_1);

}