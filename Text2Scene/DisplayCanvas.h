#ifndef _WX_DisplayCanvas_HPP_
#define _WX_DisplayCanvas_HPP_

#include "wxSFMLCanvas.h"
#include <SFML/Audio.hpp>
#include "WorldMgr.h"

class DisplayCanvas : public wxSFMLCanvas
{
public:
    DisplayCanvas(wxWindow* Parent, wxWindowID Id, const wxPoint& Position, const wxSize& Size, long Style = 0)
        : wxSFMLCanvas(Parent, Id, Position, Size, Style)
        , m_World(ttp::WorldMgr::Instance())
        ,Cemera(0,0)
        {};
    ~DisplayCanvas() override {};
    sf::Vector2f Cemera;

private:
    ttp::WorldMgr& m_World;

    void OnUpdate() override;
    void OnKeyDown(sf::Vector2f &Cemera);
};

#endif