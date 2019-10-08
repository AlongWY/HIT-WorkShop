//
// Created by along on 17-11-21.
//
#include <iostream>
#include <fstream>
#include "Huffman.h"

int main(int argv, char **argc) {
    Huffman encoder, decoder;
    std::string encodeIn("encodeIn");
    std::string encodeOut("encodeOut");
    encoder.encodeFile(encodeIn, encodeOut);
    encoder.printCode();
    std::cout << std::endl;
    std::string decodeIn(encodeOut);
    std::string decodeOut("decodeOut");
    decoder.decodeFile(decodeIn, decodeOut);
//    decoder.printCode();

    long before, after;

    std::fstream File(encodeIn);
    File.seekg(0, std::ios::end);
    std::cout << encodeIn << "文件大小:" << (before = File.tellg()) << " Bytes" << std::endl;
    File.close();
    File.clear();

    File.open(encodeOut);
    File.seekg(0, std::ios::end);
    std::cout << encodeIn << "文件大小:" << (after = File.tellg()) << " Bytes" << std::endl;
    File.close();

    std::cout << "压缩率:" << (float) after / before << std::endl;

    return 0;
}