#! /usr/bin/env python
# encoding: utf-8

import os
import Params

VERSION='2.0.0svn'
APPNAME='gtk-theme-switch-ex'

srcdir = '.'
blddir = 'build'

def set_options(opt):
	pass

def configure(conf):
	conf.check_tool('gcc')

	if not conf.check_pkg('gtk+-2.0', destvar='GTK', vnum='2.6.0'):
		Params.fatal("gtk-theme-swith-ex requires gtk+ library.")

	conf.define('VERSION', VERSION)
	conf.define('DATADIR', os.path.join(conf.env['PREFIX'], 'share'))
	conf.define('APPNAME', APPNAME)
	#conf.add_define('GETTEXT_PACKAGE', 'gtk-theme-switch-ex')
	#conf.add_define('PACKAGE', 'gtk-theme-switch-ex')

	conf.env.append_value('CCFLAGS', '-DHAVE_CONFIG_H')

	libconf = conf.create_library_configurator()
        libconf.uselib = 'PCRE'
        libconf.name = 'pcre'
        libconf.paths = ['/usr/lib']
        if not libconf.run():
		Params.fatal("Missed dependencies (see above).")

	conf.write_config_header('config.h')

def build(bld):
	# Change linker option. Need for GtkBuilder.
	bld.m_allenvs['default']['SHLIB_MARKER'] = '-Wl,--export-dynamic'
	
	bld.add_subdirs('src')
	bld.add_subdirs('man')
	bld.add_subdirs('data')

def shutdown():
	pass

