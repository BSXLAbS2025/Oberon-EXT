cmake_minimum_required(VERSION 3.10)
project(oberon-ext C)

set(CMAKE_C_STANDARD 99)

# Поиск Curses (ncurses на Linux/Mac, pdcurses на Windows)
find_package(Curses REQUIRED)

# Включаем директории заголовков
include_directories(include ${CURSES_INCLUDE_DIR})

# Собираем список исходников
file(GLOB SOURCES 
    "src/*.c"
    "src/modules/*.c"
)

# Создаем исполняемый файл
add_executable(oberon-ext ${SOURCES})

# Линковка библиотек
if(WIN32)
    # Для Windows добавляем Winsock2
    target_link_libraries(oberon-ext ${CURSES_LIBRARIES} ws2_32)
else()
    # Для Linux/Mac добавляем pthread
    target_link_libraries(oberon-ext ${CURSES_LIBRARIES} pthread)
endif()

# Установка (для формирования dist)
install(TARGETS oberon-ext DESTINATION .)
