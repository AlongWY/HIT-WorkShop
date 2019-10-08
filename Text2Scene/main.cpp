#include <wx/wx.h>
#include "MainFrame.h"

class ConvertPlatform : public wxApp
{
public:
    virtual bool OnInit();
};

bool ConvertPlatform::OnInit()
{
    //程序标题
    MainFrame* frame = new MainFrame(wxT("文景转换工具"));
    frame->Show(true);
    return true;
}

IMPLEMENT_APP(ConvertPlatform)