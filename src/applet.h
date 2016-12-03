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

#include <string.h>
#include <unistd.h>
#include <mate-panel-applet.h>
#include <libintl.h>
#include <sys/stat.h>
#include <glib.h>
#include <stdlib.h>
#include <wait.h>

#ifdef HAVE_LIBMATENOTIFY
	#include <libmatenotify/notify.h>
#elif HAVE_LIBNOTIFY
	#include <libnotify/notify.h>
#endif

#define APPLET_FACTORY "SoftupdAppletFactory"
#define APPLET_ID "SoftupdApplet"
#define APPLET_NAME "softupd"
#define APPLET_ICON_OFF "applet_softupd_off.png"
#define APPLET_ICON_ON "applet_softupd_on.png"
#define APPLET_VERSION "1"
// Time between backend runs - for yum, apt-check, apt-get; in milliseconds
#define REFRESH_TIME 600000

typedef struct {
	GMainLoop *loop;
	MatePanelApplet *applet;
        GtkWidget *image;
        GtkWidget *event_box;
	GtkWidget *quitDialog;
	int pending;
	int icon_status;
	int flip_icon;
	int *pid_arr;
	int pid_cnt;
} softupd_applet;

// Prototypes
gboolean packagekit_main(softupd_applet *);
gboolean yumupdatesd_main(softupd_applet *);
gboolean yum_main(softupd_applet *);
gboolean aptcheck_main(softupd_applet *);
gboolean dnf_main(softupd_applet *);
