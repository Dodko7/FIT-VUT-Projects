cmake_minimum_required(VERSION 3.19)
project(Automata_Editor LANGUAGES CXX)
# --- compile flags ----------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# default to Debug if the user didn't choose one
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()


find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(Automata_Editor
    WIN32 MACOSX_BUNDLE
    main.cpp
    automatonmainwindow.cpp
    automatonmainwindow.h
    automatonmainwindow.ui
    automatonEditor.cpp
    automatonEditor.h
    stateitem.h stateitem.cpp
    stateinspectorwidget.h stateinspectorwidget.cpp
    transitionitem.h transitionitem.cpp
    statecreationdialog.h statecreationdialog.cpp
    fsmcreationdialog.h fsmcreationdialog.cpp
    startcreationdialog.h startcreationdialog.cpp
    fsminspectorwidget.h fsminspectorwidget.cpp
    transitioncreationdialog.h transitioncreationdialog.cpp
    transitioninspectorwidget.h transitioninspectorwidget.cpp
)

target_include_directories(Automata_Editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# --- sanitizers for Debug ---------------------------------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(Automata_Editor PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options   (Automata_Editor PRIVATE -fsanitize=address)
endif()

target_link_libraries(Automata_Editor
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS Automata_Editor
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Automata_Editor
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
