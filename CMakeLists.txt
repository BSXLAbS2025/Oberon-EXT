cmake_minimum_required(VERSION 3.10)
project(oberon-ext C)

set(CMAKE_C_STANDARD 99)

# Улучшенный поиск Curses
set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)

if(NOT CURSES_FOUND)
    message(FATAL_ERROR "NCURSES/PDCURSES not found! Install libncurses5-dev or pdcurses.")
endif()

include_directories(include ${CURSES_INCLUDE_DIRS})

file(GLOB SOURCES "src/*.c" "src/modules/*.c")

add_executable(oberon-ext ${SOURCES})

if(WIN32)
    # На Windows линкуем pdcurses и сокеты
    target_link_libraries(oberon-ext ${CURSES_LIBRARIES} ws2_32)
else()
    # На Linux/Mac линкуем ncurses и потоки
    target_link_libraries(oberon-ext ${CURSES_LIBRARIES} pthread)
endif()
