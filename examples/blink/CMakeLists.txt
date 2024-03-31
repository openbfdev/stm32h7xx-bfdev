# SPDX-License-Identifier: GPL-2.0-or-later
#
# Copyright(c) 2023 ffashion <helloworldffashion@gmail.com>
#

add_executable(blink main.c)
target_link_libraries(blink ${CMAKE_PROJECT_NAME})

add_custom_command(
    TARGET blink
    POST_BUILD COMMAND
    ${CMAKE_OBJCOPY} -O binary
    ${CMAKE_CURRENT_BINARY_DIR}/blink
    ${CMAKE_CURRENT_BINARY_DIR}/blink.bin
    POST_BUILD COMMAND
    ${CMAKE_OBJCOPY} -O ihex
    ${CMAKE_CURRENT_BINARY_DIR}/blink
    ${CMAKE_CURRENT_BINARY_DIR}/blink.hex
)
