#include "MainFrame.h"
#include "wx/textctrl.h"
#include "DisplayCanvas.h"
#include <wx/regex.h>
#include <cstdlib>

const int ID_TEXTCTRL = 101;
const int ID_Apply = 102;

struct objinfo {
    objinfo(std::string _name, float _x, float _y);
    objinfo(){};
    std::string name;
    float x;
    float y;
};

std::vector<objinfo> TextProcess(wxString TextString);

MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr,
          wxID_ANY,
          title,
          wxDefaultPosition,
          wxSize(1400, 755),
          wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
    , m_ResMgr(ttp::ResMgr::Instance())
    , m_World(ttp::WorldMgr::Instance())
{
//    SetIcon(wxICON(mainIcon));
    m_TextPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    m_TextGet =
        new wxTextCtrl(m_TextPanel, ID_TEXTCTRL, wxEmptyString, wxPoint(910, 0), wxSize(480, 680), wxTE_MULTILINE);
    m_ApplyButton = new wxButton(m_TextPanel, ID_Apply, wxT("解析"), wxPoint(910, 680), wxSize(480, 40));
    m_Canvas = new DisplayCanvas(m_TextPanel, wxID_ANY, wxPoint(0, 0), wxSize(900, 720));
    Centre();
}

void MainFrame::OnApply(wxCommandEvent& event)
{
    //清空容器，重新解析
    m_World.objects.clear();

    //获取文本输入框里的内容
    wxString TextString;
    TextString = m_TextGet->GetValue();

    //文本关键信息填充
    std::vector<objinfo> objvec = TextProcess(TextString);

    //通过获得的信息寻找并初始化要显示的图片
    for(auto& a : objvec) {
        printf("%10s  %10.0f %10.0f\n", a.name.c_str(), a.x, a.y);

        int ID = TextToID::Handle().find(a.name);

        sf::Texture* texture = m_ResMgr.find(ID);
        if(texture != nullptr) {
            sf::Sprite sprite(*texture);
            ttp::Object obj(sprite, a.x, a.y);
            m_World.objects.push_back(obj);
        }
    }

    m_World.Sort();
    sf::View view = m_Canvas->getDefaultView();
    view.setCenter(0, 0);
    m_Canvas->setView(view);
    m_Canvas->Cemera.x = 0;
    m_Canvas->Cemera.y = 0;
}

objinfo::objinfo(std::string _name, float _x, float _y)
    : name(_name)
    , x(_x)
    , y(_y){};

std::vector<objinfo> TextProcess(wxString TextString)
{

    wxRegEx reg_sentence(wxT("([[:alpha:]]+)[在]([[:alpha:]]+)[的]([上|下|左|右|前|后])[方|面]?([[:digit:]].?[[:digit:]"
                             "]*)([[:alpha:]]+)。"));
    //储存信息
    std::vector<objinfo> objvec;
    bool isfirst = true;

    //处理每一句话
    while(reg_sentence.Matches(TextString)) {
        size_t start, len;
        reg_sentence.GetMatch(&start, &len, 0);
        std::string tofind = reg_sentence.GetMatch(TextString, 1).ToStdString();
        std::string deps = reg_sentence.GetMatch(TextString, 2).ToStdString();
        std::string faceto = reg_sentence.GetMatch(TextString, 3).ToStdString();
        float distance = std::atof(reg_sentence.GetMatch(TextString, 4).ToStdString().c_str());
        distance *= 100;

        std::cout << "TextMatch:" << reg_sentence.GetMatch(TextString, 0) << std::endl;
        if(isfirst) {
            objvec.push_back(objinfo(deps, 0, 0));
            isfirst = false;
        }

        objinfo b;
        b.name = tofind;
        double max_sim = 0;
        for(auto& a : objvec) {
            if(a.name == deps) {
                b.x = a.x;
                b.y = a.y;
                break;
            } else {
                double curr_sim = TextToID::Handle().m_sim.getSimilairty(a.name, deps);
                if(curr_sim > max_sim) {
                    max_sim = curr_sim;
                    b.x = a.x;
                    b.y = a.y;
                }
            }
        }

        if(faceto == ((wxString)(wxT("上"))).ToStdString() || faceto == ((wxString)(wxT("前"))).ToStdString()) {
            b.y -= distance;
        } else if(faceto == ((wxString)(wxT("下"))).ToStdString() || faceto == ((wxString)(wxT("后"))).ToStdString()) {
            b.y += distance;
        } else if(faceto == ((wxString)(wxT("左"))).ToStdString()) {
            b.x -= distance;
        } else if(faceto == ((wxString)(wxT("右"))).ToStdString()) {
            b.x += distance;
        }
        objvec.push_back(b);
        TextString = TextString.Mid(start + len);
    }

    return objvec;
}

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_BUTTON(ID_Apply, MainFrame::OnApply)
END_EVENT_TABLE()
