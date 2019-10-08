#ifndef _H_SIMILARITY_
#define _H_SIMILARITY_
#include <set>
#include <cmath>
#include <map>
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include <iostream>

class WordsSimilarity
{
public:
    ~WordsSimilarity(){};
    WordsSimilarity(std::string CiLinPath);
    /**
     * @brief 返回两个词的相似度
     * @param 比较词1
     * @param 比较词2
     * @return 返回两个词的相似度
     */
    double getSimilairty(const std::string word1, const std::string word2);
    std::vector<std::string> split(std::string str, const std::string separator);

private:
    //相似度计算系数定义
    const double a = 0.65;
    const double b = 0.8;
    const double c = 0.9;
    const double d = 0.96;
    const double e = 0.5;
    const double f = 0.1;
    const double PI = 3.1415926535898;
    const double degrees = 180;

private:
    //存放的是以词为key，以该词的编码为values的List集合，其中一个词可能会有多个编码
    std::map<std::string, std::vector<std::string> > wordsEncode;
    //存放的是以编码为key，以该编码多对应的词为values的List集合，其中一个编码可能会有多个词
    std::map<std::string, std::vector<std::string> > encodeWords;

private:
    bool stringCompareIgnoreCase(const std::string lhs, const std::string rhs)
    {
        return !strcasecmp(lhs.c_str(), rhs.c_str());
    }
    std::vector<std::string> getEncode(const std::string word)
    {
        return wordsEncode.find(word)->second;
    };
    std::string getCommonStr(const std::string encode1, const std::string encode2);
    int getCount(const std::string encodeHead, const int cmpLen);
    bool isIndependent(const std::string source);
    int getN(const std::string encodeHead);
    int getK(const std::string encode1, const std::string encode2);
};
#endif
