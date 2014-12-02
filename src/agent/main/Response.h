/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/**
 * @file        Response.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @brief       This file declares class representing response from user interface
 */

#pragma once

#include <main/Request.h>
#include <ui/AskUIInterface.h>

namespace AskUser {

namespace Agent {

class Response {
public:
    Response() = default;
    Response(RequestId requestId, UIResponseType responseType) : m_id(requestId),
                                                                 m_type(responseType) {}
    ~Response() {}

    RequestId id() const {
        return m_id;
    }

    UIResponseType type() const {
        return m_type;
    }

private:
    RequestId m_id;
    UIResponseType m_type;
};

} // namespace Agent

} // namespace AskUser
