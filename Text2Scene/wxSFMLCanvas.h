#ifndef _MWX_INCLUDE_SFML_H_
#define _MWX_INCLUDE_SFML_H_

#include <wx/wx.h>
#include <SFML/Graphics.hpp>

class wxSFMLCanvas : public wxControl, public sf::RenderWindow {
public:
    explicit wxSFMLCanvas(wxWindow *Parent = NULL,
                          wxWindowID Id = -1,
                          const wxPoint &Position = wxDefaultPosition,
                          const wxSize &Size = wxDefaultSize,
                          long Style = 0);

    ~wxSFMLCanvas() override;

private:
DECLARE_EVENT_TABLE()

    virtual void OnUpdate();

    void OnIdle(wxIdleEvent &);

    void OnPaint(wxPaintEvent &);

    void OnEraseBackground(wxEraseEvent &);
};

#endif