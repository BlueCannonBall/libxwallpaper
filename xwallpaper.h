#ifndef _XWALLPAPER_H
#define _XWALLPAPER_H

#include <X11/X.h>

typedef struct Wallpaper Wallpaper;

#ifdef __cplusplus
extern "C" {
#endif

    Wallpaper* wallpaper_create();
    int wallpaper_init(Wallpaper* wallpaper, const char* display);
    void wallpaper_destroy(Wallpaper* wallpaper);
    void wallpaper_free(Wallpaper* wallpaper);

    Window wallpaper_get_window(Wallpaper* wallpaper);
    int wallpaper_get_width(Wallpaper* wallpaper);
    int wallpaper_get_height(Wallpaper* wallpaper);
    int wallpaper_get_depth(Wallpaper* wallpaper);

    void wallpaper_swap_buffers(Wallpaper* wallpaper);

#ifdef __cplusplus
}
#endif
#endif