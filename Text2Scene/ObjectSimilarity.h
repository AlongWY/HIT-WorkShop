#ifndef _H_OBJ_SIMILARITY_
#define _H_OBJ_SIMILARITY_

#include "WordSimilarity.h"
#include <cppjieba/Jieba.hpp>

class ObjSimilarity
{
    typedef std::vector<std::pair<std::string, std::string> > TagWords;

public:
    ObjSimilarity(const std::string& CiLinPath,
        const std::string& dict_path,
        const std::string& model_path,
        const std::string& user_dict_path,
        const std::string& idfPath,
        const std::string& stopWordPath)
        : m_wordseg(dict_path, model_path, user_dict_path, idfPath, stopWordPath)
        , m_wordsim(CiLinPath){};
    ~ObjSimilarity(){};
    /**
     * @brief 获取两个特征文本的相似度
     * @param words1
     * @param words2
     * @return 两个特征文本的相似度
     */
    double getSimilairty(const std::string words1, const std::string words2);
    TagWords CutTag(const std::string sentence);

    cppjieba::Jieba m_wordseg;
    WordsSimilarity m_wordsim;
};

#endif