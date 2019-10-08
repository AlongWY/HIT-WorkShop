cmake_minimum_required(VERSION 3.9)
project(TextToScene)
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(jieba)

INCLUDE_DIRECTORIES(jieba/include)
INCLUDE_DIRECTORIES(jieba/deps)

add_executable(TextToScene main.cpp
        DisplayCanvas.cpp
        DisplayCanvas.h
        MainFrame.cpp
        MainFrame.h
        ObjectSimilarity.cpp
        ObjectSimilarity.h
        StringToID.h
        WordSimilarity.cpp
        WordSimilarity.h
        WorldMgr.cpp
        WorldMgr.h
        wxSFMLCanvas.cpp
        wxSFMLCanvas.h)

if (UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK2 REQUIRED gtk+-2.0)
    include_directories(${GTK2_INCLUDE_DIRS})
    link_directories(${GTK2_LIBRARY_DIRS})
    add_definitions(${GTK2_CFLAGS_OTHER})
    target_link_libraries(TextToScene ${GTK2_LIBRARIES})
endif ()

find_package(wxWidgets COMPONENTS net gl core base)
if (wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
    target_link_libraries(TextToScene ${wxWidgets_LIBRARIES})
endif ()

find_package(SFML REQUIRED system window graphics network audio)
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(TextToScene ${SFML_LIBRARIES})
endif ()