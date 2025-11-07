add_executable(module1_week3
        main.cpp
)
target_include_directories(module1_week3 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)