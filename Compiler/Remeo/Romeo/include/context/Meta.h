//
// Created by along on 19-4-22.
//

#ifndef ROMEO_SYMBOLMANAGER_H
#define ROMEO_SYMBOLMANAGER_H

#include <string>
#include <memory>
#include <ostream>
#include <deque>
#include <utility>
#include <algorithm>

namespace Romeo {
    enum MetaType {
        Unknown,
        Bool,
        Integer,
        Real,
        String,
        Array,
        Function,
        Block,
        Logic
    };

    struct Meta {
        explicit Meta(int pos) : pos(pos) {}

        virtual explicit operator MetaType() const {
            return getMeta();
        }

        virtual MetaType getMeta() const {
            return Unknown;
        }

        virtual void display_symbol_table(int deepth, std::ostream &ostream) {
            while (deepth-- > 0) ostream << '\t';
            ostream << "pos: " << pos << '\t';
        };

        virtual ~Meta();

        int pos;
    };

    struct ValueMeta : public Meta {
        ValueMeta(int next, MetaType type, std::string value)
                : Meta(next), value(std::move(value)), type(type) {}

        MetaType getMeta() const override {
            return type;
        }

        MetaType type;
        std::string value;

        void display_symbol_table(int deepth, std::ostream &ostream) override {
            Meta::display_symbol_table(deepth, ostream);
            ostream << "variable: " << value << '\t';
        }
    };

    struct TypeMeta : public Meta {
        TypeMeta(int next, MetaType type, int width) : Meta(next), type(type), width(width) {}

        MetaType getMeta() const override {
            return type;
        }

        MetaType type;
        int width;

        void display_symbol_table(int deepth, std::ostream &ostream) override {
            Meta::display_symbol_table(deepth, ostream);
            ostream << "width: " << width << '\t';
        }
    };

    struct JmpMeta : public Meta {
        explicit JmpMeta(int pos) : Meta(pos) {}

        int jmp_to = -1;
    };


    struct BlockMeta : public Meta {
        explicit BlockMeta(int next, std::shared_ptr<BlockMeta> parent) : Meta(next), parent(std::move(parent)) {
            id = BlockId();
        }

        explicit operator MetaType() const override {
            return Block;
        }

        void display_symbol_table(int deepth, std::ostream &ostream) override {
            Meta::display_symbol_table(deepth, ostream);
            ostream << "next: " << next << '\t' << "block id: " << id;
            for (auto[label, meta]:metas) {
                ostream << std::endl;
                for (int i = deepth; i > 0; i--) { ostream << '\t'; }
                ostream << label << std::endl;
                meta->display_symbol_table(deepth + 1, ostream);
            }
        }

        template<class... T>
        auto push_back(T &&... t) {
            return metas.push_back(std::forward<T>(t)...);
        }

        template<class... T>
        auto emplace_back(T &&... t) {
            return metas.emplace_back(std::forward<T>(t)...);
        }

        auto back() {
            return metas.back();
        }

        std::shared_ptr<Romeo::Meta> find(const std::string &to_find) {
            auto found = std::find_if(metas.begin(), metas.end(), [&](auto &item) -> bool {
                return item.first == to_find;
            });

            if (found != metas.end()) return found->second;
            if (parent != nullptr)
                return parent->find(to_find);
            return nullptr;
        }

        bool check(const std::string &to_check) {
            auto found = std::find_if(metas.begin(), metas.end(), [&](auto &item) -> bool {
                return item.first == to_check;
            });
            return found != metas.end();
        }

        std::string getTmp() {
            return "tmp" + std::to_string(tmp++);
        }

        int id = 0;
        int next = -1;
        std::shared_ptr<BlockMeta> parent;
        std::deque<std::pair<std::string, std::shared_ptr<Romeo::Meta>>> metas;

    private:
        int tmp = 1;

    private:
        static int BlockId() {
            static int id = 0;
            return id++;
        }
    };

    struct FunctionMeta : public Meta {
        explicit FunctionMeta(int next) : Meta(next) {}

        MetaType getMeta() const override {
            return Function;
        }

        void display_symbol_table(int deepth, std::ostream &ostream) override {
            Meta::display_symbol_table(deepth, ostream);
            for (auto[label, meta]:args) {
                ostream << std::endl;
                for (int i = deepth; i > 0; i--) { ostream << '\t'; }
                ostream << "Arg: " << label << std::endl;
                meta->display_symbol_table(deepth + 1, ostream);
            }
            ostream << std::endl;
            block->display_symbol_table(deepth + 1, ostream);
        }

        template<class... T>
        auto args_push_front(T &&... t) {
            return args.push_front(std::forward<T>(t)...);
        }

        template<class... T>
        auto args_push_back(T &&... t) {
            return args.push_back(std::forward<T>(t)...);
        }

        template<class... T>
        auto args_emplace_back(T &&... t) {
            return args.emplace_back(std::forward<T>(t)...);
        }

        std::shared_ptr<Romeo::BlockMeta> block;
        std::shared_ptr<Romeo::Meta> res;
        std::deque<std::pair<std::string, std::shared_ptr<Romeo::Meta>>> args;
    };

    struct CtrlMeta : public Meta {
        enum Label {
            IF, ELSE, WHILE
        };

        explicit CtrlMeta(int next, Label label) : Meta(next), label(label) {}

        void display_symbol_table(int deepth, std::ostream &ostream) override {
            Meta::display_symbol_table(deepth, ostream);
            ostream << std::endl;
            block->display_symbol_table(deepth + 1, ostream);
        }

        MetaType getMeta() const override {
            return Logic;
        }

        Label label;
        std::shared_ptr<Romeo::BlockMeta> block;
    };

}
#endif //ROMEO_SYMBOLMANAGER_H
