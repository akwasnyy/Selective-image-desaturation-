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
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    

private:
    wxDECLARE_EVENT_TABLE();
    void OnOptionChange(wxCommandEvent& event);
    void OnSliderChange(wxCommandEvent& event);

    wxRadioBox* optionRadioBox;
    wxRadioBox* channelRadioBox;
    wxSlider* slider;
};



enum {
    ID_Open = 1,
    
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_Open, MyFrame::OnOpen)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)

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
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
   
    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    wxPanel* panel = new wxPanel(this, wxID_ANY);

    wxString options[] = { "RGB", "Brightness" };
    optionRadioBox = new wxRadioBox(panel, wxID_ANY, "Options", wxPoint(20, 20), wxDefaultSize, WXSIZEOF(options), options, 1, wxRA_SPECIFY_COLS);
    optionRadioBox->Bind(wxEVT_RADIOBOX, &MyFrame::OnOptionChange, this);

    wxString channels[] = { "R", "G", "B" };
    channelRadioBox = new wxRadioBox(panel, wxID_ANY, "Channels", wxPoint(20, 100), wxDefaultSize, WXSIZEOF(channels), channels, 3, wxRA_SPECIFY_COLS);
    channelRadioBox->Hide();

    slider = new wxSlider(panel, wxID_ANY, 0, 0, 255, wxPoint(20, 180), wxSize(300, -1), wxSL_HORIZONTAL);
    slider->Bind(wxEVT_SLIDER, &MyFrame::OnSliderChange, this);
    slider->Hide();


    CreateStatusBar();
    SetStatusText("Selective Image Desaturation v0.2");
}


void MyFrame::OnOptionChange(wxCommandEvent& event) {
    if (optionRadioBox->GetSelection() == 0) { // RGB selected
        channelRadioBox->Show();
        slider->Show();
    }
    else { // Brightness selected
        channelRadioBox->Hide();
        slider->Show();
    }
    this->Layout();
}

void MyFrame::OnSliderChange(wxCommandEvent& event) {
    int value = slider->GetValue();
    // Tutaj logika zmiany obrazu
}

void MyFrame::OnOpen(wxCommandEvent& event) {
    wxString filePath = wxFileSelector("Choose an image file", "", "", "",
        "JPEG files (*.jpg)|*.jpg|All files (*.*)|*.*");

    if (!filePath.empty()) {
        cv::Mat image = cv::imread(filePath.ToStdString(), cv::IMREAD_COLOR);

        //zaimplementowac jakis display handler czy cos idk
        if (!image.empty()) {
            cv::imshow("Original image", image);  // Pierwsze okno
            cv::imshow("Desaturated image", image);  // Drugie okno
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

