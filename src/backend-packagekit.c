// Required to compile - WTF?
#define I_KNOW_THE_PACKAGEKIT_GLIB2_API_IS_SUBJECT_TO_CHANGE

#include <gio/gio.h>
#include <packagekit-glib2/packagekit.h>

#include "../config.h"
#include "applet.h"

#include <stdio.h>

int glob_flag = 0;

void callback_ready(GObject *source_object, GAsyncResult *res, gpointer user_data) {
	PkClient *client = PK_CLIENT(source_object);
	GError *error = NULL;
	PkResults *results = NULL;
	GPtrArray *list = NULL;

	results = pk_client_generic_finish(PK_CLIENT(client), res, &error);

	list = pk_results_get_package_array(results);

	if (list != NULL) {
		glob_data.pending = list->len;
		g_ptr_array_unref(list);
	}
	else
		glob_data.pending = 0;

	int tmp_icon = glob_data.icon_status;

	if (glob_data.pending != 0)
		glob_data.icon_status = 1;
	else
		glob_data.icon_status = 0;

	if (tmp_icon != glob_data.icon_status)
		glob_data.flip_icon = 1;

	if (results != NULL)
		g_object_unref(results);

	glob_flag = 0;
}

gboolean plugin_loop(gpointer user_data) {
        PkClient *client = pk_client_new();

	glob_flag = 1;

        pk_client_get_updates_async(client, pk_bitfield_value(PK_FILTER_ENUM_NONE), NULL, NULL, NULL, (GAsyncReadyCallback) callback_ready, NULL);

	// Wait until the async callback completes
	while (glob_flag == 1)
		sleep(10);

	if (client != NULL)
		g_object_unref(client);

        return TRUE;
}

gboolean packagekit_main() {
	GDBusProxy *proxy = NULL;
	GError *error = NULL;
	GVariant *retval = NULL;
	GMainLoop *loop;
	
	g_type_init();

	loop = g_main_loop_new(NULL, FALSE);

	g_timeout_add(REFRESH_TIME, plugin_loop, NULL);

	g_main_loop_run(loop);

	//g_object_unref(client);

	return TRUE;
}

