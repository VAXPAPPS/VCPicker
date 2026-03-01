#include <gtk/gtk.h>
#include <adwaita.h>
#include "ui/window.h"

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  VaxpWindow *window = vaxp_window_new (ADW_APPLICATION(app));
  vaxp_window_set_color_hex(window, "#3584e4"); // Default color
  gtk_window_present (GTK_WINDOW (window));
}

int
main (int    argc,
      char **argv)
{
  g_autoptr(AdwApplication) app = NULL;
  int status;

  app = adw_application_new ("org.vaxp.ColorPicker", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);

  return status;
}
