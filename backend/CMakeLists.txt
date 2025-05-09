cmake_minimum_required(VERSION 3.15)
project(RhodiumBlockchain)

# Configurações básicas
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configuração do Qt
set(QT_PATH "D:/Qt/6.9.0/msvc2022_64")
list(APPEND CMAKE_PREFIX_PATH ${QT_PATH})

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets UiTools)

# Configuração da biblioteca JSON
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# Arquivos fonte
set(SOURCE_FILES
    src/blockchain/blockchain.cpp
    src/blockchain/blockchain.h
    src/gui/mainwindow.cpp
    src/gui/mainwindow.h
    src/gui/mainwindow.ui  # Adicione esta linha
    ${UI_HEADERS}
)

qt6_wrap_ui(UI_HEADERS
    src/gui/mainwindow.ui
)
# Executável principal - DEVE VIR ANTES DOS TARGET_INCLUDE_DIRECTORIES
add_executable(rhodium_node ${SOURCE_FILES})

# Configuração de includes - DEPOIS DE add_executable
target_include_directories(rhodium_node PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${nlohmann_json_INCLUDE_DIRS}
    ${Qt6_INCLUDE_DIRS}
)

# Linkar bibliotecas
target_link_libraries(rhodium_node PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
