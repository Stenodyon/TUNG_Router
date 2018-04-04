#include "chip_library.hpp"

#include <wx/artprov.h>

#include <functional>
#include <stdexcept>

#include "../fileutils.h"
#include "library_emitter.hpp"
#include "../command_parser.hpp"

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
    auto chip_image = wxArtProvider::GetBitmap(wxART_EXECUTABLE_FILE).ConvertToImage();
    folder_image.Rescale(16, 16);
    chip_image.Rescale(16, 16);
    auto folder_bitmap = wxBitmap(folder_image);
    auto chip_bitmap = wxBitmap(chip_image);
    const auto image_width = folder_bitmap.GetWidth();
    const auto image_height = folder_bitmap.GetHeight();

    wxImageList * image_list = new wxImageList(image_width, image_height);
    image_list->Add(folder_bitmap);
    image_list->Add(chip_bitmap);
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

wxTreeItemId ChipLibrary::FindFolder(const std::string & name)
{
    wxTreeItemIdValue cookie;
    auto item = GetFirstChild(GetRootItem(), cookie);
    const int count = GetChildrenCount(GetRootItem());

    for(int index = 0; index < count; index++)
    {
        if(GetItemText(item) == name)
            return item;
        item = GetNextChild(item, cookie);
    }
    return item;
}

wxTreeItemId ChipLibrary::FindChip(const wxTreeItemId & folder, const std::string & name)
{
    wxTreeItemIdValue cookie;
    auto item = GetFirstChild(folder, cookie);
    const int count = GetChildrenCount(folder);

    for(int index = 0; index < count; index++)
    {
        if(GetItemText(item) == name)
            return item;
        item = GetNextChild(item, cookie);
    }
    return item;
}

wxTreeItemId ChipLibrary::AddFolder(const std::string & name)
{
    return AppendItem(root_id, name, 0, -1, new detail::FolderItem(name));
}

wxTreeItemId ChipLibrary::SetChip(const wxTreeItemId & folder,
        const std::string & chip_name,
        const chip_type & chip)
{
    wxTreeItemId prev_chip = FindChip(folder, chip_name);
    if(!prev_chip.IsOk())
    {
        return AppendItem(folder, chip_name, 1, -1,
                new detail::ChipItem(new chip_type(chip)));
    }
    SetItemData(prev_chip, new detail::ChipItem(new chip_type(chip)));
    return prev_chip;
}

wxTreeItemId ChipLibrary::SetChip(const std::string & folder_name,
        const std::string & chip_name,
        const chip_type & chip)
{
    wxTreeItemId folder = FindFolder(folder_name);
    if(!folder.IsOk())
    {
        folder = AddFolder(folder_name);
        return AppendItem(folder, chip_name, 1, -1,
                new detail::ChipItem(new chip_type(chip)));
    }
    return SetChip(folder, chip_name, chip);
}

wxTreeItemId ChipLibrary::NewFolder()
{
    auto new_folder_id = AddFolder("new folder");
    SelectItem(new_folder_id);
    EditLabel(new_folder_id);
    return new_folder_id;
}

chip_type * ChipLibrary::GetChip(const wxTreeItemId & chip) const
{
    auto chip_item = dynamic_cast<detail::ChipItem*>(GetItemData(chip));
    if(chip_item == nullptr)
        return nullptr;
    return chip_item->type.get();
}

