#include "graphics.hpp"

#include <exception>

#define SIZE 20
#define PEG_SIZE 10

GraphicsOutput::GraphicsOutput()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        throw std::runtime_error("SDL: " + std::string(SDL_GetError()));
    }
    window = SDL_CreateWindow("TUNG Router", 100, 100, 32 * SIZE, 32 * SIZE, SDL_WINDOW_SHOWN);
    if (window == nullptr)
    {
        std::string msg = "SDL: " + std::string(SDL_GetError());
        SDL_Quit();
        throw std::runtime_error(msg);
    }
    render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (render == nullptr)
    {
        std::string msg = "SDL: " + std::string(SDL_GetError());
        SDL_Quit();
        throw std::runtime_error(msg);
    }
    SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
    SDL_RenderClear(render);
}

GraphicsOutput::~GraphicsOutput()
{
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void GraphicsOutput::draw_map(const grid<int> & map)
{
    SDL_SetRenderDrawColor(render, 0, 255, 0, 255);
    auto [width, height] = map.get_size();
    for(uint_t y = 0; y < height; y++)
    {
        for(uint_t x = 0; x < width; x++)
        {
            const int value = map[{x, y}];
            if(value == 1)
            {
                const SDL_Rect rect{x * SIZE, y * SIZE, SIZE, SIZE};
                SDL_RenderFillRect(render, &rect);
            }
        }
    }
}

void GraphicsOutput::draw_path(const std::vector<vi2> & path)
{
    SDL_SetRenderDrawColor(render, 255, 0, 0, 127);
    {
        const auto& pos = path[0];
        const SDL_Rect peg{
            pos.x * SIZE + (SIZE - PEG_SIZE) / 2,
            pos.y * SIZE + (SIZE - PEG_SIZE) / 2,
            PEG_SIZE,
            PEG_SIZE};
        SDL_RenderFillRect(render, &peg);
    }
    for(uint_t i = 1; i < path.size(); i++)
    {
        const auto& pos = path[i];
        const auto& prev = path[i-1];
        const SDL_Rect peg{
            pos.x * SIZE + (SIZE - PEG_SIZE) / 2,
            pos.y * SIZE + (SIZE - PEG_SIZE) / 2,
            PEG_SIZE,
            PEG_SIZE};
        SDL_RenderFillRect(render, &peg);
        SDL_RenderDrawLine(render,
                pos.x * SIZE + SIZE / 2,
                pos.y * SIZE + SIZE / 2,
                prev.x * SIZE + SIZE / 2,
                prev.y * SIZE + SIZE / 2);
    }
}

void GraphicsOutput::loop()
{
    SDL_RenderPresent(render);
    bool looping = true;
    while(looping)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
                looping = false;
        }
        SDL_Delay(60);
    }
}