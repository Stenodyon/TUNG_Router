#include "gui.hpp"

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/splitter.h>

#include "../fileutils.h"
#include "placer_control.hpp"
#include "new_chip_dialog.hpp"

MainFrame::MainFrame()
    : wxFrame(NULL, wxID_ANY, "TUNG Router")
{
    Maximize(true);
    SetBackgroundStyle(wxBG_STYLE_SYSTEM);

    wxMenuBar * menu_bar = new wxMenuBar;
    menu_bar->Append(make_file_menu(), "&File");
    menu_bar->Append(make_new_menu(), "&New");
    SetMenuBar(menu_bar);
    CreateStatusBar();

    auto sizer = new wxBoxSizer(wxVERTICAL);
    auto splitter = new wxSplitterWindow(this,
            wxID_ANY,
            wxDefaultPosition,
            wxDefaultSize,
            wxSP_3D | wxSP_LIVE_UPDATE);
    sizer->Add(splitter, 1, wxEXPAND);
    SetSizerAndFit(sizer);
    int sash_pos = splitter->GetSize().GetWidth() * 0.9;

    splitter->SplitVertically(
            new PlacerControl(splitter),
            make_library_window(splitter),
            sash_pos);

    SetMinSize({800, 600});

    Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
}

void MainFrame::OnExit(wxCommandEvent&)
{
    Close(true);
}

void MainFrame::OnOpen(wxCommandEvent&)
{
    wxFileDialog file_dialog(this, _("Open project file"), "", "",
            "Routing files (*.rf/*.trp)|*.rf;*.trp"
            , wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if(file_dialog.ShowModal() == wxID_CANCEL)
        return;
    std::string filename = file_dialog.GetPath().ToStdString();
    std::cout << "Chose file " << filename << std::endl;
    std::cout << read_file(filename) << std::endl;
}

void MainFrame::OpenNewChipDialog()
{
    NewChipDialog * dialog = new NewChipDialog(this, wxID_ANY,
            chip_library->GetFolders());

    int return_value = dialog->ShowModal();
    if(return_value == RETURN_OK)
        std::cout << "Create!" << std::endl;
    else if(return_value == RETURN_CANCEL)
        std::cout << "Canel!" << std::endl;
    else
        std::cout << "wat" << std::endl;
}

wxMenu * MainFrame::make_file_menu()
{
    wxMenu * menu_file = new wxMenu;

    menu_file->Append(ID_OPEN, "&Open\tCtrl+O");
    menu_file->Append(ID_SAVE, "&Save\tCtrl+S");
    menu_file->Append(ID_SAVE_AS, "&Save As\tShift+Ctrl+S");
    menu_file->Append(ID_CLOSE, "&Close\tCtrl+W");

    menu_file->AppendSeparator();
    menu_file->Append(wxID_EXIT);

    Bind(wxEVT_MENU, &MainFrame::OnOpen, this, ID_OPEN);

    return menu_file;
}

wxMenu * MainFrame::make_new_menu()
{
    wxMenu * menu_new = new wxMenu;

    menu_new->Append(ID_NEW_BOARD, "&Board");
    menu_new->Append(ID_NEW_CHIP, "&Chip");

    Bind(wxEVT_MENU, [this](wxCommandEvent&) { OpenNewChipDialog(); }, ID_NEW_CHIP);

    return menu_new;
}

wxWindow * MainFrame::make_library_window(wxWindow * parent)
{
    wxPanel * library_panel = new wxPanel(parent);
    wxButton * new_folder_button = new wxButton(library_panel, ID_NEW_FOLDER_BUTTON, "New Folder");
    chip_library = new ChipLibrary(library_panel);
    chip_library->AddFolder("test_folder");
    new_folder_button->Bind(wxEVT_BUTTON,
            [this](wxCommandEvent&)
            {chip_library->NewFolder();});

    wxBoxSizer * lib_sizer = new wxBoxSizer(wxVERTICAL);

    lib_sizer->Add(new_folder_button, 0, wxALL, 2);
    lib_sizer->Add(chip_library, 1, wxEXPAND);

    library_panel->SetSizerAndFit(lib_sizer);

    return library_panel;
}

wxSize MainFrame::DoGetBestClientSize() const
{
    return {1920, 1080};
}

RouterApp::~RouterApp()
{
    //delete frame;
}

bool RouterApp::OnInit()
{
    frame = new MainFrame();
    frame->Show(true);
    return true;
}
