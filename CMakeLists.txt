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

if( WIN32 )

	message("${CMAKE_CURRENT_SOURCE_DIR} ..  ${CMAKE_BINARY_DIR}")
	add_custom_command(TARGET editor
		PRE_BUILD
		#COMMAND ${CMAKE_COMMAND} -E copy "${SDL2_RUNTIME_LIBRARY}" ${CMAKE_BINARY_DIR}/RecastDemo/$(ConfigurationName)/
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res/ ${CMAKE_BINARY_DIR}/res
		#COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Bin/TestCases ${CMAKE_BINARY_DIR}/RecastDemo/$(ConfigurationName)/TestCases
		#COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Bin/DroidSans.ttf ${CMAKE_BINARY_DIR}/RecastDemo/$(ConfigurationName)/
	)
endif()