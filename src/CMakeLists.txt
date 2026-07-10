if(PLANET_SDL3)
    # SDL3 ships as two separate packages, each with its own CMake config and
    # an imported target that carries its own interface include directory, so
    # no manual include path is needed and the include guard picks <SDL3/...>.
    find_package(SDL3 REQUIRED)
    find_package(SDL3_ttf REQUIRED)
    set(planet_sdl_library SDL3::SDL3)
    set(planet_sdl_ttf_library SDL3_ttf::SDL3_ttf)
    set(planet_sdl_include_dirs)
else()
    if (NOT TARGET SDL2-static)
        find_package(SDL2)
        if("${SDL2_LIBRARIES}" STREQUAL "")
            set(SDL2_LIBRARIES SDL2)
            set(SDL2_INCLUDE_DIRS /usr/include/SDL2)
        endif()
    else()
        set(SDL2_LIBRARIES SDL2-static)
    endif()
    set(planet_sdl_library ${SDL2_LIBRARIES})
    set(planet_sdl_ttf_library SDL2_ttf)
    set(planet_sdl_include_dirs ${SDL2_INCLUDE_DIRS})
endif()


add_library(planet-sdl
        audio.cpp
        event-loop.cpp
        init.cpp
        open_url.cpp
        surface.cpp
        ttf.cpp
    )
target_include_directories(planet-sdl PUBLIC
        ${planet_sdl_include_dirs}
        ../include
    )
target_include_directories(planet-sdl PUBLIC)
target_link_libraries(planet-sdl PUBLIC
        ${planet_sdl_library}
        ${planet_sdl_ttf_library}
        planet
        felspar-io
    )
if(PLANET_SDL3)
    # PUBLIC so every consumer that includes planet/sdl/sdl.hpp or
    # planet/sdl/ttf.hpp picks up the matching <SDL3/...> include path.
    target_compile_definitions(planet-sdl PUBLIC PLANET_SDL3)
endif()
install(TARGETS planet-sdl LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)


target_sources(planet-sdl PUBLIC
  FILE_SET HEADERS
  BASE_DIRS ../include/
  FILES
        ../include/planet/sdl/audio.hpp
        ../include/planet/sdl/event-loop.hpp
        ../include/planet/sdl/forward.hpp
        ../include/planet/sdl/handle.hpp
        ../include/planet/sdl.hpp
        ../include/planet/sdl/init.hpp
        ../include/planet/sdl/open_url.hpp
        ../include/planet/sdl/rw_ops.hpp
        ../include/planet/sdl/sdl.hpp
        ../include/planet/sdl/surface.hpp
        ../include/planet/sdl/ttf.hpp
    )
set_property(TARGET planet-sdl PROPERTY VERIFY_INTERFACE_HEADER_SETS ON)
add_dependencies(check planet-sdl_verify_interface_header_sets)
