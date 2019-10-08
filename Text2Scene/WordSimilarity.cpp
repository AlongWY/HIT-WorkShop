#include "WordSimilarity.h"

std::vector<std::string> WordsSimilarity::split(std::string str, const std::string separator)
{
    std::vector<std::string> result;
    int cutAt;
    while((cutAt = str.find_first_of(separator)) != (int)str.npos) {
        if(cutAt > 0) {
            result.push_back(str.substr(0, cutAt));
        }
        str = str.substr(cutAt + 1);
    }
    if(str.length() > 0) {
        result.push_back(str);
    }
    return result;
}

WordsSimilarity::WordsSimilarity(const std::string CiLinPath)
{
    //读入同义词词林
    std::ifstream CiLin(CiLinPath);
    if(!CiLin)
        exit(1);
    std::vector<std::string> contens;
    std::string aLine;
    while(getline(CiLin, aLine)) {
        contens.push_back(aLine);
    }
    for(std::string conten : contens) {
        std::vector<std::string> strs = split(conten, " ");
        std::string encode;
        int length = strs.size();
        if(length > 1) {
            encode = strs[0]; //获取编码
        }

        std::vector<std::string> encodeWords_values;
        for(int i = 1; i < length; i++) {
            encodeWords_values.push_back(strs[i]);
        }

        //以编码为Key,其后所有值为Value
        encodeWords.insert({ encode, encodeWords_values });

        for(int i = 1; i < length; i++) {
            std::string key = strs[i];
            if(wordsEncode.find(key) != wordsEncode.end()) {
                //如果已经存在这个词，则添加其编码
                wordsEncode[key].push_back(encode);
            } else {
                std::vector<std::string> temp;
                temp.push_back(encode);
                wordsEncode.insert({ key, temp });
            }
        }
    }
}

std::string WordsSimilarity::getCommonStr(const std::string encode1, const std::string encode2)
{
    int length = encode1.length();
    std::string commonstr;
    for(int i = 0; i != length; i++) {
        if(encode1.at(i) == encode2.at(i)) {
            char lastchar = encode1.at(i);
            commonstr += lastchar;
        } else {
            break;
        }
    }
    int commonstrLen = commonstr.length();
    //注意第三层和第五层均有两个字符，所以长度不可能出现3和6的情况
    if(commonstrLen == 3 || commonstrLen == 6) {
        commonstr.pop_back();
    }
    return commonstr;
}

int WordsSimilarity::getCount(const std::string encodeHead, int cmpLen)
{
    std::set<std::string> res;
    for(auto curr_iterater : encodeWords) {
        std::string curr = curr_iterater.first;
        if(!curr.compare(0, encodeHead.length(), encodeHead, 0, encodeHead.length())) {
            std::string temp = curr.substr(0, cmpLen);
            if(res.find(temp) != res.end()) {
                continue;
            } else {
                res.insert(temp);
            }
        }
    }
    return res.size();
}

int WordsSimilarity::getN(const std::string encodeHead)
{
    int length = encodeHead.length();
    switch(length) {
    case 1:
        return getCount(encodeHead, 2);
    case 2:
        return getCount(encodeHead, 4);
    case 4:
        return getCount(encodeHead, 5);
    case 5:
        return getCount(encodeHead, 7);
    default:
        return 0;
    }
}

int WordsSimilarity::getK(const std::string encode1, const std::string encode2)
{
    std::string temp1 = encode1.substr(0, 1);
    std::string temp2 = encode2.substr(0, 1);
    if(stringCompareIgnoreCase(temp1, temp2)) {
        temp1 = encode1.substr(1, 1);
        temp2 = encode2.substr(1, 1);
    } else {
        return abs(temp1.at(0) - temp2.at(0));
    }
    if(stringCompareIgnoreCase(temp1, temp2)) {
        temp1 = encode1.substr(2, 2);
        temp2 = encode2.substr(2, 2);
    } else {
        return abs(temp1.at(0) - temp2.at(0));
    }
    if(stringCompareIgnoreCase(temp1, temp2)) {
        temp1 = encode1.substr(4, 1);
        temp2 = encode2.substr(4, 1);
    } else {
        return abs(stoi(temp1) - stoi(temp2));
    }
    if(stringCompareIgnoreCase(temp1, temp2)) {
        temp1 = encode1.substr(5, 2);
        temp2 = encode2.substr(5, 2);
    } else {
        return abs(temp1.at(0) - temp2.at(0));
    }
    return abs(stoi(temp1) - stoi(temp2));
}

bool WordsSimilarity::isIndependent(const std::string source)
{
    for(auto iter : wordsEncode) {
        std::string key = iter.first;
        if(stringCompareIgnoreCase(key, source)) {
            std::vector<std::string> values = wordsEncode.find(key)->second;
            for(std::string value : values) {
                if(value.back() == '@') {
                    return true;
                }
            }
        }
    }
    return false;
}

double WordsSimilarity::getSimilairty(const std::string word1, const std::string word2)
{
    //无论比较词在不在词林里，若是二者完全相同，相似度必然为一
    if(word1 == word2) {
        return 1;
    }

    //如果比较词没有出现在同义词词林中，则相似度为0
    if(wordsEncode.find(word1) == wordsEncode.end() || (wordsEncode.find(word2) == wordsEncode.end())) {
        return 0;
    }
    //获取比较词的编码
    std::vector<std::string> encode1 = getEncode(word1);
    std::vector<std::string> encode2 = getEncode(word2);

    //最终的计算结果值，取所有相似度里面结果最大的那个
    double maxValue = 0;

    for(std::string e1 : encode1) {
        for(std::string e2 : encode2) {
#ifndef NDEBUG
            std::cout << e1 << ' ' << e2 << std::endl;
#endif
            std::string commonStr = getCommonStr(e1, e2);
            int length = commonStr.length();
            double k = getK(e1, e2);
            double n = getN(commonStr);
#ifndef NDEBUG
            std::cout << "k:" << k << " n:" << n << " length:" << length << std::endl;
#endif
            double res = 0;
            //如果有一个"@"，那么表示自我封闭，肯定不在一棵树上，直接返回f
            if(e1.back() == '@' || e2.back() == '@' || 0 == length) {
                if(f > maxValue) {
                    maxValue = res;
                }
                continue;
            }
            if(1 == length) {
                //说明在第二层上计算
                res = fabs(a * cos(n * PI / degrees) * ((n - k + 1) / n));
            } else if(2 == length) {
                //说明在第三层上计算
                res = fabs(b * cos(n * PI / degrees) * ((n - k + 1) / n));
            } else if(4 == length) {
                //说明在第四层上计算
                res = fabs(c * cos(n * PI / degrees) * ((n - k + 1) / n));
            } else if(5 == length) {
                //说明在第五层上计算
                res = fabs(d * cos(n * PI / degrees) * ((n - k + 1) / n));
            } else {
                //注意不存在前面七个字符相同，而结尾不同的情况，所以这个分支一定是八个字符都相同，那么只需要比较最后一个字符
                if(e1.back() == '=' && e2.back() == '=') {
                    //说明两个完全相同
                    res = 1;
                } else if(e1.back() == '#' && e2.back() == '#') {
                    //只有结尾不同，说明结尾是'#'
                    res = e;
                }
            }
            if(res > maxValue) {
                maxValue = res;
            }
        }
    }
    return maxValue;
}