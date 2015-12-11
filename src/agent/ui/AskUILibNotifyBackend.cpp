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
 * @file        AskUILibNotifyBackend.cpp
 * @author      Lukasz Wojciechowski <l.wojciechow@partner.samsung.com>
 * @brief       This file implements class for ask user libnotify window
 */

#include <map>
#include <string>

#include <log/alog.h>

#include "AskUILibNotifyBackend.h"

namespace AskUser {

namespace Agent {

namespace {
    std::map<std::string, std::pair<UIResponseType, std::string> > s_options = {
        {"1", {URT_NO_ONCE,	"Deny once"}},
        {"2", {URT_NO_SESSION,	"Deny for session"}},
        {"3", {URT_NO_LIFE,	"Deny"}},
        {"4", {URT_YES_ONCE,	"Allow once"}},
        {"5", {URT_YES_SESSION,	"Allow for session"}},
        {"6", {URT_YES_LIFE,	"Allow"}},
    };
} //namespace anonymous

AskUILibNotifyBackend::AskUILibNotifyBackend() : m_dismissing(false), m_note(nullptr) {
    m_future = m_threadFinished.get_future();
}

AskUILibNotifyBackend::~AskUILibNotifyBackend() {
    if (m_note)
        g_object_unref(G_OBJECT((NotifyNotification*)(m_note)));
    notify_uninit();
}

bool AskUILibNotifyBackend::start(const std::string &client, const std::string &user,
                                  const std::string &privilege, RequestId requestId,
                                  UIResponseCallback responseCallback) {
    if (!responseCallback) {
        ALOGE("Empty response callback is not allowed");
        return false;
    }

    if (!createUI(client, user, privilege)) {
        ALOGE("UI window for request could not be created!");
        return false;
    }

    m_requestId = requestId;
    m_responseCallback = responseCallback;
    m_thread = std::thread(&AskUILibNotifyBackend::run, this);
    return true;
}

bool AskUILibNotifyBackend::createUI(const std::string &client, const std::string &user,
                                     const std::string &privilege) {
    DBusConnection *conn;
    conn = dbus_bus_get(DBUS_BUS_SESSION, NULL);
    dbus_connection_setup_with_g_main(conn, NULL);

    m_loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_ref(m_loop);

    if (!notify_init("AskUser")) {
        ALOGE("Failed to init libnotify.");
        return false;
    }

    std::string summary = std::string("Application: ") + client + " run by user: "
        + user + " requested an access to privilege: " + privilege;
    m_note = notify_notification_new(summary.c_str(), "Do You want to grant the access ?", "");
    if (!m_note) {
        ALOGE("Failed to create new notification.");
        return false;
    }

    notify_notification_set_timeout(m_note, NOTIFY_EXPIRES_NEVER);
    notify_notification_set_hint_int32(m_note, "resident", 1);
    notify_notification_set_hint_int32(m_note, "transient", 1);
    for (const auto &opt : s_options) {
        notify_notification_add_action(m_note, opt.first.c_str(), opt.second.second.c_str(),
        AskUILibNotifyBackend::onAction, this, NULL);
    }

    notify_notification_add_action(m_note, "default", "OK", AskUILibNotifyBackend::onAction,
        this, NULL);
    g_signal_connect(m_note, "closed", G_CALLBACK(AskUILibNotifyBackend::onClose), gpointer(this));
    return true;
}

bool AskUILibNotifyBackend::setOutdated() {
    //Not implemented
    return true;
}

bool AskUILibNotifyBackend::dismiss() {
    m_dismissing = true;
    auto status = m_future.wait_for(std::chrono::milliseconds(10));
    if (status == std::future_status::ready) {
        ALOGD("UI thread, for request: [" << m_requestId << "], finished and ready to join.");
        m_thread.join();
        return true;
    }

    ALOGD("UI thread, for request: [" << m_requestId << "], not finished.");
    return false;
}

void AskUILibNotifyBackend::run() {
    int ret;
    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGTERM);
    if ((ret = sigprocmask(SIG_BLOCK, &mask, nullptr)) < 0) {
        ALOGE("sigprocmask failed [<<" << ret << "]");
    }

    try {
        GError* err = NULL;
        m_response = URT_ERROR;
        if (!notify_notification_show(m_note, &err)) {
            ALOGE("Cannot show notification window");
        } else {
            g_main_loop_run(m_loop);
        }

        m_responseCallback(m_requestId, m_response);
        ALOGD("UI thread for request ID: [" << m_requestId << "] stopped execution");
    } catch (const std::exception &e) {
        ALOGE("Unexpected exception: <" << e.what() << ">");
    } catch (...) {
        ALOGE("Unexpected unknown exception caught!");
    }
    m_threadFinished.set_value(true);
}

void AskUILibNotifyBackend::onAction(NotifyNotification *note, char *action, gpointer data) {
    AskUILibNotifyBackend *self = (AskUILibNotifyBackend*)(data);
    if (self->m_note != note)
        return;
    auto it = s_options.find(std::string(action));
    self->m_response = URT_NO_ONCE;
    if (it != s_options.end()) {
        self->m_response = it->second.first;
    } else if (std::string(action) == "default") {
        self->m_response = URT_YES_ONCE;
    }
    ALOGD("onAction > action: " << action << " ; response = " << self->m_response);
    g_main_loop_quit(self->m_loop);
}

void AskUILibNotifyBackend::onClose(NotifyNotification *note, gpointer data) {
    AskUILibNotifyBackend *self = (AskUILibNotifyBackend*)(data);
    if (self->m_note != note)
        return;
    self->m_response = URT_NO_ONCE;
    ALOGD("onClose > response = " << self->m_response);
    g_main_loop_quit(self->m_loop);
}

} // namespace Agent

} // namespace AskUser
