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
 * @file        AskUINotificationBackend.cpp
 * @author      Adam Malinowski <a.malinowsk2@partner.samsung.com>
 * @brief       This file implements class for ask user window
 */

#include <bundle.h>
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <libintl.h>
#include <privilegemgr/privilege_info.h>

#include <attributes/attributes.h>
#include <log/log.h>

#include "AskUINotificationBackend.h"

namespace {

const char *errorToString(notification_error_e error) {
    if (error == NOTIFICATION_ERROR_INVALID_DATA)
        return "NOTIFICATION_ERROR_INVALID_DATA";
    if (error == NOTIFICATION_ERROR_NO_MEMORY)
        return "NOTIFICATION_ERROR_NO_MEMORY";
    if (error == NOTIFICATION_ERROR_FROM_DB)
        return "NOTIFICATION_ERROR_FROM_DB";
    if (error == NOTIFICATION_ERROR_ALREADY_EXIST_ID)
        return "NOTIFICATION_ERROR_ALREADY_EXIST_ID";
    if (error == NOTIFICATION_ERROR_FROM_DBUS)
        return "NOTIFICATION_ERROR_FROM_DBUS";
    if (error == NOTIFICATION_ERROR_NOT_EXIST_ID)
        return "NOTIFICATION_ERROR_NOT_EXIST_ID";
    if (error == NOTIFICATION_ERROR_IO)
        return "NOTIFICATION_ERROR_IO";
    if (error == NOTIFICATION_ERROR_SERVICE_NOT_READY)
        return "NOTIFICATION_ERROR_SERVICE_NOT_READY";
    if (error == NOTIFICATION_ERROR_NONE)
        return "NOTIFICATION_ERROR_NONE";

    return "UNHANDLED ERROR";
}

}

