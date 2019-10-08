#include "wxSFMLCanvas.h"
#include <SFML/Graphics.hpp>
#include <wx/wx.h>

#ifdef __WXGTK__

#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#endif

BEGIN_EVENT_TABLE(wxSFMLCanvas, wxControl)
                EVT_IDLE(wxSFMLCanvas::OnIdle)
                EVT_PAINT(wxSFMLCanvas::OnPaint)
                EVT_ERASE_BACKGROUND(wxSFMLCanvas::OnEraseBackground)
END_EVENT_TABLE()

wxSFMLCanvas::wxSFMLCanvas(wxWindow *Parent, wxWindowID Id, const wxPoint &Position, const wxSize &Size, long Style)
        : wxControl(Parent, Id, Position, Size, Style) {
#ifdef __WXGTK__
    GtkWidget *widget = m_wxwindow;
    if (!GTK_WIDGET_REALIZED(widget)) {
        gtk_widget_realize(widget);
    }
    GdkWindow *gdkWin = widget->window;
    sf::RenderWindow::create(GDK_WINDOW_XID(widget->window));;

#else
    sf::RenderWindow::create(GetHandle());
#endif
}

wxSFMLCanvas::~wxSFMLCanvas() {
}

void wxSFMLCanvas::OnUpdate() {
}

void wxSFMLCanvas::OnIdle(wxIdleEvent &) {
    Refresh();
}

void wxSFMLCanvas::OnPaint(wxPaintEvent &) {
    wxPaintDC Dc(this);
    OnUpdate();
    display();
}

void wxSFMLCanvas::OnEraseBackground(wxEraseEvent &) {
}
