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
        event-loop.cpp
        init.cpp
        surface.cpp
        ttf.cpp
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
        ../include/planet/sdl/rw_ops.hpp
        ../include/planet/sdl/surface.hpp
        ../include/planet/sdl/ttf.hpp
    )
set_property(TARGET planet-sdl PROPERTY VERIFY_INTERFACE_HEADER_SETS ON)
add_dependencies(check planet-sdl_verify_interface_header_sets)
