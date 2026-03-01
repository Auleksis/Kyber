cmake_minimum_required(VERSION 3.16)

project(KyberProject LANGUAGES CXX)

# ---- C++ стандарт ----
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- Тип сборки ----
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# ===============================
#       OpenSSL 3.6
# ===============================

# Указываем путь к нашей версии
set(OPENSSL_ROOT_DIR /opt/openssl-3.6)
set(OPENSSL_USE_STATIC_LIBS FALSE)

find_package(OpenSSL REQUIRED COMPONENTS Crypto)

message(STATUS "Using OpenSSL version: ${OpenSSL_VERSION}")
message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")

# ---- Исходники ----
set(SOURCES
    Kyber.cpp
    KPKE.cpp
    MLKEM.cpp
    PolyRing.cpp
    Poly.cpp
    PolyCoder.cpp
    PolyMatrix.cpp
    PolyVector.cpp
)

add_executable(Kyber ${SOURCES})

target_link_libraries(Kyber PRIVATE OpenSSL::Crypto)

# Добавляем rpath чтобы во время запуска брались нужные .so
set_target_properties(Kyber PROPERTIES
    BUILD_RPATH "/opt/openssl-3.6/lib64"
    INSTALL_RPATH "/opt/openssl-3.6/lib64"
)