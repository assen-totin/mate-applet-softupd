#include <stdio.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>

#include "../config.h"
#include "applet.h"

static DBusHandlerResult signal_filter (DBusConnection *connection, DBusMessage *message, softupd_applet *applet) {
	if (dbus_message_is_signal (message, "org.freedesktop.Local", "Disconnected")) {
		g_main_loop_quit (applet->loop);
		return DBUS_HANDLER_RESULT_HANDLED;
	}

	else if ((dbus_message_is_signal(message, "edu.duke.linux.yum", "NoUpdatesAvailableSignal")) || 
	(dbus_message_is_signal(message, "edu.duke.linux.yum", "UpdatesAvailableSignal"))) {
		DBusError error;
		char *s;
		int i;
		dbus_error_init (&error);
		dbus_message_get_args (message, &error, DBUS_TYPE_STRING, &s, DBUS_TYPE_INVALID); 
		g_print("Updates available: %s\n", s);
		//dbus_free (s);

		applet->pending = atoi(s);

		int tmp_icon = applet->icon_status;

		if (applet->pending != 0)
			applet->icon_status = 1;
		else
			applet->icon_status = 0;

		if (tmp_icon != applet->icon_status)
			applet->flip_icon = 1;

		return DBUS_HANDLER_RESULT_HANDLED;
	}

	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

int yumupdatesd_main (softupd_applet *applet){
        DBusConnection *bus;
        DBusError error;

        applet->loop = g_main_loop_new (NULL, FALSE);

        dbus_error_init (&error);

        bus = dbus_bus_get (DBUS_BUS_SYSTEM, &error);
        if (!bus) {
                g_warning ("Failed to connect to the D-BUS daemon: %s", error.message);
                dbus_error_free (&error);
        return 0;
        }

        dbus_connection_setup_with_g_main (bus, NULL);

        dbus_bus_add_match (bus, "type='signal', interface='edu.duke.linux.yum'", &error);
        dbus_connection_add_filter (bus, signal_filter, loop, (gpointer)applet);

        g_main_loop_run (loop);

        return 1;
}

