#ifndef _BOX2D_HELPERS_H
#define _BOX2D_HELPERS_H

#define DISPLAY_WIDTH				960
#define DISPLAY_HEIGHT				544

#define BOX2D_GFX_SCALE 100.0f //thus 1 metre = 100 pixels
#define BOX2D_GFX_OFFSET_X DISPLAY_WIDTH*0.5f
#define BOX2D_GFX_OFFSET_Y 0.0f
#define BOX2D_GFX_POS_X(x) ((x)*BOX2D_GFX_SCALE + BOX2D_GFX_OFFSET_X)
#define BOX2D_GFX_POS_Y(y) (DISPLAY_HEIGHT - ((y)*BOX2D_GFX_SCALE + BOX2D_GFX_OFFSET_Y))
#define BOX2D_GFX_SIZE(x) ((x)*BOX2D_GFX_SCALE)
#define GFX_BOX2D_POS_X(x) (((x) - BOX2D_GFX_OFFSET_X)/BOX2D_GFX_SCALE)
#define GFX_BOX2D_POS_Y(y) (((y) - DISPLAY_HEIGHT+BOX2D_GFX_OFFSET_Y)/-BOX2D_GFX_SCALE)
#define GFX_BOX2D_SIZE(x) ((x)/BOX2D_GFX_SCALE)

#endif // _BOX2D_HELPERS_H