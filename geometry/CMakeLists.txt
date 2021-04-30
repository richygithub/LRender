file(GLOB_RECURSE SOURCES *.cpp )

include_directories(
	../external/glm-0.9.7.1/
	.
)


add_library(geometry ${SOURCES}  )

if( WIN32 )
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
endif()