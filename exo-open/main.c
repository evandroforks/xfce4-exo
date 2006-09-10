/* $Id$ */
/*-
 * Copyright (c) 2005-2006 Benedikt Meurer <benny@xfce.org>.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <glib/gstdio.h>

#include <exo/exo.h>



static gboolean opt_help = FALSE;
static gboolean opt_version = FALSE;
static gchar   *opt_launch = NULL;
static gchar   *opt_working_directory = NULL;

static GOptionEntry entries[] =
{
  { "help", '?', 0, G_OPTION_ARG_NONE, &opt_help, NULL, NULL, },
  { "version", 'v', 0, G_OPTION_ARG_NONE, &opt_version, NULL, NULL, },
  { "launch", 0, 0, G_OPTION_ARG_STRING, &opt_launch, NULL, NULL, },
  { "working-directory", 0, 0, G_OPTION_ARG_FILENAME, &opt_working_directory, NULL, NULL, },
  { NULL, },
};



static void
usage (void)
{
  g_print ("%s\n", _("Usage: exo-open [URLs...]"));
  g_print ("%s\n", _("       exo-open --launch TYPE [PARAMETERs...]"));
  g_print ("\n");
  g_print ("%s\n", _("  -?, --help                          Print this help message and exit"));
  g_print ("%s\n", _("  -v, --version                       Print version information and exit"));
  g_print ("\n");
  g_print ("%s\n", _("  --launch TYPE [PARAMETERs...]       Launch the preferred application of\n"
                     "                                      TYPE with the optional PARAMETERs, where\n"
                     "                                      TYPE is one of the following values."));
  g_print ("\n");
  g_print ("%s\n", _("  --working-directory DIRECTORY       Default working directory for applications\n"
                     "                                      when using the --launch option."));
  g_print ("\n");
  g_print ("%s\n", _("The following TYPEs are supported for the --launch command:"));
  g_print ("\n");

  /* Note to Translators: Do not translate the TYPEs (WebBrowser, MailReader, TerminalEmulator),
   * since the exo-helper utility will not accept localized TYPEs.
   */
  g_print ("%s\n", _("  WebBrowser       - The preferred Web Browser.\n"
                     "  MailReader       - The preferred Mail Reader.\n"
                     "  TerminalEmulator - The preferred Terminal Emulator."));
  g_print ("\n");
  g_print ("%s\n", _("If you don't specify the --launch option, exo-open will open all specified\n"
                     "URLs with their preferred URL handlers. Else, if you specify the --launch\n"
                     "option, you can select which preferred application you want to run, and\n"
                     "pass additional parameters to the application (i.e. for TerminalEmulator\n"
                     "you can pass the command line that should be run in the terminal)."));
  g_print ("\n");
}



int
main (int argc, char **argv)
{
  GOptionContext *context;
  GtkWidget      *dialog;
  GError         *err = NULL;
  gchar          *parameter;
  gint            result = EXIT_SUCCESS;

#ifdef GETTEXT_PACKAGE
  /* setup i18n support */
  xfce_textdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");
#endif

  /* try to parse the command line parameters */
  context = g_option_context_new (NULL);
  g_option_context_set_help_enabled (context, FALSE);
  g_option_context_add_main_entries (context, entries, GETTEXT_PACKAGE);
  g_option_context_add_group (context, gtk_get_option_group (TRUE));
  if (!g_option_context_parse (context, &argc, &argv, &err))
    {
      g_fprintf (stderr, "exo-open: %s.\n", err->message);
      g_error_free (err);
      return EXIT_FAILURE;
    }

  /* setup default icon for windows */
  gtk_window_set_default_icon_name ("preferences-desktop-default-applications");

  /* check what to do */
  if (G_UNLIKELY (opt_help))
    {
      usage ();
    }
  else if (G_UNLIKELY (opt_version))
    {
      g_print ("%s %s\n\n", g_get_prgname (), PACKAGE_VERSION);
      g_print (_("Copyright (c) 2005-2006\n"
                 "        os-cillation e.K. All rights reserved.\n\n"
                 "Written by Benedikt Meurer <benny@xfce.org>.\n\n"));
      g_print (_("%s comes with ABSOLUTELY NO WARRANTY,\n"
                 "You may redistribute copies of %s under the terms of\n"
                 "the GNU Lesser General Public License which can be found in the\n"
                 "%s source package.\n\n"), g_get_prgname (), g_get_prgname (), PACKAGE_TARNAME);
      g_print (_("Please report bugs to <%s>.\n"), PACKAGE_BUGREPORT);
    }
  else if (G_LIKELY (opt_launch != NULL))
    {
      /* combine all specified parameters to one parameter string */
      parameter = (argc > 1) ? g_strjoinv (" ", argv + 1) : NULL;

      /* run the preferred application */
      if (!exo_execute_preferred_application (opt_launch, parameter, opt_working_directory, NULL, &err))
        {
          /* display an error dialog */
          dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                           _("Failed to launch preferred application for category \"%s\"."),
                                           opt_launch);
          gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s.", err->message);
          gtk_dialog_run (GTK_DIALOG (dialog));
          gtk_widget_destroy (dialog);
          result = EXIT_FAILURE;
          g_error_free (err);
        }

      /* cleanup */
      g_free (parameter);
    }
  else if (argc > 1)
    {
      /* open all specified urls */
      for (argv += 1; *argv != NULL; ++argv)
        {
          if (!exo_url_show (*argv, NULL, &err))
            {
              /* display an error dialog */
              dialog = gtk_message_dialog_new (NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                               _("Failed to open URL \"%s\"."), *argv);
              gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog), "%s.", err->message);
              gtk_dialog_run (GTK_DIALOG (dialog));
              gtk_widget_destroy (dialog);
              result = EXIT_FAILURE;
              g_error_free (err);
              break;
            }
        }
    }
  else
    {
      result = EXIT_FAILURE;
      usage ();
    }

  return result;
}
