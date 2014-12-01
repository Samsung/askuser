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
 * @file        SupportedTypes.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @brief       Definition of plugin supported types and agent type
 */

#pragma once

#include <types/PolicyType.h>

namespace AskUser {
namespace SupportedTypes {

namespace Agent {
const char* const AgentType = "AskUser";
} //namespace Agent

namespace Service {
const Cynara::PolicyType ASK_USER = 10;
} //namespace Service

namespace Client {
const Cynara::PolicyType ALLOW_ONCE = 11;
const Cynara::PolicyType ALLOW_PER_SESSION = 12;
// This one will never reach client, but will be interpreted in service plugin
const Cynara::PolicyType ALLOW_PER_LIFE = 13;
} //namespace Client

} //namespace SupportedTypes
} //namespace AskUser
