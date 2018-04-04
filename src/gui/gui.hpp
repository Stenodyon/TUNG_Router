#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <wx/wx.h>
#pragma GCC diagnostic pop

#include <memory>

#include "chip_library.hpp"

enum
{
    ID_OPEN = 1,
    ID_SAVE = 2,
    ID_SAVE_AS = 3,
    ID_CLOSE = 4,

    ID_NEW_FOLDER_BUTTON = 5,

    ID_NEW_BOARD = 6,
    ID_NEW_CHIP = 7,

    ID_LIBRARY_IMPORT = 8,
    ID_LIBRARY_EXPORT = 9,
};

class MainFrame : public wxFrame
{
    private:

        void OnExit(wxCommandEvent&);
        void OnOpen(wxCommandEvent&);
        void OnLibraryImport(wxCommandEvent&);
        void OnLibraryExport(wxCommandEvent&);
        void OpenChipEditor(const std::string & folder = "misc",
                const std::string & chip_name = "new_chip",
                chip_type * to_edit = nullptr);

        wxMenu * make_file_menu();
        wxMenu * make_new_menu();
        wxMenu * make_library_menu();

        wxWindow * make_library_window(wxWindow * parent);

        ChipLibrary * chip_library;

        wxSize DoGetBestClientSize() const override;

    public:
        MainFrame();
};

class RouterApp : public wxApp
{
    private:
        MainFrame * frame;

        void HandleEvent(wxEvtHandler * handler,
                wxEventFunction func,
                wxEvent & event) const override;

    public:
        ~RouterApp();

        virtual bool OnInit();
};
