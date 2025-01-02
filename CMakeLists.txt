cmake_minimum_required(VERSION 3.27)
project(FPSOnline)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(-DSRC_DIR="${SRC_DIR}")

set(GAME_SRC
    src/game/main.cpp
    src/game/Player.cpp
    src/game/FPSGame.cpp
    src/game/Enemy.cpp
    src/game/EnemyManager.cpp
)

set(GAME2_SRC
    src/game2/main.cpp
)

set(TEST_SRC
    tests/test_main.cpp
)

add_subdirectory(engine)

add_executable(game ${GAME_SRC})
target_include_directories(game PRIVATE src/game)
target_link_libraries(game PRIVATE Engine)

add_executable(game2 ${GAME2_SRC})
target_include_directories(game2 PRIVATE src/game2)
target_link_libraries(game2 PRIVATE Engine)

add_executable(Tests ${TEST_SRC})
target_include_directories(Tests PRIVATE tests)
target_link_libraries(Tests PRIVATE Engine)
