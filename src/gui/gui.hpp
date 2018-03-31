#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <wx/wx.h>
#pragma GCC diagnostic pop

#include <memory>

enum
{
    ID_OPEN = 1,
    ID_SAVE = 2,
    ID_CLOSE = 3
};

class MainFrame : public wxFrame
{
    private:

        void OnExit(wxCommandEvent&);

        wxMenu * make_file_menu();

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
