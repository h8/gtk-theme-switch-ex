/*
 * switch.c is a part of Gtk Theme Switch Ex software
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
 * switch.c and Gtk Theme Switch Ex is free software.
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

#include "switch.h"
#include "utils.h"
#include "preview_dialog.h"

#define INIT_GTK if (!using_gtk) { gtk_init (&argc, &argv); using_gtk = 1; }
#define OVECTOR 6

static void
get_current_theme_params (void)
{
  const gchar *rcfile = g_strconcat (homedir, "/.gtkrc-2.0", NULL);
  GIOChannel*  rc_file_io = NULL;
  GIOStatus status;
  GString *buffer = g_string_new ("");
  pcre *re_theme = NULL;
  pcre *re_font = NULL;
  pcre *re_icon = NULL;
  int re_result;
  const char *error;
  int erroffset;
  int string_length;
  int ovector[OVECTOR] = {0};
  const gchar *res = NULL;
  GList *list = NULL;
  int iter;

  rc_file_io = g_io_channel_new_file (rcfile, "r", NULL);
  if (!rc_file_io)
    return;
  g_io_channel_set_flags (rc_file_io, G_IO_FLAG_NONBLOCK, NULL);
  g_io_channel_set_encoding (rc_file_io, getenv ("LANG"), NULL);

  re_theme = pcre_compile ("^\\s*include.*themes/(.*)/gtk-2.0/gtkrc", 
			   PCRE_DOTALL | PCRE_UTF8, 
			   &error, &erroffset, NULL);
  re_font = pcre_compile ("^\\s*[gtk-]*font[-_]name=\"(.*)\"", 
			   PCRE_DOTALL | PCRE_UTF8, 
			   &error, &erroffset, NULL);
  re_icon = pcre_compile ("^\\s*gtk-icon-theme-name=\"(.*)\"", 
			   PCRE_DOTALL | PCRE_UTF8, 
			   &error, &erroffset, NULL);

  do
  {
    status = g_io_channel_read_line_string (rc_file_io, buffer, NULL, NULL);    
    string_length = (int)g_utf8_strlen (buffer->str, -1);

    /* Widgets theme name  */
    re_result = pcre_exec (re_theme, NULL, 
				 buffer->str, string_length,
				 0, 0, ovector, OVECTOR);
    if (re_result > 1)
    {
      pcre_get_substring (buffer->str, ovector, re_result, 1, &res);
      if (res)
	for(list = glist, iter = 0; 
	    list != NULL; list = list->next, iter++)
	  if (g_str_equal (list->data, res))
	    gtk_rc_theme_index  = iter;
      pcre_free_substring (res);
    }

    /* Font name  */
    re_result = pcre_exec (re_font, NULL, 
				 buffer->str, string_length,
				 0, 0, ovector, OVECTOR);
    if (re_result > 1)
    {
      pcre_get_substring (buffer->str, ovector, re_result, 1, &res);
      if (res)
	newfont = g_strdup (res);
      pcre_free_substring (res);
    }

    /* Icon theme */
    re_result = pcre_exec (re_icon, NULL, 
				 buffer->str, string_length,
				 0, 0, ovector, OVECTOR);
    if (re_result > 1)
    {
      pcre_get_substring (buffer->str, ovector, re_result, 1, &res);
      if (res)
	for(list = glist_icon_themes, iter = 0; 
	    list != NULL; list = list->next, iter++)
	  if (g_str_equal (list->data, res))
	    gtk_rc_icontheme_index  = iter;
      pcre_free_substring (res);
    }
    
  } while (status == G_IO_STATUS_NORMAL);

  pcre_free (re_theme);
  pcre_free (re_font);
  pcre_free (re_icon);
  
  g_string_free (buffer, TRUE);
  g_io_channel_shutdown (rc_file_io, TRUE, NULL);
  g_io_channel_unref (rc_file_io);
  g_free ((gchar *)rcfile);
}

