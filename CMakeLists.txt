cmake_minimum_required(VERSION 3.16)

project(qwidgetinqml VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Core Widgets Multimedia MultimediaWidgets)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appqwidgetinqml
    main.cpp
)

qt_add_qml_module(appqwidgetinqml
    URI qwidgetinqml
    VERSION 1.0
    QML_FILES
        Main.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appqwidgetinqml PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appqwidgetinqml
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appqwidgetinqml
    PRIVATE Qt6::Quick
    Qt6::Widgets
    Qt6::Core
    Qt6::Multimedia
    Qt6::MultimediaWidgets
)

include(GNUInstallDirs)
install(TARGETS appqwidgetinqml
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
