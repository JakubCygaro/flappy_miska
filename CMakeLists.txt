cmake_minimum_required(VERSION 3.15...3.30)

project(Miska)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "")
endif()
# if(CMAKE_CXX_COMPILER_ID NOT STREQUAL "GCC")
    
# endif()

set (CMAKE_CXX_STANDARD 20)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

# configure_file(
#     ./data/*
#     /data
# )

#find_library(SDL2 SDL2)
add_executable(Miska WIN32 
    src/main.cpp 
    src/Button.cpp 
    src/Font.cpp 
    src/Game.cpp 
    src/Object.cpp 
    src/Teeth.cpp 
    src/TextBox.cpp 
    src/TextBox.cpp 
    src/Texture.cpp 
    src/Vector2.cpp
)
#target_link_libraries(Dupa SDL2)

if(WIN32)
    target_compile_options(Miska PRIVATE "-Wl,-subsystem,windows")
    if(MINGW)
        target_link_libraries(
            ${PROJECT_NAME}
            mingw32
        )
    endif()
endif()

target_link_libraries(${PROJECT_NAME}
    SDL2main
    SDL2
    SDL2_image
    SDL2_mixer
    SDL2_ttf
)

add_custom_target(copyGameData ALL)
add_custom_command(TARGET copyGameData POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E echo "Copying gamedata"
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_directory_if_different
            ${CMAKE_SOURCE_DIR}/data
            ${CMAKE_BINARY_DIR}/data
)

install(TARGETS Miska DESTINATION bin)
install(DIRECTORY "${PROJECT_BINARY_DIR}/data" DESTINATION bin)
