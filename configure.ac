#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.66])
AC_INIT([softupd], [0.1.0], [assen.totin@gmail.com])
AC_CONFIG_HEADERS([config.h])

# prefix
if test "x$prefix" = "xNONE" ; then
	prefix="/usr/local"
fi
if test "x$exec_prefix" = "xNONE" ; then
	exec_prefix=$prefix
fi
PREFIX=$prefix
AC_SUBST([PREFIX])
BINDIR=$exec_prefix/bin
AC_SUBST([BINDIR])
SBINDIR=$exec_prefix/sbin
AC_SUBST([SBINDIR])
DATAROOTDIR=$prefix/share
AC_SUBST([DATAROOTDIR])
LIBEXECDIR=$prefix/libexec
AC_SUBST([LIBEXECDIR])
PIXMAPDIR=$prefix/share/pixmaps
AC_SUBST([PIXMAPDIR])
ICONSDIR=$prefix/share/icons
AC_SUBST([ICONSDIR])
LOCALEDIR=$prefix/share/locale
AC_SUBST([LOCALEDIR])
AC_DEFINE_UNQUOTED([LOCALEDIR],["$LOCALEDIR"],[Locale files])
TOPLEVELDIR=`pwd`
AC_SUBST([TOPLEVELDIR])

# Checks for programs
AC_PROG_CC
MAKEFILE_CC=`which gcc`
AC_SUBST([MAKEFILE_CC])

AC_CHECK_PROG([FOUND_MSGFMT],[msgfmt],["yes"],["no"])
if test $FOUND_MSGFMT = "no"; then
	echo "*** msgfmt not found. Cannot continue."
	exit
else
	MSGFMT=`which msgfmt`
	AC_SUBST([MSGFMT])
fi

# Check for backend tools
FOUND_BACKEND="no"

## Check for backend: packagekit
AC_CHECK_PROG([CHECK_PACKAGEKIT],[pkcon],[yes],[no])
OBJS_BACKEND_PACKAGEKIT="no"
SRC_BACKEND_PACKAGEKIT="no"
if test $CHECK_PACKAGEKIT = "no" ; then
        echo "*** package-kit not found."
else
        FOUND_BACKEND="yes"
        OBJS_BACKEND_PACKAGEKIT=backend-packagekit.o
        SRC_BACKEND_PACKAGEKIT=backend-packagekit.c
fi

## Check for backend: yum-updatesd
AC_CHECK_PROG([CHECK_YUMUPDATESD],[yum-updatesd],[yes],[no])
OBJS_BACKEND_YUMUPDATESD="no"
SRC_BACKEND_YUMUPDATESD="no"
if test $CHECK_YUMUPDATESD = "no" ; then
	echo "*** yum-updatesd not found."
else
	FOUND_BACKEND="yes"
	OBJS_BACKEND_YUMUPDATESD=backend-yumupdatesd.o
	SRC_BACKEND_YUMUPDATESD=backend-yumupdatesd.c
fi

## Check for backend: yum
AC_CHECK_PROG([CHECK_YUM],[yum],[yes],[no])
OBJS_BACKEND_YUM="no"
SRC_BACKEND_YUM="no"
if test $CHECK_YUM = "no" ; then
        echo "*** yum not found."
else
        FOUND_BACKEND="yes"
        YUM_VIEWER=`which yum`
        OBJS_BACKEND_YUM=backend-yum.o
        SRC_BACKEND_YUM=backend-yum.c
fi

## Check for backend: apt-check
AC_PATH_PROG([APTCHECK_PROG],[apt-check],[no],[/usr/lib/update-notifier:/var/lib/update-notifier:/usr/lib64/update-notifier:/var/lib64/update-notifier])
OBJS_BACKEND_APTCHECK="no"
SRC_BACKEND_APTCHECK="no"
if test $APTCHECK_PROG = "no" ; then
	echo "*** apt-check not found."
else
	FOUND_BACKEND="yes"
	OBJS_BACKEND_APTCHECK=backend-aptcheck.o
	SRC_BACKEND_APTCHECK=backend-aptcheck.c
fi

## Select which backend to use
if test $FOUND_BACKEND = "no" ; then
	echo "*** No suitable backend found. Cannot continue."
	exit
