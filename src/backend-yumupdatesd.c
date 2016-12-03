/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 *  USA.
 *
 *  MATE Software Update applet written by Assen Totin <assen.totin@gmail.com>
 *  
 */

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

gboolean yumupdatesd_main (softupd_applet *applet){
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
        dbus_connection_add_filter (bus, (DBusHandleMessageFunction)signal_filter, (void *)applet, NULL);

        g_main_loop_run(applet->loop);

        return TRUE;
}

