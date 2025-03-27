cmake_minimum_required(VERSION 3.10)

project(core)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# **添加 Qt 组件**
find_package(Qt6 REQUIRED COMPONENTS Core)

# 查找所有 .cpp 文件
file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../filesystem/src/*.cpp
    )

# 查找所有 .h 头文件
file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../filesystem/include/*.h
    )

# 获取所有头文件的目录
set(CORE_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../filesystem/include
)

# 生成静态库 core
add_library(core STATIC ${SOURCE_FILES})

#添加core的main.cpp为可执行文件
# add_executable(core_main main.cpp ${SOURCE_CORE})

# 添加头文件路径，并让 Qt Creator 识别头文件
target_sources(core PUBLIC ${HEADER_FILES})

# 让外部项目能找到并引用头文件
target_include_directories(core PUBLIC ${CORE_INCLUDE_DIRS})


# **链接 QtCore**
target_link_libraries(core PUBLIC Qt6::Core)
