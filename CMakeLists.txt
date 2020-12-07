cmake_minimum_required(VERSION 3.10)
set(CMAKE_MIN_REQ_VERSION "3.10")
# set the project name
project("glTF_2.0_Importer")


option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(Dependencies/vendor/glfw)

add_subdirectory(Dependencies/vendor/glad)


if(MSVC)

	set (CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /MDd")


    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)

else()
	if(UNIX)
		message("Linux")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++17")

		set(GLAD_LIBRARIES g dl GL GLU glfw X11 Xxf86vm Xrandr pthread Xi Xinerama Xcursor )

	endif()
endif()



# add include
include_directories(Dependencies/include/
					Dependencies/vendor/glfw/include
					Dependencies/vendor/glm/glm
					Dependencies/vendor/glad/include/
					Dependencies/vendor/imgui
					Dependencies/vendor/imgui/backends
					Dependencies/vendor/json
					)

# add source files
file(GLOB VENDORS_SOURCES Dependencies/vendor/glad/src/glad.c)
file(GLOB PROJECT_SOURCES Dependencies/source/*.cpp Dependencies/source/*.c)
file(GLOB PROJECT_HEADERS Dependencies/include/*.h)
file(GLOB PROJECT_SHADERS
		Dependencies/shaders/*.glsl)
file(GLOB IMGUI
		Dependencies/vendor/imgui/*.cpp
		Dependencies/vendor/imgui/*.h
		Dependencies/vendor/imgui/backends/imgui_impl_opengl3.cpp
		Dependencies/vendor/imgui/backends/imgui_impl_glfw.cpp)


source_group("shaders" FILES ${PROJECT_SHADERS})

source_group("imgui"   FILES ${IMGUI})

add_definitions(-DGLFW_INCLUDE_NONE -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")

set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

# add to the executable
add_executable(${PROJECT_NAME} 
							${VENDORS_SOURCES} 
							${PROJECT_SOURCES} 
							${PROJECT_HEADERS}
							${PROJECT_SHADERS}
							${PROJECT_GEOMETRY_SHADERS}
							${IMGUI})


target_link_libraries(${PROJECT_NAME}
								${GLFW_LIBRARIES} 
								${GLAD_LIBRARIES} 
								glfw
								)


set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})