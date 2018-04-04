#pragma once

#include "../command_emitter.hpp"

class LibraryEmitter : public CommandEmitter<LibraryEmitter>
{
    public:
        LibraryEmitter & EmitHeader(const std::string & name = "lib");
        LibraryEmitter & EmitFooter();
        LibraryEmitter & EmitFolder(const std::string & name);
        LibraryEmitter & EmitEndFolder();
        LibraryEmitter & EmitChip(const std::string & name, int width, int height);
        LibraryEmitter & EmitPin(const std::string & name, int side, int offset);
        LibraryEmitter & EmitEndChip();
};
