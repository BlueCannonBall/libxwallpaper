#include "xwallpaper.h"
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Wallpaper {
    Display* xdpy;
    Window root;
    int screen;
} Wallpaper;

#ifdef __cplusplus
extern "C" {
#endif

    Wallpaper* wallpaper_create() {
        Wallpaper* wallpaper = malloc(sizeof(Wallpaper));
        return wallpaper;
    }

    int wallpaper_init(Wallpaper* wallpaper, const char* display) {
        if ((wallpaper->xdpy = XOpenDisplay(display)) == NULL) {
            fprintf(stderr, "libxwallpaper: Failed to open display\n");
            return 1;
        }
        wallpaper->root = DefaultRootWindow(wallpaper->xdpy);
        wallpaper->screen = DefaultScreen(wallpaper->xdpy);

        typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

        static int visual_attribs[] = {
            GLX_RENDER_TYPE,
            GLX_RGBA_BIT,
            GLX_DRAWABLE_TYPE,
            GLX_WINDOW_BIT,
            GLX_DOUBLEBUFFER,
            true,
            GLX_RED_SIZE,
            1,
            GLX_GREEN_SIZE,
            1,
            GLX_BLUE_SIZE,
            1,
            GLX_DEPTH_SIZE,
            32,
            None,
        };

        int num_fbc = 0;
        GLXFBConfig* fbc = glXChooseFBConfig(wallpaper->xdpy,
            wallpaper->screen,
            visual_attribs,
            &num_fbc);
        if (!fbc) {
            fprintf(stderr, "libxwallpaper: glXChooseFBConfig() failed\n");
            return 1;
        }

        glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
        glXCreateContextAttribsARB =
            (glXCreateContextAttribsARBProc)
                glXGetProcAddress((const GLubyte*) "glXCreateContextAttribsARB");

        if (!glXCreateContextAttribsARB) {
            fprintf(stderr, "libxwallpaper: glXCreateContextAttribsARB() not found\n");
        }

        static int context_attribs[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3, GLX_CONTEXT_MINOR_VERSION_ARB, 3, None};
        GLXContext ctx = glXCreateContextAttribsARB(wallpaper->xdpy, fbc[0], NULL, true, context_attribs);

        glXMakeCurrent(wallpaper->xdpy, wallpaper->root, ctx);

        return 0;
    }

    void wallpaper_destroy(Wallpaper* wallpaper) {
        XCloseDisplay(wallpaper->xdpy);
    }

    void wallpaper_free(Wallpaper* wallpaper) {
        free(wallpaper);
    }

    Window wallpaper_get_window(Wallpaper* wallpaper) {
        return wallpaper->root;
    }

    int wallpaper_get_width(Wallpaper* wallpaper) {
        XWindowAttributes attrs;
        XGetWindowAttributes(wallpaper->xdpy, wallpaper->root, &attrs);

        return attrs.width;
    }

    int wallpaper_get_height(Wallpaper* wallpaper) {
        XWindowAttributes attrs;
        XGetWindowAttributes(wallpaper->xdpy, wallpaper->root, &attrs);

        return attrs.height;
    }

    void wallpaper_swap_buffers(Wallpaper* wallpaper) {
        glXSwapBuffers(wallpaper->xdpy, wallpaper->root);
    }

#ifdef __cplusplus
}
#endif