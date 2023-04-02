#pragma once


#include <planet/ui/layout.hpp>
#include <planet/ui/layout.column.hpp>
#include <planet/sdl/texture.hpp>
#include <planet/sdl/ttf.hpp>


namespace planet::sdl::ui {


    /// ## A block of text
    class text {
        std::string string;
        [[maybe_unused]] sdl::font &font;

        std::vector<std::string_view> words;
        planet::ui::layout<float> elements;
        planet::ui::column<std::vector<texture>> lines;

      public:
        text(sdl::font &, std::string);

        affine::extents2d extents(affine::extents2d const &) const;
        void draw_within(renderer &r, affine::rectangle2d);

        /// ### Identify words within a string
        static std::vector<std::string_view> identify_words(std::string &);
    };


}
