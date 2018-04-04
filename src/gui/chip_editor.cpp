#include "chip_editor.hpp"

#include <wx/notebook.h>
#include <cstdlib>
#include <string>

ChipEditor::ChipEditor(wxWindow * parent, wxWindowID id,
        std::vector<wxString> folders,
        const std::string & name,
        const std::string & folder,
        chip_type * to_edit)
    : wxDialog(parent, id, "New Chip",
            wxDefaultPosition, wxDefaultSize,
            wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
    chip_width(3), chip_height(8),
    chip_name("new_chip"),
    chip_folder("misc")
{
    const std::unordered_map<std::string, vu2> * pins = nullptr;
    if(to_edit != nullptr) {
        chip_width = to_edit->width;
        chip_height = to_edit->height;
        pins = &(to_edit->pin_labels);
    }
    else {
        chip_width = 3;
        chip_height = 8;
    }
    chip_name = name;
    chip_folder = folder;

    auto * sizer = new wxBoxSizer(wxHORIZONTAL);

    chip_display = new ChipDisplay(this, wxID_ANY);
    chip_display->SetName(chip_name);

    sizer->Add(make_input_panel(this, folders, pins), 1, wxEXPAND);
    sizer->Add(chip_display, 1, wxEXPAND);

    SetSizerAndFit(sizer);
}

chip_type ChipEditor::get_created_chip()
{
    int width = chip_display->GetWidth();
    int height = chip_display->GetHeight();

    chip_type type(width, height);

    for(const auto& [offset, label] : chip_display->GetLabels(0))
    {
        if(offset >= width)
            continue;
        type.add_pin(NORTH, offset, label.ToStdString());
    }
    for(const auto& [offset, label] : chip_display->GetLabels(1))
    {
        if(offset >= width)
            continue;
        type.add_pin(SOUTH, offset, label.ToStdString());
    }
    for(const auto& [offset, label] : chip_display->GetLabels(2))
    {
        if(offset >= height)
            continue;
        type.add_pin(WEST, offset, label.ToStdString());
    }
    for(const auto& [offset, label] : chip_display->GetLabels(3))
    {
        if(offset >= height)
            continue;
        type.add_pin(EAST, offset, label.ToStdString());
    }

    return type;
}

wxString ChipEditor::get_chip_folder()
{
    return chip_folder;
}

wxString ChipEditor::get_chip_name()
{
    return chip_name;
}

int ChipEditor::get_chip_width()
{
    return std::stoi(width_field->GetLineText(0).ToStdString());
}

int ChipEditor::get_chip_height()
{
    return std::stoi(height_field->GetLineText(0).ToStdString());
}

wxWindow * ChipEditor::make_input_panel(wxWindow * parent,
        std::vector<wxString> & folders,
        const std::unordered_map<std::string, vu2> * pin_labels)
{
    auto * panel = new wxPanel(parent);
    wxBoxSizer * sizer = new wxBoxSizer(wxVERTICAL);

    auto name_panel = make_name_panel(panel, folders);
    auto pin_panel = make_pin_window(panel, pin_labels);
    auto buttons_panel = make_buttons(panel);

    sizer->Add(name_panel, 0, wxTOP | wxLEFT | wxRIGHT, 10);
    sizer->Add(pin_panel, 1, wxEXPAND);
    sizer->Add(buttons_panel, 0, wxALIGN_RIGHT);

    panel->SetSizerAndFit(sizer);
    return panel;
}

wxWindow * ChipEditor::make_name_panel(wxWindow * parent,
        std::vector<wxString> & folders)
{
    auto * panel = new wxPanel(parent);
    auto * box = new wxStaticBoxSizer(wxVERTICAL, panel, "Board Properties");
    auto * sizer = new wxFlexGridSizer(3, 4, 2, 10);

    auto * name_field = new wxTextCtrl(panel, ID_NAME_FIELD, chip_name,
            wxDefaultPosition, wxDefaultSize,
            0,
            detail::ChipNameValidator(&chip_name));
    auto * folder_chooser = new wxComboBox(panel, ID_FOLDER_FIELD);
    for(const auto& folder : folders)
        folder_chooser->Append(folder);
    folder_chooser->ChangeValue(chip_folder);

    width_field = new wxTextCtrl(panel, ID_WIDTH_FIELD, wxString{} << chip_width,
            wxDefaultPosition, wxDefaultSize,
            wxTE_RIGHT,
            wxIntegerValidator<int>());
    height_field = new wxTextCtrl(panel, ID_HEIGHT_FIELD, wxString{} << chip_height,
            wxDefaultPosition, wxDefaultSize,
            wxTE_RIGHT,
            wxIntegerValidator<int>());
    // Name
    sizer->Add(new wxStaticText(panel, wxID_ANY, "Name: "), 0,
            wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    sizer->Add(name_field, 1,
            wxALIGN_CENTER_VERTICAL);
    // Folder
    sizer->Add(new wxStaticText(panel, wxID_ANY, "Folder: "), 0,
            wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    sizer->Add(folder_chooser, 1,
            wxALIGN_CENTER_VERTICAL);
    // Width
    sizer->Add(new wxStaticText(panel, wxID_ANY, "Width: "), 0,
            wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    sizer->Add(width_field, 1, wxALIGN_CENTER_VERTICAL);
    sizer->AddSpacer(0); sizer->AddSpacer(0);
    // Height
    sizer->Add(new wxStaticText(panel, wxID_ANY, "Height: "), 0,
            wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    sizer->Add(height_field, 1, wxALIGN_CENTER_VERTICAL);
    sizer->AddSpacer(0); sizer->AddSpacer(0);

    box->Add(sizer);

    panel->SetSizerAndFit(box);

    name_field->Bind(wxEVT_TEXT,
            [name_field, this](wxCommandEvent&)
            {
                chip_name = name_field->GetLineText(0);
                chip_display->SetName(chip_name);
            },
            ID_NAME_FIELD);
    width_field->Bind(wxEVT_TEXT,
            [this](wxCommandEvent&)
            {
                int width = get_chip_width();
                if(width > 100)
                {
                    width = 100;
                    width_field->ChangeValue(wxString{} << width);
                }
                top_pins->Resize(width);
                bottom_pins->Resize(width);
                chip_display->SetWidth(width);
            },
            ID_WIDTH_FIELD);
    height_field->Bind(wxEVT_TEXT,
            [this](wxCommandEvent&)
            {
                int height = get_chip_height();
                if(height > 100)
                {
                    height = 100;
                    height_field->ChangeValue(wxString{} << height);
                }
                left_pins->Resize(height);
                right_pins->Resize(height);
                chip_display->SetHeight(height);
            },
            ID_HEIGHT_FIELD);
    folder_chooser->Bind(wxEVT_TEXT,
            [folder_chooser, this](wxCommandEvent&)
            {
                chip_folder = folder_chooser->GetValue();
            });

    return panel;
}

wxWindow * ChipEditor::make_pin_window(wxWindow * parent,
                const std::unordered_map<std::string, vu2> * pin_labels)
{
    auto * panel = new wxNotebook(parent, wxID_ANY);
    left_pins = new detail::PinListCtrl(panel, wxID_ANY, chip_height);
    right_pins = new detail::PinListCtrl(panel, wxID_ANY, chip_height);
    top_pins = new detail::PinListCtrl(panel, wxID_ANY, chip_width);
    bottom_pins = new detail::PinListCtrl(panel, wxID_ANY, chip_width);

    if(pin_labels != nullptr)
    {
        const std::vector<detail::PinListCtrl*> grids = {
            top_pins, bottom_pins, right_pins, left_pins
        };
        for(const auto & [label, pos] : *pin_labels)
        {
            const auto & [side, offset] = pos;
            auto grid = grids[side];
            grid->SetCellValue(offset, 0, label);
        }
    }

    panel->AddPage(left_pins, "Left", true);
    panel->AddPage(right_pins, "Right", true);
    panel->AddPage(top_pins, "Top", true);
    panel->AddPage(bottom_pins, "Bottom", true);
    panel->SetSelection(0);

    top_pins->Bind(wxEVT_GRID_CELL_CHANGED, [this](wxGridEvent& event)
            {
                int row = event.GetRow();
                wxString value = top_pins->GetCellValue(row, 0);
                chip_display->SetLabel(0, row, value);
            });
    bottom_pins->Bind(wxEVT_GRID_CELL_CHANGED, [this](wxGridEvent& event)
            {
                int row = event.GetRow();
                wxString value = bottom_pins->GetCellValue(row, 0);
                chip_display->SetLabel(1, row, value);
            });
    left_pins->Bind(wxEVT_GRID_CELL_CHANGED, [this](wxGridEvent& event)
            {
                int row = event.GetRow();
                wxString value = left_pins->GetCellValue(row, 0);
                chip_display->SetLabel(2, row, value);
            });
    right_pins->Bind(wxEVT_GRID_CELL_CHANGED, [this](wxGridEvent& event)
            {
                int row = event.GetRow();
                wxString value = right_pins->GetCellValue(row, 0);
                chip_display->SetLabel(3, row, value);
            });

    return panel;
}

wxWindow * ChipEditor::make_buttons(wxWindow * parent)
{
    wxPanel * buttons_panel = new wxPanel(parent);
    wxBoxSizer * buttons_sizer = new wxBoxSizer(wxHORIZONTAL);
    auto create_button = new wxButton(buttons_panel, ID_CREATE, "Create");
    auto cancel_button = new wxButton(buttons_panel, ID_CANCEL, "Cancel");
    buttons_sizer->Add(create_button, 0, wxALL, 10);
    buttons_sizer->Add(cancel_button, 0, wxALL, 10);
    buttons_panel->SetSizerAndFit(buttons_sizer);

    create_button->Bind(wxEVT_BUTTON,
            [this](wxCommandEvent&) { this->EndModal(RETURN_OK); });
    cancel_button->Bind(wxEVT_BUTTON,
            [this](wxCommandEvent&) { this->EndModal(RETURN_CANCEL); });

    return buttons_panel;
}

namespace detail
{
    PinListCtrl::PinListCtrl(wxWindow * parent, wxWindowID id, int size)
        : wxGrid(parent, id,
                wxDefaultPosition, wxDefaultSize)
    {
        CreateGrid(size, 1);
        SetColLabelValue(0, "Label");
        DisableDragColSize();
        DisableDragRowSize();
    }

    void PinListCtrl::Resize(int new_size)
    {
        int size = GetNumberRows();
        int delta = std::abs(new_size - size);

        if(delta > 100)
        {
            std::cout << "Delta (" << delta << ") is over 100" << std::endl;
            abort();
        }

        if(new_size > size)
            AppendRows(delta);
        else if(size > new_size)
            DeleteRows(new_size, delta);
    }
}

ChipDisplay::ChipDisplay(wxWindow * parent, wxWindowID id,
        const wxString & name)
    : wxPanel(parent, id,
            wxDefaultPosition, wxDefaultSize,
            wxTAB_TRAVERSAL | wxFULL_REPAINT_ON_RESIZE),
    name(name), width(3), height(8)
{
    Bind(wxEVT_PAINT, &ChipDisplay::OnPaint, this);
}

void ChipDisplay::SetName(const wxString & name)
{
    this->name = name;
    Refresh();
}

void ChipDisplay::SetWidth(int width)
{
    this->width = width;
    Refresh();
}

void ChipDisplay::SetHeight(int height)
{
    this->height = height;
    Refresh();
}

void ChipDisplay::SetLabel(int side, int index, const wxString & text)
{
    if(text == "")
        labels[side].erase(index);
    else
        labels[side][index] = text;
    Refresh();
}

void ChipDisplay::OnPaint(wxPaintEvent&)
{
    wxPaintDC dc(this);

    auto [s_width, s_height] = dc.GetSize();

    dc.SetTextForeground(*wxWHITE);
    dc.SetTextBackground(wxTransparentColour);
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.SetBackground(*wxBLACK_BRUSH);
    dc.Clear();

    auto [n_width, n_height] = dc.GetTextExtent(name);

    const double scale = 25;

    dc.SetPen(*wxYELLOW_PEN);
    dc.DrawRectangle(
            (s_width - scale * width) / 2,
            (s_height - scale * height) / 2,
            scale * width, scale * height);
    if(height > width)
    {
        dc.DrawRotatedText(name,
                (s_width - n_height) / 2,
                (s_height + n_width) / 2,
                90.);
    }
    else
    {
        dc.DrawText(name,
                (s_width - n_width) / 2,
                (s_height - n_height) / 2);
    }

    //dc.DrawText(wxString{} << labels[0].size(), 10, 10);

    const int left_side = (s_width - scale * width) / 2;
    const int right_side = (s_width + scale * width) / 2;
    const int top_side = (s_height - scale * height) / 2;
    const int bottom_side = (s_height + scale * height) / 2;
    const int pin_size = (scale * 3) / 4;
    for(auto [offset, label] : labels[0])
    {
        if(offset >= width)
            continue;
        const int x = left_side + scale / 2 + scale * offset;
        const int y = top_side - scale / 2;

        const auto [l_width, l_height] = dc.GetTextExtent(label);

        dc.SetPen(*wxWHITE_PEN);
        dc.DrawRectangle(
                x - pin_size / 2,
                y - pin_size / 2,
                pin_size, pin_size);
        dc.DrawRotatedText(label,
                x - (l_height * 5) / 8,
                y - scale / 2,
                90.);
    }
    for(auto [offset, label] : labels[1])
    {
        if(offset >= width)
            continue;
        const int x = left_side + scale / 2 + scale * offset;
        const int y = bottom_side + scale / 2;

        const auto [l_width, l_height] = dc.GetTextExtent(label);

        dc.SetPen(*wxWHITE_PEN);
        dc.DrawRectangle(
                x - pin_size / 2,
                y - pin_size / 2,
                pin_size, pin_size);
        dc.DrawRotatedText(label,
                x - (l_height * 5) / 8,
                y + scale / 2 + l_width,
                90.);
    }
    for(auto [offset, label] : labels[2])
    {
        if(offset >= height)
            continue;
        const int x = left_side - scale / 2;
        const int y = top_side + scale / 2 + scale * offset;

        const auto [l_width, l_height] = dc.GetTextExtent(label);

        dc.SetPen(*wxWHITE_PEN);
        dc.DrawRectangle(
                x - pin_size / 2,
                y - pin_size / 2,
                pin_size, pin_size);
        dc.DrawText(label,
                x - scale / 2 - l_width,
                y - (l_height * 5) / 8);
    }
    for(auto [offset, label] : labels[3])
    {
        if(offset >= height)
            continue;
        const int x = right_side + scale / 2;
        const int y = top_side + scale / 2 + scale * offset;

        const auto [l_width, l_height] = dc.GetTextExtent(label);

        dc.SetPen(*wxWHITE_PEN);
        dc.DrawRectangle(
                x - pin_size / 2,
                y - pin_size / 2,
                pin_size, pin_size);
        dc.DrawText(label,
                x + scale / 2,
                y - (l_height * 5) / 8);
    }
}
