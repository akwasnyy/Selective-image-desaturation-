#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/radiobox.h>
#include <wx/slider.h>
#include <iostream>
#include <algorithm>

cv::Vec3b calculate_partial_gray(int low, int high, int r, int g, int b) {
    int lightness = (r + g + b) / 3;
    double factor = (lightness - low) / (high - low);
    //factor = std::clamp(factor, 0.0, 1.0); // Ensure factor is within [0, 1]
    int gray = (r + g + b) * factor / 3;
    return cv::Vec3b(r * (1 - factor) + gray, g * (1 - factor) + gray, b * (1 - factor) + gray);
    
}
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

    wxSlider* lightnessSlider;
    wxSlider* rSlider;
    wxSlider* gSlider;
    wxSlider* bSlider;
    wxRadioBox* modeRadioBox;
    wxSlider* partial_desaturation_slider;

    cv::Mat original;
    cv::Mat desaturated;

    wxStaticText* lightness_txt;
    wxStaticText* r_txt;
    wxStaticText* g_txt;
    wxStaticText* b_txt;
    wxStaticText* pd_txt;

    int lightness_val=0;
    int R_val=0;
    int G_val=0;
    int B_val=0;
    int selection = 0;
    int partial_desaturation_value = 0;
    void desaturate();
    void UpdateSliders();
};



enum {
    ID_Open = 1,
    ID_Save,
    ID_Lightness=100,
    ID_R,
    ID_G,
    ID_B,
    ID_Timer,
    ID_Mode,
    ID_Partial
    
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_Open, MyFrame::OnOpen)
EVT_MENU(ID_Save, MyFrame::OnSave)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
EVT_SLIDER(ID_Lightness, MyFrame::OnSliderChange)
EVT_SLIDER(ID_R, MyFrame::OnSliderChange)
EVT_SLIDER(ID_G, MyFrame::OnSliderChange)
EVT_SLIDER(ID_B, MyFrame::OnSliderChange)
EVT_SLIDER(ID_Partial, MyFrame::OnSliderChange)
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
    lightnessSlider = new wxSlider(panel, ID_Lightness, 0, 0, 255, wxPoint(20, 20), wxSize(300, -1), wxSL_HORIZONTAL);
    new wxStaticText(panel, wxID_ANY, "255", wxPoint(310, 45));
    lightness_txt = new wxStaticText(panel, wxID_ANY, "Lightness=0", wxPoint(20,5));

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

    new wxStaticText(panel, wxID_ANY, "0", wxPoint(20, 285));
    partial_desaturation_slider = new wxSlider(panel, ID_Partial, 0, 0, 100, wxPoint(20, 265), wxSize(300, -1), wxSL_HORIZONTAL);
    new wxStaticText(panel, wxID_ANY, " 100 ", wxPoint(20, 285));
    pd_txt = new wxStaticText(panel, wxID_ANY, "Partial Desaturation = 0", wxPoint(20, 245)); //poprawic ma byc od 0 do 100 %, gdzie 100% = lightness_value :)
    
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
    lightness_val = lightnessSlider->GetValue();
    R_val = rSlider->GetValue();
    G_val = gSlider->GetValue();
    B_val = bSlider->GetValue();
    partial_desaturation_value = partial_desaturation_slider->GetValue();
    //tutaj wrzucic logike
    lightness_txt->SetLabel(wxString::Format("Lightness=%d", lightness_val));
    r_txt->SetLabel(wxString::Format("R=%d", R_val));
    g_txt->SetLabel(wxString::Format("G=%d", G_val));
    b_txt->SetLabel(wxString::Format("B=%d", B_val));
    pd_txt->SetLabel(wxString::Format("Partial Desaturation = %d", partial_desaturation_value));
   
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
    int mode = selection; //mode dla cb kwasny ~ oki:>>
    cv::Vec3b desaturation;
    int low_desaturation_value;
    int high_desaturation_value;
    for (int y = 0; y < desaturated.rows; ++y) {
        for (int x = 0; x < desaturated.cols; ++x) {
            bool is_partial = false;
            cv::Vec3b intensity = desaturated.at<cv::Vec3b>(y, x);
            int b = intensity[0];
            int g = intensity[1];
            int r = intensity[2];

            if (mode == 1) { // RGB mode
                double factor = partial_desaturation_value / 100.0; 
                high_desaturation_value = (R_val + G_val + B_val) / 3;
                low_desaturation_value = high_desaturation_value * factor;

                if ((r > R_val || g > G_val || b > B_val)) {
                    continue; 
                }
                else if ((r >= R_val * factor && r <= R_val) &&
                    (g >= G_val * factor && g <= G_val) &&
                    (b >= B_val * factor && b <= B_val)) {
                    
                    
                }
            }
            else if (mode == 0) {  // lightness mode
                int lightness = (r + g + b) / 3;
                low_desaturation_value = lightness_val - lightness_val * partial_desaturation_value / 100;
                high_desaturation_value = lightness_val;
                if (lightness > lightness_val) {
     
                    continue;
                }
                else if ((lightness > low_desaturation_value) && (lightness < lightness_val)) {
                    is_partial = true;
                }
               
            }
            // Apply grayscale
            if (is_partial) {
                desaturation = calculate_partial_gray(std::max(0, low_desaturation_value), high_desaturation_value, r, g, b);
            }
            else {
                int gray = (r + g + b) / 3;
                desaturation = cv::Vec3b(gray, gray, gray);
            }
           
            
            desaturated.at<cv::Vec3b>(y, x) = desaturation;
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
        lightnessSlider->Enable();
        lightness_txt->Enable();

        rSlider->Disable();
        r_txt->Disable();
        gSlider->Disable();
        g_txt->Disable();
        bSlider->Disable();
        b_txt->Disable();
    }
    else { // RGB mode
        lightnessSlider->Disable();
        lightness_txt->Disable();

        rSlider->Enable();
        r_txt->Enable();
        gSlider->Enable();
        g_txt->Enable();
        bSlider->Enable();
        b_txt->Enable();
    }
}