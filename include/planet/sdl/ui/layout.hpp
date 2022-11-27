#pragma once


namespace planet::sdl::ui {


    /// A row of boxes. When presented they can line break pushing over spill
    /// into the space below
    template<typename C>
    struct row {
        using collection_type = C;
        using box_type = typename collection_type::value_type;
        collection_type items;
        /// Padding between items in the row
        float padding = {};

        row(collection_type c, float const p)
        : items{std::move(c)}, padding{p} {}

        affine::extent2d extents(affine::extent2d const outer) const {
            auto const first_ex = items[0].extents(outer);
            float width = first_ex.width();
            float height = first_ex.height();
            for (std::size_t index{1}; index < items.size(); ++index) {
                auto const ex = items[index].extents(outer);
                width += padding + ex.width();
                height = std::max(height, ex.height());
            }
            return {{0, 0}, {width, height}};
        }

        void draw_within(renderer &r, affine::extent2d const outer) const {
            auto left = outer.top_left.x();
            auto const top = outer.top_left.y(),
                       bottom = outer.bottom_right.y();
            for (auto const &item : items) {
                auto const ex = item.extents(outer);
                auto const width = ex.width();
                item.draw_within(r, {{left, top}, {left + width, bottom}});
                left += width + padding;
            }
        }
    };


    /// Performs basic line breaks
    template<typename C>
    struct breakable_row : public row<C> {
        void draw_within(renderer &r, affine::extent2d const outer) const {
            row<C>::draw_within(r, outer);
        }
    };


}