static GList*
get_icon_themes_list (void)
{
  gchar *dirname;
  GDir *dir;
  struct stat stats;
  gchar *origdir = g_get_current_dir (); /* back up cwd */
  GList *list = NULL;
  G_CONST_RETURN gchar *dname;
	
  dirname = g_strconcat (homedir, "/.icons", NULL);
  g_chdir (dirname);
  dir = g_dir_open (dirname, 0, NULL);
  if (dir)
  {
    while ((dname = g_dir_read_name (dir)))
    {
      g_stat (dname, &stats);
      if (!S_ISDIR (stats.st_mode)) continue;
      if (!g_file_test (g_strconcat (dname, "/index.theme", NULL), 
			G_FILE_TEST_EXISTS)) continue; 
      list = 
	g_list_insert_sorted (list, g_strdup (dname), (GCompareFunc)strcmp);
    }
  }
  g_dir_close (dir);
 
  /* DO NOT FREE dirname!! */
  /* We are supposed to. */
  /* But we need our own copy anyway, to store in the hash table */

  dirname = "/usr/share/icons";  /* dirty hack - hardcoded */
  g_chdir (dirname);
  dir = g_dir_open (dirname, 0, NULL);
  if (dir)
  {
    while ((dname = g_dir_read_name (dir)))
    {
      g_stat (dname, &stats);
      if (!S_ISDIR (stats.st_mode)) continue;
      if (!g_file_test (g_strconcat (dname, "/index.theme", NULL), 
			G_FILE_TEST_EXISTS)) continue; 
      //g_hash_table_insert (hash, g_strdup (dname), dirname);
      list = 
	g_list_insert_sorted (list, g_strdup (dname), (GCompareFunc)strcmp);
    }
  }
	
  /* DO NOT FREE dirname!! */
  /* We are supposed to. */
  /* But we need our own copy anyway, to store in the hash table */

  g_chdir (origdir); /* Go back to where we were */
  g_free (origdir); /* Now go play like a good little program */
  g_dir_close (dir);

  return list;
}

static GList*
get_dirs (void) 
{
  gchar *dirname;
  GDir *dir;
  struct stat stats;
  gchar *origdir = g_get_current_dir (); /* back up cwd */
  GList *list = NULL;
  G_CONST_RETURN gchar *dname;
	
  dirname = g_strconcat (homedir, "/.themes", NULL);
  g_chdir (dirname);
  dir = g_dir_open (dirname, 0, NULL);
  if (dir)
  {
    while ((dname = g_dir_read_name (dir)))
    {
      g_stat (dname, &stats);
      if (!S_ISDIR (stats.st_mode)) continue;
      if (!g_file_test (g_strconcat (dname, "/gtk-2.0/gtkrc", NULL), 
			G_FILE_TEST_EXISTS)) continue; 
      g_hash_table_insert (hash, g_strdup (dname), dirname);
      list = 
	g_list_insert_sorted (list, g_strdup (dname), (GCompareFunc)strcmp);
    }
  }
  g_dir_close (dir);
 
  /* DO NOT FREE dirname!! */
  /* We are supposed to. */
  /* But we need our own copy anyway, to store in the hash table */

  dirname = gtk_rc_get_theme_dir ();
  g_chdir (dirname);
  dir = g_dir_open (dirname, 0, NULL);
  if (dir)
  {
    while ((dname = g_dir_read_name (dir)))
    {
      g_stat (dname, &stats);
      if (!S_ISDIR (stats.st_mode)) continue;
      if (!g_file_test (g_strconcat (dname, "/gtk-2.0/gtkrc", NULL), 
			G_FILE_TEST_EXISTS)) continue; 
      g_hash_table_insert (hash, g_strdup (dname), dirname);
      list = 
	g_list_insert_sorted (list, g_strdup (dname), (GCompareFunc)strcmp);
    }
  }
	
  /* DO NOT FREE dirname!! */
  /* We are supposed to. */
  /* But we need our own copy anyway, to store in the hash table */

  g_chdir (origdir); /* Go back to where we were */
  g_free (origdir); /* Now go play like a good little program */
  g_dir_close (dir);

  return list;
}

