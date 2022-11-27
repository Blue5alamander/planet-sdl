#pragma once


#include <planet/affine2d.hpp>
#include <planet/sdl/renderer.hpp>


namespace planet::sdl::ui {


    /// Combine these to determine how an element fits inside the given space
    /**
     * When none are specified then the content is stretched to fill the space.
     * Individual flags represent a pull in that direction and will turn
     * stretching off. Two opposing flags will causing centring along that axis.
     */
    enum gravity : unsigned char {
        fill = 0,
        left = 1,
        right = 2,
        top = 4,
        bottom = 8
    };
    inline gravity operator|(gravity l, gravity r) {
        return static_cast<gravity>(
                static_cast<unsigned char>(l)
                bitor static_cast<unsigned char>(r));
    }

    /// Calculate the extent within the outer extent that the inner will have
    /// based on the gravity passed in
    affine::extent2d
            within(gravity,
                   affine::extent2d const &outer,
                   affine::extent2d const &inner);

    /// A container for another element
    template<typename C>
    struct box {
        /// What is inside the box
        using content_type = C;
        content_type content;
        /// The size of the box in its container's coordinate system
        gravity inner =
                gravity::left | gravity::right | gravity::top | gravity::bottom;
        /// The amount of padding to be added around the content.
        float hpadding = {}, vpadding = {};

        /// Calculate the extents of the box
        affine::extent2d extents() const {
            auto const inner = content.extents();
            return {{inner.top_left.x() - vpadding,
                     inner.top_left.y() - hpadding},
                    {inner.bottom_right.x() + vpadding,
                     inner.bottom_right.y() + hpadding}};
        }

        /// Draw the content within the area outlined by the top left and bottom
        /// right corners passed in. All calculations are done in the screen
        /// space co-ordinate system
        void draw_within(renderer &r, affine::extent2d const outer) {
            auto const area = within(inner, outer, extents());
            content.draw_within(r, area);
        }
    };


}
