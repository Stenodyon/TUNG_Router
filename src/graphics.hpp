#pragma once

#define SDL_MAIN_HANDLED
#include "SDL.h"

#include <vector>

#include "v2.hpp"
#include "grid.hpp"

class GraphicsOutput
{
    private:
        SDL_Window * window;
        SDL_Renderer * render;

        int width, height;

        void to_window_space(int & x, int & y);
        void to_window_space(SDL_Rect & rect);

    public:
        GraphicsOutput(int width, int height);
        ~GraphicsOutput();

        void clear();
        void draw_map(const grid<int> & map);
        void draw_path(const std::vector<vi2> & path);
        void present();
        void loop();
};