else
	echo "Chosing backend..."
	OBJS_BACKEND="none"
	SRC_BACKEND="none"
	SELECTED_BACKEND="none"
        if test $OBJS_BACKEND_PACKAGEKIT != "no" ; then
                OBJS_BACKEND=$OBJS_BACKEND_PACKAGEKIT
                SRC_BACKEND=$SRC_BACKEND_PACKAGEKIT
                AC_DEFINE([HAVE_PACKAGEKIT],[1],[Define if package-kit was found])
                SELECTED_BACKEND="package-kit"
	elif test $OBJS_BACKEND_YUMUPDATESD != "no" ; then
		OBJS_BACKEND=$OBJS_BACKEND_YUMUPDATESD
		SRC_BACKEND=$SRC_BACKEND_YUMUPDATESD
		AC_DEFINE([HAVE_YUMUPDATESD],[1],[Define if yum-updatesd was found])
		SELECTED_BACKEND="yum-updatesd"
        elif test $OBJS_BACKEND_YUM != "no" ; then
                OBJS_BACKEND=$OBJS_BACKEND_YUM
                SRC_BACKEND=$SRC_BACKEND_YUM
                AC_DEFINE([HAVE_YUM],[1],[Define if yum was found])
                AC_DEFINE_UNQUOTED([YUM_BINARY],["$YUM_VIEWER"],[Define where is the yum binary])
		SELECTED_BACKEND="yum"
	elif test $OBJS_BACKEND_APTCHECK != "no" ; then
		OBJS_BACKEND=$OBJS_BACKEND_APTCHECK
		SRC_BACKEND=$SRC_BACKEND_APTCHECK
		AC_DEFINE([HAVE_APTCHECK],[1],[Define if apt-check was found])
		AC_DEFINE_UNQUOTED([APTCHECK_BINARY],["$APTCHECK_PROG"],[Define where is the apt-check binary])
		SELECTED_BACKEND="apt-check"
	fi
	AC_SUBST([OBJS_BACKEND])
	AC_SUBST([SRC_BACKEND])
fi

# Check which software installer to use
FOUND_INSTALLER="no"

# Check for yumex
AC_CHECK_PROG([CHECK_YUMEXVIEWER],[yumex],[yes],[no])
if test $CHECK_YUMEXVIEWER = "no" ; then
        echo "*** yumex not found."
else
        FOUND_INSTALLER="yes"
        YUMEXVIEWER_INSTALLER=`which yumex`
fi

# Check for gpk-update-viewer
AC_CHECK_PROG([CHECK_GPKUPDATEVIEWER],[gpk-update-viewer],[yes],[no])
if test $CHECK_GPKUPDATEVIEWER = "no" ; then
        echo "*** gpk-update-viewer not found."
else
        FOUND_INSTALLER="yes"
        GPKUPDATEVIEWER_INSTALLER=`which gpk-update-viewer`
fi

# Check for update-manager
AC_CHECK_PROG([CHECK_UPDATEMANAGER],[update-manager],[yes],[no])
if test $CHECK_UPDATEMANAGER = "no" ; then
        echo "*** update-manager not found."
else
        FOUND_INSTALLER="yes"
        UPDATEMANAGER_INSTALLER=`which gpk-update-viewer`
fi

# Select which software installer to use
if test $FOUND_INSTALLER = "no" ; then
        echo "*** No suitable installer found. You will have to update the system manually."
	SELECTED_INSTALLER="none"

else
        echo "Chosing installer..."

        if test $CHECK_YUMEXVIEWER != "no" ; then
                AC_DEFINE_UNQUOTED([INSTALLER_BINARY],["$YUMEXVIEWER_INSTALLER"],[Define where is the installer binary])
                SELECTED_INSTALLER="yumex"
        elif test $CHECK_GPKUPDATEVIEWER != "no" ; then
		AC_DEFINE_UNQUOTED([INSTALLER_BINARY],["$GPKUPDATEVIEWER_INSTALLER"],[Define where is the installer binary])
                SELECTED_INSTALLER="gpk-update-viewer"
	elif test $CHECK_UPDATEMANAGER != "no" ; then
		AC_DEFINE_UNQUOTED([INSTALLER_BINARY],["$UPDATEMANAGER_INSTALLER"],[Define where is the installer binary])
		SELECTED_INSTALLER="update-manager"
	fi
fi

# Checks for libraries.

# Check for packages
PKG_CHECK_MODULES([GTK], [gtk+-2.0])
PKG_CHECK_MODULES([GLIB], [glib-2.0])
PKG_CHECK_MODULES([DBUS], [dbus-1])
PKG_CHECK_MODULES([DBUS_GLIB], [dbus-glib-1])
PKG_CHECK_MODULES([LIBMATEPANELAPPLET], [libmatepanelapplet-3.0])
PKG_CHECK_MODULES([LIBMATENOTIFY], [libmatenotify])

# Makefiles
AC_CONFIG_FILES([Makefile src/Makefile po/Makefile])

# Service files
AC_CONFIG_FILES([src/org.mate.applets.SoftupdApplet.mate-panel-applet src/org.mate.panel.applet.SoftupdApplet.service mate-applet-softupd.spec])

# Icons
AC_DEFINE_UNQUOTED([APPLET_ICON_PATH], ["$PIXMAPDIR"], [Define icons location])

# Grand finale
AC_OUTPUT

echo
echo "Configuration complete:"
echo " * Selected backend: $SELECTED_BACKEND"
echo " * Selected installer: $SELECTED_INSTALLER"
echo
echo "Now, run make."
echo 
