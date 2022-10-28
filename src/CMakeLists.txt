find_package(SDL2 REQUIRED)

add_library(planet-sdl
        init.cpp
        renderer.cpp
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
    )
