cmake_minimum_required(VERSION 3.15)
project(RhodiumBlockchain VERSION 1.0 LANGUAGES CXX)

if(WIN32)
    set(CMAKE_EXECUTE_PROCESS_COMMAND_ECHO STDOUT)
    set(CMAKE_MAKE_PROGRAM "powershell.exe")
endif()

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

# Configuração da interface web (opcional)
option(BUILD_WEB_INTERFACE "Build web interface" ON)

if(BUILD_WEB_INTERFACE)
    find_program(NODE_EXE node)
    find_program(NPM_EXE npm)
    
    if(NODE_EXE AND NPM_EXE)
        message(STATUS "Found Node.js: ${NODE_EXE}")
        message(STATUS "Found npm: ${NPM_EXE}")
        # Add your web interface build commands here
    else()
        message(WARNING "Node.js/npm not found properly. Web interface disabled.")
        # Alternative: use execute_process to get node/npm paths
        execute_process(COMMAND where node
            OUTPUT_VARIABLE NODE_PATH
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(NODE_PATH)
            set(NODE_EXE ${NODE_PATH})
            message(STATUS "Found Node.js via path: ${NODE_EXE}")
        endif()
    endif()
endif()

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
     add_definitions(-D_WINDOWS)
endif()

# Mensagem final
message(STATUS "Configuração do projeto RhodiumBlockchain concluída com sucesso!")
