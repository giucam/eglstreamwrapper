/*
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

#ifndef EGLSTREAMWRAPPER_SERVER_H
#define EGLSTREAMWRAPPER_SERVER_H

#include <stdbool.h>

#include <EGL/egl.h>

struct wl_display;
struct wl_resource;
struct eglstreamwrapper_display;

struct eglstreamwrapper_display *eglstreamwrapper_bind_display(EGLDisplay egl_dpy, struct wl_display *wl_dpy,
                                                               int (*gen_texture)(void *), void *ud);

bool eglstreamwrapper_own_buffer(struct wl_resource *buf);
void eglstreamwrapper_get_buffer_size(struct wl_resource *buf, int *width, int *height);
int eglstreamwrapper_acquire_buffer(struct wl_resource *buf);
#endif
