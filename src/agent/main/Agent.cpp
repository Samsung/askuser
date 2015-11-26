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

#include <chrono>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <string>
#include <unistd.h>
#include <utility>

#include <attributes/attributes.h>
#include <translator/Translator.h>
#include <types/AgentErrorMsg.h>
#include <types/SupportedTypes.h>

#include <log/alog.h>

#ifdef BUILD_WITH_NOTIFICATION
#include <ui/AskUINotificationBackend.h>
#endif

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

    ALOGD("Agent daemon initialized");
}

void Agent::run() {
    m_cynaraTalker.start();

    while (!m_stopFlag) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_event.wait_for(lock, std::chrono::milliseconds(1000));

        if (m_stopFlag) {
            break;
        }

        while (!m_incomingRequests.empty() || !m_incomingResponses.empty()) {

            if (!m_incomingRequests.empty()) {
                Request *request = m_incomingRequests.front();
                m_incomingRequests.pop();
                lock.unlock();

                ALOGD("Request popped from queue:"
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

            if (!m_incomingResponses.empty()) {
                Response response = m_incomingResponses.front();
                m_incomingResponses.pop();
                lock.unlock();

                ALOGD("Response popped from queue:"
                     " type [" << response.type() << "],"
                     " id [" << response.id() << "]");

                processUIResponse(response);

                lock.lock();
            }

            lock.unlock();
            cleanupUIThreads();
            lock.lock();
        }
    }

    ALOGD("Agent task stopped");
}

void Agent::finish() {
    if (!m_cynaraTalker.stop()) {
        ALOGE("Cynara talker thread could not be stopped. Calling quick_exit()");
        quick_exit(EXIT_SUCCESS);
    }

    while (!m_incomingRequests.empty()) {
        Request *request = m_incomingRequests.front();
        m_incomingRequests.pop();
        delete request;
    }

    if (!cleanupUIThreads()) {
        ALOGE("At least one of UI threads could not be stopped. Calling quick_exit()");
        quick_exit(EXIT_SUCCESS);
    }

    for (auto it = m_requests.begin(); it != m_requests.end();) {
        delete it->second;
        it = m_requests.erase(it);
    }

    ALOGD("Agent daemon has stopped commonly");
}

void Agent::requestHandler(Request *request) {
    ALOGD("Cynara request received:"
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
            dismissUI(request->id());
        } else {
            ALOGE("Incoming request with ID: [" << request->id() << "] is being already processed");
        }
        return;
    }

    if (request->type() == RT_Cancel) {
        ALOGE("Cancel request for unknown request: ID: [" << request->id() << "]");
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

void Agent::processUIResponse(const Response &response) {
    auto requestIt = m_requests.find(response.id());
    if (requestIt != m_requests.end()) {
        Cynara::PluginData pluginData;
        if (response.type() == URT_ERROR) {
            pluginData = Translator::Agent::answerToData(Cynara::PolicyType(),
                                                         AgentErrorMsg::Error);
        } else if (response.type() == URT_TIMEOUT) {
            pluginData = Translator::Agent::answerToData(Cynara::PolicyType(),
                                                         AgentErrorMsg::Timeout);
        } else {
            pluginData = Translator::Agent::answerToData(
                                            UIResponseToPolicyType(response.type()),
                                                                   AgentErrorMsg::NoError);
        }
        m_cynaraTalker.sendResponse(RT_Action, requestIt->second->id(), pluginData);
        delete requestIt->second;
        m_requests.erase(requestIt);
    }

    dismissUI(response.id());
}

AskUIInterfacePtr Agent::createBackend() {
#ifdef BUILD_WITH_NOTIFICATION
    return new AskUINotificationBackend();
#else
#error "No suitable backend to use"
#endif
}

bool Agent::startUIForRequest(Request *request) {
    auto data = Translator::Agent::dataToRequest(request->data());
    AskUIInterfacePtr ui(createBackend());

    auto handler = [&](RequestId requestId, UIResponseType resultType) -> void {
                       UIResponseHandler(requestId, resultType);
                   };
    bool ret = ui->start(data.client, data.user, data.privilege, request->id(), handler);
    if (ret) {
        m_UIs.insert(std::make_pair(request->id(), std::move(ui)));
    }

    return ret;
}

void Agent::UIResponseHandler(RequestId requestId, UIResponseType responseType) {
    ALOGD("UI response received: type [" << responseType << "], id [" << requestId << "]");

    std::unique_lock<std::mutex> lock(m_mutex);
    m_incomingResponses.push(Response(requestId, responseType));
    m_event.notify_one();
}

bool Agent::cleanupUIThreads() {
    bool ret = true;
    for (auto it = m_UIs.begin(); it != m_UIs.end();) {
        if (it->second->isDismissing() && it->second->dismiss()) {
            it = m_UIs.erase(it);
        } else {
            ret = false;
            ++it;
        }
    }
    return ret;
}

void Agent::dismissUI(RequestId requestId) {
    auto it = m_UIs.find(requestId);
    if (it != m_UIs.end()) {
        if (it->second->dismiss()) {
            it = m_UIs.erase(it);
        }
    }
}

Cynara::PolicyType Agent::UIResponseToPolicyType(UIResponseType responseType) {
    switch (responseType) {
        case URT_YES_ONCE:
            return AskUser::SupportedTypes::Client::ALLOW_ONCE;
        case URT_YES_SESSION:
            return AskUser::SupportedTypes::Client::ALLOW_PER_SESSION;
        case URT_YES_LIFE:
            return AskUser::SupportedTypes::Client::ALLOW_PER_LIFE;
        case URT_NO_ONCE:
            return AskUser::SupportedTypes::Client::DENY_ONCE;
        case URT_NO_SESSION:
            return AskUser::SupportedTypes::Client::DENY_PER_SESSION;
        case URT_NO_LIFE:
            return AskUser::SupportedTypes::Client::DENY_PER_LIFE;
        default:
            return AskUser::SupportedTypes::Client::DENY_ONCE;
    }
}

} // namespace Agent

} // namespace AskUser
