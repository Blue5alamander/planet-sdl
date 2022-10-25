find_package(SDL2 REQUIRED)
find_package(SDL2_TTF REQUIRED)

add_library(planet-sdl
        init.cpp
        renderer.cpp
        window.cpp
    )
target_include_directories(planet-sdl PUBLIC
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ../include
    )
target_include_directories(planet-sdl PUBLIC)
target_link_libraries(planet-sdl PUBLIC
        ${SDL2_LIBRARIES}
        SDL2_ttf
        ${SDL2_TTF_LIBRARIES}
        planet
    )
