#include "xwallpaper.h"
#include <GL/glx.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Wallpaper {
    Display* xdpy;
    Window root;
    Pixmap pmap;
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
        wallpaper->pmap = XCreatePixmap(wallpaper->xdpy, wallpaper->root, wallpaper_get_width(wallpaper), wallpaper_get_height(wallpaper), wallpaper_get_depth(wallpaper));

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
            24,
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

        glXMakeCurrent(wallpaper->xdpy, wallpaper->pmap, ctx);

        {
            Atom atom_root, atom_eroot, type;
            unsigned char *data_root, *data_eroot;
            int format;
            unsigned long length, after;

            atom_root = XInternAtom(wallpaper->xdpy, "_XROOTPMAP_ID", True);
            atom_eroot = XInternAtom(wallpaper->xdpy, "ESETROOT_PMAP_ID", True);

            /* Doing this to clean up after old background */
            if (atom_root != None && atom_eroot != None) {
                XGetWindowProperty(wallpaper->xdpy, wallpaper->root, atom_root, 0L, 1L, False, AnyPropertyType, &type, &format, &length, &after, &data_root);

                if (type == XA_PIXMAP) {
                    XGetWindowProperty(wallpaper->xdpy, wallpaper->root, atom_eroot, 0L, 1L, False, AnyPropertyType, &type, &format, &length, &after, &data_eroot);

                    if (data_root && data_eroot && type == XA_PIXMAP && *((Pixmap*) data_root) == *((Pixmap*) data_eroot))
                        XKillClient(wallpaper->xdpy, *((Pixmap*) data_root));
                }
            }

            atom_root = XInternAtom(wallpaper->xdpy, "_XROOTPMAP_ID", False);
            atom_eroot = XInternAtom(wallpaper->xdpy, "ESETROOT_PMAP_ID", False);

            if (atom_root == None || atom_eroot == None)
                return 0;

            /* Setting new background atoms */
            XChangeProperty(wallpaper->xdpy, wallpaper->root, atom_root, XA_PIXMAP, 32, PropModeReplace, (unsigned char*) &wallpaper->pmap, 1);
            XChangeProperty(wallpaper->xdpy, wallpaper->root, atom_eroot, XA_PIXMAP, 32, PropModeReplace, (unsigned char*) &wallpaper->pmap, 1);

            return 1;
        }

        XSetWindowBackgroundPixmap(wallpaper->xdpy, wallpaper->root, wallpaper->pmap);
        XClearWindow(wallpaper->xdpy, wallpaper->root);

        return 0;
    }

    void wallpaper_destroy(Wallpaper* wallpaper) {
        XFreePixmap(wallpaper->xdpy, wallpaper->pmap);
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

    int wallpaper_get_depth(Wallpaper* wallpaper) {
        XWindowAttributes attrs;
        XGetWindowAttributes(wallpaper->xdpy, wallpaper->root, &attrs);

        return attrs.depth;
    }

    void wallpaper_swap_buffers(Wallpaper* wallpaper) {
        glXSwapBuffers(wallpaper->xdpy, wallpaper->pmap);
    }

#ifdef __cplusplus
}
#endif