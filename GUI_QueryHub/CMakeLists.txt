cmake_minimum_required(VERSION 3.16)

project(GUI_QueryHub VERSION 0.1 LANGUAGES CXX)


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp



)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(GUI_QueryHub
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        include/ElaWidgetTools.qrc

        mainwindow.h mainwindow.cpp
        res.qrc
        Pages/t_basepage.h Pages/t_basepage.cpp
        Pages/t_about.h Pages/t_about.cpp
        Pages/t_setting.h Pages/t_setting.cpp
        Pages/t_adddatabase.h Pages/t_adddatabase.cpp
        Pages/t_addtable.h Pages/t_addtable.cpp
        Pages/t_deletedatabase.h Pages/t_deletedatabase.cpp
        Pages/t_deletetable.h Pages/t_deletetable.cpp
        Pages/t_addfields.h Pages/t_addfields.cpp
        Pages/t_deletefield.h Pages/t_deletefield.cpp
        IDBMSInterface.h
        Pages/t_connectpage.h Pages/t_connectpage.cpp
        Pages/t_tableview.h Pages/t_tableview.cpp
        modle/t_tableviewmodel.h modle/t_tableviewmodel.cpp
        Pages/t_select.h Pages/t_select.cpp



    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET GUI_QueryHub APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(GUI_QueryHub SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(GUI_QueryHub
            ${PROJECT_SOURCES}
        )
    endif()
endif()

include_directories(
    include
)

target_link_libraries(GUI_QueryHub PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/libElaWidgetTools.a
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.GUI_QueryHub)
endif()
set_target_properties(GUI_QueryHub PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS GUI_QueryHub
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(GUI_QueryHub)
endif()
