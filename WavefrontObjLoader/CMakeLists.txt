cmake_minimum_required(VERSION 2.8.12)

project(WavefrontObjLoader LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

IF (WIN32)
    # This you might change in order to find QT package successfully
    set(CMAKE_PREFIX_PATH D:\\Programme\\Qt\\Qt5.10.1\\5.10.1\\msvc2017_64\\lib\\cmake)
ENDIF()

find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL REQUIRED)

set(HEADERS UI/WavefrontObjLoader.h UI/GLWidget.h ObjectLoader/ObjLoader.h
    ObjectLoader/SubMesh.h ObjectLoader/MaterialLoader.h
    ObjectLoader/Material.h ObjectLoader/Mesh.h Rendering/WavefrontRenderer.h)

set(SOURCES UI/GLWidget.cpp UI/WavefrontObjLoader.cpp ObjectLoader/Mesh.cpp
    ObjectLoader/ObjLoader.cpp ObjectLoader/MaterialLoader.cpp
    ObjectLoader/SubMesh.cpp ObjectLoader/Material.cpp
    Rendering/WavefrontRenderer.cpp main.cpp)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} "shaders.qrc")

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL)

get_target_property(QtCore_location_Release Qt5::Core IMPORTED_LOCATION_RELEASE)
get_target_property(QtQui_location_Release Qt5::Gui IMPORTED_LOCATION_RELEASE)
get_target_property(QtWidgets_location_Release Qt5::Widgets IMPORTED_LOCATION_RELEASE)
get_target_property(QtOpenGL_location_Release Qt5::OpenGL IMPORTED_LOCATION_RELEASE)
get_target_property(QtCore_location_Debug Qt5::Core IMPORTED_LOCATION_DEBUG)
get_target_property(QtQui_location_Debug Qt5::Gui IMPORTED_LOCATION_DEBUG)
get_target_property(QtWidgets_location_Debug Qt5::Widgets IMPORTED_LOCATION_DEBUG)
get_target_property(QtOpenGL_location_Debug Qt5::OpenGL IMPORTED_LOCATION_DEBUG)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy ${QtCore_location_Release} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	COMMAND ${CMAKE_COMMAND} -E copy ${QtQui_location_Release} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	COMMAND ${CMAKE_COMMAND} -E copy ${QtWidgets_location_Release} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	COMMAND ${CMAKE_COMMAND} -E copy ${QtOpenGL_location_Release} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}

    COMMAND ${CMAKE_COMMAND} -E copy ${QtCore_location_Debug} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	COMMAND ${CMAKE_COMMAND} -E copy ${QtQui_location_Debug} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	COMMAND ${CMAKE_COMMAND} -E copy ${QtWidgets_location_Debug} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	COMMAND ${CMAKE_COMMAND} -E copy ${QtOpenGL_location_Debug} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)