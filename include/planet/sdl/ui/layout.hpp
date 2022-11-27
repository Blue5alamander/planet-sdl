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

        affine::extents2d extents(affine::extents2d const outer) const {
            auto const first_ex = items[0].extents(outer);
            float width = first_ex.width;
            float height = first_ex.height;
            for (std::size_t index{1}; index < items.size(); ++index) {
                auto const ex = items[index].extents(outer);
                width += padding + ex.width;
                height = std::max(height, ex.height);
            }
            return {width, height};
        }

        void draw_within(renderer &r, affine::rectangle const outer) const {
            auto left = outer.top_left.x();
            auto const top = outer.top(), bottom = outer.bottom();
            for (auto const &item : items) {
                auto const ex = item.extents(outer.extents);
                auto const width = ex.width;
                item.draw_within(
                        r,
                        {{left, top}, affine::point2d{left + width, bottom}});
                left += width + padding;
            }
        }
    };


    /// Draws the items across multiple lines when needed
    template<typename C>
    struct breakable_row {
        using collection_type = C;
        using box_type = typename collection_type::value_type;
        collection_type items;
        /// Padding between items in the row
        float hpadding = {}, vpadding = {};

        breakable_row(collection_type c, float const hp, float const vp)
        : items{std::move(c)}, hpadding{hp}, vpadding{vp} {}

        affine::extents2d extents(affine::extents2d const outer) const {
            float const fit_width = outer.width;
            float max_width = {}, row_height = {}, total_height = {}, left{};
            for (auto const &item : items) {
                auto const item_ex = item.extents(outer);
                auto const item_w = item_ex.width;
                if (left + item_w > fit_width) {
                    max_width = std::max(max_width, left);
                    if (total_height) { total_height += vpadding; }
                    total_height += row_height;
                    left = {};
                    row_height = {};
                } else {
                    if (left) { left += hpadding; }
                    left += item_w;
                }
            }
            max_width = std::max(max_width, left);
            total_height += row_height;
            return {max_width, total_height};
        }
        void draw_within(renderer &r, affine::rectangle const outer) const {
            float const fit_width = outer.extents.width;
            float row_height = {}, x = {}, y = {};
            for (auto const &item : items) {
                auto const item_ex = item.extents(outer);
                auto const item_w = item_ex.width;
                if (x + item_w > fit_width) {
                    x = {};
                    if (y) { y += vpadding; }
                    y += row_height;
                    row_height = {};
                }
                if (x) { x += hpadding; }
                item.draw_within(
                        r,
                        {outer.top_left + affine::point2d{x, y},
                         {outer.right(), outer.bottom()}});
                row_height = std::max(row_height, item_ex.height());
                x += item_w;
            }
        }
    };


}
