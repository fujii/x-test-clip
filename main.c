#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    Display* dpy;
    XTextProperty prop;
    Window top;
    GC gc;
    XGCValues gcv;
    XEvent event;

    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        return 1;
    }

    top = XCreateSimpleWindow(dpy, RootWindow(dpy, 0), 0, 0,
        300, 300, 1, BlackPixel(dpy, 0), WhitePixel(dpy, 0));
    Atom delWindow = XInternAtom(dpy, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(dpy, top, &delWindow, 1);

    prop.value = "x test";
    prop.encoding = XA_STRING;
    prop.format = 8;
    prop.nitems = strlen(prop.value);
    XSetWMName(dpy, top, &prop);

    gcv.foreground = BlackPixel(dpy, 0);
    gcv.background = WhitePixel(dpy, 0);
    gc = XCreateGC(dpy, top, GCForeground | GCBackground, &gcv);

    XSelectInput(dpy, top, ExposureMask);

    XMapSubwindows(dpy, top);
    XMapWindow(dpy, top);
    XFlush(dpy);

    int fd = ConnectionNumber(dpy);
    int count = 0;
    int width = 0, height = 0;
    while (1) {
        struct pollfd pollfd = {fd, POLLIN | POLLPRI, 0};
        poll(&pollfd, 1, 1000);
        if (pollfd.revents) {
            XNextEvent(dpy, &event);
            switch (event.type) {
            case Expose:
                if (event.xexpose.count == 0) {
                    width = event.xexpose.width;
                    height = event.xexpose.height;
                }
                break;
            case ClientMessage:
                XDestroyWindow(dpy, top);
                XCloseDisplay(dpy);
                return 0;
            }
        } else {
            count++;
            const int SIZE = 20;
            for (int y = 0; y < height; y += SIZE)
                for (int x = 0; x < width; x += SIZE) {
                    XRectangle rect = {x, y, SIZE, SIZE};
                    char str[2] = "0";
                    str[0] = '0' + count % 10;
                    XSetClipMask(dpy, gc, None);
                    XSetClipRectangles(dpy, gc, 0, 0, &rect, 1, Unsorted);
                    XDrawImageString(dpy, top, gc, x, y + SIZE, str, 1);
                }
            XFlush(dpy);
        }
    }
}
