/**
 * Copyright (c) 2019 along All rights reserved.
 **/

#ifndef ROMEO_UTILITY_H
#define ROMEO_UTILITY_H

#include <string>
#include <vector>
#include <tuple>


template<template<class, class, class...> class C, typename K, typename V, typename... Args>
V GetWithDef(const C<K, V, Args...> &m, K const &key, const V &defval) {
    auto it = m.find(key);
    if (it == m.end()) return defval;
    return it->second;
}

template<typename T,
        typename TIter = decltype(std::begin(std::declval<T>())),
        typename = decltype(std::end(std::declval<T>()))>
constexpr auto enumerate(T &&iterable) {
    struct iterator {
        size_t i;
        TIter iter;

        bool operator!=(const iterator &other) const { return iter != other.iter; }

        void operator++() {
            ++i;
            ++iter;
        }

        auto operator*() const { return std::tie(i, *iter); }
    };
    struct iterable_wrapper {
        T iterable;

        auto begin() { return iterator{0, std::begin(iterable)}; }

        auto end() { return iterator{0, std::end(iterable)}; }
    };
    return iterable_wrapper{std::forward<T>(iterable)};
}

void split(const std::string &s, std::vector<std::string> &v, const std::string &c);

#endif //ROMEO_UTILITY_H
