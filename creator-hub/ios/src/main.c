/* Creator Hub for iOS: a native UIKit shell around the app page (WKWebView).
   Written against the Objective-C runtime directly, so it builds without Xcode. */
#include <stdbool.h>
#include <stddef.h>

typedef struct objc_object *id;
typedef struct objc_class *Class;
typedef struct objc_selector *SEL;
typedef void (*IMP)(void);
typedef double CGFloat;
typedef struct { CGFloat x, y; } CGPoint;
typedef struct { CGFloat w, h; } CGSize;
typedef struct { CGPoint o; CGSize s; } CGRect;
#define nil ((id)0)

extern void objc_msgSend(void);
extern Class objc_getClass(const char *);
extern SEL sel_registerName(const char *);
extern Class objc_allocateClassPair(Class, const char *, size_t);
extern void objc_registerClassPair(Class);
extern bool class_addMethod(Class, SEL, IMP, const char *);
extern void *objc_getProtocol(const char *);
extern bool class_addProtocol(Class, void *);
extern void *objc_autoreleasePoolPush(void);
extern void objc_autoreleasePoolPop(void *);
extern int UIApplicationMain(int, char **, id, id);
extern char OBJC_CLASS_$_WKWebView; /* keeps WebKit linked */
void *const keep_webkit = &OBJC_CLASS_$_WKWebView;

#define S(n) sel_registerName(n)
#define C(n) ((id)objc_getClass(n))
static id M0(id o, const char *s) { return ((id (*)(id, SEL))objc_msgSend)(o, S(s)); }
static id M1(id o, const char *s, id a) { return ((id (*)(id, SEL, id))objc_msgSend)(o, S(s), a); }
static id M2(id o, const char *s, id a, id b) { return ((id (*)(id, SEL, id, id))objc_msgSend)(o, S(s), a, b); }
static void MB(id o, const char *s, bool b) { ((void (*)(id, SEL, bool))objc_msgSend)(o, S(s), b); }
static void MI(id o, const char *s, long v) { ((void (*)(id, SEL, long))objc_msgSend)(o, S(s), v); }
static bool RB(id o, const char *s) { return ((bool (*)(id, SEL))objc_msgSend)(o, S(s)); }
static long RI(id o, const char *s) { return ((long (*)(id, SEL))objc_msgSend)(o, S(s)); }
static bool ISA(id o, const char *cls) { return ((bool (*)(id, SEL, Class))objc_msgSend)(o, S("isKindOfClass:"), objc_getClass(cls)); }
static id NS(const char *c) { return ((id (*)(id, SEL, const char *))objc_msgSend)(C("NSString"), S("stringWithUTF8String:"), c); }
static const char *CS(id s) { return ((const char *(*)(id, SEL))objc_msgSend)(s, S("UTF8String")); }
static bool streq(const char *a, const char *b) { if (!a || !b) return false; while (*a && *a == *b) { a++; b++; } return *a == *b; }

static const char *PRIMARY_ICON = "lines_e3262d";
static id g_window, g_vc, g_web, g_bridge;
static bool g_dark;

/* Defines window.SmoreNative before the page runs, same API as the Android app. */
static const char *SHIM =
  "window.SmoreNative={"
  "setDark:function(d){try{webkit.messageHandlers.smore.postMessage({t:'dark',v:!!d})}catch(e){}},"
  "setIcon:function(n){try{webkit.messageHandlers.smore.postMessage({t:'icon',v:String(n)})}catch(e){}}};";

static void paint_background(void) {
  id col = ((id (*)(id, SEL, CGFloat, CGFloat))objc_msgSend)(C("UIColor"), S("colorWithWhite:alpha:"), g_dark ? 15.0 / 255.0 : 1.0, 1.0);
  if (g_window) M1(g_window, "setBackgroundColor:", col);
  if (g_web) { M1(g_web, "setBackgroundColor:", col); M1(M0(g_web, "scrollView"), "setBackgroundColor:", col); }
}

/* ---- view controller: status bar follows the app's light/dark theme ---- */
static long vc_statusStyle(id self, SEL cmd) { (void)self; (void)cmd; return g_dark ? 1 /* light content */ : 3 /* dark content */; }

