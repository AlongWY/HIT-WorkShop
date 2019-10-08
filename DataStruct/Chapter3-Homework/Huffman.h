//
// Created by along on 17-11-11.
//

#ifndef PROJECT_HUFFMAN_H
#define PROJECT_HUFFMAN_H

#include <map>
#include <string>
#include <iostream>

class Huffman {
private:
    //做一些数据结构的预先声明
    struct Node;
    struct NodeComp;
    using NodePtr = Node *;
public:
    Huffman();
    ~Huffman();
    std::string encode(std::string &string);
    std::string decode(std::string &code);
    void printCode(std::ostream &out = std::cout);
    void encodeFile(std::string &in, std::string &out);
    void decodeFile(std::string &in, std::string &out);
private:
    Node *root;
    std::map<char, NodePtr> nodes;
    std::string encode(std::istream &data);
    void makeEmpty(Node *&node);
    void makeTree(std::string &data);
    void makeTree(std::istream &data);
    void makeTree(std::map<char, unsigned> wordsCount);
    std::string getCode(char data);
    std::map<char, unsigned> countWords(std::istream &data);
};

#endif //PROJECT_HUFFMAN_H
