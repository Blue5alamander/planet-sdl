#pragma once


#include <planet/sdl/surface.hpp>
#include <planet/sdl/texture.hpp>
#include <planet/ui/button.hpp>


namespace planet::sdl::ui {


    template<typename Return>
    class button final :
    public planet::ui::button<Return, texture, planet::queue::pmc<Return>> {
        using superclass =
                planet::ui::button<Return, texture, planet::queue::pmc<Return>>;


      public:
        using superclass::graphic;


        button(sdl::renderer &r, surface text, queue::pmc<Return> &o, Return v)
        : superclass{
                "planet::sdl::ui::button",
                {"planet::sdl::ui::button.graphic", r, std::move(text)},
                o,
                std::move(v)} {}
        button(std::string_view const n,
               sdl::renderer &r,
               surface text,
               queue::pmc<Return> &o,
               Return v)
        : superclass{
                n,
                {std::string{n} + ".graphic", r, std::move(text)},
                o,
                std::move(v)} {}


      private:
        void do_draw() override { graphic.draw(); }
    };


}
