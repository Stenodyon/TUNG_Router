#include "placer_control.hpp"

PlacerControl::~PlacerControl() {}

wxSize PlacerControl::DoGetBestClientSize() const
{
    return wxSize{1920, 1080};
}

void PlacerControl::Init()
{
    Bind(wxEVT_PAINT, PlacerControl::OnPaint, this);
}

void PlacerControl::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    dc.SetBackground(*wxBLACK_BRUSH);
    dc.Clear();
}

wxIMPLEMENT_DYNAMIC_CLASS(PlacerControl, wxControl);
