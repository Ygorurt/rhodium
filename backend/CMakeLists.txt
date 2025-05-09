cmake_minimum_required(VERSION 3.15)
project(RhodiumBlockchain)

# Configurações básicas
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configurações para Windows
if(WIN32)
    set(OPENSSL_ROOT_DIR "D:/OpenSSL-Win64")
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif()

# Configuração do Qt
set(QT_PATH "D:/Qt/6.9.0/msvc2022_64")
list(APPEND CMAKE_PREFIX_PATH ${QT_PATH})

if(NOT EXISTS "${QT_PATH}/lib/cmake/Qt6")
    message(FATAL_ERROR "Qt6 não encontrado em ${QT_PATH}")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Encontrar dependências
find_package(OpenSSL REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Incluir diretórios
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Qt6Core_INCLUDE_DIRS}
    ${Qt6Gui_INCLUDE_DIRS}
    ${Qt6Widgets_INCLUDE_DIRS}
)

# Arquivos fonte
file(GLOB_RECURSE SOURCE_FILES
    "src/*.cpp"
    "src/*.h"
)

# Executável principal
add_executable(rhodium_node ${SOURCE_FILES})

# Linkar bibliotecas
target_link_libraries(rhodium_node PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
