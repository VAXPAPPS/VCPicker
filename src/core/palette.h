#ifndef VAXP_PALETTE_H
#define VAXP_PALETTE_H

#include "color.h"

G_BEGIN_DECLS

VaxpColorRGB vaxp_palette_get_complementary(const VaxpColorRGB *base);
VaxpColorRGB* vaxp_palette_get_analogous(const VaxpColorRGB *base);

double vaxp_color_contrast_ratio(const VaxpColorRGB *c1, const VaxpColorRGB *c2);
gboolean vaxp_color_passes_aa(double ratio);
gboolean vaxp_color_passes_aaa(double ratio);

G_END_DECLS

#endif /* VAXP_PALETTE_H */
