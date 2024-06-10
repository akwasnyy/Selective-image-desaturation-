#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/radiobox.h>
#include <wx/slider.h>
#include <thread>
class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
    void OnOpen(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnSliderChange(wxCommandEvent& event);

    

private:
    wxDECLARE_EVENT_TABLE();
    

    wxSlider* brightnessSlider;
    wxSlider* rSlider;
    wxSlider* gSlider;
    wxSlider* bSlider;

    cv::Mat original;
    cv::Mat desaturated;

    wxStaticText* brightness_txt;
    wxStaticText* r_txt;
    wxStaticText* g_txt;
    wxStaticText* b_txt;
};



enum {
    ID_Open = 1,
    ID_Save,
    ID_Brightness=100,
    ID_R,
    ID_G,
    ID_B
    
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_Open, MyFrame::OnOpen)
EVT_MENU(ID_Save, MyFrame::OnSave)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
EVT_SLIDER(ID_Brightness, MyFrame::OnSliderChange)
EVT_SLIDER(ID_R, MyFrame::OnSliderChange)
EVT_SLIDER(ID_G, MyFrame::OnSliderChange)
EVT_SLIDER(ID_B, MyFrame::OnSliderChange)
wxEND_EVENT_TABLE()



wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    wxInitAllImageHandlers(); // Inicjalizacja obs³ugi ró¿nych formatów obrazów
    MyFrame* frame = new MyFrame("Selective Image Desaturation", wxPoint(50, 50), wxSize(450, 340));
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size) {
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_Open, "&Open...\tCtrl-O", "Open an image file");
    menuFile->Append(ID_Save, "&Save...\tCtrl-S", "Save the current image"); // Add Save option
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
   
    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    wxPanel* panel = new wxPanel(this, wxID_ANY);

    new wxStaticText(panel, wxID_ANY, "0", wxPoint(20, 45));
    brightnessSlider = new wxSlider(panel, ID_Brightness, 0, 0, 255, wxPoint(20, 20), wxSize(300, -1), wxSL_HORIZONTAL);
    new wxStaticText(panel, wxID_ANY, "255", wxPoint(310, 45));
    //new wxStaticText(panel, wxID_ANY, "Brightness", wxPoint(20, 5));
    brightness_txt= new wxStaticText(panel, wxID_ANY, "Brightness=0", wxPoint(20,5));

    new wxStaticText(panel, wxID_ANY, "0", wxPoint(20, 105));
    rSlider = new wxSlider(panel, ID_R, 0, 0, 255, wxPoint(20, 80), wxSize(300, -1), wxSL_HORIZONTAL);
    new wxStaticText(panel, wxID_ANY, "255", wxPoint(310, 105));
    //new wxStaticText(panel, wxID_ANY, "R", wxPoint(20, 65));
    r_txt = new wxStaticText(panel, wxID_ANY, "R=0", wxPoint(20,65));

    new wxStaticText(panel, wxID_ANY, "0", wxPoint(20, 165));
    gSlider = new wxSlider(panel, ID_G, 0, 0, 255, wxPoint(20, 140), wxSize(300, -1), wxSL_HORIZONTAL);
    new wxStaticText(panel, wxID_ANY, "255", wxPoint(310, 165));
    //new wxStaticText(panel, wxID_ANY, "G", wxPoint(20, 125));
    g_txt = new wxStaticText(panel, wxID_ANY, "G=0", wxPoint(20,125));

    new wxStaticText(panel, wxID_ANY, "0", wxPoint(20, 225));
    bSlider = new wxSlider(panel, ID_B, 0, 0, 255, wxPoint(20, 200), wxSize(300, -1), wxSL_HORIZONTAL);
    new wxStaticText(panel, wxID_ANY, "255", wxPoint(310, 225));
    //new wxStaticText(panel, wxID_ANY, "B", wxPoint(20, 185));
    b_txt = new wxStaticText(panel, wxID_ANY, "B=0", wxPoint(20,185));

    CreateStatusBar();
    SetStatusText("Selective Image Desaturation v0.2");
}




void MyFrame::OnSliderChange(wxCommandEvent& event) {
    int brightnessValue = brightnessSlider->GetValue();
    int rValue = rSlider->GetValue();
    int gValue = gSlider->GetValue();
    int bValue = bSlider->GetValue();
    //tutaj wrzucic logike
    brightness_txt->SetLabel(wxString::Format("Brightness=%d", brightnessValue));
    r_txt->SetLabel(wxString::Format("R=%d", rValue));
    g_txt->SetLabel(wxString::Format("G=%d", gValue));
    b_txt->SetLabel(wxString::Format("B=%d", bValue));
}

void MyFrame::OnOpen(wxCommandEvent& event) {
    wxString filePath = wxFileSelector("Choose an image file", "", "", "",
        "JPEG files (*.jpg)|*.jpg|All files (*.*)|*.*");

    if (!filePath.empty()) {
        original = cv::imread(filePath.ToStdString(), cv::IMREAD_COLOR);
        desaturated = original.clone();
        //zaimplementowac jakis display handler czy cos idk
        if (!desaturated.empty()) {
            cv::imshow("Original image", original);  // Pierwsze okno
            cv::imshow("Desaturated image", desaturated);  // Drugie okno
           // cv::waitKey(0); // Czekaj na naciœniêcie klawisza w dowolnym oknie
            while (true) {
                cv::waitKey(100);
                if (cv::getWindowProperty("Original image", cv::WND_PROP_VISIBLE) <= 0.0 || cv::getWindowProperty("Desaturated image", cv::WND_PROP_VISIBLE) <= 0.0)
                    break;
            }
            cv::destroyAllWindows();
        }
        else {
            wxLogError("Cannot open or find the image.");
        }
    }
}

void MyFrame::OnExit(wxCommandEvent& event) {
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event) {
    wxMessageBox("Choose a file (.jpg) to open by clicking File->Open or pressing Ctrl+O on your keyboard", "Get some help", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnSave(wxCommandEvent& event) {
    if (desaturated.empty()) {
        wxLogError("No image to save.");
        return;
    }

    wxString savePath = wxFileSelector("Save image as", "", "", "",
        "JPEG files (*.jpg)|*.jpg|PNG files (*.png)|*.png|All files (*.*)|*.*", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (!savePath.empty()) {
        if (cv::imwrite(savePath.ToStdString(), desaturated)) {
            wxLogMessage("Image saved successfully.");
        }
        else {
            wxLogError("Failed to save the image.");
        }
    }
}
