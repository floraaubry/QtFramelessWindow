cmake_minimum_required(VERSION 3.24)
project(OdizinneFramelessWindow LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_library(OdizinneFramelessWindow STATIC
    framelesshelper.h
    framelesseventfilter.h
)

set_target_properties(OdizinneFramelessWindow PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Odizinne/FramelessWindow"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Odizinne/FramelessWindow"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Odizinne/FramelessWindow"
)

qt_add_qml_module(OdizinneFramelessWindow
    URI Odizinne.FramelessWindow
    VERSION 1.0
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Odizinne/FramelessWindow"
    QML_FILES
        FramelessWindow.qml
    SOURCES
        framelesshelper.h
        framelesseventfilter.h
)

target_link_libraries(OdizinneFramelessWindow PUBLIC
    Qt6::Quick
    dwmapi
)

target_include_directories(OdizinneFramelessWindow PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)