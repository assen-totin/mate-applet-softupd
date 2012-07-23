#include <glib.h>

#define APPLET_FACTORY "SoftupdAppletFactory"
#define APPLET_ID "SoftupdApplet"
#define APPLET_NAME "softupd"
#define APPLET_ICON_OFF "applet_softupd_off.png"
#define APPLET_ICON_ON "applet_softupd_on.png"
#define APPLET_VERSION "1"
// Time between backend runs - for yum, apt-check, apt-get; in milliseconds
#define REFRESH_TIME 600000

struct softupd_applet_data {
	int pending;
	int icon_status;
	int flip_icon;
};

struct softupd_applet_data glob_data;

// Prototypes
gboolean packagekit_main();
int yumupdatesd_main();
void yum_main();
void aptcheck_main();
void aptget_main();
