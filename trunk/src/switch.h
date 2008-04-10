/*
 * switch.h is a part of Gtk Theme Switch Ex software
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
 * switch.h and Gtk Theme Switch Ex is free software.
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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <glib/giochannel.h>
#include <stdlib.h>
#include <pcre.h>

#include "includes.h"

/* Defines - UI */
#define USE_ICON_TOGGLE "use-icon-toggle"
#define USE_FONT_TOGGLE "use-font-toggle"
#define FONT_BUTTON "font-button"

/* Variables */
GtkBuilder *ui;

GHashTable *hash;

GList *glist = NULL;
GList *glist_icon_themes = NULL;
GSList *kids = NULL;

gchar *homedir;  /* we use it a lot, so keep it handy */
gchar *execname;  /* == argv[0] */
gchar *newfont;  /* The name of a new font to use as the default if the user has
	       selected one. Otherwise NULL. */

GtkWidget *combo = NULL;
GtkWidget *icon_combo = NULL;  /* combo box for listing icon themes*/
GtkWidget *toolbar_combo = NULL;

gint hidden = 1;
gint preview_counter = 0;
gint gtk_rc_theme_index = -1;  /* index of a used GTK+ theme in a list */
gint gtk_rc_icontheme_index = -1;  /* index of a used icon theme in a list */

/* Functions */
static GList *get_dirs(void);
void preview_clicked(GtkWidget *button, gpointer data);
static void update_newfont (void);
void apply_clicked(GtkWidget *button, gpointer data);
static void ok_clicked(gchar *rc);
static void dock(void);
static void preview(gchar *rc_file);
static short install_tarball (gchar *path, gchar **rc_file);
static int switcheroo (gchar *actual);
void install_clicked (GtkWidget *w, gpointer data);
static void install_ok_clicked (GtkWidget *w, gint arg1, gpointer data);
static void search_for_theme_or_die_trying (gchar *actual, gchar **rc_file);
static void get_current_theme_params (void);
static GList *get_icon_themes_list (void);
void quit();
void set_new_font(GtkFontButton *button, gpointer _data);
void install_icons_clicked_callback(GtkButton *button, gpointer user_data);
void about_clicked_callback();
