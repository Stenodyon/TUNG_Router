#pragma once

#include <unordered_map>

#include <wx/wx.h>
#include <wx/valnum.h>
#include <wx/grid.h>

#include "../chip.hpp"

#define RETURN_OK 1
#define RETURN_CANCEL 2

#define ID_CREATE 200
#define ID_CANCEL 201

#define ID_NAME_FIELD 202
#define ID_FOLDER_FIELD 203
#define ID_WIDTH_FIELD 204
#define ID_HEIGHT_FIELD 205

#define DEFAULT_NAME "new_chip"
#define DEFAULT_FOLDER "misc"

namespace detail
{
    class ChipNameValidator : public wxTextValidator
    {
        public:
            ChipNameValidator(wxString * valPtr = NULL)
                : wxTextValidator(wxFILTER_EMPTY | wxFILTER_EXCLUDE_CHAR_LIST,
                        valPtr)
            {
                SetCharExcludes(" "); // No spaces allowed
            }
    };

    class PinListCtrl : public wxGrid
    {
        public:
            PinListCtrl(wxWindow * parent, wxWindowID id, int size);

            void Resize(int new_size);

        private:
    };
}

class ChipDisplay : public wxPanel
{
    public:
        ChipDisplay(wxWindow * parent, wxWindowID id,
                const wxString & name = "unnamed");

        void SetName(const wxString & name);
        void SetWidth(int width);
        void SetHeight(int height);
        void SetLabel(int side, int index, const wxString & text);

        int GetWidth() const { return width; }
        int GetHeight() const { return height; }
        const std::unordered_map<int, wxString> GetLabels(int side) const
        {
            return labels[side];
        }

    private:
        wxString name;
        int width, height;

        std::unordered_map<int, wxString> labels[4];

        void OnPaint(wxPaintEvent& event);
};

class ChipEditor : public wxDialog
{
    public:
        ChipEditor(wxWindow * parent, wxWindowID id,
                std::vector<wxString> folders,
                const std::string & name = "new_chip",
                const std::string & folder = "misc",
                chip_type * to_edit = nullptr);

        chip_type get_created_chip();
        wxString get_chip_name();
        wxString get_chip_folder();

    private:
        int chip_width, chip_height;
        wxString chip_name;
        wxString chip_folder;

        int get_chip_width();
        int get_chip_height();

        ChipDisplay * chip_display;
        wxTextCtrl * width_field, * height_field;

        detail::PinListCtrl * left_pins, * right_pins, * top_pins, * bottom_pins;

        wxWindow * make_input_panel(wxWindow * parent,
                std::vector<wxString> & folders,
                const std::unordered_map<std::string, vu2> * pin_labels);
        wxWindow * make_name_panel(wxWindow * parent,
                std::vector<wxString> & folders);
        wxWindow * make_pin_window(wxWindow * parent,
                const std::unordered_map<std::string, vu2> * pin_labels);
        wxWindow * make_buttons(wxWindow * parent);
};