/* ---- bridge: messages from the page ---- */
static void bridge_receive(id self, SEL cmd, id ucc, id msg) {
  (void)self; (void)cmd; (void)ucc;
  id body = M0(msg, "body");
  if (!body || !ISA(body, "NSDictionary")) return;
  id t = M1(body, "objectForKey:", NS("t")), v = M1(body, "objectForKey:", NS("v"));
  if (!t || !v || !ISA(t, "NSString")) return;
  const char *type = CS(t);
  if (streq(type, "dark") && ISA(v, "NSNumber")) {
    g_dark = RB(v, "boolValue");
    paint_background();
    if (g_vc) M0(g_vc, "setNeedsStatusBarAppearanceUpdate");
  } else if (streq(type, "icon") && ISA(v, "NSString")) {
    id app = M0(C("UIApplication"), "sharedApplication");
    if (!RB(app, "supportsAlternateIcons")) return;
    const char *want = CS(v);
    id cur = M0(app, "alternateIconName");
    const char *have = cur ? CS(cur) : NULL;
    bool wantPrimary = streq(want, PRIMARY_ICON);
    if ((wantPrimary && !have) || (!wantPrimary && streq(have, want))) return;
    ((void (*)(id, SEL, id, void *))objc_msgSend)(app, S("setAlternateIconName:completionHandler:"), wantPrimary ? nil : v, NULL);
  }
}

/* ---- app delegate ---- */
static id app_window(id self, SEL cmd) { (void)self; (void)cmd; return g_window; }
static void app_setWindow(id self, SEL cmd, id w) { (void)self; (void)cmd; g_window = w; }

static bool app_didFinish(id self, SEL cmd, id app, id opts) {
  (void)self; (void)cmd; (void)app; (void)opts;
  id screen = M0(C("UIScreen"), "mainScreen");
  CGRect bounds = ((CGRect (*)(id, SEL))objc_msgSend)(screen, S("bounds"));
  g_dark = RI(M0(screen, "traitCollection"), "userInterfaceStyle") == 2;

  g_window = ((id (*)(id, SEL, CGRect))objc_msgSend)(M0(C("UIWindow"), "alloc"), S("initWithFrame:"), bounds);
  g_vc = M0(M0(C("SHViewController"), "alloc"), "init");

  id cfg = M0(M0(C("WKWebViewConfiguration"), "alloc"), "init");
  id ucc = M0(cfg, "userContentController");
  g_bridge = M0(M0(C("SHBridge"), "alloc"), "init");
  M2(ucc, "addScriptMessageHandler:name:", g_bridge, NS("smore"));
  id script = ((id (*)(id, SEL, id, long, bool))objc_msgSend)(M0(C("WKUserScript"), "alloc"),
      S("initWithSource:injectionTime:forMainFrameOnly:"), NS(SHIM), 0 /* document start */, true);
  M1(ucc, "addUserScript:", script);

  g_web = ((id (*)(id, SEL, CGRect, id))objc_msgSend)(M0(C("WKWebView"), "alloc"), S("initWithFrame:configuration:"), bounds, cfg);
  MI(g_web, "setAutoresizingMask:", 2 | 16);
  MB(g_web, "setOpaque:", false);
  MB(g_web, "setAllowsLinkPreview:", false);
  id sv = M0(g_web, "scrollView");
  MI(sv, "setContentInsetAdjustmentBehavior:", 2 /* never: the page handles safe areas */);
  MB(sv, "setBounces:", false);
  paint_background();

  M1(g_vc, "setView:", g_web);
  M1(g_window, "setRootViewController:", g_vc);
  M0(g_window, "makeKeyAndVisible");

  id bundle = M0(C("NSBundle"), "mainBundle");
  id url = M2(bundle, "URLForResource:withExtension:", NS("index"), NS("html"));
  if (url) M2(g_web, "loadFileURL:allowingReadAccessToURL:", url, M0(url, "URLByDeletingLastPathComponent"));
  return true;
}

int main(int argc, char **argv) {
  void *pool = objc_autoreleasePoolPush();

  Class d = objc_allocateClassPair(objc_getClass("UIResponder"), "SHAppDelegate", 0);
  class_addMethod(d, S("application:didFinishLaunchingWithOptions:"), (IMP)app_didFinish, "B@:@@");
  class_addMethod(d, S("window"), (IMP)app_window, "@@:");
  class_addMethod(d, S("setWindow:"), (IMP)app_setWindow, "v@:@");
  void *p = objc_getProtocol("UIApplicationDelegate");
  if (p) class_addProtocol(d, p);
  objc_registerClassPair(d);

  Class vc = objc_allocateClassPair(objc_getClass("UIViewController"), "SHViewController", 0);
  class_addMethod(vc, S("preferredStatusBarStyle"), (IMP)vc_statusStyle, "q@:");
  objc_registerClassPair(vc);

  Class b = objc_allocateClassPair(objc_getClass("NSObject"), "SHBridge", 0);
  class_addMethod(b, S("userContentController:didReceiveScriptMessage:"), (IMP)bridge_receive, "v@:@@");
  void *wp = objc_getProtocol("WKScriptMessageHandler");
  if (wp) class_addProtocol(b, wp);
  objc_registerClassPair(b);

  int r = UIApplicationMain(argc, argv, nil, NS("SHAppDelegate"));
  objc_autoreleasePoolPop(pool);
  return r;
}
