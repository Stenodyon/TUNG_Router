#include "asolver.hpp"

#include <set>
#include <queue>
#include <algorithm>

#include "assert.hpp"

#ifdef _DEBUG
#define LOG(msg) std::cout << msg << std::endl
#else
#define LOG(x)
#endif

#if 0
static std::vector<vi2> directions { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
#else
static std::vector<vi2> directions {
    {1, 0}, {-1, 0}, {0, 1}, {0, -1},
    {1, 1}, {-1, 1}, {1, -1}, {-1, -1}
};
#endif
static uint_t cost_estimate(
        const vi2 & start,
        const vi2 & direction,
        const std::vector<vi2> & ends);
static std::vector<vi2> reconstruct_path(const grid<vi2> & came_from,
        const vi2 & last);

typedef std::vector<vi2> container_t;

std::vector<vi2> find_path(const vi2 & start,
        const std::vector<vi2> & ends,
        grid<int> * map)
{
    auto size = map->get_size();

#ifdef _DEBUG
    if((unsigned)start.x >= size.x || (unsigned)start.y >= size.y)
    {
        std::cerr << "Start is outside boundaries" << std::endl;
        ASSERT(false, false, !=);
    }
    ASSERT((*map)[start], OBSTRUCTION, !=);
    bool has_sol = false;
    for(auto end : ends)
    {
        if((unsigned)end.x < size.x && (unsigned)end.y < size.y)
        {
            has_sol = true;
            break;
        }
        if((*map)[end] == VOID)
        {
            has_sol = true;
            break;
        }
    }
    ASSERT(has_sol, true, ==);
#endif

    grid<uint_t> g_score(size.x, size.y);
    g_score.fill(-1);
    g_score[start] = 0;

    grid<uint_t> f_score(size.x, size.y);
    f_score.fill(-1);
    f_score[start] = cost_estimate(start, {-2, -2}, ends);

    grid<vi2> came_from(size.x, size.y);
    came_from.fill({-1, -1});

    grid<vi2> dir(size.x, size.y);
    dir.fill({-2, -2});

    grid<bool> closed(size.x, size.y);
    closed.fill(false);

    auto f_score_comp =
        [&f_score](const vi2 & a, const vi2 & b)
        { return f_score[a] > f_score[b]; };
    std::priority_queue<vi2,
        container_t,
        decltype(f_score_comp)> open(f_score_comp);
    open.push(start);

    grid<bool> open_set(size.x, size.y);
    open_set.fill(false);
    open_set[start] = true;

    while(!open.empty())
    {
        vi2 current = v2(open.top());
        LOG("Current node: " << current);
        open.pop();
        open_set[current] = false;
        if(std::find(ends.begin(), ends.end(), current) != ends.end())
            return reconstruct_path(came_from, current);
        closed[current] = true;
        //assert(closed.find(current) != closed.end());

        for(auto direction : directions)
        {
            if(dir[current] != vi2{-2, -2} && dir[current] != direction
                    && ((*map)[current] & DIR_MASK))
                continue; // Cannot turn if there is a wire passing this cell
            if(((*map)[current] & HOR_WIRE) && direction.y == 0)
                continue;
            if(((*map)[current] & VERT_WIRE) && direction.x == 0)
                continue;
            if(((*map)[current] & SLASH_WIRE)
                    && direction.manhattan() == 2
                    && direction.x != direction.y)
                continue;
            if(((*map)[current] & BSLASH_WIRE)
                    && direction.manhattan() == 2
                    && direction.x == direction.y)
                continue;

            auto neighbor = current + direction;
            LOG("Current neighbor: " << neighbor);
            if(((unsigned)neighbor.x >= size.x) || ((unsigned)neighbor.y >= size.y))
                continue; // if out of bounds

            if(closed[neighbor]) // if in closed set
                continue; // skip

            if((*map)[neighbor] == OBSTRUCTION) // if cell obstructed
                continue; // skip
            if(!open_set[neighbor])
            {
                open.push(neighbor);
                open_set[neighbor] = true;
                LOG("   added to open");
            }

            uint_t score = g_score[current] + (!(direction.aligned()) ? 3 : 2);
            //uint_t score = g_score[current] + 1;
            if(dir[current] != vi2{-2, -2} && dir[current] != direction)
                score += TURN_COST;
#if 0
            if((*map)[neighbor] & DIR_MASK) // if crossing a wire
                score += 1000; // add penalty
#endif

            if(score >= g_score[neighbor])
                continue; // Not a faster route

            came_from[neighbor] = current;
            dir[neighbor] = direction;
            g_score[neighbor] = score;
            f_score[neighbor] = score + cost_estimate(neighbor, direction, ends);
        }
    }

    return std::vector<vi2>();
}

static uint_t cost_estimate(
        const vi2 & start,
        const vi2 & direction,
        const std::vector<vi2> & ends)
{
    return 0;
    uint_t min = -1;
    for(auto end : ends)
    {
        auto diff = end - start;
        auto abs_x = std::abs(diff.x);
        auto abs_y = std::abs(diff.y);
        uint_t diag = std::min(abs_x, abs_y);
        uint_t strt = std::max(abs_x, abs_y) - diag;
        uint_t distance = diag * 3 + strt * 2;
        if(diag != 0)
            distance += TURN_COST;
        if(direction != vi2{-2, -2} && direction.normalized() != diff.normalized())
            distance += TURN_COST;
        if(distance < min)
            min = distance;
    }
    return min;
}

static std::vector<vi2> reconstruct_path(const grid<vi2> & came_from,
        const vi2 & last)
{
    std::vector<vi2> path;
    vi2 current = last;
    do
    {
        path.push_back(current);
        current = came_from[current];
    }
    while(current != vi2{-1, -1});
    return path;
}
