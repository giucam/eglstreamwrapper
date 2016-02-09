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

#ifndef WRAPPEDFUNCS_H
#define WRAPPEDFUNCS_H

#include "egl.h"

struct egl_api
{
    EGLBoolean (*eglChooseConfig)(EGLDisplay, const EGLint *, EGLConfig *, EGLint, EGLint *);
    EGLBoolean (*eglCopyBuffers)(EGLDisplay, EGLSurface, EGLNativePixmapType);
    EGLContext (*eglCreateContext)(EGLDisplay, EGLConfig, EGLContext, const EGLint *);
    EGLSurface (*eglCreatePbufferSurface)(EGLDisplay, EGLConfig, const EGLint *);
    EGLSurface (*eglCreatePixmapSurface)(EGLDisplay, EGLConfig, EGLNativePixmapType, const EGLint *);
    EGLSurface (*eglCreateWindowSurface)(EGLDisplay, EGLConfig, EGLNativeWindowType, const EGLint *);
    EGLBoolean (*eglDestroyContext)(EGLDisplay, EGLContext);
    EGLBoolean (*eglDestroySurface)(EGLDisplay, EGLSurface);
    EGLBoolean (*eglGetConfigAttrib)(EGLDisplay, EGLConfig, EGLint, EGLint *);
    EGLBoolean (*eglGetConfigs)(EGLDisplay, EGLConfig *, EGLint, EGLint *);
    EGLDisplay (*eglGetCurrentDisplay)(void);
    EGLSurface (*eglGetCurrentSurface)(EGLint);
    EGLDisplay (*eglGetDisplay)(EGLNativeDisplayType);
    EGLint (*eglGetError)(void);
    __eglMustCastToProperFunctionPointerType (*eglGetProcAddress)(const char *);
    EGLBoolean (*eglInitialize)(EGLDisplay, EGLint *, EGLint *);
    EGLBoolean (*eglMakeCurrent)(EGLDisplay, EGLSurface, EGLSurface, EGLContext);
    EGLBoolean (*eglQueryContext)(EGLDisplay, EGLContext, EGLint, EGLint *);
    const char *(*eglQueryString)(EGLDisplay, EGLint);
    EGLBoolean (*eglQuerySurface)(EGLDisplay, EGLSurface, EGLint, EGLint *);
    EGLBoolean (*eglSwapBuffers)(EGLDisplay, EGLSurface);
    EGLBoolean (*eglTerminate)(EGLDisplay);
    EGLBoolean (*eglWaitGL)(void);
    EGLBoolean (*eglWaitNative)(EGLint);

    EGLBoolean (*eglBindTexImage)(EGLDisplay, EGLSurface, EGLint );
    EGLBoolean (*eglReleaseTexImage)(EGLDisplay, EGLSurface, EGLint);
    EGLBoolean (*eglSurfaceAttrib)(EGLDisplay, EGLSurface, EGLint, EGLint);
    EGLBoolean (*eglSwapInterval)(EGLDisplay, EGLint);

    EGLBoolean (*eglBindAPI)(EGLenum );
    EGLenum (*eglQueryAPI)(void);
    EGLSurface (*eglCreatePbufferFromClientBuffer)(EGLDisplay, EGLenum, EGLClientBuffer, EGLConfig, const EGLint *);
    EGLBoolean (*eglReleaseThread)(void);
    EGLBoolean (*eglWaitClient)(void);

    EGLContext (*eglGetCurrentContext)(void);

    EGLSync (*eglCreateSync)(EGLDisplay, EGLenum, const EGLAttrib *);
    EGLBoolean (*eglDestroySync)(EGLDisplay, EGLSync);
    EGLint (*eglClientWaitSync)(EGLDisplay, EGLSync, EGLint, EGLTime);
    EGLBoolean (*eglGetSyncAttrib)(EGLDisplay, EGLSync, EGLint, EGLAttrib *);
    EGLImage (*eglCreateImage)(EGLDisplay, EGLContext, EGLenum, EGLClientBuffer, const EGLAttrib *);
    EGLBoolean (*eglDestroyImage)(EGLDisplay, EGLImage);
    EGLDisplay (*eglGetPlatformDisplay)(EGLenum, void *, const EGLAttrib *);
    EGLSurface (*eglCreatePlatformWindowSurface)(EGLDisplay, EGLConfig, void *, const EGLAttrib *);
    EGLSurface (*eglCreatePlatformPixmapSurface)(EGLDisplay, EGLConfig, void *, const EGLAttrib *);
    EGLBoolean (*eglWaitSync)(EGLDisplay, EGLSync, EGLint);
};

struct egl_api wrapped_funcs;

#endif
