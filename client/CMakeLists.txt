cmake_minimum_required(VERSION 3.10)
project(client)  # 项目名改为client

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用 Qt 的自动 MOC、UIC 和 RCC
set(CMAKE_AUTOMOC ON)  # 这行非常重要！
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 添加 Qt 组件
find_package(Qt6 REQUIRED COMPONENTS Core Network Widgets)

# 查找所有 .cpp 文件
file(GLOB_RECURSE SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# 查找所有 .h 头文件
file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

# 获取所有头文件的目录
set(CLIENT_INCLUDE_DIRS  # 变量名从SERVER_INCLUDE_DIRS改为CLIENT_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/core/include     # 添加core/include目录
    ${CMAKE_SOURCE_DIR}/cml              # 添加cml目录
)

# 生成静态库 client
add_library(client STATIC ${SOURCE_FILES})  # 库名从server改为client

# 添加头文件路径，并让 Qt Creator 识别头文件
target_sources(client PUBLIC ${HEADER_FILES})  # 目标名从server改为client

# 让外部项目能找到并引用头文件
target_include_directories(client PUBLIC ${CLIENT_INCLUDE_DIRS})  # 目标名和变量名都从server改为client

# 链接 Qt组件
target_link_libraries(client PUBLIC Qt6::Core Qt6::Network Qt6::Widgets)  # 目标名从server改为client

# 链接 core 和 cml 库
target_link_libraries(client PUBLIC core cml)  # 目标名从server改为client
