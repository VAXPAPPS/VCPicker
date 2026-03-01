#ifndef VAXP_PORTAL_PICKER_H
#define VAXP_PORTAL_PICKER_H

#include <glib.h>

G_BEGIN_DECLS

typedef void (*VaxpColorPickedCallback)(const gchar *hex_color, gpointer user_data);

void vaxp_portal_pick_color(VaxpColorPickedCallback callback, gpointer user_data);

G_END_DECLS

#endif /* VAXP_PORTAL_PICKER_H */
