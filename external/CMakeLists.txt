if(MSVC AND NOT "${MSVC_VERSION}" LESS 1400)
	add_definitions( "/MP" )
endif()
add_definitions(
	-DGLEW_STATIC
)

add_subdirectory (glfw-3.1.2)
include_directories(
	glfw-3.1.2/include/GLFW/
	glew-1.13.0/include/
)

set(GLEW_SOURCE
	glew-1.13.0/src/glew.c
)

set(GLEW_HEADERS
)

add_library( GLEW_1130 STATIC
	${GLEW_SOURCE}
	${GLEW_INCLUDE}
)

target_link_libraries(GLEW_1130
	${OPENGL_LIBRARY}
	${EXTRA_LIBS}
)
