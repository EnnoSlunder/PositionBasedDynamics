cmake_minimum_required(VERSION 3.0.2)
project(QtOpenGL)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(include)

#set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};/Users/enno/Qt/5.10.1/clang_64")

find_package(Qt5Widgets CONFIG REQUIRED)
find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Gui CONFIG REQUIRED)
find_package(Qt5OpenGL)
find_package(OpenGL)
find_package(assimp)
find_package(eigen3)
find_package(OpenMP)

set(CMAKE_AUTOMOC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_AUTORCC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/resources/Forms/")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_VERBOSE_MAKEFILE True)

file(GLOB SOURCES "src/*.cpp")
file(GLOB INCLUDES "include/*.h")
file(GLOB DYNAMIC_SOURCES "src/dynamics/*.cpp")
file(GLOB DYNAMIC_INCLUDES "include/dynamics/*.h")

#message("DYNAMIC SRC:          " ${DYNAMIC_SOURCES})
message("DYNAMIC SRC:          " ${STB_IMAGE_FOUND})

set(SOURCES "${SOURCES}" "resources/Forms/ControlWidget.ui")
set(SOURCES "${SOURCES}" "src/ui/DynamicsUiWidget.cpp")

set(SOURCES "${SOURCES}" "${DYNAMIC_SOURCES}")
set(INCLUDES "${INCLUDES}" "${DYNAMIC_INCLUDES}")

message("INCLUDE:          " ${INCLUDES})
message("SOURCES:          " ${SOURCES})

add_executable(QtOpenGL
    ${SOURCES}
    ${INCLUDES}
    resources.qrc
)



#target_link_libraries(QtOpenGL Qt5::Widgets assimp)
target_link_libraries(QtOpenGL Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL OpenGL::GL assimp ${OpenMP_CXX_LIBRARIES} OpenMP::OpenMP_CXX)
