/*
 *  Copyright (c) 2015 Samsung Electronics Co.
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
 * @file        PolicyResultSerialization.h
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @brief       Definition of Cynara::PolicyResult serialization to stream.
 */

#include <ostream>
#include <types/PolicyResult.h>

std::ostream &operator<<(std::ostream &os, const Cynara::PolicyResult &result) {
    os << "type: " << result.policyType()
       << ", metadata: " << result.metadata();
    return os;
}

