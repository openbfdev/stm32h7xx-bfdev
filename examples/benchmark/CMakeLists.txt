# SPDX-License-Identifier: GPL-2.0-or-later
#
# Copyright(c) 2024 John Sanpe <sanpeqf@gmail.com>
#

add_executable(benchmark main.c mpi.c crc.c rbtree.c)
target_link_libraries(benchmark ${CMAKE_PROJECT_NAME})
target_link_libraries(benchmark bfdev)

add_custom_command(
    TARGET benchmark
    POST_BUILD COMMAND
    ${CMAKE_OBJCOPY} -O binary
    ${CMAKE_CURRENT_BINARY_DIR}/benchmark
    ${CMAKE_CURRENT_BINARY_DIR}/benchmark.bin
    POST_BUILD COMMAND
    ${CMAKE_OBJCOPY} -O ihex
    ${CMAKE_CURRENT_BINARY_DIR}/benchmark
    ${CMAKE_CURRENT_BINARY_DIR}/benchmark.hex
)
