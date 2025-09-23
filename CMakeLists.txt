cmake_minimum_required(VERSION 3.20)
project(HSE_2025_2029 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# (не обязательно) предупреждения
if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Модуль 1
add_subdirectory("2025-2026/module-1")