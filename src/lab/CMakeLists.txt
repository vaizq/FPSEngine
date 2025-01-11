cmake_minimum_required(VERSION 3.27)

add_executable(lab
    main.cpp
    FPSGame.cpp
)

target_link_libraries(lab PRIVATE Engine)
