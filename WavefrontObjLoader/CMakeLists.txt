cmake_minimum_required(VERSION 2.8.12)

project(WavefrontObjLoader LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

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