cmake_minimum_required(VERSION 3.10)
project(QueryHub)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6 组件
# find_package(Qt6 REQUIRED COMPONENTS Widgets)



# 生成可执行文件
add_executable(queryhub_main ./main.cpp)
add_executable(core_main core/main.cpp)
add_executable(cml_main cml/main.cpp)

# 添加子模块
add_subdirectory(core)  # 加载 core 模块
add_subdirectory(cml)   # 加载命令行版本
#add_subdirectory(gui)   # 加载 GUI 版本


target_link_libraries(queryhub_main PUBLIC core)
target_link_libraries(core_main PUBLIC core)
target_link_libraries(cml_main PUBLIC cml)



# **默认启动 CORE 版本**
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT core_main)
