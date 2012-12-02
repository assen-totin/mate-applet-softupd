#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.66])
AC_INIT([softupd], [0.2.3], [assen.totin@gmail.com])
ORIG_CMDLINE_ARGS=$@
AC_SUBST([ORIG_CMDLINE_ARGS])
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
FORCE_BACKEND="no"
ADDITIONAL_PACKAGES=""
AC_ARG_ENABLE([backend],[AS_HELP_STRING([--enable-backend],[use the specified backend: package-kit, yum, yum-updatesd, apt-check (default: pick up the first available from the list)])],[FORCE_BACKEND=$enableval])

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
OBJS_BACKEND="none"
SRC_BACKEND="none"
SELECTED_BACKEND="none"
EXIT_NOW="no"
EXIT_MSG="none"
if test $FOUND_BACKEND = "no" ; then
	EXIT_NOW="yes"
	EXIT_MSG = "*** No suitable backend found. Cannot continue."
elif test $FORCE_BACKEND = "package-kit" ; then
	if test $OBJS_BACKEND_PACKAGEKIT = "no" ; then
		EXIT_NOW="yes"
		EXIT_MSG="Forced backend $FORCE_BACKEND not found. Cannot continue."
	else
		SELECTED_BACKEND=$FORCE_BACKEND
	fi
elif test $FORCE_BACKEND = "yum-updatesd" ; then
        if test $OBJS_BACKEND_YUMUPDATESD = "no" ; then
                EXIT_NOW="yes"
                EXIT_MSG="Forced backend $FORCE_BACKEND not found. Cannot continue."
	else
		SELECTED_BACKEND=$FORCE_BACKEND
        fi
elif test $FORCE_BACKEND = "yum" ; then
        if test $OBJS_BACKEND_YUM = "no" ; then
                EXIT_NOW="yes"
                EXIT_MSG="Forced backend $FORCE_BACKEND not found. Cannot continue."
	else
		SELECTED_BACKEND=$FORCE_BACKEND
        fi
elif test $FORCE_BACKEND = "apt-check" ; then
        if test $OBJS_BACKEND_APTCHECK = "no" ; then
                EXIT_NOW="yes"
                EXIT_MSG="Forced backend $FORCE_BACKEND not found. Cannot continue."
	else
		SELECTED_BACKEND=$FORCE_BACKEND
        fi
else
	echo "Chosing backend wisely..."
	if test $OBJS_BACKEND_PACKAGEKIT != "no" ; then
		SELECTED_BACKEND="package-kit"
	elif test $OBJS_BACKEND_YUMUPDATESD != "no" ; then
		SELECTED_BACKEND="yum-updatesd"
	elif test $OBJS_BACKEND_YUM != "no" ; then
		SELECTED_BACKEND="yum"
	elif test $OBJS_BACKEND_APTCHECK != "no" ; then
		SELECTED_BACKEND="apt-check"
	fi
fi

if test $EXIT_NOW = "yes" ; then
	echo
	echo $EXIT_MSG
	echo
	exit
fi

if test $SELECTED_BACKEND = "package-kit" ; then
        OBJS_BACKEND=$OBJS_BACKEND_PACKAGEKIT
        SRC_BACKEND=$SRC_BACKEND_PACKAGEKIT
        AC_DEFINE([HAVE_PACKAGEKIT],[1],[Define if package-kit was found])
	ADDITIONAL_PACKAGES="packagekit-glib2"
elif test $SELECTED_BACKEND = "yum-updatesd" ; then
	OBJS_BACKEND=$OBJS_BACKEND_YUMUPDATESD
	SRC_BACKEND=$SRC_BACKEND_YUMUPDATESD
	AC_DEFINE([HAVE_YUMUPDATESD],[1],[Define if yum-updatesd was found])
	ADDITIONAL_PACKAGES="dbus-1 dbus-glib-1"
elif test $SELECTED_BACKEND = "yum" ; then
        OBJS_BACKEND=$OBJS_BACKEND_YUM
        SRC_BACKEND=$SRC_BACKEND_YUM
        AC_DEFINE([HAVE_YUM],[1],[Define if yum was found])
        AC_DEFINE_UNQUOTED([YUM_BINARY],["$YUM_VIEWER"],[Define where is the yum binary])