void
preview_clicked(GtkWidget *button, gpointer data)
{
  G_CONST_RETURN gchar *entry; 
  gchar *dir; 
  gchar *rc;
  gchar *actual;

  entry = gtk_combo_box_get_active_text (GTK_COMBO_BOX (combo));
  if (entry)
  {
    dir = g_strconcat (g_hash_table_lookup (hash, entry), "/", NULL);
    rc = g_strconcat (dir, entry, NULL);
    actual = g_strconcat (dir, entry, NULL);
    rc = g_strconcat (rc, "/gtk-2.0/gtkrc", NULL);
    update_newfont ();
    preview (rc);
    g_free (rc);
    g_free (actual);
    g_free (dir);
  }
}

/* Update 'newfont' */
static void 
update_newfont (void)
{
  GtkWidget *use_button = GTK_WIDGET(gtk_builder_get_object(ui, 
							    USE_FONT_TOGGLE));
  GtkWidget *font_button = GTK_WIDGET(gtk_builder_get_object(ui, FONT_BUTTON));

  if (newfont) g_free (newfont);

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(use_button)))
  {
    G_CONST_RETURN gchar *newerfont =
      gtk_font_button_get_font_name(GTK_FONT_BUTTON(font_button));
    if (newerfont && newerfont[0])
    {
      newfont = g_strdup(newerfont);
    }
  }
  else newfont = NULL;
}

void 
apply_clicked(GtkWidget *button, gpointer data)
{
  G_CONST_RETURN gchar *entry = 
    gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo));
  gchar *dir = g_hash_table_lookup(hash,entry);
  gchar *name = g_strdup_printf ("%s/%s/gtk-2.0/gtkrc", dir, entry);

  update_newfont ();

  ok_clicked(name);
  g_free(name);

  /* make sure we get the ClientEvent ourselves */
  while (gtk_events_pending())
    gtk_main_iteration();
}

static void
write_rc_file (gchar *include_file, gchar *path)
{
  GtkWidget *use_button;

  /* first - save backup */
  gchar *bak = g_strconcat (path, ".gts-ex-save", NULL);
  g_rename (path, bak);

  rcfile_data *data = g_new(rcfile_data, 1);
  data->gtkrc_file = include_file;

  use_button = GTK_WIDGET(gtk_builder_get_object(ui, USE_FONT_TOGGLE));
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (use_button)))
    data->font_name = newfont;
  else
    data->font_name = NULL;

  use_button = GTK_WIDGET(gtk_builder_get_object(ui, USE_ICON_TOGGLE));
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (use_button)))
    data->icontheme_name = 
      gtk_combo_box_get_active_text (GTK_COMBO_BOX (icon_combo));
  else
    data->icontheme_name = NULL;
  
  dump_rcfile(path, data);

  g_free(data);
}

static void
ok_clicked (gchar *rc_file)
{
  /* Write the config file to disk */
  gchar *path_to_gtkrc = g_strdup_printf("%s/.gtkrc-2.0", homedir);
  write_rc_file (rc_file, path_to_gtkrc);
  send_refresh_signal();
  g_free(path_to_gtkrc);
}

/*static void
preview_apply_clicked (gchar *rc_file)
{*/
  /* Write the config file to disk */
/*  gchar *path_to_gtkrc = g_strdup_printf("%s/.gtkrc-2.0", homedir);
  rename (rc_file, path_to_gtkrc);
  send_refresh_signal();
  g_free(path_to_gtkrc);
  }*/

void 
install_clicked (GtkWidget *w, gpointer data)
{
  GtkWidget *fc;
  GtkWidget *checkbutton = 
    gtk_check_button_new_with_label ("Switch to theme after installation");
  fc = gtk_file_chooser_dialog_new ("Select a GTK theme tarball",
				    NULL,
				    GTK_FILE_CHOOSER_ACTION_OPEN,
				    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				    GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				    NULL);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG(fc)->vbox), checkbutton,
		      FALSE, FALSE, 0);
  gtk_widget_show (checkbutton);

  g_signal_connect(G_OBJECT (fc), "response",
		   G_CALLBACK(install_ok_clicked), fc);
  gtk_dialog_run (GTK_DIALOG (fc));
  gtk_widget_destroy (fc);
}

