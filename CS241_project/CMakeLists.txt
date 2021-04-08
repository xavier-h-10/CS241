cmake_minimum_required(VERSION 3.15)
project(CS241_project)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5 COMPONENTS Core QuickWidgets Charts WebEngineCore REQUIRED)

set(QRC_FILE src/qml.qrc)

qt5_add_resources(QRC ${QRC_FILE})

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


add_executable(CS241_project 
			   src/main.cpp 
			   src/mainwindow.cpp 
			   src/mainwindow.h 
			   src/mainwindow.ui 
			   src/Worker.h 
			   src/Worker.cpp ${QRC})

target_link_libraries(CS241_project Qt5::Widgets Qt5::Charts Qt5::Core Qt5::Quick Qt5::WebEngineCore Qt5::QuickWidgets Qt5::Qml)