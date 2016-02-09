/*
 * Copyright © 2011 Kristian Høgsberg
 * Copyright © 2011 Benjamin Franzke
 * Copyright © 2016 Giulio Camuffo <giuliocamuffo@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>

#include "wayland-egl.h"
#include "wayland-egl-priv.h"

struct wl_egl_window *
wl_egl_window_create(struct wl_surface *surface,
                     int width, int height)
{
    struct wl_egl_window *win = malloc(sizeof(struct wl_egl_window));
    if (!win) {
        return NULL;
    }

    win->surface = surface;
    win->width = width;
    win->height = height;

    return win;
}

void
wl_egl_window_destroy(struct wl_egl_window *egl_window)
{
    free(egl_window);
}

void
wl_egl_window_resize(struct wl_egl_window *egl_window,
                     int width, int height,
                     int dx, int dy)
{
    fprintf(stderr, "resize not supported yet!!! %d %d -> %d %d\n",egl_window->width,egl_window->height,width,height);
}

void
wl_egl_window_get_attached_size(struct wl_egl_window *egl_window,
                                int *width, int *height)
{
}
