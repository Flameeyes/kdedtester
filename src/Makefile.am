# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project


# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kdedtester.pot


#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kdedtester

# the application source, library search path, and link libraries
kdedtester_SOURCES = main.cpp fakekded.cpp fakekded.skel
kdedtester_LDFLAGS = $(KDE_RPATH) $(all_libraries)
kdedtester_LDADD = $(LIB_KIO) $(LIB_KDEUI)

# this is where the desktop file will go 
shelldesktopdir   = $(kde_appsdir)/Utilities


# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/kdedtester

