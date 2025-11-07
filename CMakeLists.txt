cmake_minimum_required(VERSION 3.15...3.30)

project(Miska)

set(BUILD_TYPES Debug Release)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_EXPORT_COMPILE_COMMANDS YES)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(NOT (${CMAKE_BUILD_TYPE} IN_LIST BUILD_TYPES))
    message(FATAL_ERROR "Unknown build type " ${CMAKE_BUILD_TYPE})
endif()

add_executable(Miska
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

target_include_directories(Miska
    PRIVATE ${PROJECT_SOURCE_DIR}/include
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(USE_FLAGS "\
        -s GL_ENABLE_GET_PROC_ADDRESS=1\
        -s USE_SDL=2\
        -s USE_SDL_IMAGE=2\
        -s SDL2_IMAGE_FORMATS=png\
        -s USE_LIBPNG=1\
        -s USE_SDL_TTF=2\
        -s USE_SDL_MIXER=2\
        -fsanitize=undefined \
        "
    )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS}")

    set_target_properties(Miska PROPERTIES SUFFIX ".html")

    set(shellHtml ${CMAKE_SOURCE_DIR}/web/shell.html)

    set(CMAKE_EXE_LINKER_FLAGS "\
        ${CMAKE_EXE_LINKER_FLAGS} \
        ${USE_FLAGS} \
        -s ASYNCIFY \
        -s TOTAL_STACK=64MB \
        -s INITIAL_MEMORY=128MB \
        -s NO_DISABLE_EXCEPTION_CATCHING \
        --use-preload-plugins \
        --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/data@/data \
        --shell-file ${shellHtml}"
    )
    set_target_properties(Miska
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/website"
    )
    set_property(TARGET Miska
        APPEND PROPERTY LINK_DEPENDS ${shellHtml}
    )
    add_custom_target(copy_styles ALL
        DEPENDS ${CMAKE_SOURCE_DIR}/web/styles.css
    )
    add_custom_command(TARGET copy_styles
        PRE_BUILD
        COMMAND cmake
        ARGS -E copy_if_different
            ${CMAKE_SOURCE_DIR}/web/styles.css
            $<TARGET_FILE_DIR:Miska>/
        )
else()
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    set_target_properties(Miska
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/game"
    )
endif()

target_compile_options(Miska
    PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra>
)

target_compile_options(Miska
    PRIVATE $<$<AND:$<NOT:$<CXX_COMPILER_ID:MSVC>>,$<PLATFORM_ID:WIN32>,$<CONFIG:Release>>:-Wl,-subsystem,windows>
)

target_link_libraries(Miska
    PRIVATE $<$<PLATFORM_ID:MSYS>: mingw32>
)

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    target_link_libraries(Miska
        PRIVATE SDL2::SDL2
        PRIVATE SDL2_image::SDL2_image
        PRIVATE SDL2_mixer::SDL2_mixer
        PRIVATE SDL2_ttf::SDL2_ttf
)
endif()

add_custom_target(copyGameData ALL)
add_custom_command(TARGET copyGameData POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E echo "Copying gamedata"
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_directory_if_different
            ${CMAKE_SOURCE_DIR}/data
            $<TARGET_FILE_DIR:Miska>/data
)

include(GNUInstallDirs)

install(TARGETS Miska
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(DIRECTORY "${PROJECT_BINARY_DIR}/data" DESTINATION ${CMAKE_INSTALL_BINDIR})

