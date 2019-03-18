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

#include "../config.h"
#include "applet.h"

#define _(String) gettext (String)

void warn_missing_installer(GtkWidget *widget);

void push_notification (gchar *title, gchar *body, gchar *icon) {
	NotifyNotification* notification;
	GError* error = NULL;

	notify_init(PACKAGE_NAME);

#ifdef HAVE_LIBMATENOTIFY
	notification = notify_notification_new (title, body, icon, NULL);
#elif HAVE_LIBNOTIFY
	notification = notify_notification_new (title, body, icon);
#endif

	notify_notification_set_timeout (notification, 5000);

	notify_notification_show (notification, &error);

	g_object_unref (G_OBJECT (notification));
	notify_uninit ();
}


static void quitDialogOK( GtkWidget *widget, gpointer data ){
        //GtkWidget *quitDialog = data;
        softupd_applet *applet = data;
        gtk_widget_destroy(applet->quitDialog);
	struct stat buffer;
	int status;
	int uid;

	#ifdef INSTALLER_BINARY
		status = stat(INSTALLER_BINARY, &buffer);
		if (status == 0) {
			// Get our UID
			uid = getuid();

			int pid = fork();
			if (pid == -1) {
				warn_missing_installer(widget);
				return;
			} else if (pid == 0) {
				// Child process
				// This is ugly, but no other way around it right now: 
				// yumex requires --root to run when UID is 0, so keep it happy.
				if ((uid == 0) && (! strcmp(SELECTED_INSTALLER, "yumex")))
					execl(INSTALLER_BINARY, INSTALLER_BINARY, "--root", NULL);
				// dnfdragora requires --update-only to be run as updater.
				else if (! strcmp(SELECTED_INSTALLER, "dnfdragora"))
					execl(INSTALLER_BINARY, INSTALLER_BINARY, "--update-only", NULL);
				else
					execl(INSTALLER_BINARY, INSTALLER_BINARY, NULL);
				abort();
			}
			// Parent process continues
			// Find a slot to store the PID; if none, realloc() one slot up and occupy it
			int match = 0;
			int i;
			for (i=0; i < applet->pid_cnt; i++ ) {
				if (applet->pid_arr[i] == 0) {
					match = 1;
					break;
				}
			}
			// If slot found, use it
			if (match == 1) 
				applet->pid_arr[i] = pid;
			else {
				// Re-allocate memory and use the new slot, bump counter
				void *_tmp = realloc(applet->pid_arr, ((applet->pid_cnt + 1) * sizeof(int)));
				applet->pid_arr = (int *)_tmp;
				applet->pid_cnt ++;
				applet->pid_arr[applet->pid_cnt - 1] = pid;
			}
		}
		else {
			warn_missing_installer(widget);
		}
	#endif
}


static void quitDialogCancel( GtkWidget *widget, gpointer data ){
        GtkWidget *quitDialog = data;
        gtk_widget_destroy(quitDialog);
}


gboolean check_dead_bones(softupd_applet *applet) {
	int status, pid;
	while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
		// Look up the died pid in the stack, clear
		int i;
		for (i=0; i < applet->pid_cnt; i++) {
			if (pid == applet->pid_arr[i]) {
				applet->pid_arr[i] = 0;
				break;
			}
		}
		// We could, in theory, check if the zeroed slot is the last one and realloc() down one slot
		// However, this seems unnecessary as we should never in practive have more than one child running.
	}
	return 1;
}

void warn_missing_installer(GtkWidget *widget) {
        char msg1[1024];

        sprintf(&msg1[0], "%s\n\n%s\n\n%s", _("ERROR:"), _("Could not launch installer:"), INSTALLER_BINARY);

        GtkWidget *label = gtk_label_new (&msg1[0]);

        GtkWidget *quitDialog = gtk_dialog_new_with_buttons (_("Error"), GTK_WINDOW(widget), GTK_DIALOG_MODAL, NULL);
#ifdef HAVE_GTK2
        GtkWidget *buttonOK = gtk_dialog_add_button (GTK_DIALOG(quitDialog), GTK_STOCK_OK, GTK_RESPONSE_OK);
#elif HAVE_GTK3
        GtkWidget *buttonOK = gtk_dialog_add_button (GTK_DIALOG(quitDialog), _("OK"), GTK_RESPONSE_OK);
#endif
        gtk_dialog_set_default_response (GTK_DIALOG (quitDialog), GTK_RESPONSE_CANCEL);

		#ifdef HAVE_GTK2
		gtk_container_add (GTK_CONTAINER (GTK_DIALOG(quitDialog)->vbox), label);
		#elif HAVE_GTK3
		gtk_container_add (GTK_CONTAINER (gtk_dialog_get_content_area(GTK_DIALOG(quitDialog))), label);
		#endif

        g_signal_connect (G_OBJECT(buttonOK), "clicked", G_CALLBACK (quitDialogCancel), (gpointer) quitDialog);

        gtk_widget_show_all (GTK_WIDGET(quitDialog));
}

