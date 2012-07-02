#include <unistd.h>
#include <mate-panel-applet.h>
#include <libnotify/notify.h>
#include <libintl.h>

#include "../config.h"
#include "applet.h"

struct softupd_applet_widgets {
        MatePanelApplet *applet;
        GtkWidget *image;
        GtkWidget *event_box;
};

#define _(String) gettext (String)


void push_notification (gchar *title, gchar *body, gchar *icon) {
        NotifyNotification* notification;
	GError* error = NULL;

	notify_init(PACKAGE_NAME);
        notification = notify_notification_new (title, body, icon);

        notify_notification_set_timeout (notification, 5000);

        notify_notification_show (notification, &error);

        g_object_unref (G_OBJECT (notification));
        notify_uninit ();
}



static void quitDialogOK( GtkWidget *widget, gpointer data ){
        GtkWidget *quitDialog = data;
        gtk_widget_destroy(quitDialog);
	int pid = fork();
	if (pid == 0) {
		execl(INSTALLER_BINARY, NULL);
	}
}


static void quitDialogCancel( GtkWidget *widget, gpointer data ){
        GtkWidget *quitDialog = data;
        gtk_widget_destroy(quitDialog);
}


static gboolean applet_on_button_press (GtkWidget *event_box, GdkEventButton *event, gpointer data) {
	static GtkWidget *window, *box, *image, *label, *dialog;

	if (event->button != 1)
		return FALSE;

	char msg1[1024], msg2[1024];
	sprintf(&msg1[0], _("Pending updates: %u"), glob_data.pending);
	if (glob_data.pending > 0) {
		sprintf(&msg2[0], _("Install updates?"));
		sprintf(&msg1[0],"%s\n%s", &msg1[0], &msg2[0]);
	}
	
	label = gtk_label_new (&msg1[0]);

	GtkWidget *quitDialog = gtk_dialog_new_with_buttons ("Software Updater", GTK_WINDOW(data), GTK_DIALOG_MODAL, NULL);
	GtkWidget *buttonOK = gtk_dialog_add_button (GTK_DIALOG(quitDialog), GTK_STOCK_OK, GTK_RESPONSE_OK);
	GtkWidget *buttonCancel = gtk_dialog_add_button (GTK_DIALOG(quitDialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);

	gtk_dialog_set_default_response (GTK_DIALOG (quitDialog), GTK_RESPONSE_CANCEL);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(quitDialog)->vbox), label);
        g_signal_connect (G_OBJECT(buttonOK), "clicked", G_CALLBACK (quitDialogOK), (gpointer) quitDialog);
        g_signal_connect (G_OBJECT(buttonCancel), "clicked", G_CALLBACK (quitDialogCancel), (gpointer) quitDialog);
        gtk_widget_show_all (GTK_WIDGET(quitDialog));

	return TRUE;
}


static gboolean applet_check_icon (gpointer data) {
	char image_file[1024];
	char msg[1024];
	struct softupd_applet_widgets *applet_widgets_p = data;

	if (glob_data.icon_status == 0) {
		sprintf(&image_file[0], "%s/%s", APPLET_ICON_PATH, APPLET_ICON_OFF);
		gtk_widget_set_tooltip_text (GTK_WIDGET (applet_widgets_p->applet), _("Your system is up to date."));
	}
	else {
		sprintf(&image_file[0], "%s/%s", APPLET_ICON_PATH, APPLET_ICON_ON);
		sprintf(&msg[0], _("You have %u updates. Click to proceed."), glob_data.pending);
		gtk_widget_set_tooltip_text (GTK_WIDGET (applet_widgets_p->applet), &msg[0]);

		// Notification: only if icon status is ON and flip is ON
		if (glob_data.flip_icon == 1) {
			sprintf(&msg[0], _("You have %u updates."), glob_data.pending);
			push_notification(_("Software updates available"), &msg[0], NULL);
		}
	}

	if (glob_data.flip_icon == 1) {
		gtk_image_set_from_file(GTK_IMAGE(applet_widgets_p->image), &image_file[0]);
		glob_data.flip_icon = 0;
	}

	return TRUE;
}


static gboolean applet_listener() {
	GMainLoop *loop;

	#ifdef HAVE_YUMUPDATESD
		if(yumupdatesd_main())
			return TRUE;
		else
			return FALSE;
	#endif

	#ifdef HAVE_YUM
		g_timeout_add(REFRESH_TIME, (GtkFunction) yum_main, NULL);
		loop = g_main_loop_new (NULL, FALSE);
		g_main_loop_run (loop);
		return TRUE;
	#endif

	#ifdef HAVE_APTCHECK
		g_timeout_add(REFRESH_TIME, (GtkFunction) aptcheck_main, NULL);
		loop = g_main_loop_new (NULL, FALSE);
		g_main_loop_run (loop);
		return TRUE;
	#endif

	#ifdef HAVE_APTGET
                g_timeout_add(REFRESH_TIME, (GtkFunction) aptget_main, NULL);
                loop = g_main_loop_new (NULL, FALSE);
                g_main_loop_run (loop);
		return TRUE;
	#endif
}


static gboolean applet_main (MatePanelApplet *applet, const gchar *iid, gpointer data) {
	GtkWidget *label, *image, *event_box;

	struct softupd_applet_widgets applet_widgets;

	if (strcmp (iid, APPLET_ID) != 0)
		return FALSE;

	// i18n
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE_NAME, LOCALEDIR);
	bind_textdomain_codeset(PACKAGE_NAME, "utf-8");
	textdomain (PACKAGE_NAME);

	// Init globals
	glob_data.pending = 0;
	glob_data.icon_status = 0;
	glob_data.flip_icon = 0;

	applet_widgets.applet = applet;

	// Get an image
	char image_file[1024];
	sprintf(&image_file[0], "%s/%s", APPLET_ICON_PATH, APPLET_ICON_OFF);
	image = gtk_image_new_from_file (&image_file[0]);
	applet_widgets.image = image;

	// Put the image into a container (it needs to receive actions)
	event_box = gtk_event_box_new ();
	gtk_container_add (GTK_CONTAINER (event_box), image);
	g_signal_connect (G_OBJECT (event_box), "button_press_event", G_CALLBACK (applet_on_button_press), event_box);

	// Put the container into the applet
        gtk_container_add (GTK_CONTAINER (applet), event_box);

	//Tooltip
	gtk_widget_set_tooltip_text (GTK_WIDGET (applet), _("Your system is up to date."));

	gtk_widget_show_all (GTK_WIDGET (applet));

	g_timeout_add(10000, (GtkFunction) applet_check_icon, &applet_widgets);

	applet_listener();

	return TRUE;
}

MATE_PANEL_APPLET_OUT_PROCESS_FACTORY (APPLET_FACTORY, PANEL_TYPE_APPLET, APPLET_NAME, applet_main, NULL)

