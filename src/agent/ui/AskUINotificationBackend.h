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
 * @file        AskUINotificationBackend.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @brief       This file declares class for ask user window
 */

#pragma once

#include <atomic>
#include <notification.h>
#include <notification_internal.h>
#include <notification_error.h>
#include <future>
#include <thread>

#include <ui/AskUIInterface.h>

namespace AskUser {

namespace Agent {

class AskUINotificationBackend : public AskUIInterface {
public:
    AskUINotificationBackend();
    virtual ~AskUINotificationBackend();

    virtual bool start(const std::string &client, const std::string &user,
                       const std::string &privilege, RequestId requestId,
                       UIResponseCallback responseCallback);
    virtual bool setOutdated();
    virtual bool dismiss();
    virtual bool isDismissing() const {
        return m_dismissing;
    }

private:
    notification_h m_notification;
    std::thread m_thread;
    RequestId m_requestId;
    UIResponseCallback m_responseCallback;
    static const int m_responseTimeout = 60; // seconds
    std::promise<bool> m_threadFinished;
    std::future<bool> m_future;
    std::atomic<bool> m_dismissing;

    void run();
    bool createUI(const std::string &client, const std::string &user, const std::string &privilege);
};

} // namespace Agent

} // namespace AskUser
