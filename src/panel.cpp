#include <planet/sdl/renderer.hpp>


/**
 * ## `planet::sdl::panel`
 */


planet::sdl::panel::panel() { feeder.post(*this, &panel::feed_children); }


planet::sdl::panel::panel(renderer &re) : panel{} { rend = &re; }


planet::sdl::panel::~panel() {
    if (parent) { parent->remove_child(*this); }
    /// Orphan the current children
    reparent_children(parent);
}


felspar::coro::task<void> planet::sdl::panel::feed_children() {
    while (true) {
        auto click = co_await mouse_click.next();
        for (auto &c : children) {
            if (click.x() >= c.top_left.x() and click.x() <= c.bottom_right.x()
                and click.y() >= c.top_left.y()
                and click.y() <= c.bottom_right.y()) {
                c.sub->mouse_click.push(c.sub->viewport.outof(click));
            }
        }
    }
}


void planet::sdl::panel::reparent_children(panel *const np) {
    for (auto &p : children) {
        p.sub->parent = np;
        if (np) { np->add_child(*p.sub, p.top_left, p.bottom_right); }
    }
}


void planet::sdl::panel::add_child(
        panel &c,
        affine::point2d const top_left,
        affine::point2d const bottom_right) {
    if (rend) { c.rend = rend; }
    c.parent = this;
    c.translate(top_left);
    children.emplace_back(&c, top_left, bottom_right);
}


void planet::sdl::panel::remove_child(panel &c) {
    auto pos =
            std::find_if(children.begin(), children.end(), [&c](auto const &p) {
                return &c == p.sub;
            });
    if (pos != children.end()) {
        c.parent = nullptr;
        c.rend = nullptr;
        children.erase(pos);
    }
}


void planet::sdl::panel::line(
        affine::point2d const cp1, affine::point2d const cp2) const {
    if (rend) [[likely]] {
        auto p1 = viewport.into(cp1);
        auto p2 = viewport.into(cp2);
        rend->line(p1.x(), p1.y(), p2.x(), p2.y());
    }
}


void planet::sdl::panel::copy(texture const &tex, affine::point2d const l) const {
    if (rend) [[likely]] {
        auto p = viewport.into(l);
        rend->copy(tex, p.x(), p.y());
    }
}
void planet::sdl::panel::copy(
        texture const &tex, affine::rectangle const r) const {
    if (rend) [[likely]] {
        auto tl = viewport.into(r.top_left);
        auto br = viewport.into(r.bottom_right());
        rend->copy(tex, tl.x(), tl.y(), br.x() - tl.x(), br.y() - tl.y());
    }
}


/**
 * ## `planet::sdl::panel::child`
 */


planet::sdl::panel::child::child(
        panel *const c, affine::point2d const tl, affine::point2d const br)
: sub{c}, top_left{tl}, bottom_right{br} {}
