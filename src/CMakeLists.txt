if (NOT TARGET SDL2-static)
    find_package(SDL2)
    if("${SDL2_LIBRARIES}" STREQUAL "")
        set(SDL2_LIBRARIES SDL2)
        set(SDL2_INCLUDE_DIRS /usr/include/SDL2)
    endif()
else()
    set(SDL2_LIBRARIES SDL2-static)
endif()


add_library(planet-sdl
        audio.cpp
        draw.cpp
        event-loop.cpp
        init.cpp
        renderer.cpp
        surface.cpp
        text.cpp
        texture.cpp
        ttf.cpp
        window.cpp
    )
target_include_directories(planet-sdl PUBLIC
        ${SDL2_INCLUDE_DIRS}
        ../include
    )
target_include_directories(planet-sdl PUBLIC)
target_link_libraries(planet-sdl PUBLIC
        ${SDL2_LIBRARIES}
        SDL2_ttf
        planet
        felspar-io
    )
