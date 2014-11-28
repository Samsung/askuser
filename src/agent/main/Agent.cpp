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

#include <log/log.h>

#include "Agent.h"

namespace AskUser {

namespace Agent {

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

    // TODO: wait for requests

    while (true) {
        sleep(1);
    }

    LOGD("Ask user agent task stopped");
}

void Agent::finish() {
    // TODO: implement if needed

    LOGD("Agent daemon has stopped commonly");
}

void Agent::requestHandler(Request *request) {
    LOGD("Cynara request received:"
         " type [" << request->type() << "],"
         " id [" << request->id() << "],"
         " data length: [" << request->data().size() << "]");

    delete request;
}

} // namespace Agent

} // namespace AskUser
