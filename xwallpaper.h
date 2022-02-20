#ifndef _XWALLPAPER_H
#define _XWALLPAPER_H

#include <X11/X.h>
#include <GL/glx.h>

typedef struct Wallpaper Wallpaper;

#ifdef __cplusplus
extern "C" {
#endif

    Wallpaper* wallpaper_create();
    int wallpaper_init(Wallpaper* wallpaper, const char* display);
    void wallpaper_destroy(Wallpaper* wallpaper);
    void wallpaper_free(Wallpaper* wallpaper);

#ifdef __cplusplus
}
#endif
#endif