namespace AskUser {

namespace Agent {

AskUINotificationBackend::AskUINotificationBackend() : m_notification(nullptr),
                                                       m_dismissing(false) {
    m_future = m_threadFinished.get_future();
}

AskUINotificationBackend::~AskUINotificationBackend() {
    notification_free(m_notification);
}

bool AskUINotificationBackend::start(const std::string &client, const std::string &user,
                                     const std::string &privilege, RequestId requestId,
                                     UIResponseCallback responseCallback) {
    if (!responseCallback) {
        LOGE("Empty response callback is not allowed");
        return false;
    }

    if (!createUI(client, user, privilege)) {
        LOGE("UI window for request could not be created!");
        return false;
    }

    m_requestId = requestId;
    m_responseCallback = responseCallback;
    m_thread = std::thread(&AskUINotificationBackend::run, this);
    return true;
}

bool AskUINotificationBackend::createUI(const std::string &client, const std::string &user,
                                        const std::string &privilege) {
    notification_error_e err;

    m_notification = notification_new(NOTIFICATION_TYPE_NOTI, NOTIFICATION_GROUP_ID_NONE,
                                      NOTIFICATION_PRIV_ID_NONE);
    if (m_notification == nullptr) {
        LOGE("Failed to create notification.");
        return false;
    }

    err = notification_set_pkgname(m_notification, "cynara-askuser");
    if (err != NOTIFICATION_ERROR_NONE) {
        LOGE("Unable to set notification pkgname: <" << errorToString(err) << ">");
        return false;
    }

    char *dialogTitle = dgettext(PROJECT_NAME, "SID_PRIVILEGE_REQUEST_DIALOG_TITLE");
    err = notification_set_text(m_notification, NOTIFICATION_TEXT_TYPE_TITLE, dialogTitle, nullptr,
                                NOTIFICATION_VARIABLE_TYPE_NONE);
    if (err != NOTIFICATION_ERROR_NONE) {
        LOGE("Unable to set notification title: <" << errorToString(err) << ">");
        return false;
    }

    char *messageFormat = dgettext(PROJECT_NAME, "SID_PRIVILEGE_REQUEST_DIALOG_MESSAGE");
    char *privilegeDisplayName;
    int ret = privilege_info_get_privilege_display_name(privilege.c_str(), &privilegeDisplayName);
    if (ret != PRVMGR_ERR_NONE) {
        LOGE("Unable to get privilege display name, err: [" << ret << "]");
        privilegeDisplayName = strdup(privilege.c_str());
    }
    LOGD("privilege_info_get_privilege_display_name: [" << ret << "],"
         " <" << privilegeDisplayName << ">");

    char tmpBuffer[BUFSIZ];
    ret = std::snprintf(tmpBuffer, sizeof(tmpBuffer), messageFormat, client.c_str(), user.c_str(),
                   privilegeDisplayName);
    free(privilegeDisplayName);
    if (ret < 0) {
        int erryes = errno;
        LOGE("sprintf failed with error: <" << strerror(erryes) << ">");
        return false;
    }

    err = notification_set_text(m_notification, NOTIFICATION_TEXT_TYPE_CONTENT, tmpBuffer, nullptr,
                                NOTIFICATION_VARIABLE_TYPE_NONE);
    if (err != NOTIFICATION_ERROR_NONE) {
        LOGE("Unable to set notification content: <" << errorToString(err) << ">");
        return false;
    }

    ret = std::snprintf(tmpBuffer, sizeof(tmpBuffer), "%s,%s,%s",
                   dgettext(PROJECT_NAME, "SID_PRIVILEGE_REQUEST_DIALOG_BUTTON_NO"),
                   dgettext(PROJECT_NAME, "SID_PRIVILEGE_REQUEST_DIALOG_BUTTON_YES"),
                   dgettext(PROJECT_NAME, "SID_PRIVILEGE_REQUEST_DIALOG_BUTTON_YES_FOR_SESSION"));
    if (ret < 0) {
        int erryes = errno;
        LOGE("sprintf failed with error: <" << strerror(erryes) << ">");
        return false;
    }

    bundle *b = bundle_create();
    if (!b) {
        int erryes = errno;
        LOGE("Unable to create bundle: <" << strerror(erryes) << ">");
        return false;
    }

    if (bundle_add(b, "buttons", tmpBuffer)) {
        int erryes = errno;
        LOGE("Unable to add button to bundle: <" << strerror(erryes) << ">");
        bundle_free(b);
        return false;
    }

    err = notification_set_execute_option(m_notification, NOTIFICATION_EXECUTE_TYPE_RESPONDING,
                                          nullptr, nullptr, b);
    if (err != NOTIFICATION_ERROR_NONE) {
        LOGE("Unable to set execute option: <" << errorToString(err) << ">");
        bundle_free(b);
        return false;
    }

    bundle_free(b);

    err = notification_insert(m_notification, nullptr);
    if (err != NOTIFICATION_ERROR_NONE) {
        LOGE("Unable to insert notification: <" << errorToString(err) << ">");
        return false;
    }

    return true;
}

bool AskUINotificationBackend::setOutdated() {
    // There is no possibility to update window using notifications framework - at least for now
    return true;
}

bool AskUINotificationBackend::dismiss() {
    // There is no possibility to dismiss window using notifications framework
    // We can only try to get rid of thread
    m_dismissing = true;
    auto status = m_future.wait_for(std::chrono::milliseconds(10));
    if (status == std::future_status::ready) {
        LOGD("UI thread, for request: [" << m_requestId << "], finished and ready to join.");
        m_thread.join();
        return true;
    }

    LOGD("UI thread, for request: [" << m_requestId << "], not finished.");
    return false;
}

void AskUINotificationBackend::run() {
    int ret;
    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGTERM);
    if ((ret = sigprocmask(SIG_BLOCK, &mask, nullptr)) < 0) {
        LOGE("sigprocmask failed [<<" << ret << "]");
    }

    try {
        int buttonClicked = 0;
        notification_error_e ret = notification_wait_response(m_notification, m_responseTimeout,
                                                              &buttonClicked, nullptr);
        LOGD("notification_wait_response finished with ret code: [" << ret << "]");

        UIResponseType response = URT_ERROR;
        if (ret == NOTIFICATION_ERROR_NONE) {
            if (buttonClicked) {
                static UIResponseType responseType[] = {URT_NO, URT_YES, URT_SESSION};
                LOGD("Got response from user: [" << buttonClicked << "]");
                if (static_cast<unsigned int>(buttonClicked) >
                                             sizeof(responseType) / sizeof(responseType[0])) {
                    LOGE("Wrong code of response: [" << buttonClicked << "]");
                    response = URT_NO;
                } else {
                    response = responseType[buttonClicked - 1];
                }
            } else {
                LOGD("notification_wait_response, for request ID: [" << m_requestId <<
                     "] timeouted");
                response = URT_TIMEOUT;
            }
        }
        m_responseCallback(m_requestId, response);
        LOGD("UI thread for request ID: [" << m_requestId << "] stopped execution");
    } catch (const std::exception &e) {
        LOGC("Unexpected exception: <" << e.what() << ">");
    } catch (...) {
        LOGE("Unexpected unknown exception caught!");
    }
    m_threadFinished.set_value(true);
}

} // namespace Agent

} // namespace AskUser
