#include "gui.hpp"

#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/splitter.h>

#include "../fileutils.h"
#include "placer_control.hpp"
#include "chip_editor.hpp"
#include "../command_parser.hpp"

MainFrame::MainFrame()
    : wxFrame(NULL, wxID_ANY, "TUNG Router")
{
    Maximize(true);
    SetBackgroundStyle(wxBG_STYLE_SYSTEM);

    auto library_menu = make_library_menu();

    wxMenuBar * menu_bar = new wxMenuBar;
    menu_bar->Append(make_file_menu(), "&File");
    menu_bar->Append(make_new_menu(), "&New");
    menu_bar->Append(library_menu, "&Library");
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
    library_menu->Bind(wxEVT_MENU,
            &MainFrame::OnLibraryExport, this,
            ID_LIBRARY_EXPORT);
    library_menu->Bind(wxEVT_MENU,
            &MainFrame::OnLibraryImport, this,
            ID_LIBRARY_IMPORT);
}

void MainFrame::OnExit(wxCommandEvent&)
{
    Close(true);
}

void MainFrame::OnOpen(wxCommandEvent&)
{
    wxFileDialog file_dialog(this, _("Open project file"), "", "",
            "Routing files (*.rf/*.trp)|*.rf;*.trp",
            wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if(file_dialog.ShowModal() == wxID_CANCEL)
        return;
    std::string filename = file_dialog.GetPath().ToStdString();
    std::cout << "Chose file " << filename << std::endl;
    std::cout << read_file(filename) << std::endl;
}

void MainFrame::OnLibraryImport(wxCommandEvent&)
{
    wxFileDialog file_dialog(this, _("Open library file"), "", "",
            "Library Files (*.trl)|*.trl",
            wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if(file_dialog.ShowModal() == wxID_CANCEL)
        return;
    chip_library->LoadFromFile(file_dialog.GetPath().ToStdString());
}

void MainFrame::OnLibraryExport(wxCommandEvent&)
{
    wxFileDialog file_dialog(this, _("Save library"), "", "",
            "Library Files (*.trl)|*.trl",
            wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if(file_dialog.ShowModal() == wxID_CANCEL)
        return;
    chip_library->SaveToFile(file_dialog.GetPath().ToStdString());
}

void MainFrame::OpenChipEditor(const std::string & folder,
        const std::string & chip_name,
        chip_type * to_edit)
{
    ChipEditor * dialog = new ChipEditor(this, wxID_ANY,
            chip_library->GetFolders(),
            chip_name, folder,
            to_edit);

    int return_value = dialog->ShowModal();
    if(return_value == RETURN_OK)
    {
        chip_type type = dialog->get_created_chip();
        std::string chip_name = dialog->get_chip_name().ToStdString();
        std::string folder = dialog->get_chip_folder().ToStdString();

        chip_library->SetChip(folder, chip_name, type);
    }
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

    Bind(wxEVT_MENU, [this](wxCommandEvent&) { OpenChipEditor(); }, ID_NEW_CHIP);

    return menu_new;
}

wxMenu * MainFrame::make_library_menu()
{
    auto * menu = new wxMenu;

    menu->Append(ID_LIBRARY_IMPORT, "&Import");
    menu->Append(ID_LIBRARY_EXPORT, "&Export");

    return menu;
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

    chip_library->Bind(wxEVT_TREE_ITEM_ACTIVATED, [this](wxTreeEvent& event)
            {
                auto item = event.GetItem();
                auto folder = chip_library->GetFolder(item);
                if(folder)
                {
                    auto folder_name = folder.value().ToStdString();
                    auto chip_name = chip_library->GetItemText(item).ToStdString();
                    auto chip = chip_library->GetChip(item);
                    OpenChipEditor(folder_name, chip_name, chip);
                }
            });

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

void RouterApp::HandleEvent(wxEvtHandler * handler,
        wxEventFunction func,
        wxEvent & event) const
{
    try
    {
        wxApp::HandleEvent(handler, func, event);
    } catch(std::invalid_argument & error)
    {
        wxMessageBox(wxString{} << error.what(),
                wxMessageBoxCaptionStr,
                wxOK | wxCENTER | wxICON_ERROR);
    } catch(parser::parse_error & error)
    {
        wxMessageBox(wxString{} << error.what(),
                wxMessageBoxCaptionStr,
                wxOK | wxCENTER | wxICON_ERROR);
    }
}

bool RouterApp::OnInit()
{
    frame = new MainFrame();
    frame->Show(true);
    return true;
}
