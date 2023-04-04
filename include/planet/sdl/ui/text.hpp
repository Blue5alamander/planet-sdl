#pragma once


#include <planet/ui/layout.hpp>
#include <planet/ui/layout.column.hpp>
#include <planet/sdl/texture.hpp>
#include <planet/sdl/ttf.hpp>


namespace planet::sdl::ui {


    /// ## A block of text
    class text {
        sdl::font &font;
        affine::extents2d space;

        struct word {
            std::string word;
            std::optional<sdl::texture> texture;
        };

        using layout_type =
                planet::ui::layout<std::vector<planet::ui::element<word>>>;
        using constrained_type = typename layout_type::constrained_type;
        layout_type elements;

      public:
        text(sdl::font &, std::string_view);

        void reflow(constrained_type within);
        affine::extents2d extents(affine::extents2d const &);
        void draw_within(renderer &r, affine::rectangle2d);

        /// ### Identify words within a string
        static std::vector<std::string_view> identify_words(std::string_view);
    };


}
