#include <planet/sdl/draw.hpp>


void planet::sdl::draw_hex(
        renderer &renderer,
        panel &space,
        hexmap::coordinates const loc,
        float const radius) {
    auto const vertices = loc.vertices(1.0f, radius);
    std::array<SDL_Point, 7> drawing{};
    for (std::size_t index{}; auto v : vertices) {
        auto const p = space.into(v);
        drawing[index++] = {int(p.x()), int(p.y())};
    }
    drawing[6] = drawing[0];
    renderer.lines(drawing);
}
