#
#  Copyright (c) 2025, The OpenThread Authors.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. Neither the name of the copyright holder nor the
#     names of its contributors may be used to endorse or promote products
#     derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#
set(TARGETS_CONFIGS
    "rtl8777g"                 "-DBOARD_RTL8777G"                     "app.ld.gen"
)

function(setup_target target_name)
    list(FIND TARGETS_CONFIGS "${target_name}" index)
    if(NOT index LESS 0)
        math(EXPR flag_index "${index} + 1")
        math(EXPR ld_file_index "${index} + 2")

        list(GET TARGETS_CONFIGS ${flag_index} board_flags)
        list(GET TARGETS_CONFIGS ${ld_file_index} ld_file_suffix)

        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${board_flags}" PARENT_SCOPE)
        set(LD_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_TARGET}/${ld_file_suffix}" PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Unknown build board target: ${target_name}")
    endif()
endfunction()

if(${OT_CMAKE_NINJA_TARGET} STREQUAL "matter-cli-ftd" OR ${OT_CMAKE_NINJA_TARGET} STREQUAL "matter-cli-mtd")
    message(STATUS "Build matter. Skip build board target.")
else()
    setup_target(${BUILD_BOARD_TARGET})
endif()

if (NOT OT_EXTERNAL_MBEDTLS)
target_compile_definitions(ot-config INTERFACE
        MBEDTLS_USER_CONFIG_FILE="${RT_PLATFORM}-mbedtls-config.h"
)
else()

add_library(bee4-mbedtls INTERFACE)

target_link_libraries(bee4-mbedtls
    INTERFACE
        ${REALTEK_SDK_ROOT}/lib/bee4/libmbedtls.a
        ${REALTEK_SDK_ROOT}/lib/bee4/libmbedx509.a
        ${REALTEK_SDK_ROOT}/lib/bee4/libmbedcrypto.a
)

target_include_directories(ot-config
    INTERFACE
        ${REALTEK_SDK_ROOT}/subsys/mbedtls
        ${REALTEK_SDK_ROOT}/subsys/mbedtls/repo/include
)

target_compile_definitions(ot-config INTERFACE
        MBEDTLS_CONFIG_FILE="mbedtls-config.h"
        MBEDTLS_USER_CONFIG_FILE="${RT_PLATFORM}-mbedtls-config.h"
)
endif()

add_library(bee4-peripheral
    # peripheral
    "${REALTEK_SDK_ROOT}/bsp/driver/pinmux/src/rtl87x2g/rtl_pinmux.c"
    "${REALTEK_SDK_ROOT}/bsp/driver/rcc/src/rtl87x2g/rtl_rcc.c"
    "${REALTEK_SDK_ROOT}/bsp/driver/nvic/src/rtl87x2g/rtl_nvic.c"
    "${REALTEK_SDK_ROOT}/bsp/driver/tim/src/rtl_common/rtl_tim.c"
    "${REALTEK_SDK_ROOT}/bsp/driver/uart/src/rtl_common/rtl_uart.c"
    "${REALTEK_SDK_ROOT}/bsp/driver/dma/src/rtl_common/rtl_gdma.c"
    "${REALTEK_SDK_ROOT}/bsp/driver/gpio/src/rtl_common/rtl_gpio.c"
)

set_target_properties(
    bee4-peripheral
    PROPERTIES
        C_STANDARD 99
)

target_link_directories(bee4-peripheral
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_TARGET}
        ${PROJECT_SOURCE_DIR}/lib/${RT_PLATFORM}
)

target_include_directories(bee4-peripheral
    PRIVATE
        ${REALTEK_SDK_INCPATH}
)

add_library(openthread-bee4
    ../alarm.c
    ../diag.c
    ../entropy.c
    flash.c
    logging.c
    ../misc.c
    ../radio.c
    system.c
    uart.c
    # start up and entry point
    "${REALTEK_SDK_ROOT}/bsp/boot/rtl87x2g/startup_rtl.c"
    system_rtl.c
    zb_main.c
    thread_task.c
    # crypto
    # app
    xmodem.c
    dbg_printf.c
    # profile
    # peripheral
    # utils
    "${PROJECT_SOURCE_DIR}/openthread/examples/platforms/utils/mac_frame.cpp"
    "${PROJECT_SOURCE_DIR}/openthread/examples/platforms/utils/link_metrics.cpp"
    ../mac_frame_add.cpp
    ../mac_802154_frame_parser.c
)

set_target_properties(
    openthread-bee4
    PROPERTIES
        C_STANDARD 99
        CXX_STANDARD 11
)

string(REGEX REPLACE "(/.*|_.*$)" "" BUILD_TARGET_VALID "${BUILD_TARGET}")

target_link_directories(openthread-bee4
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_TARGET}
        ${REALTEK_SDK_ROOT}/lib/${RT_PLATFORM}
        ${REALTEK_SDK_ROOT}/subsys/lwip/for_matter/lib
)

