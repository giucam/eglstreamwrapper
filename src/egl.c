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

#include <dlfcn.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "egl.h"
#include "wrapper.h"
#include "wrappedfuncs.h"
#include "server.h"

static void *libegl = NULL;
struct egl_api wrapped_funcs;

#define DLSYM(sym) \
    wrapped_funcs.sym = dlsym(libegl, #sym); \
//     assert(wrapped_funcs.sym);

__attribute__((constructor))
static void initialize()
{
    libegl = dlopen("/usr/lib/libEGL.so", RTLD_LAZY);
    assert(libegl);

fprintf(stderr, "LOAD!! \n");

    DLSYM(eglChooseConfig);
    DLSYM(eglCopyBuffers);
    DLSYM(eglCreateContext);
    DLSYM(eglCreatePbufferSurface);
    DLSYM(eglCreatePixmapSurface);
    DLSYM(eglCreateWindowSurface);
    DLSYM(eglDestroyContext);
    DLSYM(eglDestroySurface);
    DLSYM(eglGetConfigAttrib);
    DLSYM(eglGetConfigs);
    DLSYM(eglGetCurrentDisplay);
    DLSYM(eglGetCurrentSurface);
    DLSYM(eglGetDisplay)
    DLSYM(eglGetError);
    DLSYM(eglGetProcAddress);
    DLSYM(eglInitialize);
    DLSYM(eglMakeCurrent);
    DLSYM(eglQueryContext);
    DLSYM(eglQueryString);
    DLSYM(eglQuerySurface);
    DLSYM(eglSwapBuffers);
    DLSYM(eglTerminate);
    DLSYM(eglWaitGL);
    DLSYM(eglWaitNative);

    DLSYM(eglBindTexImage);
    DLSYM(eglReleaseTexImage)
    DLSYM(eglSurfaceAttrib)
    DLSYM(eglSwapInterval)

    DLSYM(eglBindAPI)
    DLSYM(eglQueryAPI)
    DLSYM(eglCreatePbufferFromClientBuffer)
    DLSYM(eglReleaseThread)
    DLSYM(eglWaitClient)

    DLSYM(eglGetCurrentContext)

    DLSYM(eglCreateSync)
    DLSYM(eglDestroySync)
    DLSYM(eglClientWaitSync)
    DLSYM(eglGetSyncAttrib)
    DLSYM(eglCreateImage)
    DLSYM(eglDestroyImage)
    DLSYM(eglGetPlatformDisplay)
    DLSYM(eglCreatePlatformWindowSurface)
    DLSYM(eglCreatePlatformPixmapSurface)
    DLSYM(eglWaitSync)

}

EGLBoolean eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{
    struct wrapper_display *wd = wrapper_display_get_wrapper(dpy);
    if (wd) {
        return wrapper_choose_config(wd, attrib_list, configs, config_size, num_config);
    }
    return wrapped_funcs.eglChooseConfig(dpy, attrib_list, configs, config_size, num_config);
}

EGLBoolean eglCopyBuffers(EGLDisplay dpy, EGLSurface surface, EGLNativePixmapType target)
{
    return wrapped_funcs.eglCopyBuffers(dpy, surface, target);
}

EGLContext eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list)
{
    return wrapped_funcs.eglCreateContext(dpy, config, share_context, attrib_list);
}

EGLSurface eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config, const EGLint *attrib_list)
{
    return wrapped_funcs.eglCreatePbufferSurface(dpy, config, attrib_list);
}

EGLSurface eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint *attrib_list)
{
    return wrapped_funcs.eglCreatePixmapSurface(dpy, config, pixmap, attrib_list);
}

EGLSurface eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list)
{
    struct wrapper_display *wd = wrapper_display_get_wrapper(dpy);
    if (wd) {
        return wrapper_create_surface(wd, config, win, attrib_list);
    }
    return wrapped_funcs.eglCreateWindowSurface(dpy, config, win, attrib_list);
}

EGLBoolean eglDestroyContext(EGLDisplay dpy, EGLContext ctx)
{
    return wrapped_funcs.eglDestroyContext(dpy, ctx);
}

EGLBoolean eglDestroySurface(EGLDisplay dpy, EGLSurface surface)
{
    return wrapped_funcs.eglDestroySurface(dpy, surface);
}

EGLBoolean eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value)
{
    return wrapped_funcs.eglGetConfigAttrib(dpy, config, attribute, value);
}

EGLBoolean eglGetConfigs(EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{
    return wrapped_funcs.eglGetConfigs(dpy, configs, config_size, num_config);
}

EGLDisplay eglGetCurrentDisplay(void)
{
    return wrapped_funcs.eglGetCurrentDisplay();
}

EGLSurface eglGetCurrentSurface(EGLint readdraw)
{
    return wrapped_funcs.eglGetCurrentSurface(readdraw);
}

EGLDisplay eglGetDisplay(EGLNativeDisplayType display_id)
{
    EGLDisplay dpy = wrapper_get_display(display_id);
    if (!dpy) {
        dpy = wrapped_funcs.eglGetDisplay(display_id);
    }
    return dpy;
}

EGLint eglGetError(void)
{
    return wrapped_funcs.eglGetError();
}

__eglMustCastToProperFunctionPointerType eglGetProcAddress(const char *procname)
{
    return wrapped_funcs.eglGetProcAddress(procname);
}

EGLBoolean eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor)
{
    return wrapped_funcs.eglInitialize(dpy, major, minor);
}

