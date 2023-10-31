cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)
project(ws2812_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_subdirectory(./include)

add_executable(ws2812
	ws2812.c
)
pico_generate_pio_header(ws2812
	${CMAKE_CURRENT_LIST_DIR}/ws2812.pio
)

target_link_libraries(ws2812
	pico_stdlib
	hardware_pio
	hardware_dma
	hardware_spi
	display
)
