/*
 * Copyright (c) 2014-2015 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        Agent.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @brief       This file defines main class of ask user agent
 */

#pragma once

#include <condition_variable>
#include <csignal>
#include <map>
#include <mutex>
#include <queue>

#include <main/CynaraTalker.h>
#include <main/Request.h>

namespace AskUser {

namespace Agent {

class Agent {
public:
    Agent();
    ~Agent();

    void run();

    static void stop() {
        m_stopFlag = 1;
    }

private:
    CynaraTalker m_cynaraTalker;
    std::map<RequestId, Request *> m_requests;
    std::queue<Request *> m_incomingRequests;
    std::condition_variable m_event;
    std::mutex m_mutex;
    static volatile sig_atomic_t m_stopFlag;

    void init();
    void finish();

    void requestHandler(Request *request);
    void processCynaraRequest(Request *request);
    bool startUIForRequest(Request *request);
};

} // namespace Agent

} // namespace AskUser
