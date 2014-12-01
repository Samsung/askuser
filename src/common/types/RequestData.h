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
 * @file        RequestData.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @brief       Definition of RequestData structure
 */

#pragma once

#include <string>

namespace AskUser {

struct RequestData {
    std::string client;
    std::string user;
    std::string privilege;
};

} // namespace AskUser
