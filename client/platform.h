/*
   Copyright (C) 2009 Red Hat, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _H_PLATFORM
#define _H_PLATFORM

#include "cursor.h"
#include "process_loop.h"
#include "event_sources.h"
#include "process_loop.h"

class WaveRecordAbstract;
class WavePlaybackAbstract;
class Icon;

class Monitor;
typedef std::list<Monitor*> MonitorsList;

class Platform {
public:
    static void init();
    static void set_process_loop(ProcessLoop& main_process_loop);
    static void msleep(unsigned int millisec);
    static void yield();
    static uint64_t get_monolithic_time();
    static void get_temp_dir(std::string& path);
    static uint64_t get_process_id();
    static uint64_t get_thread_id();

    static const MonitorsList& init_monitors();
    static void destroy_monitors();
    static bool is_monitors_pos_valid();

    static void send_quit_request();

    static void get_spice_config_dir(std::string& path);

    enum ThreadPriority {
        PRIORITY_INVALID,
        PRIORITY_TIME_CRITICAL,
        PRIORITY_HIGH,
        PRIORITY_ABOVE_NORMAL,
        PRIORITY_NORMAL,
        PRIORITY_BELOW_NORMAL,
        PRIORITY_LOW,
        PRIORITY_IDLE,
    };

    static void set_thread_priority(void *thread, ThreadPriority priority);

    class RecordClient;
    static WaveRecordAbstract* create_recorder(RecordClient& client,
                                               uint32_t sampels_per_sec,
                                               uint32_t bits_per_sample,
                                               uint32_t channels);
    static WavePlaybackAbstract* create_player(uint32_t sampels_per_sec,
                                               uint32_t bits_per_sample,
                                               uint32_t channels);

    enum {
        SCROLL_LOCK_MODIFIER_SHIFT,
        NUM_LOCK_MODIFIER_SHIFT,
        CAPS_LOCK_MODIFIER_SHIFT,

        SCROLL_LOCK_MODIFIER = (1 << SCROLL_LOCK_MODIFIER_SHIFT),
        NUM_LOCK_MODIFIER = (1 << NUM_LOCK_MODIFIER_SHIFT),
        CAPS_LOCK_MODIFIER = (1 << CAPS_LOCK_MODIFIER_SHIFT),
    };

    static uint32_t get_keyboard_lock_modifiers();
    static void set_keyboard_lock_modifiers(uint32_t modifiers);

    enum {
        L_SHIFT_MODIFIER_SHIFT,
        R_SHIFT_MODIFIER_SHIFT,
        L_CTRL_MODIFIER_SHIFT,
        R_CTRL_MODIFIER_SHIFT,
        L_ALT_MODIFIER_SHIFT,
        R_ALT_MODIFIER_SHIFT,

        L_SHIFT_MODIFIER = (1 << L_SHIFT_MODIFIER_SHIFT),
        R_SHIFT_MODIFIER = (1 << R_SHIFT_MODIFIER_SHIFT),
        L_CTRL_MODIFIER = (1 << L_CTRL_MODIFIER_SHIFT),
        R_CTRL_MODIFIER = (1 << R_CTRL_MODIFIER_SHIFT),
        L_ALT_MODIFIER = (1 << L_ALT_MODIFIER_SHIFT),
        R_ALT_MODIFIER = (1 << R_ALT_MODIFIER_SHIFT),
    };

    static uint32_t get_keyboard_modifiers();

    static void reset_cursor_pos();

    static LocalCursor* create_local_cursor(CursorData* cursor_data);
    static LocalCursor* create_inactive_cursor();
    static LocalCursor* create_default_cursor();

    static Icon* load_icon(int id);

    class EventListener;
    static void set_event_listener(EventListener* listener);

    class DisplayModeListner;
    static void set_display_mode_listner(DisplayModeListner* listener);
};

class Platform::EventListener {
public:
    virtual ~EventListener() {}
    virtual void on_app_activated() = 0;
    virtual void on_app_deactivated() = 0;
    virtual void on_monitors_change() = 0;
};

class Platform::RecordClient {
public:
    virtual ~RecordClient() {}
    virtual void add_event_source(EventSources::File& evnet_source) = 0;
    virtual void remove_event_source(EventSources::File& evnet_source) = 0;
    virtual void add_event_source(EventSources::Trigger& evnet_source) = 0;
    virtual void remove_event_source(EventSources::Trigger& evnet_source) = 0;
    virtual void push_frame(uint8_t *frame) = 0;
};

class Platform::DisplayModeListner {
public:
    virtual ~DisplayModeListner() {}
    virtual void on_display_mode_change() = 0;
};

class NamedPipe {
public:
    typedef unsigned long ListenerRef;
    typedef unsigned long ConnectionRef;
    static const ConnectionRef INVALID_CONNECTION = ~0;

    class ConnectionInterface {
    public:
        ConnectionInterface() : _opaque (INVALID_CONNECTION) {}
        virtual ~ConnectionInterface() {}
        virtual void bind(ConnectionRef conn_ref) = 0;
        virtual void on_data() = 0;

    protected:
        ConnectionRef _opaque;
    };

    class ListenerInterface {
    public:
        virtual ~ListenerInterface() {}
        virtual ConnectionInterface &create() = 0;
    };

    static ListenerRef create(const char *name, ListenerInterface& listener_interface);
    static void destroy(ListenerRef listener_ref);
    static void destroy_connection(ConnectionRef conn_ref);
    static int32_t read(ConnectionRef conn_ref, uint8_t *buf, int32_t size);
    static int32_t write(ConnectionRef conn_ref, const uint8_t *buf, int32_t size);
};

#endif

