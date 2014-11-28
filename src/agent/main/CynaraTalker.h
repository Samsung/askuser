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
 * @file        CynaraTalker.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @brief       This file declares class representing request from cynara service
 */

#pragma once

#include <functional>
#include <future>
#include <mutex>
#include <thread>

#include <cynara-agent.h>
#include <cynara-plugin.h>

#include <main/Request.h>

namespace AskUser {

namespace Agent {

typedef std::function<void(Request *)> RequestHandler;

class CynaraTalker {
public:
    CynaraTalker(RequestHandler requestHandler);
    ~CynaraTalker() {}

    bool start();
    bool stop();

    bool sendResponse(RequestType requestType, RequestId requestId, const Cynara::PluginData &data);

private:
    RequestHandler m_requestHandler;
    cynara_agent *m_cynara;
    std::thread m_thread;
    std::mutex m_mutex;
    std::promise<bool> m_threadFinished;
    std::future<bool> m_future;

    void run();
};

} // namespace Agent

} // namespace AskUser
