#include "ObjectSimilarity.h"

const double k_obj_sim = 0.9;
const double k_fea_sim = 0.1;

//获取相似度
double ObjSimilarity::getSimilairty(const std::string words1, const std::string words2)
{
    //文本完全相同相似度必然为1
    if(words1 == words2) {
        return 1;
    }

    double result = 0;
    TagWords Tags1 = CutTag(words1);
    TagWords Tags2 = CutTag(words2);

    //接下来将以短的为基准，计算二者相似度
    if(Tags1.size() > Tags2.size()) {
        swap(Tags1, Tags2);
    }

    result += k_obj_sim * m_wordsim.getSimilairty((Tags1.end() - 1)->first, (Tags2.end() - 1)->first);
    //基部部分比对完毕，删除
    Tags1.pop_back();
    Tags2.pop_back();
    const double k_curr_fea = k_fea_sim / Tags1.size();

    for(auto dep : Tags1) {
        //取得各各特征比对的最大值
        double max_sim = 0;
        double curr_sim = 0;
        for(auto cmp : Tags2) {
            curr_sim = m_wordsim.getSimilairty(dep.first, cmp.first);
            max_sim = max_sim > curr_sim ? max_sim : curr_sim;
//                        std::cout << dep.first << "------" << cmp.first << ":" << curr_sim << std::endl;
        }
        result += max_sim * k_curr_fea;
//                std::cout << "Final:" << result << std::endl;
    }
    return result;
};

//拆分并进行词性标注
ObjSimilarity::TagWords ObjSimilarity::CutTag(const std::string sentence)
{
    TagWords result;
    m_wordseg.Tag(sentence, result);
    return result;
};
