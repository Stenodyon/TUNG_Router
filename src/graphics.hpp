#pragma once

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <vector>

#include "v2.hpp"
#include "grid.hpp"

class GraphicsOutput
{
    private:
        SDL_Window * window;
        SDL_Renderer * render;

    public:
        GraphicsOutput();
        ~GraphicsOutput();

        void clear();
        void draw_map(const grid<int> & map);
        void draw_path(const std::vector<vi2> & path);
        void present();
        void loop();
};
