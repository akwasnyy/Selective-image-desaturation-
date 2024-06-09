#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/radiobox.h>
#include <wx/slider.h>
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
    void OnAdjustImage(wxCommandEvent& event);

private:
    wxDECLARE_EVENT_TABLE();
};

class ImageAdjustDialog : public wxDialog {
public:
    ImageAdjustDialog(wxWindow* parent, wxWindowID id, const wxString& title);

private:
    wxRadioBox* optionRadioBox;
    wxRadioBox* channelRadioBox;
    wxSlider* slider;
    void OnOptionChange(wxCommandEvent& event);
    void OnSliderChange(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
};


enum {
    ID_Open = 1,
    ID_AdjustImage = 2
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_MENU(ID_Open, MyFrame::OnOpen)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
EVT_MENU(ID_AdjustImage, MyFrame::OnAdjustImage)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(ImageAdjustDialog, wxDialog)
EVT_RADIOBOX(wxID_ANY, ImageAdjustDialog::OnOptionChange)
EVT_SLIDER(wxID_ANY, ImageAdjustDialog::OnSliderChange)
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
    menuFile->Append(ID_AdjustImage, "&Adjust Image...\tCtrl-A", "Adjust image properties");
    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);
    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");
}

void MyFrame::OnOpen(wxCommandEvent& event) {
    wxString filePath = wxFileSelector("Choose an image file", "", "", "",
        "JPEG files (*.jpg)|*.jpg|All files (*.*)|*.*");

    if (!filePath.empty()) {
        cv::Mat image = cv::imread(filePath.ToStdString(), cv::IMREAD_COLOR);

        if (!image.empty()) {
            cv::imshow("Original image", image);  // Pierwsze okno
            cv::imshow("Desaturated image", image);  // Drugie okno
            cv::waitKey(0); // Czekaj na naciœniêcie klawisza w dowolnym oknie
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

ImageAdjustDialog::ImageAdjustDialog(wxWindow* parent, wxWindowID id, const wxString& title)
    : wxDialog(parent, id, title, wxDefaultPosition, wxSize(250, 300)) {
    wxString options[] = { "RGB", "Brightness" };
    optionRadioBox = new wxRadioBox(this, wxID_ANY, "Options", wxDefaultPosition, wxDefaultSize, WXSIZEOF(options), options, 1, wxRA_SPECIFY_COLS);

    wxString channels[] = { "R", "G", "B" };
    channelRadioBox = new wxRadioBox(this, wxID_ANY, "Channels", wxDefaultPosition, wxDefaultSize, WXSIZEOF(channels), channels, 1, wxRA_SPECIFY_COLS);
    channelRadioBox->Show(false); // Initially hidden

    slider = new wxSlider(this, wxID_ANY, 0, 0, 255, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
    slider->Show(false); // Initially hidden

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(optionRadioBox, 0, wxALL, 10);
    sizer->Add(channelRadioBox, 0, wxALL, 10);
    sizer->Add(slider, 0, wxALL, 10);

    SetSizer(sizer);
    Layout();
}

void ImageAdjustDialog::OnOptionChange(wxCommandEvent& event) {
    if (optionRadioBox->GetSelection() == 0) { // RGB selected
        channelRadioBox->Show();
        slider->Show();
    }
    else { // Brightness selected
        channelRadioBox->Show(false);
        slider->Show();
    }
    Layout();
}

void ImageAdjustDialog::OnSliderChange(wxCommandEvent& event) {
    // Tutaj mo¿na zaimplementowaæ logikê zmiany wartoœci obrazu na podstawie pozycji suwaka
    int value = slider->GetValue();
    // Zastosuj wartoœæ value do obrazu
}

void MyFrame::OnAdjustImage(wxCommandEvent& event) {
    ImageAdjustDialog dialog(this, wxID_ANY, "Adjust Image");
    if (dialog.ShowModal() == wxID_OK) {
        // Tutaj mo¿na obs³u¿yæ wyniki z dialogu, jeœli to potrzebne
    }
}