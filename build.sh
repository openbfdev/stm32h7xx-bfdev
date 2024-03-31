#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright(c) 2024 John Sanpe <sanpeqf@gmail.com>
#

options+=" -DHOST_C_COMPILER=/bin/gcc"
options+=" -DENABLE_EXAMPLES=ON"
options+=" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
options+=" --toolchain cmake/toolchain.cmake"

cmake -Bbuild $options
cmake --build build -j$(nproc)
