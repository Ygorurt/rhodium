cmake_minimum_required(VERSION 3.15)
project(RhodiumBlockchain VERSION 1.0 LANGUAGES CXX)

# Configuração para Windows
if(WIN32)
    # Usa PowerShell tradicional e desativa mensagens desnecessárias
    set(CMAKE_EXECUTE_PROCESS_COMMAND_ECHO STDOUT)
    set(CMAKE_SUPPRESS_REGENERATION_MESSAGE TRUE)
endif()

# Configurações do Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Padrão C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(JSON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(EXISTS "${JSON_INCLUDE_DIR}/json.hpp")
    message(STATUS "Usando nlohmann/json local em: ${JSON_INCLUDE_DIR}")
    
    # Cria um alvo interface para a biblioteca
    add_library(nlohmann_json INTERFACE)
    target_include_directories(nlohmann_json INTERFACE ${JSON_INCLUDE_DIR})
    
    # Cria o alias no namespace
    add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)
else()
    message(FATAL_ERROR "Arquivo json.hpp não encontrado em ${JSON_INCLUDE_DIR}/nlohmann")
endif()

# Configuração do Qt
set(QT_PATH "D:/Qt/6.9.0/msvc2022_64")
list(APPEND CMAKE_PREFIX_PATH "${QT_PATH}")
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets UiTools)

# Opções de configuração
option(BUILD_WEB_INTERFACE "Habilita interface web" ON)

# Configuração da interface web (opcional)
if(BUILD_WEB_INTERFACE)
    # Busca Node.js em locais comuns do Windows
    find_program(NODE_EXE
        NAMES node node.exe
        PATHS
            "$ENV{ProgramFiles}/nodejs"
            "$ENV{SystemDrive}/nodejs"
            "$ENV{LOCALAPPDATA}/Microsoft/WindowsApps"
        DOC "Caminho para o Node.js"
    )
    
    if(NODE_EXE)
        message(STATUS "Node.js encontrado: ${NODE_EXE}")
        # Adicione aqui os comandos para build do frontend
    else()
        message(WARNING "Node.js não encontrado - Interface web desabilitada")
    endif()
endif()

# Configuração de diretórios
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

# Geração de arquivos UI e meta-objetos Qt
qt6_wrap_ui(UI_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/mainwindow.ui"
)

qt6_wrap_cpp(MOC_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/mainwindow.h"
)

# Lista de arquivos fonte
set(SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/blockchain/blockchain.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/blockchain/block.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/blockchain/transaction.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/mainwindow.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/mainwindow.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/mainwindow.ui"
    ${UI_HEADERS}
    ${MOC_SOURCES}
)

# Executável principal
add_executable(rhodium_node WIN32 ${SOURCE_FILES})

# Configuração de includes
target_include_directories(rhodium_node PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

# Linkagem de bibliotecas
target_link_libraries(rhodium_node PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    nlohmann_json::nlohmann_json  # Agora deve funcionar
)

# Mensagem final
message(STATUS "\nConfiguração do RhodiumBlockchain concluída com sucesso!")
message(STATUS "  - Versão: ${RhodiumBlockchain_VERSION}")
message(STATUS "  - Interface web: ${BUILD_WEB_INTERFACE}")
message(STATUS "  - Qt versão: ${Qt6_VERSION}\n")
