cmake_minimum_required(VERSION 3.15)
project(RhodiumBlockchain VERSION 1.0 LANGUAGES CXX)

# Windows-specific configurations
if(WIN32)
    set(CMAKE_EXECUTE_PROCESS_COMMAND_ECHO STDOUT)
    set(CMAKE_SUPPRESS_REGENERATION_MESSAGE TRUE)
endif()

# Qt configurations
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt setup
set(QT_PATH "D:/Qt/6.9.0/msvc2022_64")
list(APPEND CMAKE_PREFIX_PATH "${QT_PATH}")
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets UiTools)

# JSON library configuration
set(JSON_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/backend/include")
set(JSON_HEADER_FILE "${JSON_BASE_DIR}/nlohmann/json.hpp")

if(EXISTS "${JSON_HEADER_FILE}")
    message(STATUS "Using local nlohmann/json from: ${JSON_HEADER_FILE}")
    
    add_library(nlohmann_json INTERFACE)
    target_include_directories(nlohmann_json INTERFACE ${JSON_BASE_DIR})
    add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)
else()
    message(WARNING "Local json.hpp not found, downloading via FetchContent...")
    
    include(FetchContent)
    FetchContent_Declare(
        nlohmann_json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/include.zip
        URL_HASH SHA256=a22461d13119ac5c78f205d3df1db13403e58ce1bb1794edc9313677313f4a9d
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# Web interface option
option(BUILD_WEB_INTERFACE "Enable web interface" OFF)

if(BUILD_WEB_INTERFACE)
    find_program(NODE_EXE
        NAMES node node.exe
        PATHS
            "$ENV{ProgramFiles}/nodejs"
            "$ENV{SystemDrive}/nodejs"
            "$ENV{LOCALAPPDATA}/Microsoft/WindowsApps"
        DOC "Path to Node.js executable"
    )
    
    if(NODE_EXE)
        message(STATUS "Found Node.js: ${NODE_EXE}")
        # Add web interface build commands here
    else()
        message(WARNING "Node.js not found - Web interface disabled")
    endif()
endif()

# Source files
set(SOURCE_FILES
    src/main.cpp
    src/blockchain/blockchain.cpp
    src/blockchain/block.cpp
    src/blockchain/transaction.cpp
    src/gui/mainwindow.cpp
)

# Header files
set(HEADER_FILES
    src/blockchain/blockchain.h
    src/blockchain/block.h
    src/blockchain/transaction.h
    src/gui/mainwindow.h
)

# UI files
set(UI_FILES
    src/gui/mainwindow.ui
)

# Create main executable
add_executable(rhodium_node WIN32 ${SOURCE_FILES} ${HEADER_FILES})

# Process UI and MOC files
qt6_wrap_ui(UI_HEADERS ${UI_FILES})
qt6_wrap_cpp(MOC_SOURCES ${HEADER_FILES})
target_sources(rhodium_node PRIVATE ${UI_HEADERS} ${MOC_SOURCES})

# Include directories
target_include_directories(rhodium_node PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/backend/include"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

# Link libraries
target_link_libraries(rhodium_node PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    nlohmann_json::nlohmann_json
)

# Final status message
message(STATUS "\nRhodiumBlockchain configuration successful!")
message(STATUS "  - Version: ${RhodiumBlockchain_VERSION}")
message(STATUS "  - Qt version: ${Qt6_VERSION}")
message(STATUS "  - Web interface: ${BUILD_WEB_INTERFACE}")
message(STATUS "  - JSON path: ${JSON_BASE_DIR}\n")