target_link_libraries(openthread-bee4
    PRIVATE
        ${OT_MBEDTLS}
        ot-config
        bee4-internal
        bee4-peripheral
        "${REALTEK_SDK_ROOT}/subsys/usb/usb_lib/lib/rtl87x2g/gcc/libusb.a"
        "${REALTEK_SDK_ROOT}/subsys/usb/usb_hal/lib/rtl87x2g/gcc/libusb_hal.a"
        "${REALTEK_SDK_ROOT}/subsys/bluetooth/gap_ext/lib/rtl87x2g/bt_host_0_0/gcc/libgap_utils.a"
        "${REALTEK_SDK_ROOT}/subsys/mac_driver/portable/bee4/bee4-internal.axf"
        "${REALTEK_SDK_ROOT}/bsp/driver/driver_lib/lib/rtl87x2g/gcc/librtl87x2g_io.a"
        "${REALTEK_SDK_ROOT}/bsp/sdk_lib/lib/rtl87x2g/gcc/librtl87x2g_sdk.a"
        "${REALTEK_SDK_ROOT}/bin/rtl87x2g/rom_lib/libROM_NS.a"
        "${REALTEK_SDK_ROOT}/bin/rtl87x2g/rom_lib/ROM_CMSE_Lib.o"
        "${REALTEK_SDK_ROOT}/bin/rtl87x2g/rom_lib/liblowerstack.a"
)

target_link_options(openthread-bee4
    PUBLIC
        -T${LD_FILE}
        -Wl,--no-wchar-size-warning
        -Wl,--wrap,_malloc_r
        -Wl,--wrap,_free_r
        -Wl,--wrap,_realloc_r
        -Wl,--wrap,_calloc_r
        -Wl,--wrap,memcpy
        -Wl,--wrap,memset
        -Wl,--wrap,snprintf
        -Wl,--wrap,vsnprintf
        -Wl,--wrap,mac_EnterCritical_rom
        -Wl,--wrap,mac_ExitCritical_rom
        -Wl,--wrap,mac_RstRF_rom
        -Wl,--gc-sections
        -Wl,--print-memory-usage
        -Wl,--no-warn-rwx-segments
)

target_compile_definitions(openthread-bee4
    PUBLIC
        ${OT_PLATFORM_DEFINES}
        "BUILD_WITHOUT_FTL=1"
        "RT_PLATFORM_BEE4"
)

if(${OT_CMAKE_NINJA_TARGET} STREQUAL "ot-ncp-ftd")
    target_compile_definitions(openthread-bee4 PUBLIC "BUILD_NCP=1")
endif()
if(${OT_CMAKE_NINJA_TARGET} STREQUAL "ot-ncp-mtd")
    target_compile_definitions(openthread-bee4 PUBLIC "BUILD_NCP=1")
endif()
if(${OT_CMAKE_NINJA_TARGET} STREQUAL "ot-rcp")
    target_compile_definitions(openthread-bee4 PUBLIC "BUILD_RCP=1")
endif()

if(${ENABLE_CLI} STREQUAL "ON")
    target_compile_definitions(openthread-bee4 PUBLIC "ENABLE_CLI=1")
else()
if(${BUILD_BOARD_TARGET} STREQUAL "rtl8777g_cert")
    target_compile_definitions(openthread-bee4 PUBLIC "ENABLE_CLI=1")
else()
    target_compile_definitions(openthread-bee4 PUBLIC "ENABLE_CLI=0")
endif()
endif()

target_compile_options(openthread-bee4
    PRIVATE
        ${OT_CFLAGS}
        -Wno-attributes
)

target_include_directories(openthread-bee4
    PRIVATE
        ${OT_PUBLIC_INCLUDES}
        ${PROJECT_SOURCE_DIR}/src/core
        ${PROJECT_SOURCE_DIR}/openthread/examples/platforms
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/third_party/Realtek/bee4/common
        ${REALTEK_SDK_INCPATH}
        ${REALTEK_SDK_ROOT}/subsys/mac_driver
        ${REALTEK_SDK_ROOT}/subsys/mac_driver/portable/bee4
        ${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_TARGET}
        ${CMAKE_CURRENT_SOURCE_DIR}/common
        ${REALTEK_SDK_ROOT}/subsys/usb/usb_hal/inc
        ${REALTEK_SDK_ROOT}/subsys/usb/usb_lib/inc/class
        ${REALTEK_SDK_ROOT}/subsys/usb/usb_lib/inc/composite
)

##################################################
# ot-cli-ftd
##################################################
if(${OT_CMAKE_NINJA_TARGET} STREQUAL "ot-cli-ftd")
target_sources(ot-cli-ftd
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src/bee4/main_ns.c
)

target_include_directories(ot-cli-ftd
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src/bee4/${BUILD_TARGET}
)

target_compile_definitions(ot-cli-ftd
    PRIVATE
        "DLPS_EN=0"
)

if(${BUILD_BOARD_TARGET} STREQUAL "rtl8777g_test")
target_compile_definitions(openthread-bee4
    PRIVATE
        "BUILD_CERT"
        "XMODEM_ENABLE=1"
)
endif()
endif()

##################################################
# ot-cli-mtd
##################################################
if(${OT_CMAKE_NINJA_TARGET} STREQUAL "ot-cli-mtd")
target_sources(ot-cli-mtd
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src/bee4/main_ns.c
    ${REALTEK_SDK_ROOT}/bsp/power/io_dlps.c
)

target_include_directories(ot-cli-mtd
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src/bee4/${BUILD_TARGET}
)

target_compile_definitions(ot-cli-mtd
    PRIVATE
        "DLPS_EN=1"
)

target_compile_definitions(openthread-bee4
    PRIVATE
        "DLPS_EN=1"
)
endif()
