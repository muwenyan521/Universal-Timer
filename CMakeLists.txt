cmake_minimum_required(VERSION 3.16)
project(UniversalTimer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Multimedia)

set(PROJECT_SOURCES
    src/main.cpp
    src/UniversalTimer.h
    src/UniversalTimer.cpp
    icon.rc
    resources.qrc
)

qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Qt::Multimedia
)

set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE TRUE)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/sounds
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/sounds
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/lang
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/lang
    COMMENT "¸´ÖÆÎÄ¼þ"
)