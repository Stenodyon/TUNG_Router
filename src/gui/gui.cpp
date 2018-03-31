#include "gui.hpp"

MainFrame::MainFrame()
    : wxFrame(NULL, wxID_ANY, "TUNG Router")
{
    wxMenuBar * menu_bar = new wxMenuBar;

    menu_bar->Append(make_file_menu(), "&File");

    SetMenuBar(menu_bar);
    CreateStatusBar();

    Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
}

void MainFrame::OnExit(wxCommandEvent&)
{
    Close(true);
}

wxMenu * MainFrame::make_file_menu()
{
    wxMenu * menu_file = new wxMenu;

    menu_file->Append(ID_OPEN, "&Open\tCtrl-O");
    menu_file->Append(ID_SAVE, "&Save\tCtrl-S");
    menu_file->Append(ID_CLOSE, "&Close\tCtrl-W");

    menu_file->AppendSeparator();
    menu_file->Append(wxID_EXIT);

    return menu_file;
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
