#include "engine.h"
#include "global_vars.h"
#include "karea.h" // karea_draw
#include <stdlib.h> // exit
#include <ctype.h> // isspace
#include <errno.h> // errno

gboolean engine_input_handler (GIOChannel *source, GIOCondition condition, gpointer data) {
    static gchar *p;
    static gchar *line;
    GError *err = NULL;
    GIOStatus status;
    int i;

    status = g_io_channel_read_line (from_engine, &line, NULL, NULL, &err);
    switch (status) {
        case G_IO_STATUS_ERROR:
            g_printerr ("Error reading from engine: %s\n", err->message);
            exit(1);
            break;
        case G_IO_STATUS_NORMAL:
            break;
        case G_IO_STATUS_EOF:
            g_printerr ("Engine no longer exists");
            exit (1);
            break;
        case G_IO_STATUS_AGAIN:
            g_assert_not_reached ();
            break;
    }

    if (line[0] == 'K') {
        unsigned int t1, t2;
        p = line+1;
        for (i=0; i<MAX_GUESSES; i++) {
            while (*p && isspace(*p)) p++;
            
            if (!*p || sscanf(p, "%2x%2x", &t1, &t2) != 2) {
                i--;
                break;
            }
            
            kanjiguess[i].d[0] = t1;
            kanjiguess[i].d[1] = t2;
            while (*p && !isspace(*p)) p++;
        }
        
        num_guesses = i+1;
        karea_draw(karea);
    }

    g_free (line);

    return TRUE;
}

/* Open the connection to the engine */
void init_engine() {
    gchar *argv[] = { BINDIR G_DIR_SEPARATOR_S "kpengine", "--data-file", NULL, NULL };
    GError *err = NULL;
    gchar *uninstalled;
    int stdin_fd, stdout_fd;

    uninstalled = g_build_filename (".", "kpengine", NULL);
    if (g_file_test (uninstalled, G_FILE_TEST_EXISTS))
        argv[0] = uninstalled;

    if (data_file)
        argv[2] = data_file;
    else
        argv[1] = NULL;

    if (!g_spawn_async_with_pipes (NULL, /* working directory */
				 argv, NULL,	/* argv, envp */
				 0,
				 NULL, NULL,	/* child_setup */
				 &engine_pid,   /* child pid */
				 &stdin_fd, &stdout_fd, NULL,
				 &err)) {
        GtkWidget *dialog;

        dialog = gtk_message_dialog_new (NULL, 0,
				       GTK_MESSAGE_ERROR,
				       GTK_BUTTONS_OK,
				       "Could not start engine '%s': %s",
				       argv[0], err->message);
        gtk_dialog_run (GTK_DIALOG (dialog));
        g_error_free (err);
        exit (1);
    }

    g_free (uninstalled);

    if (!(to_engine = g_io_channel_unix_new (stdin_fd)))
        g_error ("Couldn't create pipe to child process: %s", g_strerror(errno));
    if (!(from_engine = g_io_channel_unix_new (stdout_fd)))
        g_error ("Couldn't create pipe from child process: %s", g_strerror(errno));

    g_io_add_watch (from_engine, G_IO_IN, engine_input_handler, NULL);
}
