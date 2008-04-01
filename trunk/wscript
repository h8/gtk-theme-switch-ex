#! /usr/bin/env python
# encoding: utf-8

import os
import Params

VERSION='2.0.0-rc4'
APPNAME='gtk-theme-switch-ex'

srcdir = '.'
blddir = 'build'

def set_options(opt):
	pass

def configure(conf):
	conf.check_tool('gcc')

	conf.check_pkg('gtk+-2.0', destvar='GTK', vnum='2.6.0')

	conf.define('VERSION', VERSION)
	conf.define('DATADIR', os.path.join(conf.env['PREFIX'], 'share'))
	#conf.add_define('GETTEXT_PACKAGE', 'gtk-theme-switch-ex')
	#conf.add_define('PACKAGE', 'gtk-theme-switch-ex')

	conf.env.append_value('CCFLAGS', '-DHAVE_CONFIG_H')

	libconf = conf.create_library_configurator()
        libconf.uselib = 'PCRE'
        libconf.name = 'pcre'
        libconf.paths = ['/usr/lib']
        libconf.run()

	conf.write_config_header('config.h')

def build(bld):
	bld.add_subdirs('src')
	bld.add_subdirs('man')

def shutdown():
	pass

