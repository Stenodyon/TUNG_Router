#pragma once

#include <optional>
#include <vector>

#include <wx/wx.h>
#include <wx/dcbuffer.h>

#include "../v2.hpp"
#include "../chip.hpp"
#include "chip_instance.hpp"

typedef wxAutoBufferedPaintDC DC;

class PlacerControl : public wxControl
{
    public:
        PlacerControl();
        PlacerControl(wxWindow * parent,
                wxWindowID winid = wxID_ANY)
            : wxControl(parent, winid,
                    wxDefaultPosition, wxDefaultSize,
                    wxFULL_REPAINT_ON_RESIZE)
        {
            Init();
        }
        ~PlacerControl();

        void SetSelected(ChipInstance chip);

    private:
        static const int GRID_SCALE;

        vi2 viewpos;
        double scaling = 1.0;
        wxBitmap buffer;
        std::optional<ChipInstance> selected_chip;

        std::vector<ChipInstance> chip_instances;

        wxSize DoGetBestClientSize() const override;

        vi2 prev_mouse_pos;
        vi2 GetMousePos();

        vi2 ToGridSpace(vi2 from_screen_space);
        vi2 ToScreenSpace(vi2 from_grid_space);
        vi2 SnapToGrid(vi2 point); // in screen space
        vi2 ChipExtent(const ChipInstance & chip);
        void DrawPoint(DC & dc, vi2 pos);
        void DrawLine(DC & dc, vi2 start, vi2 end);

        void Init();
        void OnMouseMoved(wxMouseEvent & event);
        void OnMouseWheel(wxMouseEvent & event);
        void OnLeftClick(wxMouseEvent & event);
        void OnResize(wxSizeEvent& event);
        void OnPaint(wxPaintEvent& event);
        void PaintGrid(DC & dc);
        void PaintSelectedChip(DC & dc);
        void PaintChipInstances(DC & dc);
        void PaintChip(DC & dc, const ChipInstance & chip);

        wxDECLARE_DYNAMIC_CLASS(PlacerControl);
        //wxDECLARE_EVENT_TABLE();
};
