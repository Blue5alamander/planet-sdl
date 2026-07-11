# SDL3 ships as two separate packages, each with its own CMake config and
# an imported target that carries its own interface include directory, so
# no manual include path is needed and the include guard picks <SDL3/...>.
# When BUILD_SDL_LIBRARIES is on, SDL3 / SDL3_ttf are added via
# add_subdirectory (in b5/_external) and the SDL3::SDL3 / SDL3_ttf::SDL3_ttf
# alias targets already exist -- only fall back to find_package for the
# system-package case (e.g. the default Linux build).
if(NOT TARGET SDL3::SDL3)
    find_package(SDL3 REQUIRED)
endif()
if(NOT TARGET SDL3_ttf::SDL3_ttf)
    find_package(SDL3_ttf REQUIRED)
endif()


add_library(planet-sdl
        audio.cpp
        event-loop.cpp
        init.cpp
        open_url.cpp
        surface.cpp
        ttf.cpp
    )
target_include_directories(planet-sdl PUBLIC ../include)
target_include_directories(planet-sdl PUBLIC)
target_link_libraries(planet-sdl PUBLIC
        SDL3::SDL3
        SDL3_ttf::SDL3_ttf
        planet
        felspar-io
    )
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
