cmake_minimum_required(VERSION 3.5)

project(SDL_TD VERSION 0.1 LANGUAGES CXX)

# Find SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Set SDL include directories and libraries
set(SDL_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS})
set(SDL_LIBRARIES ${SDL2_LIBRARIES})

# Find SDL2_mixer, SDL2_image, and SDL2_ttf
find_library(SDL_MIXER_LIBRARY NAMES SDL2_mixer)
find_library(SDL_IMAGE_LIBRARY NAMES SDL2_image)
find_library(SDL_TTF_LIBRARY NAMES SDL2_ttf)

include_directories(${SDL_INCLUDE_DIRS})

# Gather source and header files
file(GLOB_RECURSE PROJECT_SOURCES src/*.cpp)
file(GLOB_RECURSE PROJECT_HEADERS src/*.hpp)

# Create executable
add_executable(SDL_TD
	${PROJECT_SOURCES}
	${PROJECT_HEADERS}
)

# Link libraries
target_link_libraries(SDL_TD
	${SDL_LIBRARIES}
	${SDL_MIXER_LIBRARY}
	${SDL_IMAGE_LIBRARY}
	${SDL_TTF_LIBRARY}
	fmt
)

file(GLOB ASSETS "assets/*")
foreach(ASSET ${ASSETS})
	file(COPY ${ASSET} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)
endforeach()

if(WIN32)
    target_compile_definitions(SDL_TD PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )

    target_link_libraries(SDL_TD ws2_32)
endif()