std::optional<wxString> ChipLibrary::GetFolder(const wxTreeItemId & chip) const
{
    wxTreeItemId folder = GetItemParent(chip);
    if(!folder.IsOk()) // If chip is root
        return {};
    wxTreeItemId folder_parent = GetItemParent(folder);
    if(!folder_parent.IsOk()) // If "folder" is root
        return {};
    if(GetItemParent(folder_parent).IsOk()) // folder parent is root
        return {};
    return GetItemText(folder);
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

void ChipLibrary::SaveToFile(const std::string & filename) const
{
    write_file(filename, SaveToString());
}

std::string ChipLibrary::SaveToString() const
{
    auto root = GetRootItem();
    wxTreeItemIdValue cookie;
    auto folder = GetFirstChild(root, cookie);
    LibraryEmitter emitter;
    emitter.EmitHeader();
    while(folder.IsOk())
    {
        const std::string folder_name = GetItemText(folder).ToStdString();
        emitter.EmitFolder(folder_name);

        wxTreeItemIdValue chip_cookie;
        auto chip_id = GetFirstChild(folder, chip_cookie);
        while(chip_id.IsOk())
        {
            const std::string chip_name = GetItemText(chip_id).ToStdString();
            chip_type * type = GetChip(chip_id);
            emitter.EmitChip(chip_name, type->width, type->height);
            for(const auto & [label, pos] : type->pin_labels)
            {
                const auto & [side, offset] = pos;
                emitter.EmitPin(label, side, offset);
            }
            emitter.EmitEndChip();
            chip_id = GetNextChild(chip_id, chip_cookie);
        }

        emitter.EmitEndFolder();
        folder = GetNextChild(folder, cookie);
    }
    emitter.EmitFooter();
    return emitter.GetStr();
}

void ChipLibrary::LoadFromFile(const std::string & filename)
{
    LoadFromText(read_file(filename));
}

enum ParseState
{
    STATE_ROOT,
    STATE_LIB,
    STATE_FOLDER,
    STATE_CHIP,
};

void CheckState(const ParseState & current,
        const ParseState & expected)
{
    if(current == expected)
        return;
    switch(expected)
    {
        case STATE_ROOT:
            throw std::invalid_argument("expected 'library'");
        case STATE_LIB:
            throw std::invalid_argument("expected 'folder' or 'endlibrary'");
        case STATE_FOLDER:
            throw std::invalid_argument("expected 'chip' or 'endfolder'");
        case STATE_CHIP:
            throw std::invalid_argument("expected 'pin' or 'endchip'");
        default:
            std::cerr << "Invalid parse state: " << current << std::endl;
            abort();
    }
}

void ChipLibrary::LoadFromText(const std::string & contents)
{
    parser::CommandParser parser;
    auto state = STATE_ROOT;
    wxTreeItemId current_folder;
    std::string chip_name;
    chip_type * current_chip;

    std::function<void(std::string)> library_action = [&state](std::string name)
    {
        CheckState(state, STATE_ROOT);
        state = STATE_LIB;
    };
    parser.add_command("library", library_action);

    std::function<void()> endlibrary_action = [&state]()
    {
        CheckState(state, STATE_LIB);
        state = STATE_ROOT;
    };
    parser.add_command("endlibrary", endlibrary_action);

    std::function<void(std::string)> folder_action =
        [&state, &current_folder, this](std::string name)
    {
        CheckState(state, STATE_LIB);
        auto folder = FindFolder(name);
        if(folder.IsOk())
            current_folder = folder;
        else
            current_folder = AddFolder(name);
        state = STATE_FOLDER;
    };
    parser.add_command("folder", folder_action);

    std::function<void()> endfolder_action = [&state]()
    {
        CheckState(state, STATE_FOLDER);
        state = STATE_LIB;
    };
    parser.add_command("endfolder", endfolder_action);

    std::function<void(std::string, int, int)> chip_action =
        [&](std::string name, int width, int height)
    {
        CheckState(state, STATE_FOLDER);
        state = STATE_CHIP;
        current_chip = new chip_type(width, height);
        chip_name = name;
    };
    parser.add_command("chip", chip_action);

    std::function<void(std::string,int,int)> pin_action =
        [&](std::string name, int side, int offset)
    {
        CheckState(state, STATE_CHIP);
        current_chip->add_pin(side, offset, name);
    };
    parser.add_command("pin", pin_action);

    std::function<void()> endchip_action =
        [&]()
    {
        CheckState(state, STATE_CHIP);
        state = STATE_FOLDER;
        SetChip(current_folder, chip_name, *current_chip);
        delete current_chip; current_chip = nullptr;
    };
    parser.add_command("endchip", endchip_action);

    try
    {
        parser.parse(contents);
    } catch(parser::parse_error & error)
    {
        if(current_chip != nullptr)
            delete current_chip;
        throw error;
    }
}
