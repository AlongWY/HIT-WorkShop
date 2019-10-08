//
// Created by along on 17-11-11.
//

#include "Huffman.h"
#include <fstream>
#include <sstream>
#include <bitset>
#include <queue>

using namespace std;

struct Huffman::Node {
    bool leaf;
    char data;
    unsigned weight;
    Node *parent;
    Node *left;
    Node *right;
    Node(char _data, unsigned _weight, Node *_left, Node *_right, bool _leaf)
        : data(_data), weight(_weight), left(_left), right(_right), leaf(_leaf), parent(nullptr) {};
    Node(char _data, unsigned _weight, bool _leaf)
        : data(_data), weight(_weight), left(nullptr), right(nullptr), leaf(_leaf), parent(nullptr) {};
    bool operator<(const Node &rhs) const { return weight < rhs.weight; }
    bool operator==(const Node &rhs) const { return leaf == rhs.leaf && data == rhs.data; }
    bool operator!=(const Node &rhs) const { return !(rhs == *this); }
};

struct Huffman::NodeComp {
    bool operator()(const Node *left, const Node *right) const {
        return *right < *left;
    }
};

void Huffman::makeTree(map<char, unsigned> wordsCount) {
    priority_queue<Node *, vector<Node *>, NodeComp> heap;
    //将统计数据存入优先队列
    for (auto &word:wordsCount) {
        auto theNode = new Node(word.first, word.second, true);
        nodes.insert({word.first, theNode});
        heap.push(theNode);
    }
    wordsCount.clear();

    //构建huffman树
    while (!heap.empty()) {
        Node *first = heap.top();
        heap.pop();
        if (heap.empty()) {
            root = first;
            break;
        }
        Node *second = heap.top();
        heap.pop();
        auto *parentNode = new Node(' ', first->weight + second->weight, first, second, false);
        first->parent = parentNode;
        second->parent = parentNode;
        heap.push(parentNode);
    }
}

string Huffman::getCode(char data) {
    auto node = nodes[data];
    string code;
    while (node != root) {
        code.insert(0, node == node->parent->left ? "0" : "1");
        node = node->parent;
    }
    return code;
}

string Huffman::encode(string &data) {
    makeTree(data);
    stringstream sStream(data);
    return encode(sStream);
}

string Huffman::decode(string &code) {
    string data;
    stringstream sStream(code);
    char isOne;
    const Node *node = root;
    while (sStream >> isOne) {
        node = isOne == '0' ? node->left : node->right;
        if (node->leaf) {
            data += node->data;
            node = root;
        }
    }
    return data;
}

void Huffman::encodeFile(string &in, string &out) {
    ifstream inFile(in);
    ofstream outFile(out, ios::binary);
    if (!inFile) {
        cerr << "Cant Open the File:" << in << endl;
        exit(EXIT_FAILURE);
    }
    if (!outFile) {
        cerr << "Cant Open the File:" << out << endl;
        exit(EXIT_FAILURE);
    }

    makeTree(inFile);
    string code = encode(inFile);

    //写入huffman重建数据
    unsigned long size = nodes.size();
    outFile.write((char *) &size, sizeof(size));
    for (auto &pair:nodes) {
        outFile.write(&pair.first, sizeof(char));
        outFile.write((char *) &pair.second->weight, sizeof(unsigned));
    }

    //写入存储数据
    size = code.size();
    outFile.write((char *) &size, sizeof(size));
    unsigned long codeToSave;
    for (unsigned long i = 0; i != (size / 64 * 64 + 64) - size; ++i)
        code += '0';
    for (unsigned long i = 0; i <= size / 64; ++i) {
        bitset<64> toWriteByte(code.substr(i * 64, 64));
        codeToSave = toWriteByte.to_ulong();
        outFile.write((char *) &codeToSave, sizeof(unsigned long));
    }

    inFile.close();
    outFile.close();
}

void Huffman::decodeFile(string &in, std::string &out) {
    ifstream file(in, ios::binary);
    ofstream outFile(out);
    if (!file) {
        cerr << "Cant Open the File:" << in << endl;
        exit(EXIT_FAILURE);
    }
    unsigned long mapSize;
    unsigned weight;
    char aChar;
    std::map<char, unsigned> wordsCount;
    file.read((char *) &mapSize, sizeof(unsigned long));
    for (int i = 0; i != mapSize; ++i) {
        file.read(&aChar, sizeof(char));
        file.read((char *) &weight, sizeof(unsigned));
        wordsCount.insert({aChar, weight});
    }
    makeTree(wordsCount);

    unsigned long codeSize, someCode;
    string code;
    file.read((char *) &codeSize, sizeof(unsigned long));
    for (unsigned long i = 0; i <= codeSize / 64; ++i) {
        file.read((char *) &someCode, sizeof(unsigned long));
        bitset<64> readByte(someCode);
        code += readByte.to_string();
    }

    code.resize(codeSize);
    stringstream sStream(code);
    outFile << decode(code);
}

void Huffman::makeEmpty(Huffman::Node *&node) {
    if (node == nullptr)
        return;
    makeEmpty(node->left);
    makeEmpty(node->right);
    delete node;
    node = nullptr;
}

Huffman::~Huffman() { makeEmpty(root); }

Huffman::Huffman() : root(nullptr) {}

void Huffman::makeTree(std::string &data) {
    std::stringstream sStream(data);
    auto wordCounts = countWords(sStream);
    makeTree(wordCounts);
}

void Huffman::printCode(std::ostream &out) {
    unsigned long total = 0;
    for (auto &pair:nodes) {
        total += pair.second->weight;
    }
    for (auto &pair:nodes) {
        out << pair.first << ":" << (double) (pair.second->weight) / total << ":" << getCode(pair.first) << endl;
    }
}

std::map<char, unsigned> Huffman::countWords(std::istream &data) {
    char word;
    map<char, unsigned> wordCounts;
    //重置流状态
    data.clear();
    data.seekg(0, ios::beg);
    while (data.get(word)) {
        if (wordCounts.find(word) == wordCounts.end())
            wordCounts[word] = 0;
        ++wordCounts[word];
    }
    return wordCounts;
}

void Huffman::makeTree(std::istream &data) {
    auto wordCounts = countWords(data);
    makeTree(wordCounts);
}

std::string Huffman::encode(std::istream &data) {
    char aChar;
    string code;
    //重置流状态
    data.clear();
    data.seekg(0, ios::beg);
    while (data.get(aChar)) {
        code.append(getCode(aChar));
    }
    return code;
}

