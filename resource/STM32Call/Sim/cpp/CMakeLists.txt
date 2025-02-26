cmake_minimum_required(VERSION 3.0)

# 设置工程属性
project(oled_sender)
# add_definitions(-DUNICODE -D_UNICODE) # 定义工程的字符编码为Unicode编码
# set(CMAKE_CXX_FLAGS_RELEASE "/MT") # Release模式使用静态链接其他动态库，调试模式下勿开
# set(CMAKE_CXX_FLAGS_DEBUG "/MTd") # Debug模式使用静态链接其他动态库，调试模式下勿开

# 引用Boost库
# find_package(Boost 1.7.0 REQUIRED COMPONENTS system)
# set(Boost_USE_STATIC_LIBS ON) # 使用boost静态库
# add_definitions(-DBOOST_ALL_DYN_LINK) # 使用boost动态库

# 编译参数
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    # add_definitions(-std=c++14) # c++14标准
    add_compile_options(-g) # gdb调试模式，默认则是三级二进制优化模式-O3
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    # add_definitions(/std:c++14)
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

# 获取源文件
aux_source_directory(. SourceFiles)
# message(STATUS "Boost_INCLUDE_DIRS:${Boost_INCLUDE_DIRS}")
# message(STATUS "Boost_LIBRARY_DIRS:${Boost_LIBRARY_DIRS}")
# message(STATUS "Boost_LIBRARIES:${Boost_LIBRARIES}")


# 库文件查找路径
# link_directories(${Boost_LIBRARY_DIRS})

add_subdirectory(main)
# add_subdirectory(curl)