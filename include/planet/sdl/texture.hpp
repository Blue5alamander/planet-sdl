#pragma once


#include <planet/affine2d.hpp>
#include <planet/sdl/handle.hpp>
#include <planet/ui/reflowable.hpp>
#include <planet/ui/scale.hpp>

#include <SDL.h>
#undef main


namespace planet::sdl {


    class renderer;
    class surface;


    /// ## Graphics textures
    /**
     * TODO There ought to be a separate texture type for UI use. One type owns
     * the texture it uses (for buttons, labels, text etc.) and another
     * references a texture loaded as part of the assets.
     */
    class texture final : public planet::ui::reflowable {
        handle<SDL_Texture, SDL_DestroyTexture> pt;
        affine::extents2d size;
        renderer *rp;


      public:
        texture(renderer &, surface const &);
        texture(std::string_view, renderer &, surface const &);

        SDL_Texture *get() const noexcept { return pt.get(); }


        /// ### Control how the texture is draw
        ui::scale fit = ui::scale::lock_aspect;
        using constrained_type = planet::ui::constrained2d<float>;


        /// ### Return the texture extents
        affine::extents2d const &extents() const noexcept { return size; }
        affine::extents2d extents(affine::extents2d) const noexcept;

        float width() const noexcept { return size.width; }
        std::size_t uzwidth() const noexcept { return size.uzwidth(); }
        float height() const noexcept { return size.height; }
        std::size_t uzheight() const noexcept { return size.uzheight(); }


        /// ### Draw the texture
        void draw();
        /// Draw the texture at the given screen co-ordinates
        void draw_within(renderer &, affine::rectangle2d const &) const;


      private:
        constrained_type do_reflow(constrained_type const &) override;
        affine::rectangle2d
                move_sub_elements(affine::rectangle2d const &o) override {
            return o;
        }
    };


}
