add_executable(module1_week2
        main.cpp
        function.cpp
        plus.cpp
        plus.h
        minus.cpp
        minus.h
        multiply.cpp
        multiply.h
        divide.cpp
        divide.h
)
# На всякий случай добавим путь к заголовку в текущей папке
target_include_directories(module1_week2 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)