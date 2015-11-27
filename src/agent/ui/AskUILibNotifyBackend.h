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
 * @file        AskUILibNotifyBackend.h
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @brief       This file declares class for ask user libnotify window
 */

#pragma once

#include <ui/AskUIInterface.h>

namespace AskUser {

namespace Agent {

class AskUILibNotifyBackend : public AskUIInterface {
public:
    AskUILibNotifyBackend();
    virtual ~AskUILibNotifyBackend();

    virtual bool start(const std::string &client, const std::string &user,
                       const std::string &privilege, RequestId requestId,
                       UIResponseCallback responseCallback);
    virtual bool setOutdated();
    virtual bool dismiss();
    virtual bool isDismissing() const {
        return m_dismissing;
    }

private:
    bool m_dismissing;
};

} // namespace Agent

} // namespace AskUser
