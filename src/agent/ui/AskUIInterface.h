/*
 *  Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License
 */
/**
 * @file        AskUIInterface.h
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @brief       This file contains ask user UI interface declaration.
 */

#pragma once

#include <functional>
#include <memory>
#include <string>

#include <main/Request.h>

namespace AskUser {

namespace Agent {

typedef enum {
    URT_YES,
    URT_NO,
    URT_SESSION,
    URT_TIMEOUT,
    URT_ERROR
} UIResponseType;

typedef std::function<void(RequestId, UIResponseType)> UIResponseCallback;

class AskUIInterface {
public:
    virtual ~AskUIInterface() {};

    virtual bool start(const std::string &client, const std::string &user,
                       const std::string &privilege, RequestId requestId, UIResponseCallback) = 0;
    virtual bool setOutdated() = 0;
    virtual bool dismiss() = 0;
    virtual bool isDismissing() = 0;
};

typedef std::unique_ptr<AskUIInterface> AskUIInterfacePtr;

} // namespace Agent

} // namespace AskUser
