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
    void OnUpdate(wxTimerEvent& event);
    void OnModeChange(wxCommandEvent& event);
    ~MyFrame();

private:
    wxDECLARE_EVENT_TABLE();
    
    wxTimer timer;

    wxSlider* brightnessSlider;
    wxSlider* rSlider;
    wxSlider* gSlider;
    wxSlider* bSlider;
    wxRadioBox* modeRadioBox;

    cv::Mat original;
    cv::Mat desaturated;

    wxStaticText* brightness_txt;
    wxStaticText* r_txt;
    wxStaticText* g_txt;
    wxStaticText* b_txt;

    int brightness_val=0;
    int R_val=0;
    int G_val=0;
    int B_val=0;
    int selection = 0;

    void desaturate();
    void UpdateSliders();
};



enum {
    ID_Open = 1,
    ID_Save,
    ID_Brightness=100,
    ID_R,
    ID_G,
    ID_B,
    ID_Timer,
    ID_Mode
    
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
EVT_TIMER(ID_Timer, MyFrame::OnUpdate)
EVT_RADIOBOX(ID_Mode, MyFrame::OnModeChange)
wxEND_EVENT_TABLE()



wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    wxInitAllImageHandlers(); // Inicjalizacja obs³ugi ró¿nych formatów obrazów
    MyFrame* frame = new MyFrame("Selective Image Desaturation", wxPoint(50, 50), wxSize(600, 400));
    frame->Show(true);
    return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size), timer(this,ID_Timer) {
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
    brightness_txt= new wxStaticText(panel, wxID_ANY, "Brightness=0", wxPoint(20,5));

    new wxStaticText(panel, wxID_ANY, "0", wxPoint(20, 105));
    rSlider = new wxSlider(panel, ID_R, 0, 0, 255, wxPoint(20, 80), wxSize(300, -1), wxSL_HORIZONTAL);
    new wxStaticText(panel, wxID_ANY, "255", wxPoint(310, 105));
    r_txt = new wxStaticText(panel, wxID_ANY, "R=0", wxPoint(20,65));

    new wxStaticText(panel, wxID_ANY, "0", wxPoint(20, 165));
    gSlider = new wxSlider(panel, ID_G, 0, 0, 255, wxPoint(20, 140), wxSize(300, -1), wxSL_HORIZONTAL);
    new wxStaticText(panel, wxID_ANY, "255", wxPoint(310, 165));
    g_txt = new wxStaticText(panel, wxID_ANY, "G=0", wxPoint(20,125));

    new wxStaticText(panel, wxID_ANY, "0", wxPoint(20, 225));
    bSlider = new wxSlider(panel, ID_B, 0, 0, 255, wxPoint(20, 200), wxSize(300, -1), wxSL_HORIZONTAL);
    new wxStaticText(panel, wxID_ANY, "255", wxPoint(310, 225));
    b_txt = new wxStaticText(panel, wxID_ANY, "B=0", wxPoint(20,185));

    wxString choices[] = { "Brightness", "RGB" };
    modeRadioBox = new wxRadioBox(panel, ID_Mode, "Mode", wxPoint(400, 20), wxDefaultSize, 2, choices, 1, wxRA_SPECIFY_COLS);
    modeRadioBox->SetSelection(0); // Default to Brightness mode
    
    CreateStatusBar();
    SetStatusText("Selective Image Desaturation v0.2");

    timer.Start(10);
    UpdateSliders(); // Initialize slider states
  
}


void MyFrame::OnUpdate(wxTimerEvent& event)
{
    if (cv::getWindowProperty("Original image", cv::WND_PROP_VISIBLE) <= 0.0 ||
        cv::getWindowProperty("Desaturated image", cv::WND_PROP_VISIBLE) <= 0.0) {
        cv::destroyAllWindows();
        original.release();
        desaturated.release();
    }

    if (!original.empty() && !desaturated.empty()) {
            

            desaturate();
            cv::imshow("Original image", original);
            cv::imshow("Desaturated image", desaturated);
           
    }

}


void MyFrame::OnSliderChange(wxCommandEvent& event) {
    brightness_val = brightnessSlider->GetValue();
    R_val = rSlider->GetValue();
    G_val = gSlider->GetValue();
    B_val = bSlider->GetValue();
    //tutaj wrzucic logike
    brightness_txt->SetLabel(wxString::Format("Brightness=%d", brightness_val));
    r_txt->SetLabel(wxString::Format("R=%d", R_val));
    g_txt->SetLabel(wxString::Format("G=%d", G_val));
    b_txt->SetLabel(wxString::Format("B=%d", B_val));

   
}

void MyFrame::OnOpen(wxCommandEvent& event) {
    wxString filePath = wxFileSelector("Choose an image file", "", "", "",
        "JPEG files (*.jpg)|*.jpg|All files (*.*)|*.*");

    if (!filePath.empty()) {
        original = cv::imread(filePath.ToStdString(), cv::IMREAD_COLOR);
        desaturated = original.clone();
        //zaimplementowac jakis display handler czy cos idk
        if (!desaturated.empty()) {
            cv::namedWindow("Original image", cv::WINDOW_NORMAL);  // Pierwsze okno
            cv::namedWindow("Desaturated image", cv::WINDOW_NORMAL);  // Drugie okno
           
           
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
    wxMessageBox("Choose a file (.jpg) to open by clicking File->Open or pressing Ctrl+O on your keyboard.\nThen, set the brightness or RGB threshold to desaturate those parts of the photo, which have pixels with values lower than the threshold.", "Get some help", wxOK | wxICON_INFORMATION);
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

void MyFrame::desaturate()
{
    desaturated = original.clone();
    int mode = selection; //mode dla cb kwasny
    for (int y = 0; y < desaturated.rows; ++y) {
        for (int x = 0; x < desaturated.cols; ++x) {
            cv::Vec3b intensity = desaturated.at<cv::Vec3b>(y, x);
            int b = intensity[0];
            int g = intensity[1];
            int r = intensity[2];

            if (mode == 1) {  // RGB mode
                if (r > R_val || g > G_val || b > B_val) {
                    continue;
                }
            }
            else if (mode == 0) {  // Brightness mode
                int brightness = (r + g + b) / 3;
                if (brightness > brightness_val) {
                    continue;
                }
            }
            // Apply grayscale
            int gray = (r + g + b) / 3;
            desaturated.at<cv::Vec3b>(y, x) = cv::Vec3b(gray, gray, gray);
        }
    }

   
}

void MyFrame::OnModeChange(wxCommandEvent& event) {
    UpdateSliders();
}

MyFrame::~MyFrame() {
    timer.Stop();
}

void MyFrame::UpdateSliders() {
    selection = modeRadioBox->GetSelection();
    if (selection == 0) { // Brightness mode
        brightnessSlider->Enable();
        brightness_txt->Enable();

        rSlider->Disable();
        r_txt->Disable();
        gSlider->Disable();
        g_txt->Disable();
        bSlider->Disable();
        b_txt->Disable();
    }
    else { // RGB mode
        brightnessSlider->Disable();
        brightness_txt->Disable();

        rSlider->Enable();
        r_txt->Enable();
        gSlider->Enable();
        g_txt->Enable();
        bSlider->Enable();
        b_txt->Enable();
    }
}