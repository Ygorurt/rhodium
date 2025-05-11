cmake_minimum_required(VERSION 3.15)
project(RhodiumBlockchain VERSION 1.0 LANGUAGES CXX)

# Configuração para Windows
if(WIN32)
    set(CMAKE_EXECUTE_PROCESS_COMMAND_ECHO STDOUT)
    set(CMAKE_SUPPRESS_REGENERATION_MESSAGE TRUE)
endif()

# Configurações básicas
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configurações do Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Configuração do Qt
set(QT_PATH "D:/Qt/6.9.0/msvc2022_64")
list(APPEND CMAKE_PREFIX_PATH "${QT_PATH}")
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Configuração robusta para nlohmann_json
set(JSON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/nlohmann")
set(JSON_HEADER "${JSON_INCLUDE_DIR}/json.hpp")

if(EXISTS "${JSON_HEADER}")
    message(STATUS "Usando nlohmann/json local em: ${JSON_INCLUDE_DIR}")
    
    # Cria um alvo de interface manualmente
    add_library(nlohmann_json INTERFACE)
    target_include_directories(nlohmann_json INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/backend/include")
    
    # Cria o alias no namespace
    add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)
else()
    message(FATAL_ERROR "Arquivo json.hpp não encontrado em ${JSON_INCLUDE_DIR}")
endif()

# Configuração do frontend web (opcional)
option(ENABLE_WEB_INTERFACE "Habilitar interface web" OFF)

if(ENABLE_WEB_INTERFACE)
    # Busca Node.js em locais comuns
    find_program(NODE_EXE node
        PATHS
        "$ENV{ProgramFiles}/nodejs"
        "$ENV{SystemDrive}/nodejs"
        "$ENV{LOCALAPPDATA}/Microsoft/WindowsApps"
    )
    
    if(NODE_EXE)
        message(STATUS "Node.js encontrado: ${NODE_EXE}")
        # Adicione aqui os comandos para build do frontend
    else()
        message(STATUS "Node.js não encontrado - Interface web desabilitada")
        set(ENABLE_WEB_INTERFACE OFF)
    endif()
endif()

# Lista de arquivos fonte
set(SOURCE_FILES
    src/main.cpp
    src/blockchain/blockchain.cpp
    src/blockchain/block.cpp
    src/blockchain/transaction.cpp
    src/gui/mainwindow.cpp
)

# Lista de cabeçalhos
set(HEADER_FILES
    src/blockchain/blockchain.h
    src/blockchain/block.h
    src/blockchain/transaction.h
    src/gui/mainwindow.h
)

# Lista de arquivos UI
set(UI_FILES
    src/gui/mainwindow.ui
)

# Cria o executável
add_executable(rhodium_node ${SOURCE_FILES} ${HEADER_FILES})

# Processa arquivos UI e MOC
qt6_wrap_ui(UI_HEADERS ${UI_FILES})
qt6_wrap_cpp(MOC_SOURCES ${HEADER_FILES})
target_sources(rhodium_node PRIVATE ${UI_HEADERS} ${MOC_SOURCES})

# Configuração de includes
target_include_directories(rhodium_node PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

# Verifica se o alvo existe antes de linkar
if(TARGET nlohmann_json::nlohmann_json)
    message(STATUS "Configurando linkagem com nlohmann_json::nlohmann_json")
    target_link_libraries(rhodium_node PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        nlohmann_json::nlohmann_json
    )
else()
    message(FATAL_ERROR "Alvo nlohmann_json::nlohmann_json não encontrado para linkagem")
endif()

# Configuração para Windows
if(WIN32)
    set_target_properties(rhodium_node PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Desativa a criação de atalhos se não for necessário
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)

message(STATUS "\nConfiguração concluída com sucesso!")
message(STATUS "  - Interface web: ${ENABLE_WEB_INTERFACE}")
message(STATUS "  - Qt versão: ${Qt6_VERSION}")