EGLBoolean eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
    return wrapped_funcs.eglMakeCurrent(dpy, draw, read, ctx);
}

EGLBoolean eglQueryContext(EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint *value)
{
    return wrapped_funcs.eglQueryContext(dpy, ctx, attribute, value);
}

const char *eglQueryString(EGLDisplay dpy, EGLint name)
{
    return wrapped_funcs.eglQueryString(dpy, name);
}

EGLBoolean eglQuerySurface(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value)
{
    return wrapped_funcs.eglQuerySurface(dpy, surface, attribute, value);
}

EGLBoolean eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
    struct wrapper_display *wd = wrapper_display_get_wrapper(dpy);
    if (wd) {
        return wrapper_swap_buffers(wd, surface);
    }
    return wrapped_funcs.eglSwapBuffers(dpy, surface);
}

EGLBoolean eglTerminate(EGLDisplay dpy)
{
    return wrapped_funcs.eglTerminate(dpy);
}

EGLBoolean eglWaitGL(void)
{
    return wrapped_funcs.eglWaitGL();
}

EGLBoolean eglWaitNative(EGLint engine)
{
    return wrapped_funcs.eglWaitNative(engine);
}


EGLBoolean eglBindTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
    return wrapped_funcs.eglBindTexImage(dpy, surface, buffer);
}

EGLBoolean eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
{
    return wrapped_funcs.eglReleaseTexImage(dpy, surface, buffer);
}

EGLBoolean eglSurfaceAttrib(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
{
    return wrapped_funcs.eglSurfaceAttrib(dpy, surface, attribute, value);
}

EGLBoolean eglSwapInterval(EGLDisplay dpy, EGLint interval)
{
    struct wrapper_display *wd = wrapper_display_get_wrapper(dpy);
    if (wd) {
        return wrapper_swap_interval(wd, interval);
    }
    return wrapped_funcs.eglSwapInterval(dpy, interval);
}


EGLBoolean eglBindAPI(EGLenum api)
{
    return wrapped_funcs.eglBindAPI(api);
}

EGLenum eglQueryAPI(void)
{
    return wrapped_funcs.eglQueryAPI();
}

EGLSurface eglCreatePbufferFromClientBuffer(EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer,
                                            EGLConfig config, const EGLint *attrib_list)
{
    return wrapped_funcs.eglCreatePbufferFromClientBuffer(dpy, buftype, buffer, config, attrib_list);
}

EGLBoolean eglReleaseThread(void)
{
    return wrapped_funcs.eglReleaseThread();
}

EGLBoolean eglWaitClient(void)
{
    return wrapped_funcs.eglWaitClient();
}


EGLContext eglGetCurrentContext(void)
{
    return wrapped_funcs.eglGetCurrentContext();
}


EGLSync eglCreateSync(EGLDisplay dpy, EGLenum type, const EGLAttrib *attrib_list)
{
    return wrapped_funcs.eglCreateSync(dpy, type, attrib_list);
}

EGLBoolean eglDestroySync(EGLDisplay dpy, EGLSync sync)
{
    return wrapped_funcs.eglDestroySync(dpy, sync);
}

EGLint eglClientWaitSync(EGLDisplay dpy, EGLSync sync, EGLint flags, EGLTime timeout)
{
    return wrapped_funcs.eglClientWaitSync(dpy, sync, flags, timeout);
}

EGLBoolean eglGetSyncAttrib(EGLDisplay dpy, EGLSync sync, EGLint attribute, EGLAttrib *value)
{
    return wrapped_funcs.eglGetSyncAttrib(dpy, sync, attribute, value);
}

EGLImage eglCreateImage(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLAttrib *attrib_list)
{
    return wrapped_funcs.eglCreateImage(dpy, ctx, target, buffer, attrib_list);
}

EGLBoolean eglDestroyImage(EGLDisplay dpy, EGLImage image)
{
    return wrapped_funcs.eglDestroyImage(dpy, image);
}

EGLDisplay eglGetPlatformDisplay(EGLenum platform, void *native_display, const EGLAttrib *attrib_list)
{
    return wrapped_funcs.eglGetPlatformDisplay(platform, native_display, attrib_list);
}

EGLSurface eglCreatePlatformWindowSurface(EGLDisplay dpy, EGLConfig config, void *native_window, const EGLAttrib *attrib_list)
{
    return wrapped_funcs.eglCreatePlatformWindowSurface(dpy, config, native_window, attrib_list);
}

EGLSurface eglCreatePlatformPixmapSurface(EGLDisplay dpy, EGLConfig config, void *native_pixmap, const EGLAttrib *attrib_list)
{
    return wrapped_funcs.eglCreatePlatformPixmapSurface(dpy, config, native_pixmap, attrib_list);
}

EGLBoolean eglWaitSync(EGLDisplay dpy, EGLSync sync, EGLint flags)
{
    return wrapped_funcs.eglWaitSync(dpy, sync, flags);
}
