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
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# Configuração para OpenSSL
find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
    message(STATUS "OpenSSL encontrado:")
    message(STATUS "  - Versão: ${OPENSSL_VERSION}")
else()
    message(FATAL_ERROR "OpenSSL não encontrado - Necessário para criptografia ECDSA")
endif()

# Substitua a seção nlohmann_json por:
set(JSON_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE ${JSON_INCLUDE_DIR})
add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)

# Definir estrutura de diretórios
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)

# Lista de arquivos fonte
file(GLOB_RECURSE SOURCE_FILES 
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.cc"
)

# Lista de cabeçalhos
file(GLOB_RECURSE HEADER_FILES 
    "${SOURCE_DIR}/*.h"
    "${SOURCE_DIR}/*.hpp"
)

# Arquivos de recursos
file(GLOB RESOURCE_FILES
    "${RESOURCE_DIR}/*.qrc"
    "${RESOURCE_DIR}/*.rc"
)

# Arquivos UI
file(GLOB UI_FILES
    "${SOURCE_DIR}/gui/*.ui"
)

# Cria o executável principal
add_executable(rhodium_node 
    ${SOURCE_FILES}
    ${HEADER_FILES}
    ${RESOURCE_FILES}
)

# Configuração de includes
target_include_directories(rhodium_node PRIVATE
    ${SOURCE_DIR}
    ${INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
)

# Configurações de compilação
if(MSVC)
    target_compile_options(rhodium_node PRIVATE 
        /W4
        /WX
        /permissive-
    )
    target_compile_definitions(rhodium_node PRIVATE
        _CRT_SECURE_NO_WARNINGS
    )
else()
    target_compile_options(rhodium_node PRIVATE 
        -Wall
        -Wextra
        -Werror
    )
endif()

# Processa arquivos UI e MOC
qt6_wrap_ui(UI_HEADERS ${UI_FILES})
qt6_wrap_cpp(MOC_SOURCES ${HEADER_FILES})
target_sources(rhodium_node PRIVATE ${UI_HEADERS} ${MOC_SOURCES})

# Linkagem de bibliotecas
target_link_libraries(rhodium_node PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    OpenSSL::Crypto
    OpenSSL::SSL
    nlohmann_json::nlohmann_json
)

# Configuração para Windows
if(WIN32)
    set_target_properties(rhodium_node PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    
    # Copia DLLs necessárias
    add_custom_command(TARGET rhodium_node POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:rhodium_node>/platforms"
        COMMAND ${CMAKE_COMMAND} -E copy
            "${QT_PATH}/plugins/platforms/qwindows.dll"
            "$<TARGET_FILE_DIR:rhodium_node>/platforms/"
        COMMAND ${CMAKE_COMMAND} -E copy
            "${OPENSSL_CRYPTO_LIBRARY}"
            "$<TARGET_FILE_DIR:rhodium_node>/"
        COMMAND ${CMAKE_COMMAND} -E copy
            "${OPENSSL_SSL_LIBRARY}"
            "$<TARGET_FILE_DIR:rhodium_node>/"
        COMMENT "Copiando dependências..."
    )
endif()

# Configurações de instalação
install(TARGETS rhodium_node
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

install(DIRECTORY ${RESOURCE_DIR}/
    DESTINATION resources
)

message(STATUS "\nConfiguração concluída com sucesso!")
message(STATUS "  - Versão: ${RhodiumBlockchain_VERSION}")
message(STATUS "  - Qt versão: ${Qt6_VERSION}")
message(STATUS "  - OpenSSL versão: ${OPENSSL_VERSION}")
message(STATUS "  - Diretório fonte: ${SOURCE_DIR}")
message(STATUS "  - Número de arquivos fonte: ${SOURCE_FILES}")
