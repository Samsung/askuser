# Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
# @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
#

PKG_CHECK_MODULES(AGENT_DEP
    REQUIRED
    cynara-agent
    cynara-plugin
    notification
    )

SET(ASKUSER_AGENT_PATH ${ASKUSER_PATH}/agent)

SET(ASKUSER_SOURCES
    ${ASKUSER_AGENT_PATH}/log/alog.cpp
    ${ASKUSER_AGENT_PATH}/main/Agent.cpp
    ${ASKUSER_AGENT_PATH}/main/CynaraTalker.cpp
    ${ASKUSER_AGENT_PATH}/main/main.cpp
    ${ASKUSER_AGENT_PATH}/ui/AskUINotificationBackend.cpp
    )

INCLUDE_DIRECTORIES(
    ${ASKUSER_PATH}/common
    ${AGENT_DEP_INCLUDE_DIRS}
    ${SYSTEMD_DEP_INCLUDE_DIRS}
    ${ASKUSER_AGENT_PATH}
    )

ADD_EXECUTABLE(${TARGET_ASKUSER} ${ASKUSER_SOURCES})

TARGET_LINK_LIBRARIES(${TARGET_ASKUSER}
    ${AGENT_DEP_LIBRARIES}
    ${ASKUSER_DEP_LIBRARIES}
    ${SYSTEMD_DEP_LIBRARIES}
    ${TARGET_ASKUSER_COMMON}
    -lcapi-security-privilege-manager
    )

INSTALL(TARGETS ${TARGET_ASKUSER} DESTINATION ${BIN_INSTALL_DIR})

ADD_SUBDIRECTORY(po)
