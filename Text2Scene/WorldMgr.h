#ifndef _WORLDPOS_H_
#define _WORLDPOS_H_

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <wx/string.h>

namespace ttp
{

//物体
class Object
{
public:
    Object(sf::Sprite sprite, sf::Vector2f pos);
    Object(sf::Sprite sprite, float x, float y);
    ~Object(){};
    sf::Vector2f pos;
    sf::Sprite sprite;
};

//视口管理器
class WorldMgr : public sf::Drawable
{
public:
    static WorldMgr& Instance()
    {
        static WorldMgr world;
        return world;
    }
    void setPos(const sf::Vector2f Cemera);
    //深度排序
    void Sort();
    std::vector<Object> objects;

private:
    WorldMgr(){};
    ~WorldMgr(){};
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        for(auto& obj : objects) {
            target.draw(obj.sprite, states);
        }
    };
};

//资源管理器
class ResMgr
{
public:
    static ResMgr& Instance()
    {
        static ResMgr _resmgr;
        return _resmgr;
    };
    sf::Texture* find(int ID);

private:
    ResMgr();
    ~ResMgr();
    std::map<int, sf::Texture*> texmap;
};
}
#endif