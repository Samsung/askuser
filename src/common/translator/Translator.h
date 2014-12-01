/*
 *  Copyright (c) 2014 Samsung Electronics Co.
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
 * @file        Translator.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @brief       Definition of Translator methods and TranslateErrorException class
 */

#pragma once

#include <types/RequestData.h>
#include <types/SupportedTypes.h>
#include <cynara-plugin.h>

#include <exception>
#include <string>

namespace AskUser {
namespace Translator {

class TranslateErrorException : std::exception {
public:
    TranslateErrorException(const std::string &msg) : m_what(msg) {};
    virtual const char* what() const noexcept {
        return m_what.c_str();
    }
private:
    std::string m_what;
};

namespace Agent {
    RequestData dataToRequest(const Cynara::PluginData &data);
    Cynara::PluginData answerToData(Cynara::PolicyType answer, const std::string &errMsg);
} // namespace Agent

namespace Plugin {
    Cynara::PolicyType dataToAnswer(const Cynara::PluginData &data);
    Cynara::PluginData requestToData(const std::string &client,
                                     const std::string &user,
                                     const std::string &privilege);
} // namespace Plugin

} // namespace Translator
} // namespace AskUser

