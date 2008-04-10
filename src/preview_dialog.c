/*
 * preview_dialog.c is a part of Gtk Theme Switch Ex software
 * Gtk Theme Switch Ex, a fast and small Gtk theme switching utility 
 * that has a GUI and console interface.
 *
 * Copyright (C) Dmitry Stropaloff 2008 <helions8@gmail.com>
 *
 * preview_dialog.c and Gtk Theme Switch Ex is free software.
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

#include "utils.h"
#include "preview_dialog.h"


void 
show_preview_dialog(gchar *rcfile)
{
  GtkWidget *win;
  GtkListStore *store;
  GtkWidget *view;
  gchar *default_files[] = {rcfile, NULL};
  G_CONST_RETURN gchar *ui_path = 
    g_build_filename(DATADIR, APPNAME, "ui_preview.xml", NULL);

  rc_file = rcfile;

  gtk_rc_set_default_files(default_files);
  gtk_init(NULL, NULL);

  ui = gtk_builder_new();
  gtk_builder_add_from_file(ui, ui_path, NULL);
  gtk_builder_connect_signals(ui, NULL);
  g_free((gchar *)ui_path);

  /* Icon view */
  store = create_icons_store();
  view = GTK_WIDGET(gtk_builder_get_object(ui, "icon-view"));
  gtk_icon_view_set_model(GTK_ICON_VIEW(view), GTK_TREE_MODEL(store));
  g_object_unref(store);

  /* Tree view  */
  store = create_list_store();
  view = GTK_WIDGET(gtk_builder_get_object(ui, "tree-view"));
  gtk_tree_view_set_model(GTK_TREE_VIEW(view), GTK_TREE_MODEL(store));
  g_object_unref(store);

  /* Main Window */
  win = GTK_WIDGET(gtk_builder_get_object(ui, "preview-window"));
  gtk_widget_show_all(win);

  gtk_main();
}

GtkListStore*
create_icons_store()
{
  GdkPixbuf *pix;
  GtkTreeIter iter;

  GtkListStore *store = gtk_list_store_new(2, GDK_TYPE_PIXBUF, G_TYPE_STRING);
  GtkIconTheme* theme = gtk_icon_theme_get_default();

  G_CONST_RETURN gchar *names_file_path = 
    g_build_filename(DATADIR, APPNAME, "icon-names", NULL);

  GIOChannel*  file_io = NULL;
  GIOStatus status;
  GString *buffer = g_string_new("");

  file_io = g_io_channel_new_file(names_file_path, "r", NULL);

  if (!file_io)
    return NULL;

  g_io_channel_set_flags(file_io, G_IO_FLAG_NONBLOCK, NULL);
  g_io_channel_set_encoding(file_io, getenv("LANG"), NULL);


  do
  {
    status = g_io_channel_read_line_string(file_io, buffer, NULL, NULL);
    buffer = g_string_truncate(buffer, buffer->len - 1);

    pix = gtk_icon_theme_load_icon(theme, buffer->str, 32, 0, NULL);

    if (pix != NULL)
    {
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter, 0, pix, 1, buffer->str, -1);
    }
  } while (status == G_IO_STATUS_NORMAL);

  g_string_free(buffer, TRUE);
  g_free((gchar *)names_file_path);
  g_io_channel_shutdown(file_io, TRUE, NULL);
  g_io_channel_unref(file_io);

  return store;
}

GtkListStore *create_list_store()
{
  int it;

  GtkListStore *store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
  GtkTreeIter iter;

  for (it = 0; it < 5; it++)
  {
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter,
		       0, g_strdup_printf("Item %i", it),
		       1, g_strdup_printf("Item %i value", it), -1);
  }

  return store;
}

gboolean 
delete_callback(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  g_unlink(rc_file);

  g_object_unref(ui);
  gtk_main_quit();

  return FALSE;
}

void 
apply_clicked_callback(GtkButton *button, gpointer user_data)
{
  /* Write the config file to disk */
  gchar *path_to_gtkrc = g_strdup_printf("%s/.gtkrc-2.0", g_get_home_dir());
  g_rename(rc_file, path_to_gtkrc);
  send_refresh_signal();
  g_free(path_to_gtkrc);
}
