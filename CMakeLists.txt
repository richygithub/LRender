cmake_minimum_required (VERSION 3.0)

set (CMAKE_CXX_STANDARD 17)

project (LRender)
#set_property(TARGET LRender PROPERTY CXX_STANDARD 17)
find_package(OpenGL REQUIRED)


add_subdirectory (external)

include_directories(
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
	imgui/
	render/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_1130
)
add_definitions(
	-DGLEW_STATIC
	#-D_CRT_SECURE_NO_WARNINGS
)

file(GLOB_RECURSE SOURCES render/*.cpp imgui/*.cpp common/*.cpp)

if( WIN32 )
  source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
endif()

add_executable(editor ${SOURCES})
target_link_libraries(editor
	${ALL_LIBS}
)