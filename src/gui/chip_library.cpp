#include "chip_library.hpp"

#include <wx/artprov.h>

ChipLibrary::ChipLibrary(wxWindow * parent)
    : wxTreeCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT | wxTR_EDIT_LABELS)
{
    make_image_list();

    root_id = AddRoot("root");

    //Bind(wxEVT_RIGHT_DOWN, [](wxMouseEvent&) {}); // Fixes a weird bug
    //Bind(wxEVT_CONTEXT_MENU, ChipLibrary::OnRightClick, this); // Causing bugs
    Bind(wxEVT_TREE_ITEM_MENU, ChipLibrary::OnItemRightClick, this);
}

void ChipLibrary::make_image_list()
{
    auto folder_image = wxArtProvider::GetBitmap(wxART_FOLDER).ConvertToImage();
    folder_image.Rescale(16, 16);
    auto folder_bitmap = wxBitmap(folder_image);
    const auto image_width = folder_bitmap.GetWidth();
    const auto image_height = folder_bitmap.GetHeight();

    wxImageList * image_list = new wxImageList(image_width, image_height);
    image_list->Add(folder_bitmap);
    AssignImageList(image_list);
}

bool ChipLibrary::IsFolder(wxTreeItemId & id)
{
    auto data = GetItemData(id);
    return dynamic_cast<detail::FolderItem*>(data) != nullptr;
}

void ChipLibrary::OnRightClick(wxContextMenuEvent & event)
{
    wxMenu popup_menu;
    popup_menu.Append(ID_NEW_FOLDER, "New Folder");
    popup_menu.Connect(wxEVT_MENU,
            wxCommandEventHandler(ChipLibrary::OnFolderPopupClick),
            NULL, this);
    PopupMenu(&popup_menu);
}

void ChipLibrary::OnItemRightClick(wxTreeEvent & event)
{
    auto item_id = event.GetItem();
    SelectItem(item_id);
    wxMenu popup_menu;
    if(IsFolder(item_id))
    {
        popup_menu.Append(ID_NEW_FOLDER, "New Folder");
        popup_menu.Append(ID_RENAME_FOLDER, "Rename");
        popup_menu.Append(ID_DELETE_FOLDER, "Delete");
        popup_menu.Bind(wxEVT_MENU, ChipLibrary::OnFolderPopupClick, this);
    }
    PopupMenu(&popup_menu);
}

void ChipLibrary::OnFolderPopupClick(wxCommandEvent & event)
{
    switch (event.GetId())
    {
        case ID_NEW_FOLDER:
            {
                NewFolder();
                break;
            }
        case ID_RENAME_FOLDER:
            {
                auto folder_id = GetSelection();
                EditLabel(folder_id);
                break;
            }
        case ID_DELETE_FOLDER:
            {
                auto folder_id = GetSelection();
                Delete(folder_id);
                break;
            }
    }
}

wxTreeItemId ChipLibrary::AddFolder(const std::string & name)
{
    return AppendItem(root_id, name, 0, -1, new detail::FolderItem(name));
}

wxTreeItemId ChipLibrary::NewFolder()
{
    auto new_folder_id = AddFolder("new folder");
    SelectItem(new_folder_id);
    EditLabel(new_folder_id);
    return new_folder_id;
}

std::vector<wxString> ChipLibrary::GetFolders() const
{
    std::vector<wxString> folders;

    wxTreeItemIdValue cookie;
    auto item = GetFirstChild(GetRootItem(), cookie);
    const int count = GetChildrenCount(GetRootItem());

    for(int index = 0; index < count; index++)
    {
        folders.push_back(GetItemText(item));
        item = GetNextChild(item, cookie);
    }

    return folders;
}
