cmake_minimum_required(VERSION 3.5)

project(SDL_TD VERSION 0.1 LANGUAGES CXX)

find_package(SDL2 QUIET)

set(SDL_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS})
set(SDL_LIBRARIES ${SDL2_LIBRARIES})

find_library(SDL_MIXER_LIBRARY NAMES SDL2_mixer)
find_library(SDL_IMAGE_LIBRARY NAMES SDL2_image)
find_library(SDL_TTF_LIBRARY NAMES SDL2_ttf)

include_directories(${SDL_INCLUDE_DIRS})

file(GLOB_RECURSE PROJECT_SOURCES src/*.cpp)
file(GLOB_RECURSE PROJECT_HEADERS src/*.hpp)

add_executable(SDL_TD
	${PROJECT_SOURCES}
	${PROJECT_HEADERS}
)

target_link_libraries(SDL_TD
	${SDL_LIBRARIES}
	${SDL_MIXER_LIBRARY}
	${SDL_IMAGE_LIBRARY}
	${SDL_TTF_LIBRARY}
	fmt
)

file(GLOB ASSETS
	"assets/*"
)

foreach(ASSET ${ASSETS})
	file(COPY ${ASSET} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)
endforeach()
