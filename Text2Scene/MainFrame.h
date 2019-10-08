#ifndef _MWX_INCLUDE_FRAME_H_
#define _MWX_INCLUDE_FRAME_H_

#include <wx/wx.h>
#include <string>
#include "DisplayCanvas.h"
#include "StringToID.h"


class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title);
    //按下按钮
    void OnApply(wxCommandEvent& event);
    //文本框内容改变应当做的事
    void OnTextChange(wxCommandEvent& event);

private:
    //资源管理器
    ttp::ResMgr &m_ResMgr;
    //世界管理器
    ttp::WorldMgr &m_World;
    //面板
    wxPanel* m_TextPanel;
    //文本输入框
    wxTextCtrl* m_TextGet;
    //按钮
    wxButton* m_ApplyButton;
    //绘制框
    DisplayCanvas* m_Canvas;
    DECLARE_EVENT_TABLE()
};

#endif