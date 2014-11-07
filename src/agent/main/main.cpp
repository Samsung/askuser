/*
 *  Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        src/main/main.cpp
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @brief       Main ask user daemon file
 */

#include <cstdlib>
#include <unistd.h>

#include <systemd/sd-journal.h>
#include <systemd/sd-daemon.h>

#include <attributes/attributes.h>
#include <log/log.h>

int main(int argc UNUSED, char **argv UNUSED) {
    init_log();

    int ret = sd_notify(0, "READY=1");
    if (ret == 0) {
        LOGW("Ask user agent was not configured to notify its status");
    } else if (ret < 0) {
        LOGE("sd_notify failed: [" << ret << "]");
    }

    while (true) {
        sleep(1);
    }

    return EXIT_SUCCESS;
}
