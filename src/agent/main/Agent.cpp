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
 * @file        Agent.cpp
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @brief       This file implements main class of ask user agent
 */

#include <unistd.h>
#include <utility>

#include <attributes/attributes.h>
#include <log/log.h>
#include <translator/Translator.h>
#include <types/AgentErrorMsg.h>
#include <types/SupportedTypes.h>

#include "Agent.h"

namespace AskUser {

namespace Agent {

volatile sig_atomic_t Agent::m_stopFlag = 0;

Agent::Agent() : m_cynaraTalker([&](Request *request) -> void { requestHandler(request); }) {
    init();
}

Agent::~Agent() {
    finish();
}

void Agent::init() {
    // TODO: implement if needed

    LOGD("Agent daemon initialized");
}

void Agent::run() {
    m_cynaraTalker.start();

    while (!m_stopFlag) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_event.wait(lock);

        while (!m_incomingRequests.empty()) {
            Request *request = m_incomingRequests.front();
            m_incomingRequests.pop();
            lock.unlock();

            LOGD("Request popped from queue:"
                 " type [" << request->type() << "],"
                 " id [" << request->id() << "],"
                 " data length [" << request->data().size() << "]");

            if (request->type() == RT_Close) {
                delete request;
                m_stopFlag = 1;
                break;
            }

            processCynaraRequest(request);

            lock.lock();
        }

        //TODO: do sth here with available data from UIs
    }

    //TODO: dismiss all threads if possible

    LOGD("Agent task stopped");
}

void Agent::finish() {
    m_cynaraTalker.stop();

    LOGD("Agent daemon has stopped commonly");
}

void Agent::requestHandler(Request *request) {
    LOGD("Cynara request received:"
         " type [" << request->type() << "],"
         " id [" << request->id() << "],"
         " data length: [" << request->data().size() << "]");

    std::unique_lock<std::mutex> lock(m_mutex);
    m_incomingRequests.push(request);
    m_event.notify_one();
}

void Agent::processCynaraRequest(Request *request) {
    std::unique_ptr<Request> requestPtr(request);

    auto existingRequest = m_requests.find(request->id());
    if (existingRequest != m_requests.end()) {
        if (request->type() == RT_Cancel) {
            delete existingRequest->second;
            m_requests.erase(existingRequest);
            m_cynaraTalker.sendResponse(request->type(), request->id());
            //TODO: get UI for request and dismiss or update it
        } else {
            LOGE("Incoming request with ID: [" << request->id() << "] is being already processed");
        }
        return;
    }

    if (request->type() == RT_Cancel) {
        LOGE("Cancel request for unknown request: ID: [" << request->id() << "]");
        return;
    }

    if (!startUIForRequest(request)) {
        auto data = Translator::Agent::answerToData(Cynara::PolicyType(), AgentErrorMsg::Error);
        m_cynaraTalker.sendResponse(RT_Action, request->id(), data);
        return;
    }

    m_requests.insert(std::make_pair(request->id(), request));
    requestPtr.release();
}

bool Agent::startUIForRequest(Request *request UNUSED) {
    // TODO: start UI for request
    return false;
}

} // namespace Agent

} // namespace AskUser