static void
install_ok_clicked (GtkWidget *w, gint arg1, gpointer data)
{
  if (arg1 != GTK_RESPONSE_ACCEPT)
    return;

  gchar *rc_file, *beginning_of_theme_name, *thn;
  G_CONST_RETURN gchar *filename;
  gint i, pos;
  short slashes = 0;
  gboolean cbstate = 
    gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (g_object_get_data(G_OBJECT(data), "checkbutton")));
  filename = 
    gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (data));
  thn = g_strdup(filename);
  search_for_theme_or_die_trying(thn, &rc_file);
  g_free(thn);
  gtk_widget_destroy(GTK_WIDGET(data));
  /* ok, we're like evil or something, but that won't stop us */
  for (i=strlen(rc_file); i != 0; --i)
  {
    if (rc_file[i] == '/') ++slashes;
    if (slashes == 2) { rc_file[i] = '\0'; break; }
  }
  beginning_of_theme_name = rc_file;
  for (i=strlen(rc_file) /*different*/; i != 0; --i)
  {
    if (rc_file[i] == '/') { beginning_of_theme_name = &rc_file[i+1]; break; }
  }
  /* we've been very naugthy, but we should have the theme's NAME now..
   * it's about time. */
  /* get the list item that contains this */
  pos = g_list_position (glist, g_list_find_custom (glist, 
						    beginning_of_theme_name, 
						    (GCompareFunc) strcmp));
  if (pos != -1)
    /* set combo's item to the newly-installed theme */
    gtk_combo_box_set_active (GTK_COMBO_BOX (combo), pos);
	
  if (cbstate) /* checkbutton pressed */
    apply_clicked(NULL, NULL);

  /* I guess we should free this... */
  g_free (rc_file);
}

void 
set_new_font(GtkFontButton *button, gpointer _data)
{
  if (newfont)
    g_free(newfont);

  newfont = g_strdup(gtk_font_button_get_font_name(button));
}

void 
quit()
{
  GSList *l;
  for (l = kids; l ; l=l->next) 
  {
    kill(GPOINTER_TO_INT(l->data), 9);
  }

  g_object_unref(ui);

  gtk_main_quit();
}

static void
dock (void)
{
  GList *list = NULL;

  GtkWidget *win;
  GtkWidget *box;
  GtkWidget *use_button;

  G_CONST_RETURN gchar *ui_path = 
    g_build_filename(DATADIR, APPNAME, "ui_main.xml", NULL);

  glist = get_dirs();
  glist_icon_themes = get_icon_themes_list();
  get_current_theme_params();

  ui = gtk_builder_new();
  gtk_builder_add_from_file(ui, ui_path, NULL);
  gtk_builder_connect_signals(ui, NULL);

  g_free((gchar *)ui_path);

  /* Theme combo box */
  combo = gtk_combo_box_new_text();
  box = GTK_WIDGET(gtk_builder_get_object(ui, "mw-theme-hbox"));
	
  for(list = glist; list != NULL; list = list->next)
  {
    gtk_combo_box_append_text (GTK_COMBO_BOX (combo), list->data);
  }
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), gtk_rc_theme_index);

  gtk_box_pack_start(GTK_BOX(box), combo, TRUE, TRUE, FALSE);

  /* Icon theme  */
  use_button = GTK_WIDGET(gtk_builder_get_object(ui, USE_ICON_TOGGLE));
  box = GTK_WIDGET(gtk_builder_get_object(ui, "mw-icon-hbox"));
  icon_combo = gtk_combo_box_new_text();

  for (list = glist_icon_themes; list != NULL; list = list->next)
  {
    gtk_combo_box_append_text(GTK_COMBO_BOX(icon_combo), list->data);
  }

  if (gtk_rc_icontheme_index >= 0)
  {
    gtk_combo_box_set_active(GTK_COMBO_BOX(icon_combo), gtk_rc_icontheme_index);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(use_button), TRUE);
  }

  gtk_box_pack_start(GTK_BOX(box), icon_combo, TRUE, TRUE, FALSE);
  
  /* Font */
  GtkWidget *font_button = 
    GTK_WIDGET(gtk_builder_get_object(ui, FONT_BUTTON));
  use_button = GTK_WIDGET(gtk_builder_get_object(ui, USE_FONT_TOGGLE));

  if (newfont)
  {
    gtk_font_button_set_font_name(GTK_FONT_BUTTON(font_button), newfont);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(use_button), TRUE);
    g_free (newfont);
  }

  /* Main Window */
  win = GTK_WIDGET(gtk_builder_get_object(ui, "main-window"));
  gtk_widget_show_all(win);
}
	
