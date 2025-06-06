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

cmake_minimum_required(VERSION 3.10.2)
project(ot-realtek VERSION 0.2.0 LANGUAGES C CXX ASM)

if("${OT_CMAKE_NINJA_TARGET}" STREQUAL "matter-cli-ftd" OR "${OT_CMAKE_NINJA_TARGET}" STREQUAL "matter-cli-mtd")

string(FIND "${BUILD_TARGET}" "rtl8777g" FOUND_INDEX)
if(NOT FOUND_INDEX EQUAL -1)
set(RT_PLATFORM "bee4" CACHE STRING "")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DBOARD_RTL8777G")
endif()

set(LD_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/${RT_PLATFORM}/${BUILD_TARGET}/app.ld.gen")

endif()

include(script/${BUILD_TYPE}.cmake)

set(RT_PLATFORM_VALUES
    "bee4"
)
set_property(CACHE RT_PLATFORM PROPERTY STRINGS ${RT_PLATFORM_VALUES})
if(NOT RT_PLATFORM IN_LIST RT_PLATFORM_VALUES)
    message(FATAL_ERROR "Please select a supported platform: ${RT_PLATFORM_VALUES}")
endif()

# OpenThread config
if(${OT_CMAKE_NINJA_TARGET} STREQUAL "matter-cli-ftd" OR ${OT_CMAKE_NINJA_TARGET} STREQUAL "matter-cli-mtd")
set(OT_PLATFORM "external" CACHE STRING "")
set(OT_SLAAC ON CACHE BOOL "")
set(OT_BORDER_AGENT OFF CACHE BOOL "")
set(OT_BORDER_ROUTER OFF CACHE BOOL "")
set(OT_COMMISSIONER OFF CACHE BOOL "")
set(OT_JOINER ON CACHE BOOL "")
set(OT_DNS_CLIENT ON CACHE BOOL "")
set(OT_DHCP6_SERVER OFF CACHE BOOL "")
set(OT_DHCP6_CLIENT ON CACHE BOOL "")
set(OT_COAP ON CACHE BOOL "")
set(OT_COAPS ON CACHE BOOL "")
set(OT_COAP_OBSERVE OFF CACHE BOOL "")
set(OT_LINK_RAW OFF CACHE BOOL "")
set(OT_MAC_FILTER ON CACHE BOOL "")
set(OT_SERVICE ON CACHE BOOL "")
set(OT_UDP_FORWARD OFF CACHE BOOL "")
set(OT_ECDSA ON CACHE BOOL "")
set(OT_SNTP_CLIENT OFF CACHE BOOL "")
set(OT_CHILD_SUPERVISION ON CACHE BOOL "")
set(OT_JAM_DETECTION OFF CACHE BOOL "")
set(OT_LOG_LEVEL_DYNAMIC ON CACHE BOOL "")
set(OT_EXTERNAL_HEAP ON CACHE BOOL "")
set(OT_SRP_CLIENT ON CACHE BOOL "")
set(OT_DUA ON CACHE BOOL "")
set(OT_BACKBONE_ROUTER OFF CACHE BOOL "")
set(OT_MLR ON CACHE BOOL "")
set(OT_CSL_RECEIVER ON CACHE BOOL "")
set(OT_LINK_METRICS_INITIATOR OFF CACHE BOOL "")
set(OT_LINK_METRICS_SUBJECT ON CACHE BOOL "")
set(OT_CHANNEL_MONITOR OFF CACHE BOOL "")
set(OT_CHANNEL_MANAGER OFF CACHE BOOL "")
else()
set(OT_PLATFORM "external" CACHE STRING "")
set(OT_SLAAC ON CACHE BOOL "")
set(OT_BORDER_AGENT ON CACHE BOOL "")
set(OT_BORDER_ROUTER ON CACHE BOOL "")
set(OT_COMMISSIONER ON CACHE BOOL "")
set(OT_JOINER ON CACHE BOOL "")
set(OT_DNS_CLIENT ON CACHE BOOL "")
set(OT_DHCP6_SERVER ON CACHE BOOL "")
set(OT_DHCP6_CLIENT ON CACHE BOOL "")
set(OT_COAP ON CACHE BOOL "")
set(OT_COAPS ON CACHE BOOL "")
set(OT_COAP_OBSERVE ON CACHE BOOL "")
set(OT_LINK_RAW ON CACHE BOOL "")
set(OT_MAC_FILTER ON CACHE BOOL "")
set(OT_SERVICE ON CACHE BOOL "")
set(OT_UDP_FORWARD ON CACHE BOOL "")
set(OT_ECDSA ON CACHE BOOL "")
set(OT_SNTP_CLIENT ON CACHE BOOL "")
set(OT_CHILD_SUPERVISION ON CACHE BOOL "")
set(OT_JAM_DETECTION ON CACHE BOOL "")
set(OT_LOG_LEVEL_DYNAMIC ON CACHE BOOL "")
set(OT_EXTERNAL_HEAP ON CACHE BOOL "")
set(OT_SRP_CLIENT ON CACHE BOOL "")
set(OT_DUA ON CACHE BOOL "")
set(OT_BACKBONE_ROUTER ON CACHE BOOL "")
set(OT_MLR ON CACHE BOOL "")
set(OT_CSL_RECEIVER ON CACHE BOOL "")
set(OT_LINK_METRICS_INITIATOR ON CACHE BOOL "")
set(OT_LINK_METRICS_SUBJECT ON CACHE BOOL "")
set(OT_CHANNEL_MONITOR OFF CACHE BOOL "")
set(OT_CHANNEL_MANAGER OFF CACHE BOOL "")
endif()

set(OT_PLATFORM_LIB "openthread-${RT_PLATFORM}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

add_subdirectory(openthread)

target_compile_definitions(ot-config INTERFACE
    OPENTHREAD_CONFIG_FILE="openthread-core-${RT_PLATFORM}-config.h"
    OPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE="openthread-core-${RT_PLATFORM}-config-check.h"
)

if(${OT_CMAKE_NINJA_TARGET} STREQUAL "ot-rcp")
target_compile_definitions(ot-config INTERFACE
    OPENTHREAD_PROJECT_CORE_CONFIG_FILE="openthread-core-${BUILD_TARGET}-rcp-config.h"
)
else()
target_compile_definitions(ot-config INTERFACE
    OPENTHREAD_PROJECT_CORE_CONFIG_FILE="openthread-core-${RT_PLATFORM}-config.h"
)
endif()

target_include_directories(ot-config INTERFACE
    ${PROJECT_SOURCE_DIR}/openthread/examples/platforms
    ${PROJECT_SOURCE_DIR}/src/${RT_PLATFORM}
    ${REALTEK_SDK_INCPATH}
)

add_subdirectory(src/${RT_PLATFORM})
