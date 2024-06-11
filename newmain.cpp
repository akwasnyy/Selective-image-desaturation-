#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/radiobox.h>
#include <wx/slider.h>
#include <iostream>
#include <algorithm>


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
    cv::Vec3b calculate_partial_gray(int r, int g, int b);
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

cv::Vec3b MyFrame::calculate_partial_gray(int r, int g, int b) {
    
    cv::Vec3b result(0, 0, 0);
    return result;

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
    partial_desaturation_slider = new wxSlider(panel, ID_Partial, 0, 0, 100, wxPoint(20, 260), wxSize(300, -1), wxSL_HORIZONTAL);
    new wxStaticText(panel, wxID_ANY, "100", wxPoint(310, 285));
    pd_txt = new wxStaticText(panel, wxID_ANY, "Partial Desaturation = 0", wxPoint(20, 245)); //poprawic ma byc od 0 do 100 %, gdzie 100% = lightness_value :)
    
    wxString choices[] = { "Lightness", "RGB" };
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
   
    int low_desaturation_value = std::max(0, lightness_val - partial_desaturation_value);

    for (int y = 0; y < desaturated.rows; ++y) {
        for (int x = 0; x < desaturated.cols; ++x) {
            //bool is_partial = false;
            cv::Vec3b intensity = desaturated.at<cv::Vec3b>(y, x);
            int b = intensity[0];
            int g = intensity[1];
            int r = intensity[2];
            //tutaj zamieniam rgb na hsl bo tak:
            double B_ = b / 255.0;
            double G_ = g / 255.0;
            double R_ = r / 255.0;

            double max_rgb = std::max({ R_, G_, B_ });
            double min_rgb = std::min({ R_, G_, B_ });
            double L =(max_rgb + min_rgb) / 2.0;
            
            double delta = max_rgb - min_rgb;
            double S = 0;
            
            if (L <= 0.5)
            {
                S = delta / (max_rgb + min_rgb);
            }
            else
            {
                S = delta / (2.0 - max_rgb - min_rgb);
            }
            double H = 0;
            if (delta != 0)
            {
                if (max_rgb == R_)
                    H = (G_ - B_) / delta;
                else if (max_rgb == G_)
                    H = 2.0 + (B_ - R_) / delta;
                else
                    H = 4.0 + (R_ - G_) / delta;
            }
            H = H * 60;
            if (H < 0)
                H += 360;
            //tutaj skonczylem zamieniac rgb na hsl
            double factor = 0; //wspolczynnik zmiany desaturacji, na podstawie parametru rozmycia
            cv::Vec3b HSL_color(H, S, L);
            if (mode == 1) { // RGB mode
                //tu na razie skip x d todo: partial desaturation przy rgb ale to juz formalnosc teraz              
              
                if ((r > R_val || g > G_val || b > B_val)) {
                    continue; 
                }
                //na razie roboczo:
                S = S*factor; //czyli 0, domyslnie bedzie else if ktory bedzie sprawdzal partial desaturation i obliczal inny factor w razie potrzeby
                //S=0 oznacza calkowita desaturacje

            }
            else if (mode == 0) {  // lightness mode
                
                if (L > lightness_val / 255.0)
                    continue; //dziele tu wszedzie lightness val i low_saturation_value przez 255 bo uzywam L jako sredniej znormalizownych max rgb i min rgb, czyli wczesniej podzielonych przez 255 
                else if ((L > low_desaturation_value/255.0) && (L < lightness_val/255.0)) {
                    //is_partial = true;
                    //tutaj dla jasnosci wyznaczamy factor:
                    int diff = L*255 - low_desaturation_value; //tu se mnoze L przez 255 zeby sie matematyka zgadzala bo wczesniej L jest znormalizowane to tutaj se pomnoze ;-D
                    factor = diff / static_cast<double> (partial_desaturation_value); //ten factor bedzie okreslal zmiane saturacji, tzn factor*S to nasza nowa saturacja
                    
                }
                S = S * factor;
               
            }
            // Apply grayscale - haha grayscale to nie desaturacja 8=======D
            //tutaj w zasadzie obliczamy nowe R,G,B a na koncu je przypisujemy
            if (S!=0) {
                //desaturation = calculate_partial_gray(low_desaturation_value, high_desaturation_value, r, g, b);
                //S = S * factor;
                double q = 0;
                double p = 0; //jakies fancy parameters do hsl->rgb conversion idk
                if (L <= 0.5)
                    q = L * (1 + S);
                else
                    q = L + S - L * S;
               
                p = 2 * L - q;

                auto h = [](double t, double p_, double q_) {
                    if (t < 0) t += 1;
                    if (t > 1) t -= 1;
                    if (6 * t < 1)
                        return p_ + (q_ - p_) * 6 * t;
                    if (2 * t < 1)
                        return q_;
                    if (3 * t < 2)
                        return p_ + (q_ - p_) * (2 / 3.0 - t) * 6;
                    return p_;
                    };//nazwalem se t¹ lambde h bo mi tak pasowalo ~_~
                //tu obliczamy nowe rgb:
                R_ = h(H/360.0 + 1 / 3.0, p, q);
                G_ = h(H/360.0, p, q);
                B_ = h(H/360.0 - 1 / 3.0, p, q);
                R_ = R_ * 255;
                G_ = G_ * 255;
                B_ = B_ * 255; //przeskalowanie, bo wczesniej normalizowalismy rgb
                //desaturation = cv::Vec3b(B_, G_, R_); //tutaj mamy wrzucona nowa saturacje naszego piksela w super kolejnosci bgr
            }
            else {
                //jesli ma byc pelna desaturacja to S=0 i skladowe rgb po prostu s¹ wartoœci¹ L * 255
                //S = 0;
                R_ = G_ = B_ = L * 255;
                //desaturation = cv::Vec3b(B_, G_, R_);
            
            }
           
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