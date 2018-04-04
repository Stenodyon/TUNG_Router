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
};

class MainFrame : public wxFrame
{
    private:

        void OnExit(wxCommandEvent&);
        void OnOpen(wxCommandEvent&);
        void OpenNewChipDialog();

        wxMenu * make_file_menu();
        wxMenu * make_new_menu();

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

    public:
        ~RouterApp();

        virtual bool OnInit();
};
