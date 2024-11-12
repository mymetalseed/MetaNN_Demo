include_directories(
	SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/
)
file(GLOB_RECURSE TESTLib ./ *.cpp)

add_library(testLib ${TESTLib})
