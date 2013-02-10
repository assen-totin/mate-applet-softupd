#include <unistd.h>
#include <mate-panel-applet.h>
#include <libmatenotify/notify.h>
#include <libintl.h>
#include <sys/stat.h>
#include <glib.h>

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
	int pending;
	int icon_status;
	int flip_icon;
} softupd_applet;

// Prototypes
gboolean packagekit_main();
int yumupdatesd_main();
void yum_main();
void aptcheck_main();
void aptget_main();