elif test $SELECTED_BACKEND = "apt-check" ; then
	OBJS_BACKEND=$OBJS_BACKEND_APTCHECK
	SRC_BACKEND=$SRC_BACKEND_APTCHECK
	AC_DEFINE([HAVE_APTCHECK],[1],[Define if apt-check was found])
	AC_DEFINE_UNQUOTED([APTCHECK_BINARY],["$APTCHECK_PROG"],[Define where is the apt-check binary])
fi
AC_SUBST([OBJS_BACKEND])
AC_SUBST([SRC_BACKEND])

# Check which software installer to use
FOUND_INSTALLER="no"
FORCE_INSTALLER="no"
AC_ARG_ENABLE([installer],[AS_HELP_STRING([--enable-installer],[use the specified backend: yumex, gpk-update-viewer, update-manager (default: pick up the first available from the list)])],[FORCE_INSTALLER=$enableval])

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
        UPDATEMANAGER_INSTALLER=`which update-manager`
fi

# Select which software installer to use
if test $FORCE_INSTALLER = "yumex" ; then
	if test $CHECK_YUMEXVIEWER = "yes" ; then
		SELECTED_INSTALLER="yumex"
	else 
                EXIT_NOW="yes"
                EXIT_MSG="Forced installer $FORCE_INSTALLER not found. Cannot continue."	
	fi
elif test $FORCE_INSTALLER = "gpk-update-viewer" ; then
        if test $CHECK_GPKUPDATEVIEWER = "yes" ; then
                SELECTED_INSTALLER="gpk-update-viewer"
        else
                EXIT_NOW="yes"
                EXIT_MSG="Forced installer $FORCE_INSTALLER not found. Cannot continue." 
	fi
elif test $FORCE_INSTALLER = "update-manager" ; then
        if test $CHECK_UPDATEMANAGER = "yes" ; then
                SELECTED_INSTALLER="update-manager"
        else
                EXIT_NOW="yes"
                EXIT_MSG="Forced installer $FORCE_INSTALLER not found. Cannot continue." 
        fi
else
	echo "Chosing installer wisely..."
	if test $CHECK_YUMEXVIEWER = "yes" ; then
        	SELECTED_INSTALLER="yumex"
	elif test $CHECK_GPKUPDATEVIEWER = "yes" ; then
        	SELECTED_INSTALLER="gpk-update-viewer"
	elif test $CHECK_UPDATEMANAGER != "yes" ; then
        	SELECTED_INSTALLER="update-manager"
	else
		SELECTED_INSTALLER="none"
	fi
fi

if test $EXIT_NOW = "yes" ; then
        echo
        echo $EXIT_MSG
        echo
        exit
fi

if test $SELECTED_INSTALLER = "yumex" ; then
        AC_DEFINE_UNQUOTED([INSTALLER_BINARY],["$YUMEXVIEWER_INSTALLER"],[Define where is the installer binary])
elif test $SELECTED_INSTALLER = "gpk-update-viewer" ; then
        AC_DEFINE_UNQUOTED([INSTALLER_BINARY],["$GPKUPDATEVIEWER_INSTALLER"],[Define where is the installer binary])
elif test $SELECTED_INSTALLER = "update-manager" ; then
        AC_DEFINE_UNQUOTED([INSTALLER_BINARY],["$UPDATEMANAGER_INSTALLER"],[Define where is the installer binary])
else
	echo "*** No suitable installer found. You will have to update the system manually."
fi

AC_SUBST([ADDITIONAL_PACKAGES])

# Checks for libraries.

# Check for packages
PKG_CHECK_MODULES([GTK], [gtk+-2.0])
PKG_CHECK_MODULES([GLIB], [glib-2.0])
PKG_CHECK_MODULES([LIBMATENOTIFY], [libmatenotify])
PKG_CHECK_MODULES([LIBMATEPANELAPPLET_4], [libmatepanelapplet-4.0], [FOUND_LMPA_4="yes"], [FOUND_LMPA_4="no"])
PKG_CHECK_MODULES([LIBMATEPANELAPPLET_3], [libmatepanelapplet-3.0], [FOUND_LMPA_3="yes"], [FOUND_LMPA_3="no"])
if test $FOUND_LMPA_4 = "yes" ; then
	LIBMATEPANELAPPLET="libmatepanelapplet-4.0"
elif test $FOUND_LMPA_3 = "yes" ; then
	LIBMATEPANELAPPLET="libmatepanelapplet-3.0"
else
	AC_MSG_ERROR(["* * * libmatepanelapplet not found."])
fi
AC_SUBST([LIBMATEPANELAPPLET])

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
