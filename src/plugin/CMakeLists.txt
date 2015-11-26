# Copyright (c) 2014 Samsung Electronics Co.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#
# @file        CMakeLists.txt
# @author      Zofia Abramowska <z.abramowska@samsung.com>
#

SET(PLUGIN_PATH ${ASKUSER_PATH}/plugin)

PKG_CHECK_MODULES(SERVICE_DEP
    REQUIRED
    cynara-plugin
    )

INCLUDE_DIRECTORIES(
    ${SERVICE_DEP_INCLUDE_DIRS}
    ${ASKUSER_PATH}/common
    ${PLUGIN_PATH}/service
    )

SET(SERVICE_PLUGIN_SOURCES
    ${PLUGIN_PATH}/service/ServicePlugin.cpp
    )

SET(CLIENT_PLUGIN_SOURCES
    ${PLUGIN_PATH}/client/ClientPlugin.cpp
    )

ADD_DEFINITIONS("-fvisibility=default")

ADD_LIBRARY(${TARGET_PLUGIN_SERVICE} SHARED ${SERVICE_PLUGIN_SOURCES})
ADD_LIBRARY(${TARGET_PLUGIN_CLIENT} SHARED ${CLIENT_PLUGIN_SOURCES})

TARGET_LINK_LIBRARIES(${TARGET_PLUGIN_SERVICE}
    ${TARGET_ASKUSER_COMMON}
    ${TARGET_ASKUSER_COMMON_DEPS}
    )
TARGET_LINK_LIBRARIES(${TARGET_PLUGIN_CLIENT}
    ${TARGET_ASKUSER_COMMON}
    ${TARGET_ASKUSER_COMMON_DEPS}
    )

INSTALL(TARGETS ${TARGET_PLUGIN_SERVICE}
    DESTINATION ${LIB_DIR}/cynara/plugin/service/)
INSTALL(TARGETS ${TARGET_PLUGIN_CLIENT}
    DESTINATION ${LIB_DIR}/cynara/plugin/client/)
