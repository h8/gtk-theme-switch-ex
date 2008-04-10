/*
 * utils.h is a part of Gtk Theme Switch Ex software
 * Gtk Theme Switch Ex, a fast and small Gtk theme switching utility 
 * that has a GUI and console interface.
 *
 * Gtk Theme Switch Ex is based on the source code of 
 * Gtk Theme Switch v. 2.0.0.rc2 software, originally developed by 
 * Aaron Lehman and Maher Awamy (their copyrights are included).
 *
 * Copyright (C) Dmitry Stropaloff 2008 <helions8@gmail.com>
 *
 * Copyrights of original Gtk Theme Switch:
 * Copyright (C) Maher Awamy 2003 <muhri@muhri.net>
 * Copyright (C) Aaron Lehman 2003 <aaronl@vitelus.com>
 * 
 * utils.h and Gtk Theme Switch Ex is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 3 of the License, or (at your option)
 * any later version.
 * 
 * Gtk Theme Switch Ex is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Gtk Theme Switch Ex.  If not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#include "includes.h"

typedef struct
{
  gchar *gtkrc_file;
  gchar *font_name;
  gchar *icontheme_name;
  int toolbar_style;
} rcfile_data;

void usage (void);
gboolean is_themedir (gchar *path, gchar **rc_file);
gboolean is_installed_theme (gchar *path, gchar **rc_file, gchar *homedir);
GList* compare_glists (GList *t1, GList *t2, GCompareFunc cmpfunc);
void send_refresh_signal(void);
void dump_rcfile(gchar *path, rcfile_data *data);
gint install_icons_tarball(const gchar *file);
