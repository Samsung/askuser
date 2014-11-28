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
 * @file        Request.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @brief       This file declares class representing request from cynara service
 */

#pragma once

#include <cstdlib>
#include <vector>

#include <cynara-agent.h>
#include <cynara-plugin.h>

namespace AskUser {

namespace Agent {

typedef enum {
    RT_Action,
    RT_Cancel,
    RT_Close
} RequestType;

typedef cynara_agent_req_id RequestId;

class Request {
public:
    Request() = default;
    Request(RequestType type, RequestId id, void *data, std::size_t dataSize)
        : m_type(type), m_id(id), m_data(static_cast<char *>(data), dataSize) {}
    ~Request() {}

    RequestType type() const {
        return m_type;
    }

    RequestId id() const {
        return m_id;
    }

    const Cynara::PluginData &data() const {
        return m_data;
    }

private:
    RequestType m_type;
    RequestId m_id;
    Cynara::PluginData m_data;
};

} // namespace Agent

} // namespace AskUser
