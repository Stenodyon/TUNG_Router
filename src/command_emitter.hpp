#pragma once

#include <sstream>
#include <cassert>
#include <iostream>

template <typename T>
class CommandEmitter
{
    public:
        T& PushIndent() {
            indent_depth++;
            return static_cast<T&>(*this);
        }

        T& PopIndent() {
            if(indent_depth == 0) {
                std::cerr << "CommandEmitter::PopIndent() -> CommandEmitter::indent_depth == 0"
                    << std::endl;
                abort();
            }
            indent_depth--;
            return static_cast<T&>(*this);
        }

        T& Emit(const std::string & command) {
            for(int count = 0; count < indent_depth; count++)
                stream << "    ";
            stream << command << "\n";
            return static_cast<T&>(*this);
        }

        std::string GetStr() const {
            return stream.str();
        }

    private:
        int indent_depth = 0;
        std::ostringstream stream;
};

class GenericCommandEmitter : public CommandEmitter<GenericCommandEmitter> {};
