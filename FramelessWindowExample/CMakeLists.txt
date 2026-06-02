cmake_minimum_required(VERSION 3.16)
project(FramelessWindowExample VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.10)

include(FetchContent)
FetchContent_Declare(
    QtFramelessWindow
    GIT_REPOSITORY https://github.com/floraaubry/QtFramelessWindow.git
    GIT_TAG main
)
FetchContent_MakeAvailable(QtFramelessWindow)

qt_add_executable(appFramelessWindowExample
    main.cpp
)

qt_add_qml_module(appFramelessWindowExample
    URI FramelessWindowExample
    VERSION 1.0
    QML_FILES
        Main.qml
    IMPORTS
        Odizinne.FramelessWindow
)

set_target_properties(appFramelessWindowExample PROPERTIES
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appFramelessWindowExample
    PRIVATE
        Qt6::Quick
        OdizinneFramelessWindowplugin
)

install(TARGETS appFramelessWindowExample
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)