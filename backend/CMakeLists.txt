cmake_minimum_required(VERSION 3.15)
project(RhodiumBlockchain)

# Configurações básicas
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Opções para Windows
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0A00)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Incluir diretórios
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(WIN32)
    set(OPENSSL_ROOT_DIR "D:/OpenSSL-Win64")
    set(OPENSSL_USE_STATIC_LIBS TRUE)
endif()

# Encontrar dependências
find_package(OpenSSL REQUIRED)

# Arquivos fonte (ajuste conforme sua estrutura)
file(GLOB_RECURSE SOURCE_FILES 
    "src/*.cpp" 
    "src/*.h"
)

# Executável principal
add_executable(rhodium_node ${SOURCE_FILES})

# Linkar bibliotecas
target_link_libraries(rhodium_node
    PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
)
