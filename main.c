#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    Display* d;
    XTextProperty prop;
    Window top;
    GC gc;
    XEvent event;
    char* label_str = "Hello, World";

    d = XOpenDisplay(NULL);
    if (!d) {
        return 1;
    }

    top = XCreateSimpleWindow(d, RootWindow(d, 0), 0, 0,
        300, 300, 1, BlackPixel(d, 0), WhitePixel(d, 0));
    Atom delWindow = XInternAtom(d, "WM_DELETE_WINDOW", 0);
    XSetWMProtocols(d, top, &delWindow, 1);

    prop.value = "x test";
    prop.encoding = XA_STRING;
    prop.format = 8;
    prop.nitems = strlen(prop.value);
    XSetWMName(d, top, &prop);

    gc = XCreateGC(d, top, 0, 0);

    XSelectInput(d, top, ExposureMask);

    XMapSubwindows(d, top);
    XMapWindow(d, top);
    XFlush(d);

    while (1) {
        XNextEvent(d, &event);
        switch (event.type) {
        case Expose:
            if (event.xexpose.count == 0) {
                XDrawString(d, top, gc, 30, 30, label_str, strlen(label_str));
                XFlush(d);
            }
            break;
        case ClientMessage:
            XDestroyWindow(d, top);
            XCloseDisplay(d);
            return 0;
        }
    }
}
