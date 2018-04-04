#pragma once

#include <wx/wx.h>

class PlacerControl : public wxControl
{
    public:
        PlacerControl() { Init(); }
        PlacerControl(wxWindow * parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = "PlacerControl")
            : wxControl(parent, winid, pos, size, style, val, name)
        {
            Init();
        }
        ~PlacerControl();

    private:
        wxSize DoGetBestClientSize() const override;

        void Init();
        void OnPaint(wxPaintEvent& event);

        wxDECLARE_DYNAMIC_CLASS(PlacerControl);
        //wxDECLARE_EVENT_TABLE();
};
