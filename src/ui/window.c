#include "window.h"
#include "../core/color.h"
#include "../core/palette.h"
#include "../portal/picker.h"
#include "headerbar.h"
#include <string.h>

struct _VaxpWindow {
    GtkApplicationWindow parent_instance;

    GtkWidget *header_container;
    GtkWidget *color_preview_box;
    GtkWidget *hex_entry;
    GtkWidget *rgb_entry;
    GtkWidget *hsl_entry;
    GtkWidget *hsv_entry;
    GtkWidget *cmyk_entry;
    
    GtkWidget *copy_hex_btn;
    GtkWidget *copy_rgb_btn;
    GtkWidget *copy_hsl_btn;
    GtkWidget *copy_hsv_btn;
    GtkWidget *copy_cmyk_btn;

    GtkWidget *wcag_white_label;
    GtkWidget *wcag_black_label;
    GtkWidget *comp_color_box;
    GtkWidget *comp_hex_label;
};

G_DEFINE_TYPE(VaxpWindow, vaxp_window, GTK_TYPE_APPLICATION_WINDOW)

static void update_colors_from_hex(VaxpWindow *self, const gchar *hex) {
    if (!hex || strlen(hex) < 4) return;
    
    VaxpColorRGB rgb = vaxp_color_hex_to_rgb(hex);
    VaxpColorHSL hsl = vaxp_color_rgb_to_hsl(&rgb);
    VaxpColorHSV hsv = vaxp_color_rgb_to_hsv(&rgb);
    VaxpColorCMYK cmyk = vaxp_color_rgb_to_cmyk(&rgb);

    gchar *rgb_str = g_strdup_printf("rgb(%d, %d, %d)", 
        (int)(rgb.r * 255.0 + 0.5), (int)(rgb.g * 255.0 + 0.5), (int)(rgb.b * 255.0 + 0.5));
    gchar *hsl_str = g_strdup_printf("hsl(%.0f, %.0f%%, %.0f%%)", 
        hsl.h, hsl.s * 100.0, hsl.l * 100.0);
    gchar *hsv_str = g_strdup_printf("hsv(%.0f, %.0f%%, %.0f%%)", 
        hsv.h, hsv.s * 100.0, hsv.v * 100.0);
    gchar *cmyk_str = g_strdup_printf("cmyk(%.0f%%, %.0f%%, %.0f%%, %.0f%%)", 
        cmyk.c * 100.0, cmyk.m * 100.0, cmyk.y * 100.0, cmyk.k * 100.0);

    gtk_editable_set_text(GTK_EDITABLE(self->rgb_entry), rgb_str);
    gtk_editable_set_text(GTK_EDITABLE(self->hsl_entry), hsl_str);
    gtk_editable_set_text(GTK_EDITABLE(self->hsv_entry), hsv_str);
    gtk_editable_set_text(GTK_EDITABLE(self->cmyk_entry), cmyk_str);

    gchar *css = g_strdup_printf(
        ".color-preview { background-color: %s; border-radius: 12px; }", hex);
    
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(provider, css);
    gtk_style_context_add_provider(
        gtk_widget_get_style_context(self->color_preview_box),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    // WCAG Contrast
    VaxpColorRGB white_rgb = {1.0, 1.0, 1.0, 1.0};
    VaxpColorRGB black_rgb = {0.0, 0.0, 0.0, 1.0};
    double white_ratio = vaxp_color_contrast_ratio(&rgb, &white_rgb);
    double black_ratio = vaxp_color_contrast_ratio(&rgb, &black_rgb);

    gchar *white_text = g_strdup_printf("%.2f:1 (%s AA, %s AAA)", 
        white_ratio, 
        vaxp_color_passes_aa(white_ratio) ? "Pass" : "Fail",
        vaxp_color_passes_aaa(white_ratio) ? "Pass" : "Fail");
    gchar *black_text = g_strdup_printf("%.2f:1 (%s AA, %s AAA)", 
        black_ratio, 
        vaxp_color_passes_aa(black_ratio) ? "Pass" : "Fail",
        vaxp_color_passes_aaa(black_ratio) ? "Pass" : "Fail");

    gtk_label_set_text(GTK_LABEL(self->wcag_white_label), white_text);
    gtk_label_set_text(GTK_LABEL(self->wcag_black_label), black_text);
    
    // Complementary Color
    VaxpColorRGB comp_rgb = vaxp_palette_get_complementary(&rgb);
    gchar *comp_hex = vaxp_color_rgb_to_hex(&comp_rgb);
    gtk_label_set_text(GTK_LABEL(self->comp_hex_label), comp_hex);

    gchar *comp_css = g_strdup_printf(
        ".comp-preview { background-color: %s; border-radius: 12px; }", comp_hex);
    
    GtkCssProvider *comp_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(comp_provider, comp_css);
    gtk_style_context_add_provider(
        gtk_widget_get_style_context(self->comp_color_box),
        GTK_STYLE_PROVIDER(comp_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    gtk_widget_add_css_class(self->comp_color_box, "comp-preview");

    g_free(white_text);
    g_free(black_text);
    g_free(comp_hex);
    g_free(comp_css);
    g_object_unref(comp_provider);

    g_free(rgb_str);
    g_free(hsl_str);
    g_free(hsv_str);
    g_free(cmyk_str);
    g_free(css);
    g_object_unref(provider);
}

static void on_hex_changed(GtkEditable *editable, gpointer user_data) {
    VaxpWindow *self = VAXP_WINDOW(user_data);
    const gchar *text = gtk_editable_get_text(editable);
    if (g_str_has_prefix(text, "#") && (strlen(text) == 7 || strlen(text) == 9)) {
        update_colors_from_hex(self, text);
    }
}

static void copy_to_clipboard(GtkWidget *widget, const gchar *text) {
    GdkClipboard *clipboard = gtk_widget_get_clipboard(widget);
    gdk_clipboard_set_text(clipboard, text);
}

static void on_copy_hex(GtkButton *btn, gpointer user_data) {
    VaxpWindow *self = VAXP_WINDOW(user_data);
    copy_to_clipboard(GTK_WIDGET(self), gtk_editable_get_text(GTK_EDITABLE(self->hex_entry)));
}
static void on_copy_rgb(GtkButton *btn, gpointer user_data) {
    VaxpWindow *self = VAXP_WINDOW(user_data);
    copy_to_clipboard(GTK_WIDGET(self), gtk_editable_get_text(GTK_EDITABLE(self->rgb_entry)));
}
static void on_copy_hsl(GtkButton *btn, gpointer user_data) {
    VaxpWindow *self = VAXP_WINDOW(user_data);
    copy_to_clipboard(GTK_WIDGET(self), gtk_editable_get_text(GTK_EDITABLE(self->hsl_entry)));
}
static void on_copy_hsv(GtkButton *btn, gpointer user_data) {
    VaxpWindow *self = VAXP_WINDOW(user_data);
    copy_to_clipboard(GTK_WIDGET(self), gtk_editable_get_text(GTK_EDITABLE(self->hsv_entry)));
}
static void on_copy_cmyk(GtkButton *btn, gpointer user_data) {
    VaxpWindow *self = VAXP_WINDOW(user_data);
    copy_to_clipboard(GTK_WIDGET(self), gtk_editable_get_text(GTK_EDITABLE(self->cmyk_entry)));
}

static void on_color_picked_cb(const gchar *hex, gpointer user_data) {
    VaxpWindow *self = VAXP_WINDOW(user_data);
    vaxp_window_set_color_hex(self, hex);
}

static void on_pick_screen_clicked(GtkButton *btn, gpointer user_data) {
    VaxpWindow *self = VAXP_WINDOW(user_data);
    vaxp_portal_pick_color(on_color_picked_cb, self);
}

static void vaxp_window_class_init(VaxpWindowClass *klass) {
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    gtk_widget_class_set_template_from_resource(widget_class, "/org/vaxp/ColorPicker/ui/window.ui");

    gtk_widget_class_bind_template_child(widget_class, VaxpWindow, header_container);
    gtk_widget_class_bind_template_child(widget_class, VaxpWindow, color_preview_box);
    gtk_widget_class_bind_template_child(widget_class, VaxpWindow, hex_entry);
    gtk_widget_class_bind_template_child(widget_class, VaxpWindow, rgb_entry);
    gtk_widget_class_bind_template_child(widget_class, VaxpWindow, hsl_entry);
    gtk_widget_class_bind_template_child(widget_class, VaxpWindow, hsv_entry);
    gtk_widget_class_bind_template_child(widget_class, VaxpWindow, cmyk_entry);
    
    // buttons
    gtk_widget_class_bind_template_child(widget_class, VaxpWindow, copy_hex_btn);
    gtk_widget_class_bind_template_child(widget_class, VaxpWindow, copy_rgb_btn);
    gtk_widget_class_bind_template_child(widget_class, VaxpWindow, copy_hsl_btn);
    gtk_widget_class_bind_template_child(widget_class, VaxpWindow, copy_hsv_btn);
    gtk_widget_class_bind_template_child(widget_class, VaxpWindow, copy_cmyk_btn);
    
    // palette and wcag
    gtk_widget_class_bind_template_child(widget_class, VaxpWindow, wcag_white_label);
    gtk_widget_class_bind_template_child(widget_class, VaxpWindow, wcag_black_label);
    gtk_widget_class_bind_template_child(widget_class, VaxpWindow, comp_color_box);
    gtk_widget_class_bind_template_child(widget_class, VaxpWindow, comp_hex_label);
    
    gtk_widget_class_bind_template_callback(widget_class, on_pick_screen_clicked);
}

static void vaxp_window_init(VaxpWindow *self) {
    gtk_widget_init_template(GTK_WIDGET(self));

    VaxpHeaderBar *headerbar = vaxp_headerbar_new(GTK_WINDOW(self));
    gtk_box_append(GTK_BOX(self->header_container), GTK_WIDGET(headerbar));

    GtkWidget *pick_screen_btn = gtk_button_new_from_icon_name("color-select-symbolic");
    gtk_widget_set_tooltip_text(pick_screen_btn, "Pick from Screen");
    g_signal_connect(pick_screen_btn, "clicked", G_CALLBACK(on_pick_screen_clicked), self);
    gtk_box_append(GTK_BOX(vaxp_headerbar_get_end_box(headerbar)), pick_screen_btn);

    // Custom Translucent Background: ARGB(100, 0, 0, 0)
    // In CSS, alpha is 0-1, so 100/255 = 0.392
    const gchar *window_css = 
        "window { background-color: rgba(0, 0, 0, 0.392); }\n"
        ".var-window-btn {\n"
        "    min-width: 14px;\n"
        "    min-height: 14px;\n"
        "    padding: 0;\n"
        "    border-radius: 50%;\n"
        "    margin: 4px 6px;\n"
        "    border: 1px solid rgba(0, 0, 0, 0.4);\n"
        "    background-size: 8px 8px;\n"
        "    background-position: center;\n"
        "    background-repeat: no-repeat;\n"
        "    transition: all 0.2s ease;\n"
        "}\n"
        ".var-btn-close { background-color: #ff5f56; }\n"
        ".var-btn-minimize { background-color: #ffbd2e; }\n"
        ".var-btn-maximize { background-color: #27c93f; }\n"
        ".var-window-btn:hover { filter: brightness(1.2); }\n"
        ".title { font-weight: bold; font-size: 14px; color: white; }\n"
        ".subtitle { font-size: 11px; color: rgba(255, 255, 255, 0.6); }\n";
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(css_provider, window_css);
    
    // Apply to the specific window instance
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    g_object_unref(css_provider);

    g_signal_connect(self->hex_entry, "changed", G_CALLBACK(on_hex_changed), self);
    g_signal_connect(self->copy_hex_btn, "clicked", G_CALLBACK(on_copy_hex), self);
    g_signal_connect(self->copy_rgb_btn, "clicked", G_CALLBACK(on_copy_rgb), self);
    g_signal_connect(self->copy_hsl_btn, "clicked", G_CALLBACK(on_copy_hsl), self);
    g_signal_connect(self->copy_hsv_btn, "clicked", G_CALLBACK(on_copy_hsv), self);
    g_signal_connect(self->copy_cmyk_btn, "clicked", G_CALLBACK(on_copy_cmyk), self);
}

VaxpWindow* vaxp_window_new(GtkApplication *app) {
    return g_object_new(VAXP_TYPE_WINDOW, "application", app, NULL);
}

void vaxp_window_set_color_hex(VaxpWindow *self, const gchar *hex) {
    gtk_editable_set_text(GTK_EDITABLE(self->hex_entry), hex);
    update_colors_from_hex(self, hex);
}
