add_executable(module1_week2
        main.cpp
        function.cpp
)
# На всякий случай добавим путь к заголовку в текущей папке
target_include_directories(module1_week2 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)