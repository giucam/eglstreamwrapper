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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <wayland-server-core.h>

#include "eglstreamwrapper-server.h"
#include "wayland-eglstreamwrapper-server-protocol.h"
#include "eglext.h"

struct eglstreamwrapper_display {
    EGLDisplay egl_dpy;
    struct wl_display *wl_dpy;
    struct wl_global *global;
    int (*gen_tex)(void *);
    void *ud;
};

struct eglstream {
    struct eglstreamwrapper_display *dpy;
    struct wl_resource *resource;
    EGLStreamKHR stream;
    int tex[3];
    int num_textures;
    int32_t width, height;
};

static EGLStreamKHR (*create_stream_from_fd)(EGLDisplay, EGLNativeFileDescriptorKHR);
static EGLBoolean (*query_stream)(EGLDisplay, EGLStreamKHR, EGLenum, EGLint *);
static EGLBoolean (*stream_consumer_gltexture)(EGLDisplay, EGLStreamKHR);
static EGLBoolean (*stream_consumer_gltexture_attribs)(EGLDisplay, EGLStreamKHR, EGLAttrib *);
static EGLBoolean (*stream_consumer_acquire)(EGLDisplay, EGLStreamKHR);

static void buffer_destroy(struct wl_client *client, struct wl_resource *res)
{
    wl_resource_destroy(res);
}

static const struct wl_buffer_interface eglstream_buffer_impl = {
    buffer_destroy,
};

static void eglstream_get_buffer(struct wl_client *client, struct wl_resource *resource,
                                 uint32_t id, int32_t width, int32_t height)
{
    struct eglstream *eglstream = wl_resource_get_user_data(resource);
    struct wl_resource *res = wl_resource_create(client, &wl_buffer_interface, 1, id);
    wl_resource_set_implementation(res, &eglstream_buffer_impl, eglstream, 0);

    eglstream->width = width;
    eglstream->height = height;
}

static const struct eglstreamwrapper_feedback_interface eglstreamwrapper_feedback_impl = {
    eglstream_get_buffer,
};

static void eglstreamwrapper_bind_eglstream(struct wl_client *client, struct wl_resource *resource, uint32_t id, int32_t fd)
{
    struct eglstreamwrapper_display *dpy = wl_resource_get_user_data(resource);
    struct eglstream *eglstream;
    struct wl_resource *res = wl_resource_create(client, &eglstreamwrapper_feedback_interface,
                                                 wl_resource_get_version(resource), id);

    eglstream = malloc(sizeof(*eglstream));
    if (!eglstream)
            goto fail;
    eglstream->dpy = dpy;
    eglstream->resource = res;

    wl_resource_set_implementation(res, &eglstreamwrapper_feedback_impl, eglstream, NULL);

    eglstream->stream = create_stream_from_fd(dpy->egl_dpy, fd);
    close(fd);
    if (eglstream->stream == EGL_NO_STREAM_KHR) {
        goto fail;
    }

    eglstream->num_textures = 1;

    if (eglstream->num_textures == 1) {
        eglstream->tex[0] = dpy->gen_tex(dpy->ud);

        EGLBoolean ret = stream_consumer_gltexture(dpy->egl_dpy, eglstream->stream);
        if (ret == EGL_FALSE) {
            goto fail;
        }
    } else {
//             for (i = 0; i < eglstream->num_textures; ++i) {
//                     glGenTextures(1, &eglstream->tex[i]);
//                     glActiveTexture(GL_TEXTURE0 + i);
//                     glBindTexture(GL_TEXTURE_EXTERNAL_OES, eglstream->tex[i]);
//             }
//
//             EGLAttrib attribs[11] = {
//                     EGL_COLOR_BUFFER_TYPE, EGL_YUV_BUFFER_EXT,
//                     EGL_YUV_NUMBER_OF_PLANES_EXT, eglstream->num_textures,
//                     EGL_YUV_PLANE0_TEXTURE_UNIT_NV, 0,
//                     EGL_YUV_PLANE1_TEXTURE_UNIT_NV, 1,
//                     EGL_NONE
//             };
//             if (eglstream->num_textures == 3) {
//                     attribs[8] = EGL_YUV_PLANE2_TEXTURE_UNIT_NV;
//                     attribs[9] = 2;
//                     attribs[10] = EGL_NONE;
//             }
//
//             EGLBoolean ret = stream_consumer_gltexture_attribs(dpy->egl_dpy, eglstream->stream, attribs);
//             if (ret == EGL_FALSE) {
//                     goto fail;
//             }
    }


// 	EGLBoolean ret = _eglStreamConsumerAcquire(dpy->egl_dpy, stream);
// 	if (ret == EGL_FALSE) {
// 		goto fail;
// 	}

    eglstreamwrapper_feedback_send_success(res);
    return;

fail:
    fprintf(stderr, "err 0x%x\n", eglGetError());
    eglstreamwrapper_feedback_send_failed(res);
    wl_resource_destroy(res);
}

static const struct eglstreamwrapper_interface eglstreamwrapper_impl = {
    eglstreamwrapper_bind_eglstream,
};

static void eglstreamwrapper_bind(struct wl_client *client, void *data, uint32_t version, uint32_t id)
{
    struct wl_resource *res = wl_resource_create(client, &eglstreamwrapper_interface, version, id);
    wl_resource_set_implementation(res, &eglstreamwrapper_impl, data, NULL);
}

struct eglstreamwrapper_display *eglstreamwrapper_bind_display(EGLDisplay egl_dpy, struct wl_display *wl_dpy,
                                                               int (*gen_texture)(void *), void *ud)
{
    struct eglstreamwrapper_display *dpy = malloc(sizeof(struct eglstreamwrapper_display));
    if (!dpy) {
        return EGL_FALSE;
    }

    dpy->egl_dpy = egl_dpy;
    dpy->wl_dpy = wl_dpy;
    dpy->global = wl_global_create(wl_dpy, &eglstreamwrapper_interface, 1, dpy, eglstreamwrapper_bind);
    dpy->gen_tex = gen_texture;
    dpy->ud = ud;

    if (!create_stream_from_fd) {
        create_stream_from_fd =
                (void *) eglGetProcAddress("eglCreateStreamFromFileDescriptorKHR");
        query_stream =
                (void *) eglGetProcAddress("eglQueryStreamKHR");
        stream_consumer_gltexture =
                (void *) eglGetProcAddress("eglStreamConsumerGLTextureExternalKHR");
        stream_consumer_gltexture_attribs =
                (void *) eglGetProcAddress("eglStreamConsumerGLTextureExternalAttribsNV");
        stream_consumer_acquire =
                (void *) eglGetProcAddress("eglStreamConsumerAcquireKHR");
    }

    return dpy;
}

bool eglstreamwrapper_own_buffer(struct wl_resource *buf)
{
    return wl_resource_instance_of(buf, &wl_buffer_interface, &eglstream_buffer_impl);
}

void eglstreamwrapper_get_buffer_size(struct wl_resource *buf, int *width, int *height)
{
    struct eglstream *eglstream = wl_resource_get_user_data(buf);
    *width = eglstream->width;
    *height = eglstream->height;
}

int eglstreamwrapper_acquire_buffer(struct wl_resource *buf)
{
    struct eglstream *eglstream = wl_resource_get_user_data(buf);

    EGLBoolean ret = stream_consumer_acquire(eglstream->dpy->egl_dpy, eglstream->stream);
    if (ret == EGL_FALSE) {
        fprintf(stderr, "no acquire\n");
        return 0;
    }
    return eglstream->tex[0];
}
