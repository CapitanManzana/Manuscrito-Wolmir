cmake_minimum_required(VERSION 3.16)
project(Manuscrito)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Archivos fuente
file(GLOB SRC_FILES src/*.cpp)

# Crear ejecutable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Incluir cabeceras (tu código + SDL)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/libs/SDL3/include
)

# Directorio donde están los .lib y .dll
target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/SDL3/lib
)

# Vincular librerías
target_link_libraries(${PROJECT_NAME}
    SDL3
    SDL3_image
    SDL3_ttf
    SDL3_mixer
)

# Copiar automáticamente las DLL al lado del ejecutable
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/libs/SDL3/lib/SDL3.dll"
        "${CMAKE_SOURCE_DIR}/libs/SDL3/lib/SDL3_image.dll"
        "${CMAKE_SOURCE_DIR}/libs/SDL3/lib/SDL3_ttf.dll"
        "${CMAKE_SOURCE_DIR}/libs/SDL3/lib/SDL3_mixer.dll"
        $<TARGET
)