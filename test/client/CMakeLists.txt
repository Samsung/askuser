# Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
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

PKG_CHECK_MODULES(CLIENT_DEP
    REQUIRED
    cynara-client
    )

SET(CLIENT_PATH ${PROJECT_SOURCE_DIR}/test/client/src)

SET(CLIENT_SOURCES
    ${CLIENT_PATH}/main.c
    )

INCLUDE_DIRECTORIES(
    ${CLIENT_DEP_INCLUDE_DIRS}
    )

ADD_EXECUTABLE(${TARGET_CLIENT} ${CLIENT_SOURCES})

TARGET_LINK_LIBRARIES(${TARGET_CLIENT}
    ${CLIENT_DEP_LIBRARIES}
    cynara-commons
    )

INSTALL(TARGETS ${TARGET_CLIENT} DESTINATION ${BIN_INSTALL_DIR})

