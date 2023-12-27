#pragma once


#include <planet/ui/layout.hpp>
#include <planet/ui/layout.column.hpp>
#include <planet/ui/reflowable.hpp>
#include <planet/sdl/texture.hpp>
#include <planet/sdl/ttf.hpp>


namespace planet::sdl::ui {


    /// ## A block of text
    class text : public planet::ui::reflowable {
        renderer &r;
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

        constrained_type do_reflow(constrained_type const &within) override;
        affine::rectangle2d
                move_sub_elements(affine::rectangle2d const &r) override {
            return r;
        }

      public:
        text(renderer &, sdl::font &, std::string_view);

        /// ### Draw the texture
        void draw();

        /// ### Identify words within a string
        static std::vector<std::string_view> identify_words(std::string_view);
    };


}
