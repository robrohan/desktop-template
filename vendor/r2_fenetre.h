/* r2_fenetre - v0.0 - public domain very basic, utf8 string library
    no warranty implied; use at your own risk

    Built in the style of: https://github.com/nothings/stb

    This is written with game development in mind.

    Do this:
       #define R2_FENETRE_IMPLEMENTATION
    before you include this file in *one* C or C++ file 
    to create the implementation.

    // i.e. it should look like this:
    #include ...
    #include ...
    #include ...
    #define R2_FENETRE_IMPLEMENTATION
    #include "r2_fenetre.h"

    You can then include without the define to just use the types

LICENSE
    See end of file for license information.
*/
#ifndef R2_FENETRE
#define R2_FENETRE

#ifdef __cplusplus
extern "C"
{
#endif

/* implementation */
#ifdef R2_FENETRE_IMPLEMENTATION


///////////////////////////////////////////////////////////////////
// Linux
// Linux
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// Windows
// Windows
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
// Apple
#if defined(__APPLE__)
#include <objc/runtime.h>
#include <objc/message.h>
#include <Carbon/Carbon.h>
#include <stdio.h>

#define cls objc_getClass
#define sel sel_getUid
#define msg ((id (*)(id, SEL))objc_msgSend)
#define msg_int ((id (*)(id, SEL, int))objc_msgSend)
#define msg_id  ((id (*)(id, SEL, id))objc_msgSend)
#define msg_ptr ((id (*)(id, SEL, void*))objc_msgSend)
#define msg_cls ((id (*)(Class, SEL))objc_msgSend)
#define msg_cls_chr ((id (*)(Class, SEL, char*))objc_msgSend)

typedef enum NSApplicationActivationPolicy {
    NSApplicationActivationPolicyRegular   = 0,
    NSApplicationActivationPolicyAccessory = 1,
    NSApplicationActivationPolicyERROR     = 2,
} NSApplicationActivationPolicy;

typedef enum NSWindowStyleMask {
    NSWindowStyleMaskBorderless     = 0,
    NSWindowStyleMaskTitled         = 1 << 0,
    NSWindowStyleMaskClosable       = 1 << 1,
    NSWindowStyleMaskMiniaturizable = 1 << 2,
    NSWindowStyleMaskResizable      = 1 << 3,
} NSWindowStyleMask;

typedef enum NSBackingStoreType {
    NSBackingStoreBuffered = 2,
} NSBackingStoreType;

int main(int argc, char *argv[]) {
    // Create an autorelease pool
    id pool = msg_cls(cls("NSAutoreleasePool"), sel("alloc"));
    pool = msg(pool, sel("init"));
    // Get shared application instance
    id app = msg_cls(cls("NSApplication"), sel("sharedApplication"));
    // Set application activation policy
    msg_int(app, sel("setActivationPolicy:"), NSApplicationActivationPolicyRegular);

    struct CGRect frameRect = {0, 0, 600, 500};

    // Allocate and initialize NSWindow
    id window = ((id (*)(id, SEL, struct CGRect, int, int, int))objc_msgSend)(
        msg_cls(cls("NSWindow"), sel("alloc")),
        sel("initWithContentRect:styleMask:backing:defer:"),
        frameRect,
        NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|NSWindowStyleMaskResizable,
        NSBackingStoreBuffered,
        false
    );

    // Set window title
    msg_id(window, sel("setTitle:"), msg_cls_chr(cls("NSString"), sel("stringWithUTF8String:"), "Pure C App"));
    // Make window key and order front
    msg_ptr(window, sel("makeKeyAndOrderFront:"), nil);
    // Activate app ignoring other apps
    msg_int(app, sel("activateIgnoringOtherApps:"), true);
    // Get NSString for default run loop mode
    id defaultMode = msg_cls_chr(cls("NSString"), sel("stringWithUTF8String:"), "kCFRunLoopDefaultMode");

    // Start the event loop
    while (1) {
        // Create an autorelease pool for the event loop
        id eventPool = msg_cls(cls("NSAutoreleasePool"), sel("alloc"));
        eventPool = msg(eventPool, sel("init"));

        // Get the next event
        id event = ((id (*)(id, SEL, unsigned long long, id, id, bool))objc_msgSend)(app,
            sel("nextEventMatchingMask:untilDate:inMode:dequeue:"),
            UINT64_MAX, nil, defaultMode, true);

        // Check if an event was received
        if (event) {
            // Print event description
            id description = msg(event, sel("description"));
            const char *eventDesc = ((const char* (*)(id, SEL))objc_msgSend)(description, sel("UTF8String"));
            printf("Event: %s\n", eventDesc);

            // Dispatch the event
            msg_id(app, sel("sendEvent:"), event);
            msg(app, sel("updateWindows"));
        }
        // Drain the autorelease pool for the event loop
        msg(eventPool, sel("drain"));
    }

    // Release allocated objects (to prevent memory leak)
    msg(window, sel("release"));
    msg(app, sel("release"));
    // Drain the main autorelease pool
    msg(pool, sel("drain"));
    return 0;
}

#endif
// Apple
///////////////////////////////////////////////////////////////////

#endif
/* implementation */

#ifdef __cplusplus
}
#endif

#endif /* R2_STRINGS */

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2020 Rob Rohan
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/
