


set(editor_lib "Navmesh_Export")
include_directories(
    ../external/glm-0.9.7.1/
	../geometry/
	../navmesh
	.
)
add_library(${editor_lib} SHARED "export_unity.cpp")
set_target_properties(${editor_lib} PROPERTIES OUTPUT_NAME "JJNavMesh2D")
target_link_libraries(${editor_lib}
    navmesh
)


