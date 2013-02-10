// Required to compile - WTF?
#define I_KNOW_THE_PACKAGEKIT_GLIB2_API_IS_SUBJECT_TO_CHANGE

#include <gio/gio.h>
#include <packagekit-glib2/packagekit.h>

#include "../config.h"
#include "applet.h"

#include <stdio.h>

void callback_ready(GObject *source_object, GAsyncResult *res, softupd_applet *applet) {
	PkClient *client = PK_CLIENT(source_object);
	GError *error = NULL;
	PkResults *results = NULL;
	GPtrArray *list = NULL;

	results = pk_client_generic_finish(PK_CLIENT(client), res, &error);
	list = pk_results_get_package_array(results);

	if (list != NULL) {
		applet->pending = list->len;
		g_ptr_array_unref(list);
	}
	// DO nothing on ELSE - if results are NULL, it usually means timeout. 
	//else
	//	glob_data.pending = 0;

	int tmp_icon = applet->icon_status;

	if (applet->pending != 0)
		applet->icon_status = 1;
	else
		applet->icon_status = 0;

	if (tmp_icon != applet->icon_status)
		applet->flip_icon = 1;

	if (results != NULL) {
		g_object_unref(results);
	}
}

gboolean plugin_loop(softupd_applet *applet) {
	GError *error = NULL;
        PkClient *client = pk_client_new();

        pk_client_get_updates_async(client, pk_bitfield_value(PK_FILTER_ENUM_NONE), NULL, NULL, NULL, (GAsyncReadyCallback) callback_ready, (gpointer)applet);

        return TRUE;
}

gboolean packagekit_main(softupd_applet *applet) {
	GDBusProxy *proxy = NULL;
	GError *error = NULL;
	GVariant *retval = NULL;
	GMainLoop *loop;
	
	g_type_init();

	loop = g_main_loop_new(NULL, FALSE);

	g_timeout_add(REFRESH_TIME, (GSourceFunc)plugin_loop, (gpointer)applet);

	g_main_loop_run(loop);

	//g_object_unref(client);

	return TRUE;
}

