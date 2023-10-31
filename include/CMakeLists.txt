file(GLOB FILES *.c *.h)
add_library(display ${FILES})
target_link_libraries(display pico_stdlib hardware_spi)
target_include_directories(display PUBLIC ./)
