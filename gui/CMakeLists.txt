qt_standard_project_setup()  # Qt6 默认配置

add_executable(queryhub_gui
    src/main.cpp
    src/mainwindow.cpp
    include/mainwindow.h
    ui/mainwindow.ui
)

target_link_libraries(queryhub_gui PRIVATE core Qt6::Widgets)
