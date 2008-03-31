/*
 * utils.c is a part of Gtk Theme Switch Ex software
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
 * utils.c and Gtk Theme Switch Ex is free software.
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

void
usage (void)
{
  g_printf("\
Command line options:\n\
-h[elp]\t\t\t(display this help message)\n\
-d[ock]\t\t\t(open dock)\n\
file\t\t\t(switch to theme (install if theme is a tarball))\n\
-p[review] file\t\t(preview a theme (installs if file is a tarball))\n\
-i[nstall] theme.tar.gz\t(install a .tar.gz)\n\
-f[ont] fontstring\t(set GTK's main font to fontstring)\n\n\
\
Passing no command line arguments will cause application to start in dock-mode)\n\n\
\
\"file\" represents any one of (looked for in the listed order):\n\
1) An absoulte or relative path to a GTK theme directory.\n\
   A directory is considered a theme directory if it contains a\n\
   gtk/gtkrc file.\n\
2) A gzipped tar file which expands to a GTK theme directory as explained in 1).\n\
3) A GTK theme directory with the name passed located in ~/.themes.\n\
4) A GTK theme directory with the name passed located in the global\n\
   \"gtk_themedir\"\n\
If none of these files are located and found to be correct, the program will\n\
exit with an error.\n");
}

/* Sets rc_file to the rc_file of the theme if the result is true.
 * It is the caller's repsonsibility to free rc_file */
gboolean 
is_themedir (gchar *path, gchar **rc_file)
{
  gchar *test_rc_file;
  struct stat info;
  test_rc_file = g_strdup_printf ("%s/gtk-2.0/gtkrc",path);
  if (stat(test_rc_file, &info) == 0 
      && (S_ISREG(info.st_mode) || S_ISLNK(info.st_mode)))
  {
    /* $path/gtk/gtkrc exists, and is a regular file */
    *rc_file = test_rc_file;
    return TRUE;
  }
  else
  {
    g_free (test_rc_file);
    return FALSE;
  }
}

gboolean 
is_installed_theme (gchar *path, 
		    gchar **rc_file, gchar *homedir)
{
  gchar *gtk_rc_theme_dir = gtk_rc_get_theme_dir();
  /* don't strlen things twice when computing the size to use for fullpath */
  gint path_len = strlen(path), homedir_len = strlen(homedir);
  /* ditto with get_theme_dir */
  gint gtk_rc_theme_dir_len = strlen(gtk_rc_theme_dir);
  gchar *fullpath = (gchar *) g_malloc (MAX(homedir_len+path_len+10,
					    gtk_rc_theme_dir_len+path_len+1));
  gboolean result;

  /* use memcpy since we already have the lengths */
  memcpy (fullpath, homedir, homedir_len);
  memcpy (fullpath+homedir_len, "/.themes/", 9);
  /* add 1 to length so we get the null char too */
  memcpy (fullpath+homedir_len+9, path, path_len+1);

  if (is_themedir(fullpath, rc_file))
  {
    g_free(fullpath);
    return TRUE;
  }
  memcpy (fullpath, gtk_rc_theme_dir, gtk_rc_theme_dir_len);
  /* add 1 to length so we get the null char too */
  memcpy (fullpath+gtk_rc_theme_dir_len, path, path_len+1);
	
  result = is_themedir(fullpath, rc_file);

  g_free(fullpath);

  return result;
}


GList*
compare_glists (GList *t1, GList *t2, GCompareFunc cmpfunc)
{
  /* Find the first element in t2 that does not exist in t1.
   * Uses the supplied cmpfunc() for determining equality of list->data
   * strcmp() is the recommended compare function */
  GList *tmp1;
  for (; t2; t2=t2->next)
  { 
    short matched = 0;
    for (tmp1=t1; tmp1; tmp1=tmp1->next)
      if ((*cmpfunc)(tmp1->data, t2->data) == 0) { matched = 1; break; }
    if (!matched) return t2;
  }
  return 0;
}

void
send_refresh_signal(void)
{
  GdkEventClient event;
  event.type = GDK_CLIENT_EVENT;
  event.send_event = TRUE;
  event.window = NULL;
  event.message_type = gdk_atom_intern("_GTK_READ_RCFILES", FALSE);
  event.data_format = 8;
  gdk_event_send_clientmessage_toall((GdkEvent *)&event);
}
