#pragma once

#include <string>
#include <memory>

#include <wx/wx.h>
#include <wx/treectrl.h>

#include "../chip.hpp"

#define ID_NEW_FOLDER 100
#define ID_RENAME_FOLDER 101
#define ID_DELETE_FOLDER 102

namespace detail
{
    struct FolderItem : public wxTreeItemData
    {
        std::string name;

        FolderItem(const std::string & name) : name(name) {}
    };

    struct ChipItem : public wxTreeItemData
    {
        std::shared_ptr<chip_type> type;

        ChipItem(chip_type * type) : type(type) {}
    };
}

class ChipLibrary : public wxTreeCtrl
{
    private:
        wxTreeItemId root_id;

        void make_image_list();

        bool IsFolder(wxTreeItemId & id);

        void OnRightClick(wxContextMenuEvent & event);
        void OnItemRightClick(wxTreeEvent & event);
        void OnFolderPopupClick(wxCommandEvent & event);
        wxTreeItemId FindFolder(const std::string & name);

    public:
        ChipLibrary(wxWindow * parent);

        wxTreeItemId AddFolder(const std::string & name);
        wxTreeItemId AddChip(const std::string & folder_name,
                const std::string & chip_name,
                const chip_type & chip);
        wxTreeItemId NewFolder();

        std::vector<wxString> GetFolders() const;
        
};