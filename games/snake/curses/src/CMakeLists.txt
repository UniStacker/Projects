add_executable(snake
    main.cpp
    game.cpp
    input.cpp
    render.cpp
    terminal.cpp
)

target_include_directories(snake PUBLIC ../include)

target_sources(snake PRIVATE
    ../include/game_fwd.h
)