static gboolean applet_on_button_press (GtkWidget *event_box, GdkEventButton *event, softupd_applet *applet) {
	static GtkWidget *label;

	if (event->button != 1)
		return FALSE;

	char msg1[1024];
	if (applet->pending >= 0) {
		sprintf(&msg1[0], _("Pending updates: %u"), applet->pending);
	} else {
		sprintf(&msg1[0], _("Could not get count of pending updates"));
	}
	if (applet->pending > 0) {
		#ifdef INSTALLER_BINARY
			sprintf(&msg1[0],"%s\n%s", &msg1[0], _("Install updates?"));
		#else
			sprintf(&msg1[0],"%s\n%s", &msg1[0], _("You have to update your system manually."));
		#endif
	}
	
	label = gtk_label_new (&msg1[0]);

	applet->quitDialog = gtk_dialog_new_with_buttons ("Software Updater", GTK_WINDOW(applet->event_box), GTK_DIALOG_MODAL, NULL);
	#ifdef HAVE_GTK2
	GtkWidget *buttonOK = gtk_dialog_add_button (GTK_DIALOG(applet->quitDialog), GTK_STOCK_OK, GTK_RESPONSE_OK);
	#elif HAVE_GTK3
	GtkWidget *buttonOK = gtk_dialog_add_button (GTK_DIALOG(applet->quitDialog), _("OK"), GTK_RESPONSE_OK);
	#endif

	#ifdef INSTALLER_BINARY
			#ifdef HAVE_GTK2
	GtkWidget *buttonCancel = gtk_dialog_add_button (GTK_DIALOG(applet->quitDialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
			#elif HAVE_GTK3
	GtkWidget *buttonCancel = gtk_dialog_add_button (GTK_DIALOG(applet->quitDialog), _("Cancel"), GTK_RESPONSE_CANCEL);
			#endif
	#endif

	gtk_dialog_set_default_response (GTK_DIALOG (applet->quitDialog), GTK_RESPONSE_CANCEL);

	#ifdef HAVE_GTK2
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(applet->quitDialog)->vbox), label);
	#elif HAVE_GTK3
	gtk_container_add (GTK_CONTAINER (gtk_dialog_get_content_area(GTK_DIALOG(applet->quitDialog))), label);
	#endif

	g_signal_connect (G_OBJECT(buttonOK), "clicked", G_CALLBACK (quitDialogOK), (gpointer) applet);

	#ifdef INSTALLER_BINARY
	        g_signal_connect (G_OBJECT(buttonCancel), "clicked", G_CALLBACK (quitDialogCancel), (gpointer) applet->quitDialog);
	#endif

        gtk_widget_show_all (GTK_WIDGET(applet->quitDialog));

	return TRUE;
}


static gboolean applet_check_icon (softupd_applet *applet) {
	char image_file[1024];
	char msg[1024];

	// Check if there are dead bones to collect
	check_dead_bones(applet);

	if (applet->icon_status == 0) {
		sprintf(&image_file[0], "%s/%s", APPLET_ICON_PATH, APPLET_ICON_OFF);
		gtk_widget_set_tooltip_text (GTK_WIDGET (applet->applet), _("Your system is up to date."));
	}
	else {
		sprintf(&image_file[0], "%s/%s", APPLET_ICON_PATH, APPLET_ICON_ON);
		if (applet->pending >= 0) {
			sprintf(&msg[0], _("You have %u updates. Click to proceed."), applet->pending);
		} else {
			sprintf(&msg[0], _("Could not get count of pending updates."));
		}
		gtk_widget_set_tooltip_text (GTK_WIDGET (applet->applet), &msg[0]);

		// Notification: only if icon status is ON and flip is ON
		if (applet->flip_icon == 1) {
			if (applet->pending >= 0) {
				sprintf(&msg[0], _("You have %u updates."), applet->pending);
			} else {
				sprintf(&msg[0], _("Could not get count of pending updates."));
			}
			push_notification(_("Software updates available"), &msg[0], NULL);
		}
	}

	if (applet->flip_icon == 1) {
		gtk_image_set_from_file(GTK_IMAGE(applet->image), &image_file[0]);
		applet->flip_icon = 0;
	}

	return TRUE;
}


