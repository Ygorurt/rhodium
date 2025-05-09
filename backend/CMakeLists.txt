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

# Adicionar biblioteca nlohmann/json (SOLUÇÃO PARA O PRIMEIRO ERRO)
# Opção 1: Se você tiver o json.hpp localmente
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Opção 2: Se estiver usando vcpkg
# find_package(nlohmann_json REQUIRED)

# Incluir diretórios (SOLUÇÃO PARA O SEGUNDO ERRO)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/blockchain  # Adicionado para encontrar blockchain.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui         # Adicionado para mainwindow.h
    ${Qt6Core_INCLUDE_DIRS}
    ${Qt6Gui_INCLUDE_DIRS}
    ${Qt6Widgets_INCLUDE_DIRS}
)

# Arquivos fonte - MELHOR PRÁTICA: listar explicitamente em vez de usar GLOB
set(SOURCE_FILES
    src/blockchain/blockchain.cpp
    src/blockchain/blockchain.h
    src/gui/mainwindow.cpp
    src/gui/mainwindow.h
    # Adicione outros arquivos conforme necessário
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
    # nlohmann_json::nlohmann_json  # Descomente se usar vcpkg
)

# Configurar propriedades do destino para incluir diretórios
target_include_directories(rhodium_node PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/blockchain
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui
)
