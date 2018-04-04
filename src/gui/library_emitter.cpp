#include "library_emitter.hpp"

LibraryEmitter & LibraryEmitter::EmitHeader(const std::string & name)
{
    return Emit("library " + name).PushIndent();
}

LibraryEmitter & LibraryEmitter::EmitFooter()
{
    return PopIndent().Emit("endlibrary");
}

LibraryEmitter & LibraryEmitter::EmitFolder(const std::string & name)
{
    return Emit("folder " + name).PushIndent();
}

LibraryEmitter & LibraryEmitter::EmitEndFolder()
{
    return PopIndent().Emit("endfolder");
}

LibraryEmitter & LibraryEmitter::EmitChip(const std::string & name,
        int width, int height)
{
    std::ostringstream sstream;
    sstream << "chip " << name << " " << width << " " << height;
    return Emit(sstream.str()).PushIndent();
}

LibraryEmitter & LibraryEmitter::EmitPin(const std::string & name,
        int side, int offset)
{
    std::ostringstream sstream;
    sstream << "pin " << name << " " << side << " " << offset;
    return Emit(sstream.str());
}

LibraryEmitter & LibraryEmitter::EmitEndChip()
{
    return PopIndent().Emit("endchip");
}
