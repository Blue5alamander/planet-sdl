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

        affine::extent2d extents() const {
            float width = items[0].width();
            float height = items[0].height();
            for (std::size_t index{1}; index < items.size(); ++index) {
                width += padding + items[index].width();
                height = std::max(height, items[index].height());
            }
            return {{0, 0}, {width, height}};
        }
        float width() const {
            float width = items[0].width();
            for (std::size_t index{1}; index < items.size(); ++index) {
                width += padding + items[index].width();
            }
            return width;
        }
        float height() const {
            float height = items[0].height();
            for (std::size_t index{1}; index < items.size(); ++index) {
                height = std::max(height, items[index].height());
            }
            return height;
        }

        void draw_within(renderer &r, affine::extent2d const outer) const {
            auto left = outer.top_left.x();
            auto const top = outer.top_left.y(), bottom = outer.bottom_right.y();
            for (auto const &item : items) {
                auto const ex = item.extents();
                auto const width = ex.width();
                item.draw_within(
                        r, {{left, top}, {left + width, bottom}});
                left += width + padding;
            }
        }
    };


}
