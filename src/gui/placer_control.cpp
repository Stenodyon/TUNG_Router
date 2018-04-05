#include "placer_control.hpp"

const int PlacerControl::GRID_SCALE = 20;

PlacerControl::PlacerControl()
    : viewpos({0, 0}), buffer(GetSize())
{
    Init();
}

PlacerControl::~PlacerControl() {}

void PlacerControl::SetSelected(ChipInstance type)
{
    selected_chip = type;
}

wxSize PlacerControl::DoGetBestClientSize() const
{
    return wxSize{1920, 1080};
}

void PlacerControl::Init()
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    Bind(wxEVT_PAINT, PlacerControl::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
    Bind(wxEVT_SIZE, PlacerControl::OnResize, this);
    Bind(wxEVT_MOTION, PlacerControl::OnMouseMoved, this);
    Bind(wxEVT_LEFT_UP, PlacerControl::OnLeftClick, this);
    Bind(wxEVT_MOUSEWHEEL, PlacerControl::OnMouseWheel, this);
}

void PlacerControl::OnMouseMoved(wxMouseEvent & event)
{
    const auto mouse_pos = GetMousePos();
    if(event.RightIsDown())
    {
        const auto& [x, y] = prev_mouse_pos - mouse_pos;
        viewpos += vi2{x, y};
    }
    prev_mouse_pos = mouse_pos;

    if(selected_chip)
    {
        selected_chip.value().pos =
            ToGridSpace(GetMousePos() - ChipExtent(selected_chip.value()) / 2);
    }
    Refresh();
}

void PlacerControl::OnMouseWheel(wxMouseEvent & event)
{
    int rot = event.GetWheelRotation();
    double total_scaling = 1.0;
    if(rot > 0)
    {
        for(int i = 0; i < rot; i++)
            total_scaling *= 1.001;
    }
    else
    {
        for(int i = 0; i < -rot; i++)
            total_scaling /= 1.001;
    }
    const auto prev_center = viewpos + GetMousePos();
    const auto diff = vi2{0, 0} - GetMousePos();
    scaling *= total_scaling;
    const vi2 offset = {diff.x / total_scaling, diff.y / total_scaling};
    viewpos = prev_center + offset;
    Refresh();
}

void PlacerControl::OnLeftClick(wxMouseEvent & event)
{
    if(!event.Dragging())
    {
        if(selected_chip)
        {
            chip_instances.push_back(selected_chip.value());
            selected_chip = std::nullopt;
        }
    }
}

void PlacerControl::OnResize(wxSizeEvent & event)
{
    const auto& [width, height] = event.GetSize();
    if(buffer.GetWidth() < width)
        buffer.SetWidth(width + 100);
    if(buffer.GetHeight() < height)
        buffer.SetHeight(height + 100);
}

vi2 PlacerControl::GetMousePos()
{
    const auto& [screenx, screeny] = GetScreenPosition();
    const auto& [mousex, mousey] = wxGetMousePosition();
    return {mousex - screenx, mousey - screeny};
}

vi2 PlacerControl::ToGridSpace(vi2 point)
{
    const auto& [x, y] = point + viewpos;
    return { x / scaling / GRID_SCALE, y / scaling / GRID_SCALE };
}

vi2 PlacerControl::ToScreenSpace(vi2 point)
{
    const auto& [x, y] = point;
    return vi2{x * GRID_SCALE * scaling, y * GRID_SCALE * scaling} - viewpos;
}

vi2 PlacerControl::SnapToGrid(vi2 point)
{
    return ToScreenSpace(ToGridSpace(point));
}

vi2 PlacerControl::ChipExtent(const ChipInstance & chip)
{
    const auto& [w, h] = chip.chip->size();
    return {w * GRID_SCALE, h * GRID_SCALE};
}

void PlacerControl::DrawPoint(DC & dc, vi2 pos)
{
    const auto& [x, y] = pos;
    dc.DrawPoint(x, y);
}

void PlacerControl::DrawLine(DC & dc, vi2 start, vi2 end)
{
    const auto& [x1, y1] = start;
    const auto& [x2, y2] = end;
    dc.DrawLine(x1, y1, x2, y2);
}

void PlacerControl::OnPaint(wxPaintEvent& event)
{
    DC dc(this);

    dc.SetBackground(*wxBLACK_BRUSH);
    dc.Clear();

    PaintChipInstances(dc);
    if(selected_chip)
        PaintSelectedChip(dc);


    PaintGrid(dc);
}

void PlacerControl::PaintSelectedChip(DC & dc)
{
    PaintChip(dc, selected_chip.value());
}

void PlacerControl::PaintGrid(DC & dc)
{
    dc.SetPen(*wxWHITE);
    const auto& [width, height] = GetSize();
    for(int y = 0; y < height / GRID_SCALE; y++)
    {
        for(int x = 0; x < width / GRID_SCALE; x++)
        {
            DrawPoint(dc, ToScreenSpace({x, y}));
        }
    }
}

void PlacerControl::PaintChipInstances(DC & dc)
{
    for(const ChipInstance & instance : chip_instances)
        PaintChip(dc, instance);
}

void PlacerControl::PaintChip(DC & dc,
        const ChipInstance & chip_instance)
{
    const chip_type * chip = chip_instance.chip;
    const vi2 topleft = ToScreenSpace(chip_instance.pos);
    const vi2 bottomright =
        ToScreenSpace(chip_instance.pos + chip->size());
    const vi2 topright = {bottomright.x, topleft.y};
    const vi2 bottomleft = {topleft.x, bottomright.y};
    //Box
    dc.SetPen(*wxYELLOW);
    DrawLine(dc, topleft, topright);
    DrawLine(dc, topright, bottomright);
    DrawLine(dc, bottomright, bottomleft);
    DrawLine(dc, bottomleft, topleft);
}

wxIMPLEMENT_DYNAMIC_CLASS(PlacerControl, wxControl);
