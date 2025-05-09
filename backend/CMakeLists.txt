cmake_minimum_required(VERSION 3.15)
project(RhodiumBlockchain)

# Configurações básicas
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Configurações para Windows
if(WIN32)
    set(OPENSSL_ROOT_DIR "D:/OpenSSL-Win64")
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif()

# Configuração do Qt
set(QT_PATH "D:/Qt/6.9.0/msvc2022_64")
list(APPEND CMAKE_PREFIX_PATH ${QT_PATH})

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets UiTools)
find_package(OpenSSL REQUIRED)

# Substitua a seção de UI por:
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/mainwindow.ui")
    qt6_wrap_ui(UI_HEADERS src/gui/mainwindow.ui)
    message(STATUS "Arquivo UI encontrado e será processado")
else()
    message(WARNING "Arquivo mainwindow.ui não encontrado - criando um vazio")
    file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/mainwindow.ui "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<ui version=\"4.0\">\n <class>MainWindow</class>\n <widget class=\"QMainWindow\" name=\"MainWindow\">\n  <property name=\"geometry\">\n   <rect>\n    <x>0</x>\n    <y>0</y>\n    <width>400</width>\n    <height>300</height>\n   </rect>\n  </property>\n  <widget class=\"QWidget\" name=\"centralwidget\"/>\n </widget>\n</ui>")
    qt6_wrap_ui(UI_HEADERS src/gui/mainwindow.ui)
endif()

# Arquivos fonte
set(SOURCE_FILES
    src/blockchain/blockchain.cpp
    src/blockchain/blockchain.h
    src/gui/mainwindow.cpp
    src/gui/mainwindow.h
    ${UI_HEADERS}
)

# Verifica se os arquivos existem
foreach(file ${SOURCE_FILES})
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${file})
        message(WARNING "Arquivo não encontrado: ${file}")
    endif()
endforeach()

# Executável principal
add_executable(rhodium_node ${SOURCE_FILES})

# Configuração de includes
target_include_directories(rhodium_node PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}  # Para arquivos gerados como ui_*.h
    ${Qt6Core_INCLUDE_DIRS}
    ${Qt6Gui_INCLUDE_DIRS}
    ${Qt6Widgets_INCLUDE_DIRS}
)

# Linkar bibliotecas
target_link_libraries(rhodium_node PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
