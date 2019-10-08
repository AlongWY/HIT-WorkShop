#ifndef _H_STRINGTOID_
#define _H_STRINGTOID_

#include "ObjectSimilarity.h"

const char* const CILIN_PATH = "./dict/CiLin.txt";
const char* const DICT_PATH = "./dict/jieba.dict.utf8";
const char* const HMM_PATH = "./dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "./dict/user.dict.utf8";
const char* const IDF_PATH = "./dict/idf.utf8";
const char* const STOP_WORD_PATH = "./dict/stop_words.utf8";

class TextToID
{
private:
    TextToID()
        : m_sim(CILIN_PATH, DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH)
    {
        std::ifstream res("../Data/Res.txt");
        int ID;
        std::string path;
        std::string this_line;
        std::string obj;

        while(getline(res, this_line)) {
            std::stringstream stream(this_line);
            stream >> path >> ID >> obj;
            m_objs.insert({ obj, ID });
            m_IDs.insert({ID, obj });
        }
        res.close();
    };
    ~TextToID(){};
    std::map<std::string, int> m_objs;
    std::map<int, std::string> m_IDs;

public:
    ObjSimilarity m_sim;
    static TextToID& Handle()
    {
        static TextToID m_ID;
        return m_ID;
    }

    int find(std::string obj)
    {
        auto iter = m_objs.find(obj);
        if(iter != m_objs.end())
            return iter->second;
        std::pair<double, int> ID;
        ID.first = 0;
        ID.second = 1;
        for(auto iter : m_objs) {
            double currSim = m_sim.getSimilairty(obj, iter.first);
            if(currSim > ID.first) {
                ID.first = currSim;
                ID.second = iter.second;
            }
        }
        return ID.second;
    };
    std::string find(int ID)
    {
        auto iter = m_IDs.find(ID);
        if(iter != m_IDs.end())
            return iter->second;
        else
            return "NO DATA!!!";
    };
};

#endif