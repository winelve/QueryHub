cmake_minimum_required(VERSION 3.10)
project(cml)
# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 添加 Qt 组件
find_package(Qt6 REQUIRED COMPONENTS Core)
# 查找所有 .cpp 文件
file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)
# 查找所有 .h 头文件
file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)
# 获取所有头文件的目录
set(CORE_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/core/include  # 添加core/include目录
)
# 生成静态库 cml
add_library(cml STATIC ${SOURCE_FILES})
# 添加头文件路径，并让 Qt Creator 识别头文件
target_sources(cml PUBLIC ${HEADER_FILES})
# 让外部项目能找到并引用头文件
target_include_directories(cml PUBLIC ${CORE_INCLUDE_DIRS})
# 链接 QtCore
target_link_libraries(cml PUBLIC Qt6::Core)
# 如果core是另一个Target，还需要链接core库
target_link_libraries(cml PUBLIC core)
