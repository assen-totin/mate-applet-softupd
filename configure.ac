#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.66])
AC_INIT([softupd], [0.0.1], [assen.totin@gmail.com])
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

### Check for backend: apt-get
#AC_CHECK_PROG([CHECK_APTGET],[apt-get],[yes],[no])
#OBJS_BACKEND_APTGET="no"
#SRC_BACKEND_APTGET="no"
#if test $CHECK_APTGET = "no" ; then
#        echo "*** apt-get not found."
#else
#        FOUND_BACKEND="yes"
#  	 APTGET_PROG = `which apt-get`
#        OBJS_BACKEND_APTGET=backend-aptget.o
#        SRC_BACKEND_APTGET=backend-aptget.c
#fi


## Select which backend to use
if test $FOUND_BACKEND = "no" ; then
	echo "*** No suitable backend found. Cannot continue."
	exit
else
	echo "Chosing backend..."
	OBJS_BACKEND="none"
	SRC_BACKEND="none"
	SELECTED_BACKEND="none"
	if test $OBJS_BACKEND_YUMUPDATESD != "no" ; then
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
#        elif test $OBJS_BACKEND_APTGET != "no" ; then
#                OBJS_BACKEND=$OBJS_BACKEND_APTGET
#                SRC_BACKEND=$SRC_BACKEND_APTGET
#                AC_DEFINE([HAVE_APTGET],[1],[Define if apt-check was found])
#                AC_DEFINE_UNQUOTED([APTGET_BINARY],["$APTGET_PROG"],[Define where is the apt-get binary])
#                SELECTED_BACKEND="apt-check"
	fi
	AC_SUBST([OBJS_BACKEND])
	AC_SUBST([SRC_BACKEND])
fi

# Select which software installer to use
GPK_UPDATE_VIEWER=`which gpk-update-viewer`
AC_DEFINE_UNQUOTED([INSTALLER_BINARY],["$GPK_UPDATE_VIEWER"],[Define where is the installer binary])

# Checks for libraries.

# Check for packages
PKG_CHECK_MODULES([GTK], [gtk+-2.0])
PKG_CHECK_MODULES([GLIB], [glib-2.0])
PKG_CHECK_MODULES([DBUS], [dbus-1])
PKG_CHECK_MODULES([DBUS_GLIB], [dbus-glib-1])
PKG_CHECK_MODULES([LIBMATEPANELAPPLET], [libmatepanelapplet-3.0])
PKG_CHECK_MODULES([LIBNOTIFY], [libnotify])

# Makefiles
AC_CONFIG_FILES([Makefile src/Makefile po/Makefile])

# Service files
AC_CONFIG_FILES([src/org.mate.applets.SoftupdApplet.mate-panel-applet src/org.mate.panel.applet.SoftupdApplet.service])

# Icons
AC_DEFINE_UNQUOTED([APPLET_ICON_PATH], ["$PIXMAPDIR"], [Define icons location])

# Grand finale
AC_OUTPUT

echo
echo "Configuration complete:"
echo " * Selected backend: $SELECTED_BACKEND"
echo
echo "Now, run make."
echo 
