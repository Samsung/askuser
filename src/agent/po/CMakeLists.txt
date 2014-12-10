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

SET(POFILES
    en.po pl.po
)

SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(SHAREDIR "${PREFIX}/share")
SET(LOCALEDIR "${SHAREDIR}/locale")
SET(MSGFMT "/usr/bin/msgfmt")

FOREACH(pofile ${POFILES})
    SET(pofile ${CMAKE_CURRENT_SOURCE_DIR}/${pofile})
    MESSAGE("PO: ${pofile}")
    GET_FILENAME_COMPONENT(absPofile ${pofile} ABSOLUTE)
    GET_FILENAME_COMPONENT(lang ${absPofile} NAME_WE)
    SET(moFile ${CMAKE_CURRENT_BINARY_DIR}/${lang}.mo)
    ADD_CUSTOM_COMMAND(
        OUTPUT ${moFile}
        COMMAND ${MSGFMT} -o ${moFile} ${absPofile}
        DEPENDS ${absPofile}
    )
    INSTALL(FILES ${moFile}
        DESTINATION ${LOCALEDIR}/${lang}/LC_MESSAGES RENAME ${PROJECT_NAME}.mo)
    SET(moFiles ${moFiles} ${moFile})
ENDFOREACH(pofile)

ADD_CUSTOM_TARGET(po ALL DEPENDS ${moFiles})
