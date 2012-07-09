// Required to compile - WTF?
#define I_KNOW_THE_PACKAGEKIT_GLIB2_API_IS_SUBJECT_TO_CHANGE

#include <gio/gio.h>
#include <packagekit-glib2/packagekit.h>

#include "../config.h"
#include "applet.h"

void callback_ready(GObject *source_object, GAsyncResult *res, gpointer user_data) {
	PkClient *client = PK_CLIENT(source_object);
	GError *error = NULL;
	PkResults *results = NULL;
	GPtrArray *list = NULL;

	results = pk_client_generic_finish(PK_CLIENT(client), res, &error);

	list = pk_results_get_package_array(results);

	glob_data.pending = list->len;

	int tmp_icon = glob_data.icon_status;

	if (glob_data.pending != 0)
		glob_data.icon_status = 1;
	else
		glob_data.icon_status = 0;

	if (tmp_icon != glob_data.icon_status)
		glob_data.flip_icon = 1;

	g_object_unref(results);
	g_ptr_array_unref(list);
}

gboolean plugin_loop(gpointer user_data) {
        PkClient *client = (PkClient*) user_data;

        pk_client_get_updates_async(client, pk_bitfield_value(PK_FILTER_ENUM_NONE), NULL, NULL, NULL, (GAsyncReadyCallback) callback_ready, NULL);

        return TRUE;
}

gboolean packagekit_main() {
	GDBusProxy *proxy = NULL;
	GError *error = NULL;
	GVariant *retval = NULL;
	GMainLoop *loop;
	PkClient *client;
	
	g_type_init();

	loop = g_main_loop_new(NULL, FALSE);

	client = pk_client_new();

	g_timeout_add(REFRESH_TIME, plugin_loop, (gpointer) client);

	g_main_loop_run(loop);

	g_object_unref(client);

	return TRUE;
}

