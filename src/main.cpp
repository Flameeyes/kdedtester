/***************************************************************************
 *   Copyright (C) 2004 by Diego 'Flameeyes' Petteno                       *
 *   flameeyes@users.berlios.de                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kdedmodule.h>
#include <klibloader.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kservice.h>

#include <qfile.h>
#include <qvariant.h>

static const char description[] =
    I18N_NOOP("A simple command line utility to test KDED modules");

static const char version[] = "0.1";

static KCmdLineOptions options[] =
{
    { "+modulename", I18N_NOOP( "Name of the module to test" ), 0 },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
	KAboutData about("kdedtester", I18N_NOOP("KDED Module tester"), version, description,
		     KAboutData::License_GPL, "(C) 2004 Diego 'Flameeyes' Pettenò", 0, 0, "flameeyes@users.berlios.de");
	about.addAuthor( "Diego 'Flameeyes' Pettenò", 0, "flameeyes@users.berlios.de" );
	KCmdLineArgs::init(argc, argv, &about);
	KCmdLineArgs::addCmdLineOptions( options );
	KApplication app;

	// no session.. just start up normally
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	
	if ( args->count() == 0 )
	{
		kdError() << "You must call kdedtester with a valid kded module name." << endl;
		return -1;
	}

	/// @todo do something with the command line args here
	QCString modulename = args->arg(0);
	args->clear();
	
	if ( modulename.isEmpty() )
	{
		kdError() << "You must call kdedtester with a valid kded module name." << endl;
		return -1;
	}
	
	KService::Ptr s = KService::serviceByDesktopPath("kded/"+modulename+".desktop");
	if ( ! s || s->library().isEmpty() )
	{
		kdError() << "Unable to load the service for requested module." << endl;
		return -2;
	}
	
	// get the library loader instance
	KLibLoader *loader = KLibLoader::self();
	
	QVariant v = s->property("X-KDE-Factory");
	QString factory = v.isValid() ? v.toString() : QString::null;
	if (factory.isEmpty())
		factory = s->library();

	factory = "create_" + factory;
	QString libname = "kded_"+s->library();
	
	kdWarning() << "Recap: the factory is '" << factory << "', and the library '" << libname << "'." << endl;

	KLibrary *lib = loader->library(QFile::encodeName(libname));
	if ( ! lib )
	{
		kdWarning() << "Library not found, trying '" << libname << "' instead" << endl
			<< "KLibLoader says: " << loader->lastErrorMessage() << endl;
		
		libname.prepend("lib");
		lib = loader->library(QFile::encodeName(libname));
	}
	
	if ( ! lib )
	{
		kdError() << "Library still not found. Exiting." << endl
			<< "KLibLoader says: " << loader->lastErrorMessage() << endl;
		return -3;
	}
	
	void *create = lib->symbol(QFile::encodeName(factory));
	if ( ! create )
	{
		kdError() << "Unable to find factory symbol into library. Exiting." << endl;
		loader->unloadLibrary(QFile::encodeName(libname));
		return -4;
	}
	
        KDEDModule* (*func)(const QCString &);
        func = (KDEDModule* (*)(const QCString &)) create;
	
	KDEDModule *module = func(modulename);
	if ( ! module )
	{
		kdError() << "Factory returned NULL module. Exiting." << endl;
		loader->unloadLibrary(QFile::encodeName(libname));
		return -5;
	}
	
	delete module;
	loader->unloadLibrary(QFile::encodeName(libname));
	kdWarning() << "Module loaded (and already unloaded) correctly." << endl;
	return 0;
}
