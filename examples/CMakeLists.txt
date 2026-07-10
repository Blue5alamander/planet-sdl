if(TARGET planet-examples)
    add_executable(tone-example-planet-sdl tone-sdl.cpp)
    target_link_libraries(tone-example-planet-sdl planet-sdl)
endif()
