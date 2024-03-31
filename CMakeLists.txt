# SPDX-License-Identifier: GPL-2.0-or-later
#
# Copyright(c) 2024 John Sanpe <sanpeqf@gmail.com>
#

cmake_minimum_required(VERSION 3.9)
project(stm32h7xx VERSION "1.0" LANGUAGES ASM C)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(STM32_HEADER_PATH ${CMAKE_SOURCE_DIR}/include)

configure_file(
    ${CMAKE_MODULE_PATH}/config.h.in
    ${CMAKE_BINARY_DIR}/generated/config.h
)

add_definitions(
    "-fno-builtin"
    "-ffreestanding"
    "-ffunction-sections"
    "-fdata-sections"
    "-D CORE_CM7"
    "-D STM32H750xx"
)

option(ENABLE_EXAMPLES "Build examples" OFF)
set(STM32_CMSIS_PATH ${CMAKE_SOURCE_DIR}/drivers/CMSIS)
set(STM32_DEVICE_PATH ${STM32_CMSIS_PATH}/Device/ST/STM32H7xx)
set(STM32_HAL_PATH ${CMAKE_SOURCE_DIR}/drivers/STM32H7xx_HAL_Driver)
set(STM32_LDS_PATH ${CMAKE_SOURCE_DIR}/ldscripts)

FILE(GLOB_RECURSE SRC_HEADER "include/*.h")
FILE(GLOB_RECURSE SRC_SOURCE "src/*.c")

FILE(GLOB_RECURSE DRIVERS_HEADER "drivers/*.h")
FILE(GLOB_RECURSE DRIVERS_SOURCE "${STM32_HAL_PATH}/*.c")

set(DRIVERS_SOURCE
    ${DRIVERS_SOURCE}
    ${STM32_DEVICE_PATH}/Source/Templates/gcc/startup_stm32h750xx.s
    ${STM32_DEVICE_PATH}/Source/Templates/system_stm32h7xx.c
)

set(PROJECT_FILES
    ${SRC_HEADER}
    ${SRC_SOURCE}
    ${DRIVERS_HEADER}
    ${DRIVERS_SOURCE}
)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_BINARY_DIR}/generated)
include_directories(${CMAKE_SOURCE_DIR}/bfdev/include)
include_directories(${CMAKE_SOURCE_DIR}/bfdev/arch/generic/include)
include_directories(${CMAKE_BINARY_DIR}/bfdev/generated)

include_directories(${STM32_CMSIS_PATH}/Include)
include_directories(${STM32_DEVICE_PATH}/Include)
include_directories(${STM32_HAL_PATH}/Inc)
include_directories(${STM32_HAL_PATH}/Inc/Legacy)

set(MCPU "-mcpu=cortex-m7 -mthumb")
set(MFLOAT_ABI "-mfloat-abi=soft")
set(LINKER_SCRIPT ${STM32_LDS_PATH}/STM32H750VBTX_FLASH.ld)

set(CMAKE_C_FLAGS "${MCPU} ${MFLOAT_ABI} -std=gnu11 --specs=nano.specs")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp")

set(CMAKE_EXE_LINKER_FLAGS
    "-T ${LINKER_SCRIPT} \
     --specs=nosys.specs \
     -Wl,--gc-sections \
     -Wl,--print-memory-usage \
     -Wl,-Map=${CMAKE_PROJECT_NAME}.map"
)

add_subdirectory(bfdev)
add_library(${CMAKE_PROJECT_NAME} OBJECT ${PROJECT_FILES})

if(ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif()
