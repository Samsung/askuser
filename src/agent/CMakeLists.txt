# Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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

SET(ASKUSER_AGENT_PATH ${ASKUSER_PATH}/agent)

SET(ASKUSER_SOURCES
    ${ASKUSER_AGENT_PATH}/main/main.cpp
    )

INCLUDE_DIRECTORIES(
    ${ASKUSER_PATH}/common
    ${ASKUSER_AGENT_PATH}
    )

ADD_EXECUTABLE(${TARGET_ASKUSER} ${ASKUSER_SOURCES})

TARGET_LINK_LIBRARIES(${TARGET_ASKUSER}
    ${ASKUSER_DEP_LIBRARIES}
    ${TARGET_ASKUSER_COMMON}
    )

INSTALL(TARGETS ${TARGET_ASKUSER} DESTINATION ${BIN_INSTALL_DIR})
