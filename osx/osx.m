#if 0
#!/bin/bash
gcc osx.m -o osx -framework Cocoa -x objective-c
if [ "$?" = "0" ]; then
    time ./osx
fi
exit
#endif

#include <stdio.h>

#import <Cocoa/Cocoa.h>

struct cocoa_app_class {
    id menubar;
} cocoa_app;

#define USE_STRUCT

void cocoa_add_menu_item(id appMenu, id quitTitle, id key, SEL event) {
    id quitMenuItem = [[[NSMenuItem alloc] initWithTitle:quitTitle
                                                  action:event keyEquivalent:key] autorelease];
    [appMenu addItem:quitMenuItem];
}

int main(int argc, char* argv[]) {
#ifdef USE_STRUCT
    [NSAutoreleasePool new];
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    cocoa_app.menubar = [[NSMenu new] autorelease];
    id appMenuItem = [[NSMenuItem new] autorelease];
    [cocoa_app.menubar addItem:appMenuItem];
    [NSApp setMainMenu:cocoa_app.menubar];
    id appMenu = [[NSMenu new] autorelease];
    id appName = [[NSProcessInfo processInfo] processName];
    
    // Creates quit menu item.
    id quitTitle = [@"Quit " stringByAppendingString:appName];
    cocoa_add_menu_item(appMenu, quitTitle, @"q", @selector(terminate:));
    
    [appMenuItem setSubmenu:appMenu];
    id window = [[[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 200, 200)
                                             styleMask:NSTitledWindowMask backing:NSBackingStoreBuffered defer:NO]
                 autorelease];
    [window cascadeTopLeftFromPoint:NSMakePoint(20,20)];
    [window setTitle:appName];
    [window makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
    [NSApp run];
    return 0;
#else
    [NSAutoreleasePool new];
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    id menubar = [[NSMenu new] autorelease];
    id appMenuItem = [[NSMenuItem new] autorelease];
    [menubar addItem:appMenuItem];
    [NSApp setMainMenu:menubar];
    id appMenu = [[NSMenu new] autorelease];
    id appName = [[NSProcessInfo processInfo] processName];
    id quitTitle = [@"Quit " stringByAppendingString:appName];
    id quitMenuItem = [[[NSMenuItem alloc] initWithTitle:quitTitle
                                                  action:@selector(terminate:) keyEquivalent:@"q"] autorelease];
    [appMenu addItem:quitMenuItem];
    [appMenuItem setSubmenu:appMenu];
    id window = [[[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 200, 200)
                                             styleMask:NSTitledWindowMask backing:NSBackingStoreBuffered defer:NO]
                 autorelease];
    [window cascadeTopLeftFromPoint:NSMakePoint(20,20)];
    [window setTitle:appName];
    [window makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
    [NSApp run];
    return 0;
#endif
}
