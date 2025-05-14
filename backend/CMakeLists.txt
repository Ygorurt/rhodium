cmake_minimum_required(VERSION 3.15)
project(RhodiumBlockchain VERSION 1.0 LANGUAGES CXX)

# Configuração para Windows
if(WIN32)
    set(CMAKE_EXECUTE_PROCESS_COMMAND_ECHO STDOUT)
    set(CMAKE_SUPPRESS_REGENERATION_MESSAGE TRUE)
    
    # Usa PowerShell tradicional se pwsh não estiver disponível
    find_program(PWSH_PATH pwsh.exe)
    if(NOT PWSH_PATH)
        set(CMAKE_MAKE_PROGRAM "powershell.exe")
    endif()
endif()

# Configurações básicas
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Adicione estas linhas
set(NETWORK_SOURCES
    src/blockchain/network/p2p.cpp
    src/blockchain/network/message.cpp
)

# Configurações do Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC_MOC_OPTIONS "--no-warnings")  # Suprime avisos de MOC vazio

# Configuração do Qt
set(QT_PATH "D:/Qt/6.9.0/msvc2022_64")
list(APPEND CMAKE_PREFIX_PATH "${QT_PATH}")
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Configuração para ignorar Vulkan se não for necessário
set(Vulkan_FOUND FALSE)

# Configuração robusta para nlohmann/json
set(JSON_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/nlohmann")
set(JSON_HEADER "${JSON_INCLUDE_DIR}/json.hpp")

if(EXISTS "${JSON_HEADER}")
    message(STATUS "Usando nlohmann/json local em: ${JSON_INCLUDE_DIR}")
    
    add_library(nlohmann_json INTERFACE)
    target_include_directories(nlohmann_json INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)
else()
    message(FATAL_ERROR "Arquivo json.hpp não encontrado em ${JSON_INCLUDE_DIR}")
endif()

# Configuração do frontend web (opcional)
option(ENABLE_WEB_INTERFACE "Habilitar interface web" OFF)

if(ENABLE_WEB_INTERFACE)
    set(FRONTEND_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../frontend")  # Assume estrutura irmã
    
    if(EXISTS "${FRONTEND_DIR}/package.json")
        find_program(NODE_EXE node
            PATHS
            "$ENV{ProgramFiles}/nodejs"
            "$ENV{SystemDrive}/nodejs"
            "$ENV{LOCALAPPDATA}/Microsoft/WindowsApps"
            NO_CACHE
        )
        
        if(NODE_EXE)
            message(STATUS "Node.js encontrado: ${NODE_EXE}")
            add_custom_target(
                build_frontend
                COMMAND npm install
                COMMAND npm run build
                WORKING_DIRECTORY ${FRONTEND_DIR}
                COMMENT "Building frontend..."
            )
        else()
            message(WARNING "Node.js não encontrado - Interface web desabilitada")
            set(ENABLE_WEB_INTERFACE OFF)
        endif()
    else()
        message(WARNING "package.json não encontrado em ${FRONTEND_DIR} - Interface web desabilitada")
        set(ENABLE_WEB_INTERFACE OFF)
    endif()
endif()

# Lista de arquivos fonte
set(SOURCE_FILES
    src/main.cpp
    src/blockchain/blockchain.cpp
    src/blockchain/block.cpp
    src/blockchain/transaction.cpp
    src/blockchain/walletwidget.cpp
    src/blockchain/miningwidget.cpp
    src/blockchain/networkwidget.cpp
    src/blockchain/transactionwidget.cpp
    src/gui/mainwindow.cpp
)

set(CRYPTO_SOURCES
    src/crypto/sha256.cpp
    src/crypto/ecdsa.cpp
)

# Lista de cabeçalhos
set(HEADER_FILES
    src/blockchain/blockchain.h
    src/blockchain/block.h
    src/blockchain/transaction.h
    src/blockchain/walletwidget.h
    src/blockchain/miningwidget.h
    src/blockchain/networkwidget.h
    src/blockchain/transactionwidget.h
    src/crypto/sha256.h
    src/network/p2p.h
    src/gui/mainwindow.h
)

# Arquivos de recursos
set(RESOURCES
    resources/resources.qrc
)

# Lista de arquivos UI
set(UI_FILES
    src/gui/mainwindow.ui
)

# Cria o executável principal
add_executable(rhodium_node ${SOURCE_FILES} ${HEADER_FILES} ${CRYPTO_SOURCES} ${NETWORK_SOURCES} ${RESOURCES})

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

# Linkagem de bibliotecas
target_link_libraries(rhodium_node PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    nlohmann_json::nlohmann_json
)

# Configuração para Windows
if(WIN32)
    set_target_properties(rhodium_node PROPERTIES
        WIN32_EXECUTABLE TRUE
        LINK_FLAGS "/ENTRY:mainCRTStartup"
    )
endif()

# Copia os plugins necessários após o build
add_custom_command(TARGET rhodium_node POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/Release/platforms"
    COMMAND ${CMAKE_COMMAND} -E copy
        "${QT_PATH}/plugins/platforms/qwindows.dll"
        "${CMAKE_CURRENT_BINARY_DIR}/Release/platforms/"
    COMMENT "Copiando plugins Qt..."
)

# Adiciona dependência do frontend se estiver habilitado
if(ENABLE_WEB_INTERFACE AND TARGET build_frontend)
    add_dependencies(rhodium_node build_frontend)
endif()

message(STATUS "\nConfiguração concluída com sucesso!")
message(STATUS "  - Versão: ${RhodiumBlockchain_VERSION}")
message(STATUS "  - Interface web: ${ENABLE_WEB_INTERFACE}")
message(STATUS "  - Qt versão: ${Qt6_VERSION}")
