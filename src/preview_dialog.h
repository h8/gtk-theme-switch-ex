/*
 * preview_dialog.h is a part of Gtk Theme Switch Ex software
 * Gtk Theme Switch Ex, a fast and small Gtk theme switching utility 
 * that has a GUI and console interface.
 *
 * Copyright (C) Dmitry Stropaloff 2008 <helions8@gmail.com>
 *
 * preview_dialog.h and Gtk Theme Switch Ex is free software.
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

GtkBuilder *ui;
gchar *rc_file;

void show_preview_dialog(gchar *rcfile);
GtkListStore *create_icons_store();

gboolean delete_callback(GtkWidget *widget, GdkEvent *event, gpointer data);
void apply_clicked_callback(GtkButton *button, gpointer user_data);
