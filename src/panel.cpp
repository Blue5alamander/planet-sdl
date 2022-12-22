#include <planet/sdl/renderer.hpp>


/**
 * ## `planet::sdl::panel`
 */


planet::sdl::panel::panel() { feeder.post(*this, &panel::feed_children); }


planet::sdl::panel::panel(panel &&p, felspar::source_location const &loc)
: panel{} {
    if (p.parent or not p.children.empty()) {
        throw felspar::stdexcept::logic_error{
                "A panel cannot be moved once it is in the hierarchy", loc};
    }
    rend = p.rend;
    viewport = p.viewport;
}


planet::sdl::panel::panel(renderer &re) : panel{} { rend = &re; }


planet::sdl::panel::~panel() {
    if (parent) { parent->remove_child(*this); }
    /// Orphan the current children
    reparent_children(parent);
}


felspar::coro::task<void> planet::sdl::panel::feed_children() {
    while (true) {
        auto click = co_await clicks.next();
        for (auto &c : children) {
            if (c.area) {
                if (click.location.x() >= c.area->top_left.x()
                    and click.location.x() <= c.area->bottom_right().x()
                    and click.location.y() >= c.area->top_left.y()
                    and click.location.y() <= c.area->bottom_right().y()) {
                    auto transformed{click};
                    transformed.location =
                            c.sub->viewport.outof(click.location);
                    c.sub->clicks.push(transformed);
                }
            }
        }
    }
}


void planet::sdl::panel::reparent_children(panel *const np) {
    for (auto &p : children) {
        p.sub->parent = np;
        if (np) {
            if (p.area) {
                np->add_child(*p.sub, *p.area);
            } else {
                np->add_child(*p.sub);
            }
        }
    }
}


planet::sdl::panel::child &planet::sdl::panel::add(panel *c) {
    if (rend) { c->rend = rend; }
    c->parent = this;
    children.emplace_back(c);
    return children.back();
}
void planet::sdl::panel::add_child(panel &c) { add(&c); }
void planet::sdl::panel::add_child(panel &c, affine::rectangle const area) {
    add(&c).area = area;
    c.translate(area.top_left);
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


void planet::sdl::panel::move_to(affine::rectangle const area) {
    if (parent) {
        auto pos = std::find_if(
                parent->children.begin(), parent->children.end(),
                [this](auto const &p) { return p.sub == this; });
        if (pos != parent->children.end()) {
            if (pos->area) { translate(-pos->area->top_left); }
            translate(area.top_left);
            pos->area = area;
        }
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


planet::sdl::panel::child::child(panel *const c) : area{}, sub{c} {}
planet::sdl::panel::child::child(panel *const c, affine::rectangle const a)
: area{a}, sub{c} {}
