#include "picker.h"
#include <gio/gio.h>

static guint signal_id = 0;
static VaxpColorPickedCallback current_callback = NULL;
static gpointer current_user_data = NULL;
static gchar *current_request_path = NULL;

static void
on_response_signal (GDBusConnection *connection,
                    const gchar     *sender_name,
                    const gchar     *object_path,
                    const gchar     *interface_name,
                    const gchar     *signal_name,
                    GVariant        *parameters,
                    gpointer         user_data)
{
    guint32 response;
    GVariant *results = NULL;

    if (g_variant_n_children (parameters) < 2) goto out;

    g_variant_get (parameters, "(u@a{sv})", &response, &results);

    if (response == 0 && results != NULL) {
        GVariant *color_var = g_variant_lookup_value(results, "color", G_VARIANT_TYPE("(ddd)"));
        if (color_var) {
            double r, g, b;
            g_variant_get(color_var, "(ddd)", &r, &g, &b);
            gchar *hex = g_strdup_printf("#%02X%02X%02X", 
                (int)(r * 255.0 + 0.5), 
                (int)(g * 255.0 + 0.5), 
                (int)(b * 255.0 + 0.5));
            
            if (current_callback) {
                current_callback(hex, current_user_data);
            }
            g_free(hex);
            g_variant_unref(color_var);
        }
    }

    if (results) g_variant_unref (results);

out:
    g_dbus_connection_signal_unsubscribe (connection, signal_id);
    signal_id = 0;
    current_callback = NULL;
    current_user_data = NULL;
    g_clear_pointer(&current_request_path, g_free);
}

static void
pick_color_ready_cb (GObject      *source_object,
                     GAsyncResult *res,
                     gpointer      user_data)
{
    GDBusConnection *connection = G_DBUS_CONNECTION (source_object);
    GError *error = NULL;
    GVariant *result;
    gchar *request_path = NULL;

    result = g_dbus_connection_call_finish (connection, res, &error);
    if (!result) {
        g_warning ("PickColor call failed: %s", error->message);
        g_error_free (error);
        return;
    }

    g_variant_get (result, "(o)", &request_path);
    current_request_path = g_strdup(request_path);

    signal_id = g_dbus_connection_signal_subscribe (
        connection,
        "org.freedesktop.portal.Desktop",
        "org.freedesktop.portal.Request",
        "Response",
        request_path,
        NULL,
        G_DBUS_SIGNAL_FLAGS_NO_MATCH_RULE,
        on_response_signal,
        NULL,
        NULL
    );

    g_free (request_path);
    g_variant_unref (result);
}

void vaxp_portal_pick_color(VaxpColorPickedCallback callback, gpointer user_data) {
    g_autoptr(GError) error = NULL;
    GDBusConnection *connection = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);
    if (!connection) {
        g_warning("Failed to connect to session bus: %s", error->message);
        return;
    }

    if (signal_id != 0) {
        g_warning("A color picking request is already in progress");
        return;
    }

    current_callback = callback;
    current_user_data = user_data;

    GVariantBuilder opt_builder;
    g_variant_builder_init (&opt_builder, G_VARIANT_TYPE_VARDICT);

    g_dbus_connection_call (
        connection,
        "org.freedesktop.portal.Desktop",
        "/org/freedesktop/portal/desktop",
        "org.freedesktop.portal.Screenshot",
        "PickColor",
        g_variant_new ("(sa{sv})", "", &opt_builder),
        G_VARIANT_TYPE ("(o)"),
        G_DBUS_CALL_FLAGS_NONE,
        -1,
        NULL,
        pick_color_ready_cb,
        NULL
    );
    
    // connection ref is transferred internally or we don't unref sync connection
    g_object_unref(connection);
}
