CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(Romeo LANGUAGES CXX)

SET(NO_CONSOLE ON)                              # 命令行窗口

# 配置Qt
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt
if (MSVC)
    set(QT_PATH "C:\\Qt\\5.12.0\\msvc2017_64")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT_PATH})
endif ()

FIND_PACKAGE(Qt5 COMPONENTS Core Widgets REQUIRED)
FIND_PACKAGE(Qt5LinguistTools)

SET(CODE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)      # 代码目录
SET(RES_PATH ${CMAKE_CURRENT_SOURCE_DIR}/res)       # 资源目录
SET(UI_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ui)         # UI界面
SET(TEST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/test)     # 测试代码
SET(LANG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lang)     # 翻译

SET(CMAKE_AUTOUIC_SEARCH_PATHS ${UI_PATH})

ADD_SUBDIRECTORY(${CODE_PATH})