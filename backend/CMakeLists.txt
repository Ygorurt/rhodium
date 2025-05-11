cmake_minimum_required(VERSION 3.15)
project(RhodiumBlockchain VERSION 1.0 LANGUAGES CXX)

# Configurações do Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Configurações básicas
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configuração do Qt
set(QT_PATH "D:/Qt/6.9.0/msvc2022_64")
list(APPEND CMAKE_PREFIX_PATH "${QT_PATH}")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets UiTools)

# Configuração da biblioteca JSON
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# Configuração de diretórios
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

# Geração dos arquivos UI
qt6_wrap_ui(UI_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/mainwindow.ui"
)

# Geração dos meta-objetos
qt6_wrap_cpp(MOC_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/mainwindow.h"
)

# Arquivos fonte
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
add_executable(rhodium_node ${SOURCE_FILES})

# Configuração de includes
target_include_directories(rhodium_node PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

# Linkar bibliotecas - note a mudança aqui para usar o alvo correto
target_link_libraries(rhodium_node PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    nlohmann_json::nlohmann_json  # Esta é a forma correta de referenciar a biblioteca
)

# Configurações específicas para Windows
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")
endif()