static gboolean applet_listener(softupd_applet *applet) {
	#ifdef HAVE_PACKAGEKIT
		if(packagekit_main(applet))
			return TRUE;
		else
			return FALSE;
	#endif

	#ifdef HAVE_YUMUPDATESD
		if(yumupdatesd_main(applet))
			return TRUE;
		else
			return FALSE;
	#endif

	#ifdef HAVE_DNF
		#ifdef HAVE_GTK2
		g_timeout_add(REFRESH_TIME, (GtkFunction) dnf_main, (gpointer)applet);
		#elif HAVE_GTK3
		g_timeout_add(REFRESH_TIME, (GSourceFunc) dnf_main, (gpointer)applet);
		#endif
		applet->loop = g_main_loop_new (NULL, FALSE);
		g_main_loop_run (applet->loop);
		return TRUE;
	#endif

	#ifdef HAVE_YUM
		#ifdef HAVE_GTK2
		g_timeout_add(REFRESH_TIME, (GtkFunction) yum_main, (gpointer)applet);
		#elif HAVE_GTK3
		g_timeout_add(REFRESH_TIME, (GSourceFunc) yum_main, (gpointer)applet);
		#endif
		applet->loop = g_main_loop_new (NULL, FALSE);
		g_main_loop_run (applet->loop);
		return TRUE;
	#endif

	#ifdef HAVE_APTCHECK
		#ifdef HAVE_GTK2
		g_timeout_add(REFRESH_TIME, (GtkFunction) aptcheck_main, (gpointer)applet);
		#elif HAVE_GTK3
		g_timeout_add(REFRESH_TIME, (GSourceFunc) aptcheck_main, (gpointer)applet);
		#endif
		applet->loop = g_main_loop_new (NULL, FALSE);
		g_main_loop_run (applet->loop);
		return TRUE;
	#endif

}


#ifdef HAVE_GTK2
static void applet_back_change (MatePanelApplet *a, MatePanelAppletBackgroundType type, GdkColor *color, GdkPixmap *pixmap, softupd_applet *applet) {
#elif HAVE_GTK3
static void applet_back_change (MatePanelApplet *a, MatePanelAppletBackgroundType type, GdkRGBA *color, cairo_pattern_t *pattern, softupd_applet *applet) {
#endif

	// Use MATE-provided wrapper to change the background (same for both GTK2 and GTK3)
	mate_panel_applet_set_background_widget (a, GTK_WIDGET(applet->applet));
	mate_panel_applet_set_background_widget (a, GTK_WIDGET(applet->event_box));
}

static void applet_destroy(MatePanelApplet *applet_widget, softupd_applet *applet) {
        g_assert(applet);

	// Remove all timers
	while (g_source_remove_by_user_data((gpointer) applet))
		;

	g_main_loop_quit(applet->loop);
	g_free(applet->pid_arr);
        g_free(applet);
        return;
}

static gboolean applet_main (MatePanelApplet *applet_widget, const gchar *iid, gpointer data) {
	softupd_applet *applet;

	if (strcmp (iid, APPLET_ID) != 0)
		return FALSE;

	// i18n
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE_NAME, LOCALEDIR);
	bind_textdomain_codeset(PACKAGE_NAME, "utf-8");
	textdomain (PACKAGE_NAME);

	// Init 
	applet = g_malloc0(sizeof(softupd_applet));
	applet->pending = 0;
	applet->icon_status = 0;
	applet->flip_icon = 0;
	applet->applet = applet_widget;

	// Children array
	applet->pid_cnt = 1;
	applet->pid_arr = g_malloc0(sizeof(int));
	applet->pid_arr[0] = 0;

	// Get an image
	char image_file[1024];
	sprintf(&image_file[0], "%s/%s", APPLET_ICON_PATH, APPLET_ICON_OFF);
	applet->image = gtk_image_new_from_file (&image_file[0]);

	// Put the image into a container (it needs to receive actions)
	applet->event_box = gtk_event_box_new();
	gtk_container_add (GTK_CONTAINER (applet->event_box), applet->image);

	// Put the container into the applet
	gtk_container_add (GTK_CONTAINER (applet->applet), applet->event_box);

	g_signal_connect (G_OBJECT (applet->event_box), "button_press_event", G_CALLBACK (applet_on_button_press), (gpointer)applet);
	g_signal_connect(G_OBJECT(applet->applet), "change_background", G_CALLBACK (applet_back_change), (gpointer)applet);
	g_signal_connect(G_OBJECT(applet->applet), "destroy", G_CALLBACK(applet_destroy), (gpointer)applet);

	// Tooltip
	gtk_widget_set_tooltip_text (GTK_WIDGET (applet->applet), _("Your system is up to date."));

	gtk_widget_show_all (GTK_WIDGET (applet->applet));

#ifdef HAVE_GTK2
	g_timeout_add(10000, (GtkFunction) applet_check_icon, (gpointer)applet);
#elif HAVE_GTK3
	g_timeout_add(10000, (GSourceFunc) applet_check_icon, (gpointer)applet);
#endif

	applet_listener(applet);

	return TRUE;
}

MATE_PANEL_APPLET_OUT_PROCESS_FACTORY (APPLET_FACTORY, PANEL_TYPE_APPLET, APPLET_NAME, applet_main, NULL)