static void
preview (gchar *rc_file)
{
  FILE *pipe;
  gint got_it = 0, it_died = 0;
  gchar *line;
  gchar *path = g_strdup_printf ("%s/.gtkrc.tmp-%i", homedir, preview_counter);
  gchar *command = g_strdup_printf ("%s -_dont-use-this %s &", execname, path);
  write_rc_file (rc_file, path);
	
  pipe = popen(command,"r");
     	
  if (pipe == NULL) {
    g_print("gts: error previewing\n");
    return;
  }

  fcntl(fileno(pipe), F_SETFL, O_NONBLOCK);
     	
  line = (gchar *)g_malloc(1024);     
  while(!feof(pipe)) {
    fgets(line,1024,pipe);
    line[strlen(line)-1] = '\0';	       
    if (!got_it && !g_strncasecmp(line,"pid=",4)) {
      kids = g_slist_append(kids,GINT_TO_POINTER(atoi(line+4)));
      got_it = 1;
    } else if (!it_died && !g_strncasecmp(line,"die=",4)) {
      kids = g_slist_remove(kids,GINT_TO_POINTER(atoi(line+4)));
      it_died = 1;
      break;
    }
	     
    while (gtk_events_pending())
      gtk_main_iteration();
    usleep(50000);
  }
            
  pclose(pipe);
  g_free (line);
  g_free (path);
  g_free (command);
  ++preview_counter;
}

static short 
install_tarball (gchar *path, gchar **rc_file)
{
  gchar *command, *themedir;
  gint result;
  GList *new_list, *new_theme;
  GList *list = NULL;

  themedir = g_strdup_printf ("%s/.themes", homedir);

  if (path[0] != '/')
  {
    gchar *cwd = g_get_current_dir();
    command = g_strdup_printf ("tar --directory %s -xzf %s/%s 2>/dev/null", 
			       themedir, cwd, path);
    g_free (cwd);
  }
  else
    command = g_strdup_printf ("tar --directory %s -xzf %s 2>/dev/null", 
			       themedir, path);

  /* Ensure that ~/.themes exists */
  mkdir (themedir, 755);

  result = system(command);
  g_free (command);
  g_free (themedir);
  if (result != EXIT_SUCCESS)
    return 0;
  /* We don't do that anymore. Now we find the first new directory
   * in either themedir, and presume that to be the name of the new theme.
   * NOT FOOLPROOF, but good as long as you don't mess with stuff while the
   * program is running. At least better than deriving the theme name from
   * the tarball name. Ugh. */
	
  new_list = get_dirs();
  new_theme = compare_glists(glist, new_list, (GCompareFunc)strcmp);
  if (new_theme)
  {
    result = is_installed_theme(new_theme->data, rc_file, homedir);
    g_list_foreach (glist, (GFunc)g_free, NULL);
    g_list_free (glist);
    glist = new_list;
		
    /* Update combo, but only in dock mode */
    if (combo)	
    {
      gtk_list_store_clear (GTK_LIST_STORE (gtk_combo_box_get_model (GTK_COMBO_BOX(combo))));
      for(list = glist; list != NULL; list = list->next)
      {
	gtk_combo_box_append_text(GTK_COMBO_BOX(combo), list->data);
      }
    }
  }
  else
  {
    gchar *interestingpath, basename[1024];
    g_list_foreach (new_list, (GFunc)g_free, NULL);
    g_list_free (new_list);
    /* fall back to our old hack if no new dir was created, say if the theme
     * was already installed... */
	
    /* Set rc_file to our best darn guess of the resultant gtk theme
     * dir/gtk/gtkrc. This is very tricky. The hack that is used to is
     * to return the segment in path between the last slash and the
     * first dot or dash after that. */
    interestingpath = &path[strlen(path)-1];
    while (interestingpath != path 
	   && *(interestingpath-1) != '/') interestingpath--;
    strcpy (basename, interestingpath);
    for (interestingpath = &basename[0]; 
	 *interestingpath != '\0'; ++interestingpath)
    {
      if (*interestingpath == '-' || *interestingpath == '.')
      {
	*interestingpath = '\0';
	break;
      }
    }
    result = is_installed_theme(basename, rc_file, homedir);
  }
	
  return result;
}

