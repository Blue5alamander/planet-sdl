if (NOT TARGET SDL2-static)
    find_package(SDL2 REQUIRED)
endif()

add_library(planet-sdl
        asset_manager.cpp
        audio.cpp
        draw.cpp
        init.cpp
        renderer.cpp
        surface.cpp
        texture.cpp
        ttf.cpp
        window.cpp
    )
if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    target_sources(planet-sdl PRIVATE asset_manager.android.cpp)
else()
    target_sources(planet-sdl PRIVATE asset_manager.filesystem.cpp)
endif()
target_include_directories(planet-sdl PUBLIC
        ${SDL2_INCLUDE_DIRS}
        ../include
    )
target_include_directories(planet-sdl PUBLIC)
target_link_libraries(planet-sdl PUBLIC
        ${SDL2_LIBRARIES}
        SDL2_ttf
        planet
        planet-audio
        felspar-io
    )
if(TARGET SDL2-static)
    target_link_libraries(planet-sdl PUBLIC
            SDL2-static
        )
endif()
