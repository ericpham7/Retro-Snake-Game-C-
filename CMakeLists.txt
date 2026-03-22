cmake_minimum_required(VERSION 4.0.2)
project(untitled11)

set(CMAKE_CXX_STANDARD 20)

add_executable(untitled11
        snakeGamee.cpp
        snake.cpp
        Food.cpp
)

find_package(raylib REQUIRED)

target_link_libraries(untitled11 PRIVATE raylib)

set(ASSET_FILES
        greenapple.png
        backGroundSong.mp3
        munch-sound-effect.mp3
        erro.mp3
)

foreach(asset IN LISTS ASSET_FILES)
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${asset}")
        add_custom_command(TARGET untitled11 POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/${asset}"
                "$<TARGET_FILE_DIR:untitled11>/${asset}"
        )
    endif()
endforeach()
