if(TARGET planet-examples)
    add_executable(mouse-example-planet-sdl mouse.cpp)
    target_link_libraries(mouse-example-planet-sdl planet-sdl)
    install(TARGETS mouse-example-planet-sdl
        EXPORT mouse-example-planet-sdl
        RUNTIME DESTINATION bin)

    add_executable(tone-example-planet-sdl tone-sdl.cpp)
    target_link_libraries(tone-example-planet-sdl planet-sdl)
endif()
