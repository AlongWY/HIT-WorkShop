//
// Created by along on 17-12-18.
//

#ifndef PROJECT_SORT_H
#define PROJECT_SORT_H

#include <vector>

using ulong = unsigned long;

template<typename Comparable>
void BubbleSort(std::vector<Comparable> &data) {
    ulong length = data.size();
    for (ulong i = 0; i != length; ++i)
        for (ulong j = length - 1; j != i; --j)
            if (data[j] < data[j - 1])
                std::swap(data[j], data[j - 1]);
}

template<typename Comparable>
const Comparable &GetPivot(std::vector<Comparable> &data, ulong left, ulong right) {
    ulong center = (left + right) / 2;

    // 使得 data[left] < data[center] < data[right]
    if (data[center] < data[left])
        std::swap(data[left], data[center]);
    if (data[right] < data[left])
        std::swap(data[left], data[right]);
    if (data[right] < data[center])
        std::swap(data[center], data[right]);

    // 将枢纽元置于right-1处
    std::swap(data[center], data[right - 1]);
    return data[right - 1];
}

template<typename Comparable>
void QuickSort(std::vector<Comparable> &data, int left, int right) {
    if (left < right) {
        const Comparable &pivot = GetPivot(data, left, right);

        // 根据枢纽元进行分割
        int i = left, j = right - 1;
        while (i < j) {
            while (data[++i] < pivot);
            while (pivot < data[--j]);
            if (i < j) {
                std::swap(data[i], data[j]);
            }
        }

        std::swap(data[i], data[right - 1]);
        QuickSort(data, left, i - 1);
        QuickSort(data, i + 1, right);
    }
}

template<typename Comparable>
void QuickSort(std::vector<Comparable> &data) {
    QuickSort(data, 0, (int) (data.size() - 1));
}

template<typename Comparable>
int BinSearchR(std::vector<Comparable> &data, int start, int end, Comparable key) {
    if (start > end)
        return -1;
    int mid = start + (end - start) / 2;
    if (data[mid] > key)
        return BinSearchR(data, start, mid - 1, key);
    else if (data[mid] < key)
        return BinSearchR(data, mid + 1, end, key);
    else
        return mid;
}

template<typename Comparable>
int BinSearch(std::vector<Comparable> &data, int start, int end, Comparable key) {
    for (int mid = start + (end - start) / 2; start <= end; mid = start + (end - start) / 2) {
        if (data[mid] < key) {
            start = mid + 1;
        } else if (data[mid] > key) {
            end = mid - 1;
        } else {
            return mid;
        }
    }
    return -1;
}

#endif //PROJECT_SORT_H
