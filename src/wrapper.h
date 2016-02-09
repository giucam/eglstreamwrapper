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

#ifndef WRAPPER_H
#define WRAPPER_H

#include "egl.h"

EGLDisplay wrapper_get_display(EGLNativeDisplayType native);

struct wrapper_display *wrapper_display_get_wrapper(EGLDisplay egl_dpy);
EGLBoolean wrapper_choose_config(struct wrapper_display *dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config);
EGLSurface wrapper_create_surface(struct wrapper_display *dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list);
EGLBoolean wrapper_swap_buffers(struct wrapper_display *dpy, EGLSurface surface);
EGLBoolean wrapper_swap_interval(struct wrapper_display *dpy, EGLint interval);

#endif
