#include <stdio.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>

#include "../config.h"
#include "applet.h"

static DBusHandlerResult signal_filter (DBusConnection *connection, DBusMessage *message, void *user_data);

int yumupdatesd_main (){
	GMainLoop *loop;
	DBusConnection *bus;
	DBusError error;

	loop = g_main_loop_new (NULL, FALSE);

	dbus_error_init (&error);

	bus = dbus_bus_get (DBUS_BUS_SYSTEM, &error);
	if (!bus) {
		g_warning ("Failed to connect to the D-BUS daemon: %s", error.message);
		dbus_error_free (&error);
	return 0;
	}

	dbus_connection_setup_with_g_main (bus, NULL);

	dbus_bus_add_match (bus, "type='signal', interface='edu.duke.linux.yum'", &error);
	dbus_connection_add_filter (bus, signal_filter, loop, NULL);

	g_main_loop_run (loop);

	return 1;
}

static DBusHandlerResult signal_filter (DBusConnection *connection, DBusMessage *message, void *user_data) {
	GMainLoop *loop = user_data;

	if (dbus_message_is_signal (message, "org.freedesktop.Local", "Disconnected")) {
		g_main_loop_quit (loop);
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

		glob_data.pending = atoi(s);

		int tmp_icon = glob_data.icon_status;

		if (glob_data.pending != 0)
			glob_data.icon_status = 1;
		else
			glob_data.icon_status = 0;

		if (tmp_icon != glob_data.icon_status)
			glob_data.flip_icon = 1;

		return DBUS_HANDLER_RESULT_HANDLED;
	}

	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}
