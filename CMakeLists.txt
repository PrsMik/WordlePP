cmake_minimum_required(VERSION 3.10.0)
project(Wordle++ VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

# Use vendored libs
set(SDLTTF_VENDORED ON)
set(SDLIMAGE_VENDORED OFF)
set(SDLMIXER_VENDORED OFF)

# This assumes the SDL source is available in vendored/SDL
add_subdirectory(lib/SDL EXCLUDE_FROM_ALL)

# This assumes the SDL_ttf source is available in vendored/SDL_ttf
add_subdirectory(lib/SDL_ttf EXCLUDE_FROM_ALL)

# This assumes the SDL_ttf source is available in vendored/SDL_ttf
add_subdirectory(lib/SDL_image EXCLUDE_FROM_ALL)

# This assumes the SDL_ttf source is available in vendored/SDL_ttf
add_subdirectory(lib/SDL_mixer EXCLUDE_FROM_ALL)

set(SOURCES main.cpp
    src/Controller/InputHandler.cpp
    src/Controller/SDLEngine.cpp
    src/Model/Dictionary/Dictionary.cpp
    src/Model/WordleModel.cpp
    src/View/AssetManager.cpp
    src/View/GameView.cpp
    src/View/CharBox.cpp
    src/View/WordRow.cpp
    src/View/KeyboardDisplay.cpp
    src/View/GameLayout.cpp
    )

set(DATA_DIR "${PROJECT_SOURCE_DIR}/data")

# Generate a header file (config.h) that will define this path as a C++ macro
configure_file(
    "cmake/config.h.in"  # Create this template file
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
    @ONLY
)

add_executable(${PROJECT_NAME} ${SOURCES})

if(WIN32)
    # Определяем макросы архитектуры, которые ожидают некоторые библиотеки
    if(CMAKE_SIZEOF_VOID_P EQUAL 8) # 64-битная архитектура
        target_compile_definitions(${PROJECT_NAME} PRIVATE "_AMD64_") 
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4) # 32-битная архитектура
        target_compile_definitions(${PROJECT_NAME} PRIVATE "_X86_")
    endif()
    
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    endif()
endif()

# Add the directory containing the generated header to your include paths
target_include_directories(Wordle++ PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

# Pass the actual path to the compiler as a preprocessor definition
target_compile_definitions(Wordle++
    PRIVATE 
    # DATA_DIR содержит путь к папке data, включая кавычки
    DATA_DIR="${DATA_DIR}" 
)

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf SDL3_image::SDL3_image)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    # Используем команду execute_process, которая будет выполняться на этапе конфигурации
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                "${CMAKE_BINARY_DIR}/compile_commands.json" 
                "${CMAKE_SOURCE_DIR}/compile_commands.json"
        # Убедитесь, что эта команда не блокирует конфигурацию
        RESULT_VARIABLE COPY_STATUS
        # ERROR_VARIABLE COPY_ERROR # Можно использовать для отладки
    )
    if(NOT COPY_STATUS EQUAL 0)
        message(STATUS "Не удалось скопировать compile_commands.json в ${CMAKE_SOURCE_DIR}")
    endif()
endif()