static void
search_for_theme_or_die_trying (gchar *actual, gchar **rc_file)
{
  if (!is_themedir(actual, rc_file) &&
      !install_tarball(actual, rc_file) &&
      !is_installed_theme(actual, rc_file, homedir))
  {
    fprintf (stderr, "\
%s: Sorry, \"%s\" does not appear to be a valid theme directory or tarball!\n", execname, actual);
    exit (EXIT_FAILURE);
  }
}

static gint
switcheroo (gchar *actual)
{
  gchar *rc_file;
  search_for_theme_or_die_trying (actual, &rc_file);
  /* If we get here, we actually found the theme */
  ok_clicked(rc_file);
  g_free (rc_file);
  return EXIT_SUCCESS;
}



int 
main (int argc, char *argv[])
{
  gchar *rc_file;
  gchar *actual;
  gint i;
  gint result = EXIT_SUCCESS;
  GSList *l=NULL;
  short using_gtk = 0;

  newfont = 0;
  execname = argv[0];
  homedir = getenv("HOME");
  hash = g_hash_table_new (g_str_hash, g_str_equal);

  if (argc == 1) /* start in dock mode auto if no args */
  {
    INIT_GTK
      dock();
  }

  else if (strcmp(argv[1], "-_dont-use-this") == 0)
  {
    show_preview_dialog(argv[2]);
    exit(0);
  }				  
	
  for (i=1; i != argc; ++i)
  {
    if (argv[i][0] == '-')
    {
      /* Single-arg commands/options */
      if (argv[i][1] == 'd')
      {
	INIT_GTK
	  dock();
	continue;
      }
			
      /* Double-arg commands/options */
      if (i+1 != argc)
      {
	if (argv[i][1] == 'p')
	{
	  glist = get_dirs ();
	  actual = argv[++i];
	  if (!is_themedir(actual, &rc_file) 
	      && !install_tarball(actual, &rc_file) 
	      && !is_installed_theme(actual, &rc_file, homedir))
	  {
	    fprintf (stderr, "\
%s: Sorry, \"%s\" does not appear to be a valid theme directory or tarball!\n", execname, actual);
	    result = EXIT_FAILURE;
	  }
	  else
	  {
	    preview (rc_file);
	    g_free (rc_file);
	  }
	  continue;
	}

	if (argv[i][1] == 'i')
	{
	  actual = argv[++i];
	  if (!install_tarball(actual, &rc_file))
	  {
	    fprintf (stderr, "\
%s: Sorry, \"%s\" does not appear to be a valid theme tarball!\n", 
		     execname, actual);
	  }
	  result = EXIT_FAILURE;
	  continue;
	}

	if (argv[i][1] == 'f')
	{
	  newfont = g_strdup (argv[++i]);
	  continue;
	}
      }

      /* if this starts with a minus, it's either an unrecognized command
       * or -help. Perfect */
      usage();
      result = EXIT_FAILURE;
      continue;
    }
    /* got here... fallback and assume it's a theme */
    glist = get_dirs ();
    gtk_init (&argc, &argv);
    result |= switcheroo(argv[i]);
  }

  if (using_gtk) {
    gtk_main();
    for (l=kids;l;l=l->next) {
      kill(GPOINTER_TO_INT(l->data), 9);
			
    }
  }
     
  return result;
}

