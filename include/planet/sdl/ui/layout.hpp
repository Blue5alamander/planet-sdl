#pragma once


namespace planet::sdl::ui {


    /// A row of boxes. When presented they can line break pushing over spill
    /// into the space below
    template<typename C>
    struct breakable_row {
        using collection_type = C;
        using box_type = typename collection_type::value_type;
        collection_type items;
        /// Padding between items in the row
        float padding = {};
    };


}
