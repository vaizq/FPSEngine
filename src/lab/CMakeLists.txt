cmake_minimum_required(VERSION 3.27)

add_executable(lab
    main.cpp
    Player.cpp
    FPSGame.cpp
    Enemy.cpp
    EnemyManager.cpp
)

target_link_libraries(lab PRIVATE Engine)
