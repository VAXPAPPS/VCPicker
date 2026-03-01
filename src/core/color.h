#ifndef VAXP_COLOR_H
#define VAXP_COLOR_H

#include <glib.h>

G_BEGIN_DECLS

typedef struct {
    double r; // 0.0 - 1.0
    double g;
    double b;
    double a; // 0.0 - 1.0
} VaxpColorRGB;

typedef struct {
    double h; // 0.0 - 360.0
    double s; // 0.0 - 1.0
    double l; // 0.0 - 1.0
    double a;
} VaxpColorHSL;

typedef struct {
    double h; // 0.0 - 360.0
    double s; // 0.0 - 1.0
    double v; // 0.0 - 1.0
    double a;
} VaxpColorHSV;

typedef struct {
    double c; // 0.0 - 1.0
    double m;
    double y;
    double k;
    double a;
} VaxpColorCMYK;

// Convert from RGB (base model) to other formats
VaxpColorHSL  vaxp_color_rgb_to_hsl  (const VaxpColorRGB *rgb);
VaxpColorHSV  vaxp_color_rgb_to_hsv  (const VaxpColorRGB *rgb);
VaxpColorCMYK vaxp_color_rgb_to_cmyk (const VaxpColorRGB *rgb);
gchar*        vaxp_color_rgb_to_hex  (const VaxpColorRGB *rgb);

// Convert from other formats to RGB
VaxpColorRGB  vaxp_color_hsl_to_rgb  (const VaxpColorHSL *hsl);
VaxpColorRGB  vaxp_color_hex_to_rgb  (const gchar *hex);

G_END_DECLS

#endif /* VAXP_COLOR_H */
