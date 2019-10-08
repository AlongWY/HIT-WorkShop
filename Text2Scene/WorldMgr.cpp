#include "WorldMgr.h"
#include "StringToID.h"
const double k_scale = 0.0005; //放缩比例
const double d_screen = 500;
namespace ttp
{
inline bool sort_by_pos(const Object& obj1, const Object& obj2)
{
    return obj1.pos.y < obj2.pos.y;
};

void WorldMgr::setPos(const sf::Vector2f Cemera)
{
    // Object a;
    for(Object& a : objects) {
        double k_trans = (1.0 + k_scale * (a.pos.y - Cemera.y));
        const sf::Vector2f transpos(
            a.pos.x - a.sprite.getTextureRect().width / 2, a.pos.y - a.sprite.getTextureRect().height);
        if(k_trans < 1.5 && k_trans > 0.5) {
            a.sprite.setPosition((transpos.x - Cemera.x) * k_trans, (transpos.y - Cemera.y) * k_trans);
            a.sprite.setScale(k_trans, k_trans);
        } else
            a.sprite.setPosition((transpos.x - Cemera.x) * k_trans, (transpos.y - Cemera.y));
    }
}

void WorldMgr::Sort()
{
    sort(objects.begin(), objects.end(), sort_by_pos);
};

sf::Texture* ResMgr::find(int ID)
{
    auto ret = texmap.find(ID);
    if(ret == texmap.end()) {
        std::cout << "cant find" << ID << std::endl;
        return nullptr;
    }
    return ret->second;
};

ResMgr::ResMgr()
{
    std::ifstream res("../Data/Res.txt");
    int ID;
    std::string path;
    std::string ResForlder("../Data/");

    std::string this_line;
    std::string aobj;
    
    while(getline(res, this_line)) {
        std::stringstream stream(this_line);
        stream >> path >> ID >> aobj;
        std::cout << "Loading----" << ID << ":" << aobj << std::endl;
        sf::Texture* texture = new sf::Texture;
        texture->loadFromFile(ResForlder + path);
        texmap.insert({ ID, texture });
    }
    res.close();
}

ResMgr::~ResMgr()
{
    for(auto& a : texmap) {
        delete a.second;
    }
};

Object::Object(sf::Sprite sprite, sf::Vector2f pos)
    : sprite(sprite)
    , pos(pos){};

Object::Object(sf::Sprite sprite, float x, float y)
    : sprite(sprite)
    , pos(x, y){};
}