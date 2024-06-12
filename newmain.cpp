#define _CRT_SECURE_NO_WARNINGS
//#include <opencv2/opencv.hpp>
//#include <opencv2/highgui.hpp>
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/radiobox.h>
#include <wx/slider.h>
#include <iostream>
#include <algorithm>
#include "color_functions.h"

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
    wxSlider* cSlider;
    wxSlider* mSlider;
    wxSlider* ySlider;
    wxRadioBox* modeRadioBox;
    wxSlider* partial_desaturation_slider;

    cv::Mat original;
    cv::Mat desaturated;

    wxStaticText* lightness_txt;
    wxStaticText* r_txt;
    wxStaticText* g_txt;
    wxStaticText* b_txt;
    wxStaticText* c_txt;
    wxStaticText* m_txt;
    wxStaticText* y_txt;
    wxStaticText* pd_txt;

    int lightness_val = 0;
    int R_val = 0;
    int G_val = 0;
    int B_val = 0;

    int C_val = 0;
    int M_val = 0;
    int Y_val = 0;

    int selection = 0;
    int partial_desaturation_value = 0;
    void desaturate();
    void UpdateSliders();
    double Calculate_Color_Factor(const double& rc, const double& gm, const double& by, const int& rc_low, const int& gm_low, const int& by_low) const;
   
};



enum {
    ID_Open = 1,
    ID_Save,
    ID_Lightness = 100,
    ID_R,
    ID_G,
    ID_B,
    ID_C,
    ID_M,
    ID_Y,
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
EVT_SLIDER(ID_C, MyFrame::OnSliderChange)
EVT_SLIDER(ID_M, MyFrame::OnSliderChange)
EVT_SLIDER(ID_Y, MyFrame::OnSliderChange)
EVT_SLIDER(ID_Partial, MyFrame::OnSliderChange)
EVT_TIMER(ID_Timer, MyFrame::OnUpdate)
EVT_RADIOBOX(ID_Mode, MyFrame::OnModeChange)
wxEND_EVENT_TABLE()



wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    wxInitAllImageHandlers(); // Inicjalizacja obs³ugi ró¿nych formatów obrazów
    MyFrame* frame = new MyFrame("Selective Image Desaturation", wxPoint(50, 50), wxSize(600, 600));
    frame->Show(true);
    return true;
}



MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size), timer(this, ID_Timer) {
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
    lightness_txt = new wxStaticText(panel, wxID_ANY, "Lightness=0", wxPoint(20, 5));

    new wxStaticText(panel, wxID_ANY, "0", wxPoint(20, 105));
    rSlider = new wxSlider(panel, ID_R, 0, 0, 255, wxPoint(20, 80), wxSize(300, -1), wxSL_HORIZONTAL);
    new wxStaticText(panel, wxID_ANY, "255", wxPoint(310, 105));
    r_txt = new wxStaticText(panel, wxID_ANY, "R=0", wxPoint(20, 65));

    new wxStaticText(panel, wxID_ANY, "0", wxPoint(20, 165));
    gSlider = new wxSlider(panel, ID_G, 0, 0, 255, wxPoint(20, 140), wxSize(300, -1), wxSL_HORIZONTAL);
    new wxStaticText(panel, wxID_ANY, "255", wxPoint(310, 165));
    g_txt = new wxStaticText(panel, wxID_ANY, "G=0", wxPoint(20, 125));

    new wxStaticText(panel, wxID_ANY, "0", wxPoint(20, 225));
    bSlider = new wxSlider(panel, ID_B, 0, 0, 255, wxPoint(20, 200), wxSize(300, -1), wxSL_HORIZONTAL);
    new wxStaticText(panel, wxID_ANY, "255", wxPoint(310, 225));
    b_txt = new wxStaticText(panel, wxID_ANY, "B=0", wxPoint(20, 185));

    new wxStaticText(panel, wxID_ANY, "0", wxPoint(20, 285));
    cSlider = new wxSlider(panel, ID_C, 0, 0, 255, wxPoint(20, 260), wxSize(300, -1), wxSL_HORIZONTAL);
    new wxStaticText(panel, wxID_ANY, "255", wxPoint(310, 285));
    c_txt = new wxStaticText(panel, wxID_ANY, "C=0", wxPoint(20, 245));

    new wxStaticText(panel, wxID_ANY, "0", wxPoint(20, 345));
    mSlider = new wxSlider(panel, ID_M, 0, 0, 255, wxPoint(20, 320), wxSize(300, -1), wxSL_HORIZONTAL);
    new wxStaticText(panel, wxID_ANY, "255", wxPoint(310, 345));
    m_txt = new wxStaticText(panel, wxID_ANY, "M=0", wxPoint(20, 305));

    new wxStaticText(panel, wxID_ANY, "0", wxPoint(20, 405));
    ySlider = new wxSlider(panel, ID_Y, 0, 0, 255, wxPoint(20, 380), wxSize(300, -1), wxSL_HORIZONTAL);
    new wxStaticText(panel, wxID_ANY, "255", wxPoint(310, 405));
    y_txt = new wxStaticText(panel, wxID_ANY, "Y=0", wxPoint(20, 365));

    new wxStaticText(panel, wxID_ANY, "0", wxPoint(20, 465));
    partial_desaturation_slider = new wxSlider(panel, ID_Partial, 0, 0, 100, wxPoint(20, 440), wxSize(300, -1), wxSL_HORIZONTAL);
    new wxStaticText(panel, wxID_ANY, "100", wxPoint(310, 465));
    pd_txt = new wxStaticText(panel, wxID_ANY, "Partial Desaturation = 0", wxPoint(20, 425));

    wxString choices[] = { "Lightness", "RGB", "CMY" };
    modeRadioBox = new wxRadioBox(panel, ID_Mode, "Mode", wxPoint(400, 20), wxDefaultSize, 3, choices, 1, wxRA_SPECIFY_COLS);
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
    C_val = cSlider->GetValue();
    M_val = mSlider->GetValue();
    Y_val = ySlider->GetValue();
    partial_desaturation_value = partial_desaturation_slider->GetValue();

    lightness_txt->SetLabel(wxString::Format("Lightness=%d", lightness_val));
    r_txt->SetLabel(wxString::Format("R=%d", R_val));
    g_txt->SetLabel(wxString::Format("G=%d", G_val));
    b_txt->SetLabel(wxString::Format("B=%d", B_val));
    c_txt->SetLabel(wxString::Format("C=%d", C_val));
    m_txt->SetLabel(wxString::Format("M=%d", M_val));
    y_txt->SetLabel(wxString::Format("Y=%d", Y_val));
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
    wxMessageBox("Choose a file (.jpg) to open by clicking File->Open or pressing Ctrl+O on your keyboard.\nThen, set the brightness/RGB/CMY threshold to desaturate those parts of the photo, which have pixels with values lower than the threshold.\nYou can use the \"Partial desaturation\" slidebar to make the transition between grayscale and color smoothier.", "Get some help", wxOK | wxICON_INFORMATION);
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
    int rows = desaturated.rows;
    int cols = desaturated.cols;

    double lightness_low = std::max(0, lightness_val - partial_desaturation_value)/255.0;

    int c_low = C_val - partial_desaturation_value;
    int m_low = M_val - partial_desaturation_value;
    int y_low = Y_val - partial_desaturation_value;

    int r_low = R_val - partial_desaturation_value;
    int g_low = G_val - partial_desaturation_value;
    int b_low = B_val - partial_desaturation_value;//haha blow jak blow-ek haha wiecie ten jutuber ;~~D
    
    double lightness_thresh = lightness_val / 255.0;
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {

            cv::Vec3b& intensity = desaturated.at<cv::Vec3b>(y, x);
            int b = intensity[0];
            int g = intensity[1];
            int r = intensity[2];

            double B_ = b / 255.0;
            double G_ = g / 255.0;
            double R_ = r / 255.0;

            double factor = 0; //wspolczynnik zmiany desaturacji, na podstawie parametru rozmycia, domyslnie zero wtedy bedzie calkowita desaturacja
            double H, S, L;
            
            RGB_TO_HSL(R_, G_,B_,H,S,L);
            
            if (selection == 2)//CMY mode
            {
                double cyan = (1 - R_) * 255;
                double magenta = (1 - G_) * 255;
                double yellow = (1 - B_) * 255;

                if (cyan > C_val || magenta > M_val || yellow > Y_val)
                    continue;

                if (cyan > c_low || magenta > m_low || yellow > y_low)
                    factor = Calculate_Color_Factor(cyan, magenta, yellow, c_low, m_low, y_low);
            }
            else if (selection == 1) { // RGB mode             
                if ((r > R_val || g > G_val || b > B_val)) 
                    continue;

                if (r > r_low || g > g_low || b > b_low)
                    factor = Calculate_Color_Factor(r, g, b, r_low, g_low, b_low);
            }
            else if (selection == 0) {  // lightness mode
                
                if (L > lightness_thresh)
                    continue; //dziele tu wszedzie lightness val i low_saturation_value przez 255 bo uzywam L jako sredniej znormalizownych max rgb i min rgb, czyli wczesniej podzielonych przez 255 
                
                if (L > (lightness_low) && (L<lightness_thresh)) {
                    //tutaj dla jasnosci wyznaczamy factor:
                    int diff = L * 255 - lightness_low; //tu se mnoze L przez 255 zeby sie matematyka zgadzala bo wczesniej L jest znormalizowane to tutaj se pomnoze ;-D
                    factor = diff / static_cast<double> (partial_desaturation_value); //ten factor bedzie okreslal zmiane saturacji, tzn factor*S to nasza nowa 
                }
            }
            S = S * factor;
            // Apply grayscale - haha grayscale to nie desaturacja 8=======D
            //tutaj w zasadzie obliczamy nowe R,G,B a na koncu je przypisujemy
            if (S != 0)
                HSL_TO_RGB(R_, G_, B_, H, S, L);//czesciowa desaturacja poprzez zmiane wartosci wspolczynnika S i obliczenie rgb na nowo
            else
                R_ = G_ = B_ = L * 255; //calkowita desaturacja

            desaturated.at<cv::Vec3b>(y, x)[0] = B_;
            desaturated.at<cv::Vec3b>(y, x)[1] = G_;
            desaturated.at<cv::Vec3b>(y, x)[2] = R_;
        }
    }
    //nie wiem szczerze co moge w tym temacie wiecej zmienicxD

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

        cSlider->Disable();
        c_txt->Disable();
        mSlider->Disable();
        m_txt->Disable();
        ySlider->Disable();
        y_txt->Disable();
    }
    else if (selection == 1) { // RGB mode
        lightnessSlider->Disable();
        lightness_txt->Disable();

        rSlider->Enable();
        r_txt->Enable();
        gSlider->Enable();
        g_txt->Enable();
        bSlider->Enable();
        b_txt->Enable();

        cSlider->Disable();
        c_txt->Disable();
        mSlider->Disable();
        m_txt->Disable();
        ySlider->Disable();
        y_txt->Disable();
    }
    else if (selection == 2) { // CMY mode
        lightnessSlider->Disable();
        lightness_txt->Disable();

        rSlider->Disable();
        r_txt->Disable();
        gSlider->Disable();
        g_txt->Disable();
        bSlider->Disable();
        b_txt->Disable();

        cSlider->Enable();
        c_txt->Enable();
        mSlider->Enable();
        m_txt->Enable();
        ySlider->Enable();
        y_txt->Enable();
    }
}
double MyFrame::Calculate_Color_Factor  (const double& rc, const double& gm, const double& by, const int& rc_low, const int& gm_low, const int& by_low) const {
    double diff_rc = (rc - rc_low) > 0 ? rc - rc_low : 0;
    double diff_gm = (gm - gm_low) > 0 ? gm - gm_low : 0;
    double diff_by = (by - by_low) > 0 ? by - by_low : 0;

    double factor_rc = diff_rc / static_cast<double>(partial_desaturation_value);
    double factor_gm = diff_gm / static_cast<double>(partial_desaturation_value);
    double factor_by = diff_by / static_cast <double> (partial_desaturation_value);

    return std::max({ factor_rc,factor_gm,factor_by });
}