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

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#include "wrapper.h"
#include "eglext.h"
#include "wrappedfuncs.h"
#include "wayland-egl-priv.h"

#include "wayland-eglstreamwrapper-client-protocol.h"

#include <wayland-client.h>

struct wrapper_display {
    EGLDisplay egl_dpy;
    struct wl_display *wl_dpy;
    struct wl_event_queue *queue;
    struct wl_registry *registry;
    struct eglstreamwrapper *wrapper;
    int swap_interval;
};

struct wrapper_surface {
    struct wrapper_display *dpy;
    EGLSurface egl_surface;
    struct wl_egl_window *window;
    struct eglstreamwrapper_feedback *eglstream;
    struct wl_callback *frame_callback;
};

static struct wrapper_display *displays[16] = { 0 };
static struct wrapper_surface *surfaces[128] = { 0 };

EGLBoolean (*_eglQueryDevices)(EGLint, EGLDeviceEXT *, EGLint *) = NULL;
EGLDisplay (*_eglGetPlatformDisplay)(EGLenum, void *, const EGLint *) = NULL;
const char *(*_eglQueryDeviceString)(EGLDeviceEXT, EGLint) = NULL;
EGLStreamKHR (*_eglCreateStream)(EGLDisplay, const EGLint *) = NULL;
EGLBoolean (*_eglDestroyStream)(EGLDisplay, EGLStreamKHR) = NULL;
EGLNativeFileDescriptorKHR (*_eglGetStreamFileDescriptor)(EGLDisplay, EGLStreamKHR) = NULL;
EGLSurface (*_eglCreateStreamProducerSurface)(EGLDisplay, EGLConfig, EGLStreamKHR, const EGLint *) = NULL;

struct wrapper_display *wrapper_display_get_wrapper(EGLDisplay egl_dpy)
{
    int i;
    for (i = 0; i < 16; ++i) {
        struct wrapper_display *dpy = displays[i];
        if (dpy && dpy->egl_dpy == egl_dpy) {
            return dpy;
        }
    }
    return NULL;
}

void register_display(struct wrapper_display *dpy)
{
    int i;
    for (i = 0; i < 16; ++i) {
        if (!displays[i]) {
            displays[i] = dpy;
            return;
        }
    }
    assert(0);
}

struct wrapper_surface *get_surface(EGLSurface surf)
{
    int i;
    for (i = 0; i < 128; ++i) {
        struct wrapper_surface *s = surfaces[i];
        if (s && s->egl_surface == surf) {
            return s;
        }
    }
    return NULL;
}

void register_surface(struct wrapper_surface *surf)
{
    int i;
    for (i = 0; i < 128; ++i) {
        if (!surfaces[i]) {
            surfaces[i] = surf;
            return;
        }
    }
    assert(0);
}

static void registry_global(void *data, struct wl_registry *registry, uint32_t id, const char *interface, uint32_t version)
{
    struct wrapper_display *dpy = data;

    if (strcmp(interface, "eglstreamwrapper") == 0) {
        dpy->wrapper = wl_registry_bind(registry, id, &eglstreamwrapper_interface, 1);
    }
}

static void registry_global_remove(void *data, struct wl_registry *registry, uint32_t id)
{
}

static const struct wl_registry_listener registry_listener = {
    registry_global,
    registry_global_remove,
};

static void feedback_success(void *data, struct eglstreamwrapper_feedback *feedback)
{
}

static void feedback_failed(void *data, struct eglstreamwrapper_feedback *feedback)
{
}

static const struct eglstreamwrapper_feedback_listener feedback_listener = {
    feedback_success,
    feedback_failed,
};

EGLBoolean wrapper_choose_config(struct wrapper_display *dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{
    // The default value for EGL_SURFACE_TYPE is EGL_WINDOW_BIT
    // A display backend by a EGLDevice doesn't return any config for that bit, because
    // it doesn't allow by itself to show a surface on screen, so we must remove that bit
    // if present in the attrib_list, and if not we must explicitly set it to EGL_STREAM_BIT_KHR.

    int num_attribs = 0, i = 0, j = 0;
    EGLint attrib;
    bool has_surface_type = false;
    if (attrib_list) {
        do {
            attrib = attrib_list[num_attribs++];
            if (attrib == EGL_SURFACE_TYPE) {
                has_surface_type = true;
            }
        } while (attrib != EGL_NONE);
    }

    if (!has_surface_type) {
        num_attribs += 2;
    }
    EGLint *attribs = malloc(sizeof(EGLint) * num_attribs);

    if (!has_surface_type) {
        attribs[i++] = EGL_SURFACE_TYPE;
        attribs[i++] = EGL_STREAM_BIT_KHR;
    }

    for (; i < num_attribs; ++i, ++j) {
        attribs[i] = attrib_list[j];
        if (attribs[i] == EGL_SURFACE_TYPE && (attrib_list[j + 1] & EGL_WINDOW_BIT)) {
            attribs[++i] = (attrib_list[++j] & ~EGL_WINDOW_BIT) | EGL_STREAM_BIT_KHR;
        }
    }
    EGLBoolean ret = wrapped_funcs.eglChooseConfig(dpy->egl_dpy, attribs, configs, config_size, num_config);
    free(attribs);
    return ret;
}

EGLDisplay wrapper_get_display(EGLNativeDisplayType native)
{
    fprintf(stderr, "WRAP!!\n");

    bool is_wayland = *(const struct wl_interface **)native == &wl_display_interface;

    if (!is_wayland) {
        const char *platform = getenv("EGL_PLATFORM");
        is_wayland = platform && strcmp(platform, "wayland") == 0;
    }
    fprintf(stderr,"get dpy %p %p; wayland %d \n",(void*)native,&wl_display_interface,is_wayland);

    if (!is_wayland) {
        return EGL_NO_DISPLAY;
    }

    EGLDeviceEXT eglDevice;
    EGLint numDevices = 0;

    if (!_eglQueryDevices) {
        const char *extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
        if (!extensions) {
            fprintf(stderr, "Retrieving EGL client extension string failed.");
            return EGL_NO_DISPLAY;
        }

        fprintf(stderr, "EGL client extensions:\n%s\n", extensions);

#define CHECKEXT(ext) \
    if (!strstr(extensions, ext)) { \
        fprintf(stderr, ext " not supported\n"); \
        return EGL_NO_DISPLAY; \
    }

        CHECKEXT("EGL_EXT_platform_base")
        CHECKEXT("EGL_EXT_platform_device")
        CHECKEXT("EGL_EXT_device_base")
#undef CHECKEXT

        _eglQueryDevices = (void *)eglGetProcAddress("eglQueryDevicesEXT");
        _eglGetPlatformDisplay = (void *)eglGetProcAddress("eglGetPlatformDisplayEXT");
        _eglQueryDeviceString = (void *)eglGetProcAddress("eglQueryDeviceStringEXT");
    }
    _eglQueryDevices(0, NULL, &numDevices);
    fprintf(stderr, "%d devices available\n", numDevices);

    EGLBoolean ret = _eglQueryDevices(1, &eglDevice, &numDevices);
    if (numDevices == 0 || ret == EGL_FALSE) {
        fprintf(stderr, "No EGLDevice found");
        return EGL_NO_DISPLAY;
    }

    const char *exts = _eglQueryDeviceString(eglDevice, EGL_EXTENSIONS);
    fprintf(stderr, "device extensions: %s\n", exts);

    struct wrapper_display *dpy = malloc(sizeof(struct wrapper_display));
    if (!dpy) {
        return EGL_NO_DISPLAY;
    }

    memset(dpy, 0, sizeof(*dpy));
    dpy->egl_dpy = _eglGetPlatformDisplay(EGL_PLATFORM_DEVICE_EXT, eglDevice, 0);
    if (dpy->egl_dpy == EGL_NO_DISPLAY) {
        free(dpy);
        return EGL_NO_DISPLAY;
    }

    register_display(dpy);
    dpy->swap_interval = 1;
    dpy->wl_dpy = (struct wl_display *)native;
    dpy->queue = wl_display_create_queue(dpy->wl_dpy);
    dpy->registry = wl_display_get_registry(dpy->wl_dpy);
    wl_proxy_set_queue((struct wl_proxy *)dpy->registry, dpy->queue);
    wl_registry_add_listener(dpy->registry, &registry_listener, dpy);

    return dpy->egl_dpy;
}

EGLSurface wrapper_create_surface(struct wrapper_display *dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list)
{
    if (!_eglCreateStream) {
        const char *extensions = eglQueryString(dpy->egl_dpy, EGL_EXTENSIONS);
        if (!extensions) {
            fprintf(stderr, "Retrieving EGL client extension string failed.");
            return EGL_NO_DISPLAY;
        }

        fprintf(stderr, "EGL extensions:\n%s\n", extensions);

#define CHECKEXT(ext) \
    if (!strstr(extensions, ext)) { \
        fprintf(stderr, ext " not supported\n"); \
        return EGL_NO_DISPLAY; \
    }

        CHECKEXT("EGL_KHR_stream_producer_eglsurface")
        CHECKEXT("EGL_KHR_stream_cross_process_fd");
#undef CHECKEXT

        _eglCreateStream = (void *)eglGetProcAddress("eglCreateStreamKHR");
        _eglDestroyStream = (void *)eglGetProcAddress("eglDestroyStreamKHR");
        _eglGetStreamFileDescriptor = (void *)eglGetProcAddress("eglGetStreamFileDescriptorKHR");
        _eglCreateStreamProducerSurface = (void *)eglGetProcAddress("eglCreateStreamProducerSurfaceKHR");
    }


    if (!dpy->wrapper) {
        wl_display_roundtrip_queue(dpy->wl_dpy, dpy->queue);
        if (!dpy->wrapper) {
            fprintf(stderr, "No eglstreamwrapper global interface\n");
            goto fail;
        }
    }
fprintf(stderr,"attrs %p\n",attrib_list);
    struct wrapper_surface *surface = malloc(sizeof(struct wrapper_surface));
    if (!surface) {
        goto fail;
    }

    const EGLint attr[] = {
//         EGL_CONSUMER_ACQUIRE_TIMEOUT_USEC_KHR, 100000,
//         EGL_STREAM_FIFO_LENGTH_KHR, 10,
        EGL_NONE
    };
    EGLStreamKHR stream = _eglCreateStream(dpy->egl_dpy, attr);
    if (stream == EGL_NO_STREAM_KHR) {
        goto fail_surface;
    }

    EGLNativeFileDescriptorKHR fd = _eglGetStreamFileDescriptor(dpy->egl_dpy, stream);
    fprintf(stderr, "fd %d\n",fd);
    if (fd == EGL_NO_FILE_DESCRIPTOR_KHR) {
        goto fail_stream;
    }

    struct eglstreamwrapper_feedback *feedback = eglstreamwrapper_bind_eglstream(dpy->wrapper, fd);
    wl_proxy_set_queue((struct wl_proxy *)feedback, dpy->queue);
    eglstreamwrapper_feedback_add_listener(feedback, &feedback_listener, 0);

    wl_display_roundtrip_queue(dpy->wl_dpy, dpy->queue);

    surface->dpy = dpy;
    surface->eglstream = feedback;
    surface->frame_callback = NULL;
    surface->window = (struct wl_egl_window *)win;
    const EGLint stream_producer_attribs[] = {
        EGL_WIDTH,  surface->window->width,
        EGL_HEIGHT, surface->window->height,
        EGL_NONE
    };
    surface->egl_surface = _eglCreateStreamProducerSurface(dpy->egl_dpy, config, stream, stream_producer_attribs);
    if (surface->egl_surface == EGL_NO_SURFACE) {
        goto fail_stream;
    }

    register_surface(surface);
    return surface->egl_surface;

fail_stream:
    fprintf(stderr, "fail_stream, error 0x%x\n", eglGetError());
    _eglDestroyStream(dpy->egl_dpy, stream);
fail_surface:
    fprintf(stderr, "fail_surface\n");
    free(surface);
fail:
    fprintf(stderr, "fail\n");
    return EGL_NO_SURFACE;
}


static void buffer_release(void *data, struct wl_buffer *buffer)
{
    wl_buffer_destroy(buffer);
}

static const struct wl_buffer_listener buffer_listener = {
    buffer_release,
};

static void frame_callback_done(void *data, struct wl_callback *cb, uint32_t time)
{
    struct wrapper_surface *surf = data;
    surf->frame_callback = NULL;
    wl_callback_destroy(cb);
}

static const struct wl_callback_listener frame_callback_listener = {
    frame_callback_done,
};

EGLBoolean wrapper_swap_buffers(struct wrapper_display *dpy, EGLSurface egl_surface)
{
//     fprintf(stderr, "swap\n");
    if (wrapped_funcs.eglSwapBuffers(dpy->egl_dpy, egl_surface) == EGL_FALSE) {
        return EGL_FALSE;
    }
// fprintf(stderr, "swap2\n");
    struct wrapper_surface *surface = get_surface(egl_surface);

    if (surface->frame_callback && dpy->swap_interval) {
        while (surface->frame_callback && dpy->swap_interval) {
            wl_display_dispatch_queue(surface->dpy->wl_dpy, surface->dpy->queue);
        }
    } else {
        wl_display_dispatch_queue_pending(surface->dpy->wl_dpy, surface->dpy->queue);
    }

    struct wl_buffer *buf = eglstreamwrapper_feedback_get_buffer(surface->eglstream, surface->window->width, surface->window->height);
    wl_proxy_set_queue((struct wl_proxy *)buf, surface->dpy->queue);
    wl_buffer_add_listener(buf, &buffer_listener, 0);

    wl_surface_attach(surface->window->surface, buf, 0, 0);
    wl_surface_damage(surface->window->surface, -INT32_MAX / 2, -INT32_MAX / 2, INT32_MAX, INT32_MAX);

    if (dpy->swap_interval) {
        surface->frame_callback = wl_surface_frame(surface->window->surface);
        wl_proxy_set_queue((struct wl_proxy *)surface->frame_callback, surface->dpy->queue);
        wl_callback_add_listener(surface->frame_callback, &frame_callback_listener, surface);
    }
    wl_surface_commit(surface->window->surface);

    wl_display_flush(surface->dpy->wl_dpy);

    return EGL_TRUE;
}

EGLBoolean wrapper_swap_interval(struct wrapper_display *dpy, EGLint interval)
{
    dpy->swap_interval = interval;
    return EGL_TRUE;
}
