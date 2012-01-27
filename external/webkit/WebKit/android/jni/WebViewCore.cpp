/*
 * Copyright 2006, The Android Open Source Project
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define LOG_TAG "webcoreglue"

#include "config.h"
#include "WebViewCore.h"

#include "AtomicString.h"
#include "CachedNode.h"
#include "CachedRoot.h"
#include "Chrome.h"
#include "ChromeClientAndroid.h"
#include "Color.h"
#include "DatabaseTracker.h"
#include "Document.h"
#include "DOMWindow.h"
#include "Element.h"
#include "Editor.h"
#include "EditorClientAndroid.h"
#include "EventHandler.h"
#include "EventNames.h"
#include "FocusController.h"
#include "Font.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "FrameLoaderClientAndroid.h"
#include "FrameTree.h"
#include "FrameView.h"
#include "Geolocation.h"
#include "GraphicsContext.h"
#include "GraphicsJNI.h"
#include "HTMLAnchorElement.h"
#include "HTMLAreaElement.h"
#include "HTMLElement.h"
#include "HTMLImageElement.h"
#include "HTMLInputElement.h"
#include "HTMLLabelElement.h"
#include "HTMLLinkElement.h"
#include "HTMLMapElement.h"
#include "HTMLNames.h"
#include "HTMLOptGroupElement.h"
#include "HTMLOptionElement.h"
#include "HTMLSelectElement.h"
#include "HTMLTextAreaElement.h"
#include "HistoryItem.h"
#include "HitTestResult.h"
#if ENABLE(WML)
#include "WMLOptGroupElement.h"
#include "WMLOptionElement.h"
#include "WMLSelectElement.h"
#endif
#include "InlineTextBox.h"
#include "KeyboardCodes.h"
#include "Navigator.h"
#include "Node.h"
#include "NodeList.h"
#include "Page.h"
#include "PageGroup.h"
#include "PlatformKeyboardEvent.h"
#include "PlatformString.h"
#include "PluginWidgetAndroid.h"
#include "PluginView.h"
#include "Position.h"
#include "ProgressTracker.h"
#include "RenderBox.h"
#include "RenderLayer.h"
#include "RenderPart.h"
#include "RenderText.h"
#include "RenderTextControl.h"
#include "RenderThemeAndroid.h"
#include "RenderView.h"
#include "ResourceRequest.h"
#include "SelectionController.h"
#include "Settings.h"
#include "SkANP.h"
#include "SkTemplates.h"
#include "SkTDArray.h"
#include "SkTypes.h"
#include "SkCanvas.h"
#include "SkPicture.h"
#include "SkUtils.h"
#include "StringImpl.h"
#include "Text.h"
#include "TypingCommand.h"
#include "WebCoreFrameBridge.h"
#include "WebFrameView.h"
#include "android_graphics.h"

#include <JNIHelp.h>
#include <JNIUtility.h>
#include <ui/KeycodeLabels.h>
#include <wtf/CurrentTime.h>

#if USE(V8)
#include "CString.h"
#include "ScriptController.h"
#include "V8Counters.h"
#endif

#if DEBUG_NAV_UI
#include "SkTime.h"
#endif

#if ENABLE(TOUCH_EVENTS) // Android
#include "PlatformTouchEvent.h"
#endif

#ifdef ANDROID_DOM_LOGGING
#include "AndroidLog.h"
#include "RenderTreeAsText.h"
#include "CString.h"

FILE* gDomTreeFile = 0;
FILE* gRenderTreeFile = 0;
#endif

#ifdef ANDROID_INSTRUMENT
#include "TimeCounter.h"
#endif

#if USE(ACCELERATED_COMPOSITING)
#include "GraphicsLayerAndroid.h"
#include "RenderLayerCompositor.h"
#endif

// SAMSUNG CHANGES COPY PASTE
#include "visible_units.h"
// SAMSUNG CHANGES END

/*  We pass this flag when recording the actual content, so that we don't spend
    time actually regionizing complex path clips, when all we really want to do
    is record them.
 */
#define PICT_RECORD_FLAGS   SkPicture::kUsePathBoundsForClip_RecordingFlag

////////////////////////////////////////////////////////////////////////////////////////////////
//SanJose++
extern void ANPSurfaceInterfaceSetDelay(int ms);
//SanJose--

/////////////////SISO_HTMLCOMPOSER begin
namespace WebCore {
    extern WebCore::String createLocalResource(WebCore::Frame* frame , WebCore::String url);
    extern bool saveCachedImageToFile(WebCore::Frame* frame, WebCore::String imageUrl, WebCore::String filePath);;
    extern void copyImagePathToClipboard(const WebCore::String& imagePath);
    extern android::WebHTMLMarkupData* createFullMarkup(const Node* node,const String& basePath = String());
}
static const UChar NonBreakingSpaceCharacter = 0xA0;
static const UChar SpaceCharacter = ' ';
///////////////////////SISO_HTMLCOMPOSER end
namespace android {

static SkTDArray<WebViewCore*> gInstanceList;

void WebViewCore::addInstance(WebViewCore* inst) {
    *gInstanceList.append() = inst;
}

void WebViewCore::removeInstance(WebViewCore* inst) {
    int index = gInstanceList.find(inst);
    LOG_ASSERT(index >= 0, "RemoveInstance inst not found");
    if (index >= 0) {
        gInstanceList.removeShuffle(index);
    }
}

bool WebViewCore::isInstance(WebViewCore* inst) {
    return gInstanceList.find(inst) >= 0;
}

jobject WebViewCore::getApplicationContext() {

    // check to see if there is a valid webviewcore object
    if (gInstanceList.isEmpty())
        return 0;

    // get the context from the webview
    jobject context = gInstanceList[0]->getContext();

    if (!context)
        return 0;

    // get the application context using JNI
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    jclass contextClass = env->GetObjectClass(context);
    jmethodID appContextMethod = env->GetMethodID(contextClass, "getApplicationContext", "()Landroid/content/Context;");
    jobject result = env->CallObjectMethod(context, appContextMethod);
    checkException(env);
    return result;
}


struct WebViewCoreStaticMethods {
    jmethodID    m_supportsMimeType;
} gWebViewCoreStaticMethods;

// Check whether a media mimeType is supported in Android media framework.
bool WebViewCore::supportsMimeType(const WebCore::String& mimeType) {
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    jstring jMimeType = env->NewString(mimeType.characters(), mimeType.length());
    jclass webViewCore = env->FindClass("android/webkit/WebViewCore");
    bool val = env->CallStaticBooleanMethod(webViewCore,
          gWebViewCoreStaticMethods.m_supportsMimeType, jMimeType);
    checkException(env);
    env->DeleteLocalRef(jMimeType);

    return val;
}

// ----------------------------------------------------------------------------

#define GET_NATIVE_VIEW(env, obj) ((WebViewCore*)env->GetIntField(obj, gWebViewCoreFields.m_nativeClass))

// Field ids for WebViewCore
struct WebViewCoreFields {
    jfieldID    m_nativeClass;
    jfieldID    m_viewportWidth;
    jfieldID    m_viewportHeight;
    jfieldID    m_viewportInitialScale;
    jfieldID    m_viewportMinimumScale;
    jfieldID    m_viewportMaximumScale;
    jfieldID    m_viewportUserScalable;
    jfieldID    m_viewportDensityDpi;
    jfieldID    m_webView;
} gWebViewCoreFields;

// ----------------------------------------------------------------------------

struct WebViewCore::JavaGlue {
    jweak       m_obj;
    jmethodID   m_spawnScrollTo;
    jmethodID   m_scrollTo;
    jmethodID   m_scrollBy;
    jmethodID   m_contentDraw;
    jmethodID   m_requestListBox;
    jmethodID   m_openFileChooser;
    jmethodID   m_requestSingleListBox;
    jmethodID   m_jsAlert;
    jmethodID   m_jsConfirm;
    jmethodID   m_jsPrompt;
    jmethodID   m_jsUnload;
    jmethodID   m_jsInterrupt;
    jmethodID   m_didFirstLayout;
    jmethodID   m_updateViewport;
    jmethodID   m_sendNotifyProgressFinished;
    jmethodID   m_sendViewInvalidate;
    jmethodID   m_sendImmediateRepaint;
    jmethodID   m_setRootLayer;
    jmethodID   m_updateTextfield;
    jmethodID   m_updateTextSelection;
    jmethodID   m_clearTextEntry;
    jmethodID   m_restoreScale;
    jmethodID   m_restoreScreenWidthScale;
    jmethodID   m_needTouchEvents;
    jmethodID   m_requestKeyboard;
    jmethodID   m_requestKeyboardWithSelection;
    jmethodID   m_exceededDatabaseQuota;
    jmethodID   m_reachedMaxAppCacheSize;
    jmethodID   m_populateVisitedLinks;
    jmethodID   m_geolocationPermissionsShowPrompt;
    jmethodID   m_geolocationPermissionsHidePrompt;
    jmethodID   m_addMessageToConsole;
    jmethodID   m_getPluginClass;
    jmethodID   m_showFullScreenPlugin;
    jmethodID   m_hideFullScreenPlugin;
    jmethodID   m_addSurface;
    jmethodID   m_updateSurface;
    jmethodID   m_destroySurface;
    jmethodID   m_getContext;
    jmethodID   m_sendFindAgain;
    jmethodID   m_showRect;
    jmethodID   m_centerFitRect;
    jmethodID   m_setScrollbarModes;
    jmethodID   m_HttpEquivhandle; //SAMSUNG CHANGE
    ///////////////////SISO_HTMLCOMPOSER
    jmethodID   m_isEditableSupport;	
    ////////////////////////////////////   	
    AutoJObject object(JNIEnv* env) {
        return getRealObject(env, m_obj);
    }
};

/*
 * WebViewCore Implementation
 */

static jmethodID GetJMethod(JNIEnv* env, jclass clazz, const char name[], const char signature[])
{
    jmethodID m = env->GetMethodID(clazz, name, signature);
    LOG_ASSERT(m, "Could not find method %s", name);
    return m;
}

Mutex WebViewCore::gFrameCacheMutex;
Mutex WebViewCore::gButtonMutex;
Mutex WebViewCore::gCursorBoundsMutex;
Mutex WebViewCore::m_contentMutex;

WebViewCore::WebViewCore(JNIEnv* env, jobject javaWebViewCore, WebCore::Frame* mainframe)
        : m_pluginInvalTimer(this, &WebViewCore::pluginInvalTimerFired)
{
    m_mainFrame = mainframe;

    m_popupReply = 0;
    m_moveGeneration = 0;
    m_lastGeneration = 0;
    m_touchGeneration = 0;
    m_blockTextfieldUpdates = false;
    // just initial values. These should be set by client
    m_maxXScroll = 320/4;
    m_maxYScroll = 240/4;
    m_textGeneration = 0;
    m_screenWidth = 320;
    m_scale = 1;
    m_screenWidthScale = 1;
#if ENABLE(TOUCH_EVENTS)
//SanJose++
    m_forwardingTouchEvents_plugin = false;
    m_forwardingTouchEvents_others = false;
//SanJose--
#endif
    m_isPaused = false;

    LOG_ASSERT(m_mainFrame, "Uh oh, somehow a frameview was made without an initial frame!");

    jclass clazz = env->GetObjectClass(javaWebViewCore);
    m_javaGlue = new JavaGlue;
    m_javaGlue->m_obj = env->NewWeakGlobalRef(javaWebViewCore);
    m_javaGlue->m_spawnScrollTo = GetJMethod(env, clazz, "contentSpawnScrollTo", "(II)V");
    m_javaGlue->m_scrollTo = GetJMethod(env, clazz, "contentScrollTo", "(II)V");
    m_javaGlue->m_scrollBy = GetJMethod(env, clazz, "contentScrollBy", "(IIZ)V");
    m_javaGlue->m_contentDraw = GetJMethod(env, clazz, "contentDraw", "()V");
    //SAMSUNG CHANGE
    m_javaGlue->m_requestListBox = GetJMethod(env, clazz, "requestListBox", "([Ljava/lang/String;Ljava/lang/String;[I[II)V");
    m_javaGlue->m_openFileChooser = GetJMethod(env, clazz, "openFileChooser", "()Ljava/lang/String;");
    //SAMSUNG CHANGE
    ///////////////////SISO_HTMLCOMPOSER
    m_javaGlue->m_isEditableSupport = GetJMethod(env, clazz, "isEditableSupport", "()Z");	
    ////////////////////////////////////	
    m_javaGlue->m_requestSingleListBox = GetJMethod(env, clazz, "requestListBox", "([Ljava/lang/String;Ljava/lang/String;[III)V");
    m_javaGlue->m_jsAlert = GetJMethod(env, clazz, "jsAlert", "(Ljava/lang/String;Ljava/lang/String;)V");
    m_javaGlue->m_jsConfirm = GetJMethod(env, clazz, "jsConfirm", "(Ljava/lang/String;Ljava/lang/String;)Z");
    m_javaGlue->m_jsPrompt = GetJMethod(env, clazz, "jsPrompt", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
    m_javaGlue->m_jsUnload = GetJMethod(env, clazz, "jsUnload", "(Ljava/lang/String;Ljava/lang/String;)Z");
    m_javaGlue->m_jsInterrupt = GetJMethod(env, clazz, "jsInterrupt", "()Z");
    m_javaGlue->m_didFirstLayout = GetJMethod(env, clazz, "didFirstLayout", "(Z)V");
    m_javaGlue->m_updateViewport = GetJMethod(env, clazz, "updateViewport", "()V");
    m_javaGlue->m_sendNotifyProgressFinished = GetJMethod(env, clazz, "sendNotifyProgressFinished", "()V");
    m_javaGlue->m_sendViewInvalidate = GetJMethod(env, clazz, "sendViewInvalidate", "(IIII)V");
    m_javaGlue->m_sendImmediateRepaint = GetJMethod(env, clazz, "sendImmediateRepaint", "()V");
    m_javaGlue->m_setRootLayer = GetJMethod(env, clazz, "setRootLayer", "(I)V");
    m_javaGlue->m_updateTextfield = GetJMethod(env, clazz, "updateTextfield", "(IZLjava/lang/String;I)V");
    m_javaGlue->m_updateTextSelection = GetJMethod(env, clazz, "updateTextSelection", "(IIII)V");
    m_javaGlue->m_clearTextEntry = GetJMethod(env, clazz, "clearTextEntry", "()V");
    m_javaGlue->m_restoreScale = GetJMethod(env, clazz, "restoreScale", "(I)V");
    m_javaGlue->m_restoreScreenWidthScale = GetJMethod(env, clazz, "restoreScreenWidthScale", "(I)V");
//SanJose++
    m_javaGlue->m_needTouchEvents = GetJMethod(env, clazz, "needTouchEvents", "(ZZ)V");
//SanJose--
    m_javaGlue->m_requestKeyboard = GetJMethod(env, clazz, "requestKeyboard", "(Z)V");
    m_javaGlue->m_requestKeyboardWithSelection = GetJMethod(env, clazz, "requestKeyboardWithSelection", "(IIII)V");
    m_javaGlue->m_exceededDatabaseQuota = GetJMethod(env, clazz, "exceededDatabaseQuota", "(Ljava/lang/String;Ljava/lang/String;JJ)V");
    m_javaGlue->m_reachedMaxAppCacheSize = GetJMethod(env, clazz, "reachedMaxAppCacheSize", "(J)V");
    m_javaGlue->m_populateVisitedLinks = GetJMethod(env, clazz, "populateVisitedLinks", "()V");
    m_javaGlue->m_geolocationPermissionsShowPrompt = GetJMethod(env, clazz, "geolocationPermissionsShowPrompt", "(Ljava/lang/String;)V");
    m_javaGlue->m_geolocationPermissionsHidePrompt = GetJMethod(env, clazz, "geolocationPermissionsHidePrompt", "()V");
    m_javaGlue->m_addMessageToConsole = GetJMethod(env, clazz, "addMessageToConsole", "(Ljava/lang/String;ILjava/lang/String;I)V");
    m_javaGlue->m_getPluginClass = GetJMethod(env, clazz, "getPluginClass", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Class;");
    m_javaGlue->m_showFullScreenPlugin = GetJMethod(env, clazz, "showFullScreenPlugin", "(Landroid/webkit/ViewManager$ChildView;I)V");
    m_javaGlue->m_hideFullScreenPlugin = GetJMethod(env, clazz, "hideFullScreenPlugin", "()V");
    m_javaGlue->m_addSurface = GetJMethod(env, clazz, "addSurface", "(Landroid/view/View;IIII)Landroid/webkit/ViewManager$ChildView;");
    m_javaGlue->m_updateSurface = GetJMethod(env, clazz, "updateSurface", "(Landroid/webkit/ViewManager$ChildView;IIII)V");
    m_javaGlue->m_destroySurface = GetJMethod(env, clazz, "destroySurface", "(Landroid/webkit/ViewManager$ChildView;)V");
    m_javaGlue->m_getContext = GetJMethod(env, clazz, "getContext", "()Landroid/content/Context;");
    m_javaGlue->m_sendFindAgain = GetJMethod(env, clazz, "sendFindAgain", "()V");
//SAMSUNG CHANGES+
	m_javaGlue->m_showRect = GetJMethod(env, clazz, "showRect", "(IIIIIIFFFFZIIII)V");
//SAMSUNG CHANGES-
    m_javaGlue->m_centerFitRect = GetJMethod(env, clazz, "centerFitRect", "(IIII)V");
    m_javaGlue->m_setScrollbarModes = GetJMethod(env, clazz, "setScrollbarModes", "(II)V");
    m_javaGlue->m_HttpEquivhandle = GetJMethod(env, clazz, "HttpEquivhandle", "(Ljava/lang/String;)V"); //SAMSUNG CHANGE
    env->SetIntField(javaWebViewCore, gWebViewCoreFields.m_nativeClass, (jint)this);

    m_scrollOffsetX = m_scrollOffsetY = 0;

    PageGroup::setShouldTrackVisitedLinks(true);

    reset(true);

    WebViewCore::addInstance(this);
}

WebViewCore::~WebViewCore()
{
    WebViewCore::removeInstance(this);

    // Release the focused view
    Release(m_popupReply);

    if (m_javaGlue->m_obj) {
        JNIEnv* env = JSC::Bindings::getJNIEnv();
        env->DeleteWeakGlobalRef(m_javaGlue->m_obj);
        m_javaGlue->m_obj = 0;
    }
    delete m_javaGlue;
    delete m_frameCacheKit;
    delete m_navPictureKit;
}

WebViewCore* WebViewCore::getWebViewCore(const WebCore::FrameView* view)
{
    return getWebViewCore(static_cast<const WebCore::ScrollView*>(view));
}

WebViewCore* WebViewCore::getWebViewCore(const WebCore::ScrollView* view)
{
    if (!view)
        return 0;

    WebFrameView* webFrameView = static_cast<WebFrameView*>(view->platformWidget());
    if (!webFrameView)
        return 0;
    return webFrameView->webViewCore();
}

void WebViewCore::reset(bool fromConstructor)
{
    DBG_SET_LOG("");
    if (fromConstructor) {
        m_frameCacheKit = 0;
        m_navPictureKit = 0;
    } else {
        gFrameCacheMutex.lock();
        delete m_frameCacheKit;
        delete m_navPictureKit;
        m_frameCacheKit = 0;
        m_navPictureKit = 0;
        gFrameCacheMutex.unlock();
    }

    m_lastFocused = 0;
    m_lastFocusedBounds = WebCore::IntRect(0,0,0,0);
    m_focusBoundsChanged = false;
    m_lastFocusedSelStart = 0;
    m_lastFocusedSelEnd = 0;
    clearContent();
    m_updatedFrameCache = true;
    m_frameCacheOutOfDate = true;
    m_skipContentDraw = false;
    m_findIsUp = false;
    m_domtree_version = 0;
    m_check_domtree_version = true;
    m_progressDone = false;
    m_hasCursorBounds = false;

    m_scrollOffsetX = 0;
    m_scrollOffsetY = 0;
    m_screenWidth = 0;
    m_screenHeight = 0;
    m_groupForVisitedLinks = NULL;
}

static bool layoutIfNeededRecursive(WebCore::Frame* f)
{
    if (!f)
        return true;

    WebCore::FrameView* v = f->view();
    if (!v)
        return true;

    if (v->needsLayout())
        v->layout(f->tree()->parent());

    WebCore::Frame* child = f->tree()->firstChild();
    bool success = true;
    while (child) {
        success &= layoutIfNeededRecursive(child);
        child = child->tree()->nextSibling();
    }

    return success && !v->needsLayout();
}

CacheBuilder& WebViewCore::cacheBuilder()
{
    return FrameLoaderClientAndroid::get(m_mainFrame)->getCacheBuilder();
}

WebCore::Node* WebViewCore::currentFocus()
{
    return cacheBuilder().currentFocus();
}

void WebViewCore::recordPicture(SkPicture* picture)
{
    // if there is no document yet, just return
    if (!m_mainFrame->document()) {
        DBG_NAV_LOG("no document");
        return;
    }
    // Call layout to ensure that the contentWidth and contentHeight are correct
    if (!layoutIfNeededRecursive(m_mainFrame)) {
        DBG_NAV_LOG("layout failed");
        return;
    }
    // draw into the picture's recording canvas
    WebCore::FrameView* view = m_mainFrame->view();
    DBG_NAV_LOGD("view=(w=%d,h=%d)", view->contentsWidth(),
        view->contentsHeight());
    SkAutoPictureRecord arp(picture, view->contentsWidth(),
                            view->contentsHeight(), PICT_RECORD_FLAGS);
    SkAutoMemoryUsageProbe mup(__FUNCTION__);

    // Copy m_buttons so we can pass it to our graphics context.
    gButtonMutex.lock();
    WTF::Vector<Container> buttons(m_buttons);
    gButtonMutex.unlock();

    WebCore::PlatformGraphicsContext pgc(arp.getRecordingCanvas(), &buttons);
    WebCore::GraphicsContext gc(&pgc);
    view->platformWidget()->draw(&gc, WebCore::IntRect(0, 0, INT_MAX, INT_MAX));

    gButtonMutex.lock();
    updateButtonList(&buttons);
    gButtonMutex.unlock();
}

void WebViewCore::recordPictureSet(PictureSet* content)
{
    // if there is no document yet, just return
    if (!m_mainFrame->document()) {
        DBG_SET_LOG("!m_mainFrame->document()");
        return;
    }
    if (m_addInval.isEmpty()) {
        DBG_SET_LOG("m_addInval.isEmpty()");
        return;
    }
    // Call layout to ensure that the contentWidth and contentHeight are correct
    // it's fine for layout to gather invalidates, but defeat sending a message
    // back to java to call webkitDraw, since we're already in the middle of
    // doing that
    m_skipContentDraw = true;
    bool success = layoutIfNeededRecursive(m_mainFrame);
    m_skipContentDraw = false;

    // We may be mid-layout and thus cannot draw.
    if (!success)
        return;

    {   // collect WebViewCoreRecordTimeCounter after layoutIfNeededRecursive
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreRecordTimeCounter);
#endif

    // if the webkit page dimensions changed, discard the pictureset and redraw.
    WebCore::FrameView* view = m_mainFrame->view();
    int width = view->contentsWidth();
    int height = view->contentsHeight();

    // Use the contents width and height as a starting point.
    SkIRect contentRect;
    contentRect.set(0, 0, width, height);
    SkIRect total(contentRect);

    // Traverse all the frames and add their sizes if they are in the visible
    // rectangle.
    for (WebCore::Frame* frame = m_mainFrame->tree()->traverseNext(); frame;
            frame = frame->tree()->traverseNext()) {
        // If the frame doesn't have an owner then it is the top frame and the
        // view size is the frame size.
        WebCore::RenderPart* owner = frame->ownerRenderer();
        if (owner && owner->style()->visibility() == VISIBLE) {
            int x = owner->x();
            int y = owner->y();

            // Traverse the tree up to the parent to find the absolute position
            // of this frame.
            WebCore::Frame* parent = frame->tree()->parent();
            while (parent) {
                WebCore::RenderPart* parentOwner = parent->ownerRenderer();
                if (parentOwner) {
                    x += parentOwner->x();
                    y += parentOwner->y();
                }
                parent = parent->tree()->parent();
            }
            // Use the owner dimensions so that padding and border are
            // included.
            int right = x + owner->width();
            int bottom = y + owner->height();
            SkIRect frameRect = {x, y, right, bottom};
            // Ignore a width or height that is smaller than 1. Some iframes
            // have small dimensions in order to be hidden. The iframe
            // expansion code does not expand in that case so we should ignore
            // them here.
            if (frameRect.width() > 1 && frameRect.height() > 1
                    && SkIRect::Intersects(total, frameRect))
                total.join(x, y, right, bottom);
        }
    }

    // If the new total is larger than the content, resize the view to include
    // all the content.
    if (!contentRect.contains(total)) {
        // Resize the view to change the overflow clip.
        view->resize(total.fRight, total.fBottom);

        // We have to force a layout in order for the clip to change.
        m_mainFrame->contentRenderer()->setNeedsLayoutAndPrefWidthsRecalc();
        view->forceLayout();

        // Relayout similar to above
        m_skipContentDraw = true;
        bool success = layoutIfNeededRecursive(m_mainFrame);
        m_skipContentDraw = false;
        if (!success)
            return;

        // Set the computed content width
        width = view->contentsWidth();
        height = view->contentsHeight();
    }

    content->checkDimensions(width, height, &m_addInval);

    // The inval region may replace existing pictures. The existing pictures
    // may have already been split into pieces. If reuseSubdivided() returns
    // true, the split pieces are the last entries in the picture already. They
    // are marked as invalid, and are rebuilt by rebuildPictureSet().

    // If the new region doesn't match a set of split pieces, add it to the end.
    if (!content->reuseSubdivided(m_addInval)) {
        const SkIRect& inval = m_addInval.getBounds();
        SkPicture* picture = rebuildPicture(inval);
        DBG_SET_LOGD("{%d,%d,w=%d,h=%d}", inval.fLeft,
            inval.fTop, inval.width(), inval.height());
        content->add(m_addInval, picture, 0, false);
        picture->safeUnref();
    }
    // Remove any pictures already in the set that are obscured by the new one,
    // and check to see if any already split pieces need to be redrawn.
    if (content->build())
        rebuildPictureSet(content);
    } // WebViewCoreRecordTimeCounter
    WebCore::Node* oldFocusNode = currentFocus();
    m_frameCacheOutOfDate = true;
    WebCore::IntRect oldBounds;
    int oldSelStart = 0;
    int oldSelEnd = 0;
    if (oldFocusNode) {
        oldBounds = oldFocusNode->getRect();
        RenderObject* renderer = oldFocusNode->renderer();
        if (renderer && (renderer->isTextArea() || renderer->isTextField())) {
            WebCore::RenderTextControl* rtc =
                static_cast<WebCore::RenderTextControl*>(renderer);
            oldSelStart = rtc->selectionStart();
            oldSelEnd = rtc->selectionEnd();
        }
    } else
        oldBounds = WebCore::IntRect(0,0,0,0);
    unsigned latestVersion = 0;
    if (m_check_domtree_version) {
        // as domTreeVersion only increment, we can just check the sum to see
        // whether we need to update the frame cache
        for (Frame* frame = m_mainFrame; frame; frame = frame->tree()->traverseNext()) {
            latestVersion += frame->document()->domTreeVersion();
        }
    }
    DBG_NAV_LOGD("m_lastFocused=%p oldFocusNode=%p"
        " m_lastFocusedBounds={%d,%d,%d,%d} oldBounds={%d,%d,%d,%d}"
        " m_lastFocusedSelection={%d,%d} oldSelection={%d,%d}"
        " m_check_domtree_version=%s latestVersion=%d m_domtree_version=%d",
        m_lastFocused, oldFocusNode,
        m_lastFocusedBounds.x(), m_lastFocusedBounds.y(),
        m_lastFocusedBounds.width(), m_lastFocusedBounds.height(),
        oldBounds.x(), oldBounds.y(), oldBounds.width(), oldBounds.height(),
        m_lastFocusedSelStart, m_lastFocusedSelEnd, oldSelStart, oldSelEnd,
        m_check_domtree_version ? "true" : "false",
        latestVersion, m_domtree_version);
    if (m_lastFocused == oldFocusNode && m_lastFocusedBounds == oldBounds
            && m_lastFocusedSelStart == oldSelStart
            && m_lastFocusedSelEnd == oldSelEnd
            && !m_findIsUp
            && (!m_check_domtree_version || latestVersion == m_domtree_version))
    {
        return;
    }
    m_focusBoundsChanged |= m_lastFocused == oldFocusNode
        && m_lastFocusedBounds != oldBounds;
    m_lastFocused = oldFocusNode;
    m_lastFocusedBounds = oldBounds;
    m_lastFocusedSelStart = oldSelStart;
    m_lastFocusedSelEnd = oldSelEnd;
    m_domtree_version = latestVersion;
    DBG_NAV_LOG("call updateFrameCache");
    updateFrameCache();
    if (m_findIsUp) {
        LOG_ASSERT(m_javaGlue->m_obj,
                "A Java widget was not associated with this view bridge!");
        JNIEnv* env = JSC::Bindings::getJNIEnv();
        env->CallVoidMethod(m_javaGlue->object(env).get(),
                m_javaGlue->m_sendFindAgain);
        checkException(env);
    }
}

void WebViewCore::updateButtonList(WTF::Vector<Container>* buttons)
{
    // All the entries in buttons are either updates of previous entries in
    // m_buttons or they need to be added to it.
    Container* end = buttons->end();
    for (Container* updatedContainer = buttons->begin();
            updatedContainer != end; updatedContainer++) {
        bool updated = false;
        // Search for a previous entry that references the same node as our new
        // data
        Container* lastPossibleMatch = m_buttons.end();
        for (Container* possibleMatch = m_buttons.begin();
                possibleMatch != lastPossibleMatch; possibleMatch++) {
            if (updatedContainer->matches(possibleMatch->node())) {
                // Update our record, and skip to the next one.
                possibleMatch->setRect(updatedContainer->rect());
                updated = true;
                break;
            }
        }
        if (!updated) {
            // This is a brand new button, so append it to m_buttons
            m_buttons.append(*updatedContainer);
        }
    }
    size_t i = 0;
    // count will decrease each time one is removed, so check count each time.
    while (i < m_buttons.size()) {
        if (m_buttons[i].canBeRemoved()) {
            m_buttons[i] = m_buttons.last();
            m_buttons.removeLast();
        } else {
            i++;
        }
    }
}

// note: updateCursorBounds is called directly by the WebView thread
// This needs to be called each time we call CachedRoot::setCursor() with
// non-null CachedNode/CachedFrame, since otherwise the WebViewCore's data
// about the cursor is incorrect.  When we call setCursor(0,0), we need
// to set hasCursorBounds to false.
void WebViewCore::updateCursorBounds(const CachedRoot* root,
        const CachedFrame* cachedFrame, const CachedNode* cachedNode)
{
    LOG_ASSERT(root, "updateCursorBounds: root cannot be null");
    LOG_ASSERT(cachedNode, "updateCursorBounds: cachedNode cannot be null");
    LOG_ASSERT(cachedFrame, "updateCursorBounds: cachedFrame cannot be null");
    gCursorBoundsMutex.lock();
    m_hasCursorBounds = !cachedNode->isHidden();
    // If m_hasCursorBounds is false, we never look at the other
    // values, so do not bother setting them.
    if (m_hasCursorBounds) {
        WebCore::IntRect bounds = cachedNode->bounds(cachedFrame);
        if (m_cursorBounds != bounds)
            DBG_NAV_LOGD("new cursor bounds=(%d,%d,w=%d,h=%d)",
                bounds.x(), bounds.y(), bounds.width(), bounds.height());
        m_cursorBounds = bounds;
        m_cursorHitBounds = cachedNode->hitBounds(cachedFrame);
        m_cursorFrame = cachedFrame->framePointer();
        root->getSimulatedMousePosition(&m_cursorLocation);
        m_cursorNode = cachedNode->nodePointer();
    }
    gCursorBoundsMutex.unlock();
}

void WebViewCore::clearContent()
{
    DBG_SET_LOG("");
    m_contentMutex.lock();
    m_content.clear();
    m_contentMutex.unlock();
    m_addInval.setEmpty();
    m_rebuildInval.setEmpty();
}

void WebViewCore::copyContentToPicture(SkPicture* picture)
{
    DBG_SET_LOG("start");
    m_contentMutex.lock();
    PictureSet copyContent = PictureSet(m_content);
    m_contentMutex.unlock();

    int w = copyContent.width();
    int h = copyContent.height();
    copyContent.draw(picture->beginRecording(w, h, PICT_RECORD_FLAGS));
    picture->endRecording();
    DBG_SET_LOG("end");
}

bool WebViewCore::drawContent(SkCanvas* canvas, SkColor color)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewUIDrawTimeCounter);
#endif
    DBG_SET_LOG("start");
    m_contentMutex.lock();
    PictureSet copyContent = PictureSet(m_content);
    m_contentMutex.unlock();
    int sc = canvas->save(SkCanvas::kClip_SaveFlag);
    SkRect clip;
    clip.set(0, 0, copyContent.width(), copyContent.height());
    canvas->clipRect(clip, SkRegion::kDifference_Op);
    canvas->drawColor(color);
    canvas->restoreToCount(sc);
    bool tookTooLong = copyContent.draw(canvas);
    m_contentMutex.lock();
    m_content.setDrawTimes(copyContent);
    m_contentMutex.unlock();
    DBG_SET_LOG("end");
    return tookTooLong;
}

bool WebViewCore::focusBoundsChanged()
{
    bool result = m_focusBoundsChanged;
    m_focusBoundsChanged = false;
    return result;
}

bool WebViewCore::pictureReady()
{
    bool done;
    m_contentMutex.lock();
    PictureSet copyContent = PictureSet(m_content);
    done = m_progressDone;
    m_contentMutex.unlock();
    DBG_NAV_LOGD("done=%s empty=%s", done ? "true" : "false",
        copyContent.isEmpty() ? "true" : "false");
    return done || !copyContent.isEmpty();
}

SkPicture* WebViewCore::rebuildPicture(const SkIRect& inval)
{
    WebCore::FrameView* view = m_mainFrame->view();
    int width = view->contentsWidth();
    int height = view->contentsHeight();
    SkPicture* picture = new SkPicture();
    SkAutoPictureRecord arp(picture, width, height, PICT_RECORD_FLAGS);
    SkAutoMemoryUsageProbe mup(__FUNCTION__);
    SkCanvas* recordingCanvas = arp.getRecordingCanvas();

    gButtonMutex.lock();
    WTF::Vector<Container> buttons(m_buttons);
    gButtonMutex.unlock();

    WebCore::PlatformGraphicsContext pgc(recordingCanvas, &buttons);
    WebCore::GraphicsContext gc(&pgc);
    recordingCanvas->translate(-inval.fLeft, -inval.fTop);
    recordingCanvas->save();
    view->platformWidget()->draw(&gc, WebCore::IntRect(inval.fLeft,
        inval.fTop, inval.width(), inval.height()));
    m_rebuildInval.op(inval, SkRegion::kUnion_Op);
    DBG_SET_LOGD("m_rebuildInval={%d,%d,r=%d,b=%d}",
        m_rebuildInval.getBounds().fLeft, m_rebuildInval.getBounds().fTop,
        m_rebuildInval.getBounds().fRight, m_rebuildInval.getBounds().fBottom);

    gButtonMutex.lock();
    updateButtonList(&buttons);
    gButtonMutex.unlock();

    return picture;
}

void WebViewCore::rebuildPictureSet(PictureSet* pictureSet)
{
    WebCore::FrameView* view = m_mainFrame->view();
    size_t size = pictureSet->size();
    for (size_t index = 0; index < size; index++) {
        if (pictureSet->upToDate(index))
            continue;
        const SkIRect& inval = pictureSet->bounds(index);
        DBG_SET_LOGD("pictSet=%p [%d] {%d,%d,w=%d,h=%d}", pictureSet, index,
            inval.fLeft, inval.fTop, inval.width(), inval.height());
        pictureSet->setPicture(index, rebuildPicture(inval));
    }
    pictureSet->validate(__FUNCTION__);
}

bool WebViewCore::recordContent(SkRegion* region, SkIPoint* point)
{
    DBG_SET_LOG("start");
    float progress = (float) m_mainFrame->page()->progress()->estimatedProgress();
    m_contentMutex.lock();
    PictureSet contentCopy(m_content);
    m_progressDone = progress <= 0.0f || progress >= 1.0f;
    m_contentMutex.unlock();
    recordPictureSet(&contentCopy);
    if (!m_progressDone && contentCopy.isEmpty()) {
        DBG_SET_LOGD("empty (progress=%g)", progress);
        return false;
    }
    region->set(m_addInval);
    m_addInval.setEmpty();
    region->op(m_rebuildInval, SkRegion::kUnion_Op);
    m_rebuildInval.setEmpty();
    m_contentMutex.lock();
    contentCopy.setDrawTimes(m_content);
    m_content.set(contentCopy);
    point->fX = m_content.width();
    point->fY = m_content.height();
    m_contentMutex.unlock();
    DBG_SET_LOGD("region={%d,%d,r=%d,b=%d}", region->getBounds().fLeft,
        region->getBounds().fTop, region->getBounds().fRight,
        region->getBounds().fBottom);
    DBG_SET_LOG("end");
    return true;
}

void WebViewCore::splitContent()
{
    bool layoutSuceeded = layoutIfNeededRecursive(m_mainFrame);
    LOG_ASSERT(layoutSuceeded, "Can never be called recursively");
    PictureSet tempPictureSet;
    m_contentMutex.lock();
    m_content.split(&tempPictureSet);
    m_contentMutex.unlock();
    rebuildPictureSet(&tempPictureSet);
    m_contentMutex.lock();
    m_content.set(tempPictureSet);
    m_contentMutex.unlock();
}

void WebViewCore::scrollTo(int x, int y, bool animate)
{
    LOG_ASSERT(m_javaGlue->m_obj, "A Java widget was not associated with this view bridge!");

//    LOGD("WebViewCore::scrollTo(%d %d)\n", x, y);

    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(),
            animate ? m_javaGlue->m_spawnScrollTo : m_javaGlue->m_scrollTo,
            x, y);
    checkException(env);
}

void WebViewCore::sendNotifyProgressFinished()
{
    LOG_ASSERT(m_javaGlue->m_obj, "A Java widget was not associated with this view bridge!");
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(), m_javaGlue->m_sendNotifyProgressFinished);
    checkException(env);
}

void WebViewCore::viewInvalidate(const WebCore::IntRect& rect)
{
    LOG_ASSERT(m_javaGlue->m_obj, "A Java widget was not associated with this view bridge!");
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(),
                        m_javaGlue->m_sendViewInvalidate,
                        rect.x(), rect.y(), rect.right(), rect.bottom());
    checkException(env);
}

void WebViewCore::scrollBy(int dx, int dy, bool animate)
{
    if (!(dx | dy))
        return;
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(), m_javaGlue->m_scrollBy,
        dx, dy, animate);
    checkException(env);
}

#if USE(ACCELERATED_COMPOSITING)

void WebViewCore::immediateRepaint()
{
    LOG_ASSERT(m_javaGlue->m_obj, "A Java widget was not associated with this view bridge!");
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(),
                        m_javaGlue->m_sendImmediateRepaint);
    checkException(env);
}

void WebViewCore::setUIRootLayer(const LayerAndroid* layer)
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(),
                        m_javaGlue->m_setRootLayer,
                        reinterpret_cast<jint>(layer));
    checkException(env);
}

#endif // USE(ACCELERATED_COMPOSITING)

void WebViewCore::contentDraw()
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(), m_javaGlue->m_contentDraw);
    checkException(env);
}

void WebViewCore::contentInvalidate(const WebCore::IntRect &r)
{
    DBG_SET_LOGD("rect={%d,%d,w=%d,h=%d}", r.x(), r.y(), r.width(), r.height());
    SkIRect rect(r);
    if (!rect.intersect(0, 0, INT_MAX, INT_MAX))
        return;
    m_addInval.op(rect, SkRegion::kUnion_Op);
    DBG_SET_LOGD("m_addInval={%d,%d,r=%d,b=%d}",
        m_addInval.getBounds().fLeft, m_addInval.getBounds().fTop,
        m_addInval.getBounds().fRight, m_addInval.getBounds().fBottom);
    if (!m_skipContentDraw)
        contentDraw();
}

void WebViewCore::offInvalidate(const WebCore::IntRect &r)
{
    // FIXME: these invalidates are offscreen, and can be throttled or
    // deferred until the area is visible. For now, treat them as
    // regular invals so that drawing happens (inefficiently) for now.
    contentInvalidate(r);
}

static int pin_pos(int x, int width, int targetWidth)
{
    if (x + width > targetWidth)
        x = targetWidth - width;
    if (x < 0)
        x = 0;
    return x;
}

void WebViewCore::didFirstLayout()
{
    DEBUG_NAV_UI_LOGD("%s", __FUNCTION__);
    LOG_ASSERT(m_javaGlue->m_obj, "A Java widget was not associated with this view bridge!");

    WebCore::FrameLoader* loader = m_mainFrame->loader();
    const WebCore::KURL& url = loader->url();
    if (url.isEmpty())
        return;
    LOGV("::WebCore:: didFirstLayout %s", url.string().ascii().data());

    WebCore::FrameLoadType loadType = loader->loadType();

    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(), m_javaGlue->m_didFirstLayout,
            loadType == WebCore::FrameLoadTypeStandard
            // When redirect with locked history, we would like to reset the
            // scale factor. This is important for www.yahoo.com as it is
            // redirected to www.yahoo.com/?rs=1 on load.
            || loadType == WebCore::FrameLoadTypeRedirectWithLockedBackForwardList);
    checkException(env);

    DBG_NAV_LOG("call updateFrameCache");
    m_check_domtree_version = false;
    updateFrameCache();
    m_history.setDidFirstLayout(true);
}

void WebViewCore::updateViewport()
{
    DEBUG_NAV_UI_LOGD("%s", __FUNCTION__);
    LOG_ASSERT(m_javaGlue->m_obj, "A Java widget was not associated with this view bridge!");

    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(), m_javaGlue->m_updateViewport);
    checkException(env);
}

//SAMSUNG CHANGES >>
//Support for Meta Cache Tags
void WebViewCore::HttpEquivhandle(const WebCore::String& url)
{
    DEBUG_NAV_UI_LOGD("%s", __FUNCTION__);
    LOG_ASSERT(m_javaGlue->m_obj, "HttpEquivhandle");
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    AutoJObject obj = m_javaGlue->object(env);
    // if it is called during DESTROY is handled, the real object of WebViewCore
    // can be gone. Check before using it.
    if (!obj.get())
        return;
    jstring jUrlStr = env->NewString((unsigned short *)url.characters(), url.length());
    env->CallVoidMethod(obj.get(), m_javaGlue->m_HttpEquivhandle, jUrlStr);
    env->DeleteLocalRef(jUrlStr);
    checkException(env);
}
//SAMSUNG CHANGES << 

void WebViewCore::restoreScale(int scale)
{
    DEBUG_NAV_UI_LOGD("%s", __FUNCTION__);
    LOG_ASSERT(m_javaGlue->m_obj, "A Java widget was not associated with this view bridge!");

    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(), m_javaGlue->m_restoreScale, scale);
    checkException(env);
}

void WebViewCore::restoreScreenWidthScale(int scale)
{
    DEBUG_NAV_UI_LOGD("%s", __FUNCTION__);
    LOG_ASSERT(m_javaGlue->m_obj, "A Java widget was not associated with this view bridge!");

    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(),
            m_javaGlue->m_restoreScreenWidthScale, scale);
    checkException(env);
}

//SanJose++
void WebViewCore::needTouchEvents(bool need, bool isPlugin)
{
    DEBUG_NAV_UI_LOGD("%s", __FUNCTION__);
    LOG_ASSERT(m_javaGlue->m_obj, "A Java widget was not associated with this view bridge!");

#if ENABLE(TOUCH_EVENTS)
    if (isPlugin && m_forwardingTouchEvents_plugin == need)
        return;

    if (!isPlugin && m_forwardingTouchEvents_others == need)
        return;

    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(), m_javaGlue->m_needTouchEvents, need, isPlugin);
    checkException(env);

    if (isPlugin) {
        m_forwardingTouchEvents_plugin = need;
    } else {
        m_forwardingTouchEvents_others = need;
    }
#endif
}
//SanJose--

void WebViewCore::requestKeyboardWithSelection(const WebCore::Node* node,
        int selStart, int selEnd)
{
    DEBUG_NAV_UI_LOGD("%s", __FUNCTION__);
    LOG_ASSERT(m_javaGlue->m_obj, "A Java widget was not associated with this view bridge!");

    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(),
            m_javaGlue->m_requestKeyboardWithSelection,
            reinterpret_cast<int>(node), selStart, selEnd, m_textGeneration);
    checkException(env);
}

void WebViewCore::requestKeyboard(bool showKeyboard)
{
    DEBUG_NAV_UI_LOGD("%s", __FUNCTION__);
    LOG_ASSERT(m_javaGlue->m_obj, "A Java widget was not associated with this view bridge!");

    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(),
            m_javaGlue->m_requestKeyboard, showKeyboard);
    checkException(env);
}

void WebViewCore::notifyProgressFinished()
{
    DBG_NAV_LOG("call updateFrameCache");
    m_check_domtree_version = true;
    updateFrameCache();
    sendNotifyProgressFinished();
}

void WebViewCore::doMaxScroll(CacheBuilder::Direction dir)
{
    int dx = 0, dy = 0;

    switch (dir) {
    case CacheBuilder::LEFT:
        dx = -m_maxXScroll;
        break;
    case CacheBuilder::UP:
        dy = -m_maxYScroll;
        break;
    case CacheBuilder::RIGHT:
        dx = m_maxXScroll;
        break;
    case CacheBuilder::DOWN:
        dy = m_maxYScroll;
        break;
    case CacheBuilder::UNINITIALIZED:
    default:
        LOG_ASSERT(0, "unexpected focus selector");
    }
    this->scrollBy(dx, dy, true);
}

void WebViewCore::setScrollOffset(int moveGeneration, int dx, int dy)
{
    DBG_NAV_LOGD("{%d,%d} m_scrollOffset=(%d,%d)", dx, dy,
        m_scrollOffsetX, m_scrollOffsetY);
    if (m_scrollOffsetX != dx || m_scrollOffsetY != dy) {
        m_scrollOffsetX = dx;
        m_scrollOffsetY = dy;
        // The visible rect is located within our coordinate space so it
        // contains the actual scroll position. Setting the location makes hit
        // testing work correctly.
        m_mainFrame->view()->platformWidget()->setLocation(m_scrollOffsetX,
                m_scrollOffsetY);
        m_mainFrame->eventHandler()->sendScrollEvent();

        // update the currently visible screen
        sendPluginVisibleScreen();
    }
    gCursorBoundsMutex.lock();
    bool hasCursorBounds = m_hasCursorBounds;
    Frame* frame = (Frame*) m_cursorFrame;
    IntPoint location = m_cursorLocation;
    gCursorBoundsMutex.unlock();
    if (!hasCursorBounds)
        return;
    moveMouseIfLatest(moveGeneration, frame, location.x(), location.y());
}

void WebViewCore::setGlobalBounds(int x, int y, int h, int v)
{
    DBG_NAV_LOGD("{%d,%d}", x, y);
    m_mainFrame->view()->platformWidget()->setWindowBounds(x, y, h, v);
}

//SAMSUNG CHANGES+

#define DEBUG_ZOOM_COORDS 1

#ifdef DEBUG_ZOOM_COORDS
int zoomSearchRect[4]; // x, y, w, h
int zoomCenter[2];     // x, y
int zoomNodeBounds[4]; // x, y, w, h
#endif

static WebCore::Node* findBestNode(WebCore::Node *rootNode, WebCore::IntRect& rect, int anchorX, int anchorY, WebCore::Node *startNode, WebCore::IntRect *groupRect) {
    WTF::Vector<WebCore::Node*> stack; // don't recurse on C stack to avoid stack overflow and crash.
    stack.append(rootNode);
    const int rectArea = rect.width() * rect.height();
    float constK = (float)rect.width() * (float)rect.height() / (32.0 * 32.0) + 1.0; // to ensure no div by zero
    float bestNodeWeight = 0;
    WebCore::Node *best = NULL;
    WebCore::IntRect bestVisibleBounds;
    bool hasGroup=false;

	int left=groupRect->x(), right=groupRect->right(), top=groupRect->y(), bottom=groupRect->bottom();
    while (stack.size() > 0) {
        WebCore::Node *node = stack.last();
        stack.removeLast();
        for (Node* child = node->firstChild(); child; child=child->nextSibling()) {
            stack.append(child);
        }

        WebCore::IntRect bounds = node->getRect();
        if(groupRect->width()>=0 && groupRect->height() >=0){
        	if((abs(bounds.x()-groupRect->x())<textPadding &&
        			abs(bounds.y()-groupRect->y()-groupRect->height())<maxTextHeight/2) ||
        			(bounds.width()<8*textPadding &&
        			(abs(bounds.x()-right)<textPadding || abs(bounds.right()-left)<textPadding) &&
        			abs(bounds.y()-groupRect->y())<textPadding/2)){
        		DBG_NAV_LOGD("bounds: x=%d y=%d right=%d bottom=%d",
                             bounds.x(), bounds.y(), bounds.right(), bounds.bottom());
                DBG_NAV_LOGD("old group dimension, left=%d, top=%d, right=%d, bottom=%d", left, top, right, bottom);

				if(bounds.x()<left){
					left=bounds.x();
				}
				if(bounds.y()<top){
					top=bounds.y();
				}

				if(bounds.right()>right){
					right=bounds.right();
				}

				if(bounds.bottom()>bottom){
					bottom=bounds.bottom();
				}
				hasGroup=true;
        	}
        }//group width and height >= 0
        if(hasGroup)
        	continue;
        if (rect.y() + rect.height() < bounds.y()) {
            // This for sure will not intersect with the rect
            continue;
        } else {
            // Some children of this node may be outside of the right or bottom
            // bounds of the node, so we must keep going.
        }

        if (bounds.intersects(rect)) {
            bool useit = !node->isContainerNode();
            if (!useit) {
                Element* element = static_cast<Element*>(node);
                useit =
                    element->hasTagName(HTMLNames::areaTag) ||
                    element->hasTagName(HTMLNames::imgTag) ||
                    element->hasTagName(HTMLNames::inputTag) ||
                    element->hasTagName(HTMLNames::textareaTag) ||
                    element->hasTagName(HTMLNames::selectTag) ||
                    element->hasTagName(HTMLNames::buttonTag);
            }

            if (useit) {
                DBG_NAV_LOGD("node = %p: x=%d y=%d w=%d h=%d", node,
                             bounds.x(), bounds.y(), bounds.width(), bounds.height());
                WebCore::IntRect i = intersection(rect, bounds);
                int iCenterX = i.x() + i.width() / 2;
                int iCenterY = i.y() + i.height() / 2;

                float intsSize = (float)i.width() * (float)i.height();
                float diffX = (float)(iCenterX - anchorX);
                float diffY = (float)(iCenterY - anchorY);
                float distPlusK = constK + diffX * diffX + diffY * diffY;

                float nodeWeight = intsSize / distPlusK;

                bool better = false;

                if (best == NULL) {
                    better = true;
                } else if (nodeWeight > bestNodeWeight) {
                    better = true;
                } else if (bestVisibleBounds.contains(i) && (i.width() * i.height() > (rectArea / 32))) {
                    // We are going from a container to a child that's not too small. Use the child instead.
                    better = true;
                }

                if (better) {
                    DBG_NAV_LOGD("BEST -> weight = %f -> %f", (double)bestNodeWeight, (double)nodeWeight);
                    best = node;
                    bestNodeWeight = nodeWeight;
                    bestVisibleBounds = i;
                } else {
                    DBG_NAV_LOGD("HIT -> weight = %f", (double)nodeWeight);
                }
            }
        } else {
            // Some children of this node may be outside of the right or bottom
            // bounds of the node, so we must go through the loop below ...
        }


    }
//	DBG_NAV_LOGD("new group dimension, left=%d, top=%d, right=%d, bottom=%d", left, top, right, bottom);
    if(hasGroup){
		groupRect->setX(left);
		groupRect->setY(top);
		groupRect->setWidth(right-left);
		groupRect->setHeight(bottom-top);
	//    DBG_NAV_LOGD("group dimensions: x=%d y=%d right=%d bottom=%d",
	//                 groupRect->x(), groupRect->y(), groupRect->right(), groupRect->bottom());

		if(groupRect->height()>maxTextHeight){
			groupRect->setY(startNode->getRect().bottom()-maxTextHeight/2);
			groupRect->setHeight(maxTextHeight);
		}
		best=NULL;
    }else{
    	groupRect->setX(0);
    	groupRect->setY(0);
    	groupRect->setWidth(0);
    	groupRect->setHeight(0);
    }
    return best;
}

static WebCore::Node* findBestNode(WebCore::Node *startNode, int anchorX, int anchorY,
                                   int viewportLeft, int viewportTop,
                                   int screenWidth, int screenHeight, WebCore::IntRect *groupRect) {
    WebCore::Node *doc, *best = NULL;
    for (doc = startNode; doc && !doc->isDocumentNode(); doc=doc->parentNode()) {
        ;
    }
    WebCore::IntRect rect(viewportLeft, viewportTop, screenWidth, screenHeight);

#ifdef DEBUG_ZOOM_COORDS
    zoomSearchRect[0] = rect.x();
    zoomSearchRect[1] = rect.y();
    zoomSearchRect[2] = rect.width();
    zoomSearchRect[3] = rect.height();
#endif

    if (doc == NULL) {
        best = startNode;
    } else {
        best = findBestNode(doc, rect, anchorX, anchorY, startNode, groupRect);
        if (best == NULL) {
            best = startNode;
        }

    }

    if (best != startNode) {
        DBG_NAV_LOGD("*********** OVERRIDE NODE %dx%d => %dx%d", startNode->getRect().width(),
                     startNode->getRect().height(), best->getRect().width(), best->getRect().height());
    }
#ifdef DEBUG_ZOOM_COORDS
    zoomCenter[0] = anchorX;
    zoomCenter[1] = anchorY;

    zoomNodeBounds[0] = best->getRect().x();
    zoomNodeBounds[1] = best->getRect().y();
    zoomNodeBounds[2] = best->getRect().width();
    zoomNodeBounds[3] = best->getRect().height();
#endif

    return best;
}
//SAMSUNG CHANGES-



void WebViewCore::updatePlugins( int mode, int viewleft, int viewtop, int viewright, int viewbottom, float scale, int anchorX, int anchorY )
{

	//LOGD( "updatePlugins %d %d %d %d %f", left, top, right, bottom, scale );

    //ANPRectI visibleRect;
    //visibleRect.left = left;
    //visibleRect.top = top;
    //visibleRect.right = right;
    //visibleRect.bottom = bottom;

    PluginWidgetAndroid** iter = m_plugins.begin();
    PluginWidgetAndroid** stop = m_plugins.end();


        //ANPEvent event;
        //SkANP::InitEvent(&event, kLifecycle_ANPEventType);
		
        //event.data.lifecycle.action = kOffScreen_ANPLifecycleAction;

	

	
    for (; iter < stop; ++iter) {
        //(*iter)->sendEvent(event);//setVisibleScreen(visibleRect, scale);
        if( mode == 0 )
        	(*iter)->pluginView()->hide();
	else
		(*iter)->pluginView()->show();
    }

}

void WebViewCore::setSizeScreenWidthAndScale(int width, int height,
    int screenWidth, float scale, int realScreenWidth, int screenHeight,
    int anchorX, int anchorY, bool ignoreHeight,
//SAMSUNG CHANGES+
	bool needAnchorDiff, int viewportLeft, int viewportTop)
//SAMSUNG CHANGES-
{
    WebCoreViewBridge* window = m_mainFrame->view()->platformWidget();
    int ow = window->width();
    int oh = window->height();
    window->setSize(width, height);
    int osw = m_screenWidth;
    int orsw = m_screenWidth * m_screenWidthScale / m_scale;
    int osh = m_screenHeight;
//SAMSUNG CHANGES+
	int oldAnchorX = 0;
	int oldAnchorY = 0;
	int newAnchorX = 0;
	int newAnchorY = 0;
//SAMSUNG CHANGES-

    DBG_NAV_LOGD("old:(w=%d,h=%d,sw=%d,scale=%g) new:(w=%d,h=%d,sw=%d,scale=%g)",
        ow, oh, osw, m_scale, width, height, screenWidth, scale);
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
    if (scale >= 0) { // negative means ignore
        m_scale = scale;
        if (screenWidth != realScreenWidth)
            m_screenWidthScale = realScreenWidth * scale / screenWidth;
        else
            m_screenWidthScale = m_scale;
    }
    m_maxXScroll = screenWidth >> 2;
    m_maxYScroll = (screenWidth * height / width) >> 2;
    if (ow != width || (!ignoreHeight && oh != height) || osw != screenWidth) {
        WebCore::RenderObject *r = m_mainFrame->contentRenderer();
//SAMSUNG CHANGES+
		DBG_NAV_LOGD("renderer=%p view=(w=%d,h=%d), anchor=(%d,%d)", r,
			realScreenWidth, screenHeight, anchorX, anchorY);
//SAMSUNG CHANGES-
        if (r) {
            WebCore::IntPoint anchorPoint = WebCore::IntPoint(anchorX, anchorY);
            DBG_NAV_LOGD("anchorX=%d anchorY=%d", anchorX, anchorY);
            WebCore::Node* node = 0;
            WebCore::IntRect bounds;
            WebCore::IntPoint offset;
            // If the screen width changed, it is probably zoom change or
            // orientation change. Try to keep the anchor at the same place.
            if (osw && screenWidth && osw != screenWidth) {
                WebCore::HitTestResult hitTestResult =
                        m_mainFrame->eventHandler()-> hitTestResultAtPoint(
                                anchorPoint, false);
                node = hitTestResult.innerNode();
            }
            if (node) {
                bounds = node->getRect();
                DBG_NAV_LOGD("ob:(x=%d,y=%d,w=%d,h=%d)",
                    bounds.x(), bounds.y(), bounds.width(), bounds.height());
                // sites like nytimes.com insert a non-standard tag <nyt_text>
                // in the html. If it is the HitTestResult, it may have zero
                // width and height. In this case, use its parent node.
                if (bounds.width() == 0) {
                    node = node->parent();
                    if (node) {
                        bounds = node->getRect();
                        DBG_NAV_LOGD("found a zero width node and use its parent, whose ob:(x=%d,y=%d,w=%d,h=%d)",
                                bounds.x(), bounds.y(), bounds.width(), bounds.height());
                    }
                }
//SAMSUNG CHANGES+
				WebCore::IntRect dummyRect(-1,-1,-1,-1);
				node = findBestNode(node, anchorX, anchorY, viewportLeft, viewportTop, realScreenWidth, screenHeight, &dummyRect);
				bounds = node->getRect();
				oldAnchorX = bounds.x();
				oldAnchorY = bounds.y();
//SAMSUNG CHANGES-
            }
            r->setNeedsLayoutAndPrefWidthsRecalc();
            m_mainFrame->view()->forceLayout();
            // scroll to restore current screen center
            if (node) {
                const WebCore::IntRect& newBounds = node->getRect();
                DBG_NAV_LOGD("nb:(x=%d,y=%d,w=%d,"
                    "h=%d)", newBounds.x(), newBounds.y(),
                    newBounds.width(), newBounds.height());
//SAMSUNG CHANGES+
				if (needAnchorDiff ||
					 ((orsw && osh && bounds.width() && bounds.height())
					  && (bounds != newBounds))) {
//SAMSUNG CHANGES-
                    WebCore::FrameView* view = m_mainFrame->view();
                    // force left align if width is not changed while height changed.
                    // the anchorPoint is probably at some white space in the node
                    // which is affected by text wrap around the screen width.
                    const bool leftAlign = (osw != m_screenWidth)
                        && (bounds.width() == newBounds.width())
                        && (bounds.height() != newBounds.height());
                    const float xPercentInDoc =
                        leftAlign ? 0.0 : (float) (anchorX - bounds.x()) / bounds.width();
                    const float xPercentInView =
                        leftAlign ? 0.0 : (float) (anchorX - m_scrollOffsetX) / orsw;
                    const float yPercentInDoc = (float) (anchorY - bounds.y()) / bounds.height();
                    const float yPercentInView = (float) (anchorY - m_scrollOffsetY) / osh;
//SAMSUNG CHANGES+
					newAnchorX = newBounds.x();
					newAnchorY = newBounds.y();
//SAMSUNG CHANGES-
                    showRect(newBounds.x(), newBounds.y(), newBounds.width(),
                             newBounds.height(), view->contentsWidth(),
                             view->contentsHeight(),
                             xPercentInDoc, xPercentInView,
                             yPercentInDoc, yPercentInView,
//SAMSUNG CHANGES+
                             needAnchorDiff, oldAnchorX, oldAnchorY, newAnchorX, newAnchorY);
//SAMSUNG CHANGES-

                }
            }
        }
    }

    // update the currently visible screen as perceived by the plugin
    sendPluginVisibleScreen();
}

void WebViewCore::dumpDomTree(bool useFile)
{
#ifdef ANDROID_DOM_LOGGING
    if (useFile)
        gDomTreeFile = fopen(DOM_TREE_LOG_FILE, "w");
    m_mainFrame->document()->showTreeForThis();
    if (gDomTreeFile) {
        fclose(gDomTreeFile);
        gDomTreeFile = 0;
    }
#endif
}

void WebViewCore::dumpRenderTree(bool useFile)
{
#ifdef ANDROID_DOM_LOGGING
    WebCore::CString renderDump = WebCore::externalRepresentation(m_mainFrame).utf8();
    const char* data = renderDump.data();
    if (useFile) {
        gRenderTreeFile = fopen(RENDER_TREE_LOG_FILE, "w");
        DUMP_RENDER_LOGD("%s", data);
        fclose(gRenderTreeFile);
        gRenderTreeFile = 0;
    } else {
        // adb log can only output 1024 characters, so write out line by line.
        // exclude '\n' as adb log adds it for each output.
        int length = renderDump.length();
        for (int i = 0, last = 0; i < length; i++) {
            if (data[i] == '\n') {
                if (i != last)
                    DUMP_RENDER_LOGD("%.*s", (i - last), &(data[last]));
                last = i + 1;
            }
        }
    }
#endif
}

void WebViewCore::dumpNavTree()
{
#if DUMP_NAV_CACHE
    cacheBuilder().mDebug.print();
#endif
}

WebCore::HTMLAnchorElement* WebViewCore::retrieveAnchorElement(WebCore::Frame* frame, WebCore::Node* node)
{
    if (!CacheBuilder::validNode(m_mainFrame, frame, node))
        return 0;
    if (!node->hasTagName(WebCore::HTMLNames::aTag))
        return 0;
    return static_cast<WebCore::HTMLAnchorElement*>(node);
}

WebCore::String WebViewCore::retrieveHref(WebCore::Frame* frame, WebCore::Node* node)
{
    WebCore::HTMLAnchorElement* anchor = retrieveAnchorElement(frame, node);
    return anchor ? anchor->href() : WebCore::String();
}

WebCore::String WebViewCore::retrieveAnchorText(WebCore::Frame* frame, WebCore::Node* node)
{
    WebCore::HTMLAnchorElement* anchor = retrieveAnchorElement(frame, node);
    return anchor ? anchor->text() : WebCore::String();
}

WebCore::String WebViewCore::requestLabel(WebCore::Frame* frame,
        WebCore::Node* node)
{
    if (node && CacheBuilder::validNode(m_mainFrame, frame, node)) {
        RefPtr<WebCore::NodeList> list = node->document()->getElementsByTagName("label");
        unsigned length = list->length();
        for (unsigned i = 0; i < length; i++) {
            WebCore::HTMLLabelElement* label = static_cast<WebCore::HTMLLabelElement*>(
                    list->item(i));
            if (label->correspondingControl() == node) {
                Node* node = label;
                String result;
                while ((node = node->traverseNextNode(label))) {
                    if (node->isTextNode()) {
                        Text* textNode = static_cast<Text*>(node);
                        result += textNode->dataImpl();
                    }
                }
                return result;
            }
        }
    }
    return WebCore::String();
}

void WebViewCore::updateCacheOnNodeChange()
{
    gCursorBoundsMutex.lock();
    bool hasCursorBounds = m_hasCursorBounds;
    Frame* frame = (Frame*) m_cursorFrame;
    Node* node = (Node*) m_cursorNode;
    IntRect bounds = m_cursorHitBounds;
    gCursorBoundsMutex.unlock();
    if (!hasCursorBounds || !node)
        return;
    if (CacheBuilder::validNode(m_mainFrame, frame, node)) {
        RenderObject* renderer = node->renderer();
        if (renderer && renderer->style()->visibility() != HIDDEN) {
            IntRect absBox = renderer->absoluteBoundingBoxRect();
            int globalX, globalY;
            CacheBuilder::GetGlobalOffset(frame, &globalX, &globalY);
            absBox.move(globalX, globalY);
            if (absBox == bounds)
                return;
            DBG_NAV_LOGD("absBox=(%d,%d,%d,%d) bounds=(%d,%d,%d,%d)",
                absBox.x(), absBox.y(), absBox.width(), absBox.height(),
                bounds.x(), bounds.y(), bounds.width(), bounds.height());
        }
    }
    DBG_NAV_LOGD("updateFrameCache node=%p", node);
    updateFrameCache();
}

void WebViewCore::updateFrameCache()
{
    if (!m_frameCacheOutOfDate) {
        DBG_NAV_LOG("!m_frameCacheOutOfDate");
        return;
    }
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreBuildNavTimeCounter);
#endif
    m_frameCacheOutOfDate = false;
#if DEBUG_NAV_UI
    m_now = SkTime::GetMSecs();
#endif
    m_temp = new CachedRoot();
    m_temp->init(m_mainFrame, &m_history);
#if USE(ACCELERATED_COMPOSITING)
    GraphicsLayerAndroid* graphicsLayer = graphicsRootLayer();
    if (graphicsLayer)
        m_temp->setRootLayer(graphicsLayer->contentLayer());
#endif
    CacheBuilder& builder = cacheBuilder();
    WebCore::Settings* settings = m_mainFrame->page()->settings();
    builder.allowAllTextDetection();
#ifdef ANDROID_META_SUPPORT
    if (settings) {
        if (!settings->formatDetectionAddress())
            builder.disallowAddressDetection();
        if (!settings->formatDetectionEmail())
            builder.disallowEmailDetection();
        if (!settings->formatDetectionTelephone())
            builder.disallowPhoneDetection();
    }
#endif
    builder.buildCache(m_temp);
    m_tempPict = new SkPicture();
    recordPicture(m_tempPict);
    m_temp->setPicture(m_tempPict);
    m_temp->setTextGeneration(m_textGeneration);
    WebCoreViewBridge* window = m_mainFrame->view()->platformWidget();
    m_temp->setVisibleRect(WebCore::IntRect(m_scrollOffsetX,
        m_scrollOffsetY, window->width(), window->height()));
    gFrameCacheMutex.lock();
    delete m_frameCacheKit;
    delete m_navPictureKit;
    m_frameCacheKit = m_temp;
    m_navPictureKit = m_tempPict;
    m_updatedFrameCache = true;
#if DEBUG_NAV_UI
    const CachedNode* cachedFocusNode = m_frameCacheKit->currentFocus();
    DBG_NAV_LOGD("cachedFocusNode=%d (nodePointer=%p)",
        cachedFocusNode ? cachedFocusNode->index() : 0,
        cachedFocusNode ? cachedFocusNode->nodePointer() : 0);
#endif
    gFrameCacheMutex.unlock();
}

void WebViewCore::updateFrameCacheIfLoading()
{
    if (!m_check_domtree_version)
        updateFrameCache();
}

///////////////////////////////////////////////////////////////////////////////

void WebViewCore::addPlugin(PluginWidgetAndroid* w)
{
//    SkDebugf("----------- addPlugin %p", w);
    /* The plugin must be appended to the end of the array. This ensures that if
       the plugin is added while iterating through the array (e.g. sendEvent(...))
       that the iteration process is not corrupted.
     */
    *m_plugins.append() = w;
}

void WebViewCore::removePlugin(PluginWidgetAndroid* w)
{
//    SkDebugf("----------- removePlugin %p", w);
    int index = m_plugins.find(w);
    if (index < 0) {
        SkDebugf("--------------- pluginwindow not found! %p\n", w);
    } else {
        m_plugins.removeShuffle(index);
    }
}

bool WebViewCore::isPlugin(PluginWidgetAndroid* w) const
{
    return m_plugins.find(w) >= 0;
}

void WebViewCore::invalPlugin(PluginWidgetAndroid* w)
{
    const double PLUGIN_INVAL_DELAY = 1.0 / 60;

    if (!m_pluginInvalTimer.isActive()) {
        m_pluginInvalTimer.startOneShot(PLUGIN_INVAL_DELAY);
    }
}

void WebViewCore::drawPlugins()
{
    SkRegion inval; // accumualte what needs to be redrawn
    PluginWidgetAndroid** iter = m_plugins.begin();
    PluginWidgetAndroid** stop = m_plugins.end();

    for (; iter < stop; ++iter) {
        PluginWidgetAndroid* w = *iter;
        SkIRect dirty;
        if (w->isDirty(&dirty)) {
            w->draw();
            inval.op(dirty, SkRegion::kUnion_Op);
        }
    }

    if (!inval.isEmpty()) {
        // inval.getBounds() is our rectangle
        const SkIRect& bounds = inval.getBounds();
        WebCore::IntRect r(bounds.fLeft, bounds.fTop,
                           bounds.width(), bounds.height());
        this->viewInvalidate(r);
    }
}

void WebViewCore::notifyPluginsOnFrameLoad(const Frame* frame) {
    // if frame is the parent then notify all plugins
    if (!frame->tree()->parent()) {
        // trigger an event notifying the plugins that the page has loaded
        ANPEvent event;
        SkANP::InitEvent(&event, kLifecycle_ANPEventType);
        event.data.lifecycle.action = kOnLoad_ANPLifecycleAction;
        sendPluginEvent(event);
    }
    // else if frame's parent has completed
    else if (!frame->tree()->parent()->loader()->isLoading()) {
        // send to all plugins who have this frame in their heirarchy
        PluginWidgetAndroid** iter = m_plugins.begin();
        PluginWidgetAndroid** stop = m_plugins.end();
        for (; iter < stop; ++iter) {
            Frame* currentFrame = (*iter)->pluginView()->parentFrame();
            while (currentFrame) {
                if (frame == currentFrame) {
                    ANPEvent event;
                    SkANP::InitEvent(&event, kLifecycle_ANPEventType);
                    event.data.lifecycle.action = kOnLoad_ANPLifecycleAction;
                    (*iter)->sendEvent(event);
                    break;
                }
                currentFrame = currentFrame->tree()->parent();
            }
        }
    }
}

void WebViewCore::sendPluginVisibleScreen()
{
    /* We may want to cache the previous values and only send the notification
       to the plugin in the event that one of the values has changed.
     */

    ANPRectI visibleRect;
    visibleRect.left = m_scrollOffsetX;
    visibleRect.top = m_scrollOffsetY;
    visibleRect.right = m_scrollOffsetX + m_screenWidth;
    visibleRect.bottom = m_scrollOffsetY + m_screenHeight;

    PluginWidgetAndroid** iter = m_plugins.begin();
    PluginWidgetAndroid** stop = m_plugins.end();
    for (; iter < stop; ++iter) {
        (*iter)->setVisibleScreen(visibleRect, m_scale);
    }
}

void WebViewCore::sendPluginEvent(const ANPEvent& evt)
{
    /* The list of plugins may be manipulated as we iterate through the list.
       This implementation allows for the addition of new plugins during an
       iteration, but may fail if a plugin is removed. Currently, there are not
       any use cases where a plugin is deleted while processing this loop, but
       if it does occur we will have to use an alternate data structure and/or
       iteration mechanism.
     */
    for (int x = 0; x < m_plugins.count(); x++) {
        m_plugins[x]->sendEvent(evt);
    }
}

PluginWidgetAndroid* WebViewCore::getPluginWidget(NPP npp)
{
    PluginWidgetAndroid** iter = m_plugins.begin();
    PluginWidgetAndroid** stop = m_plugins.end();
    for (; iter < stop; ++iter) {
        if ((*iter)->pluginView()->instance() == npp) {
            return (*iter);
        }
    }
    return NULL;
}

static PluginView* nodeIsPlugin(Node* node) {
    RenderObject* renderer = node->renderer();
    if (renderer && renderer->isWidget()) {
        Widget* widget = static_cast<RenderWidget*>(renderer)->widget();
        if (widget && widget->isPluginView())
            return static_cast<PluginView*>(widget);
    }
    return 0;
}

Node* WebViewCore::cursorNodeIsPlugin() {
    gCursorBoundsMutex.lock();
    bool hasCursorBounds = m_hasCursorBounds;
    Frame* frame = (Frame*) m_cursorFrame;
    Node* node = (Node*) m_cursorNode;
    gCursorBoundsMutex.unlock();
    if (hasCursorBounds && CacheBuilder::validNode(m_mainFrame, frame, node)
            && nodeIsPlugin(node)) {
        return node;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
void WebViewCore::moveMouseIfLatest(int moveGeneration,
    WebCore::Frame* frame, int x, int y)
{
    DBG_NAV_LOGD("m_moveGeneration=%d moveGeneration=%d"
        " frame=%p x=%d y=%d",
        m_moveGeneration, moveGeneration, frame, x, y);
    if (m_moveGeneration > moveGeneration) {
        DBG_NAV_LOGD("m_moveGeneration=%d > moveGeneration=%d",
            m_moveGeneration, moveGeneration);
        return; // short-circuit if a newer move has already been generated
    }
    m_lastGeneration = moveGeneration;
    moveMouse(frame, x, y);
}

void WebViewCore::moveFocus(WebCore::Frame* frame, WebCore::Node* node)
{
    DBG_NAV_LOGD("frame=%p node=%p", frame, node);
    if (!node || !CacheBuilder::validNode(m_mainFrame, frame, node)
            || !node->isElementNode())
        return;
    // Code borrowed from FocusController::advanceFocus
    WebCore::FocusController* focusController
            = m_mainFrame->page()->focusController();
    WebCore::Document* oldDoc
            = focusController->focusedOrMainFrame()->document();
    if (oldDoc->focusedNode() == node)
        return;
    if (node->document() != oldDoc)
        oldDoc->setFocusedNode(0);
    focusController->setFocusedFrame(frame);
    static_cast<WebCore::Element*>(node)->focus(false);
}

// Update mouse position
void WebViewCore::moveMouse(WebCore::Frame* frame, int x, int y)
{
    DBG_NAV_LOGD("frame=%p x=%d y=%d scrollOffset=(%d,%d)", frame,
        x, y, m_scrollOffsetX, m_scrollOffsetY);
    if (!frame || CacheBuilder::validNode(m_mainFrame, frame, NULL) == false)
        frame = m_mainFrame;
    // mouse event expects the position in the window coordinate
    m_mousePos = WebCore::IntPoint(x - m_scrollOffsetX, y - m_scrollOffsetY);
    // validNode will still return true if the node is null, as long as we have
    // a valid frame.  Do not want to make a call on frame unless it is valid.
    WebCore::PlatformMouseEvent mouseEvent(m_mousePos, m_mousePos,
        WebCore::NoButton, WebCore::MouseEventMoved, 1, false, false, false,
        false, WTF::currentTime());
    frame->eventHandler()->handleMouseMoveEvent(mouseEvent);
    updateCacheOnNodeChange();
}
//SAMSUNG CHANGE BEGIN : ADVANCED_COPY_PASTE
WebCore::String WebViewCore::getSelectedText()
{
    	DEBUG_NAV_UI_LOGD("%s", __FUNCTION__);
	WebCore::Frame* frame = m_mainFrame;
	WebCore::String str = frame->selectedText();
    	DEBUG_NAV_UI_LOGD("%s: End", __FUNCTION__);
    	return str;
}
//SAMSUNG CHANGE END : ADVANCED_COPY_PASTE

////////////SISO_HTMLCOMPOSER begin

void WebViewCore::simulateDelKeyForCount(int count)
{
    LOGD("simulateDelKeyForCount enter");	
    PlatformKeyboardEvent down(AKEYCODE_DEL, 0, 0, true, false, false, false);
    PlatformKeyboardEvent up(AKEYCODE_DEL, 0, 0, false, false, false, false);
    for(int cnt = 0 ; cnt < count ; cnt++)
    {	
    	key(down);
    	key(up);
    }	
    LOGD("simulateDelKeyForCount exit");	  		
}

WebCore::String WebViewCore::getTextAroundCursor(int count , bool isBefore)
{
	LOGD("getTextAroundCursor enter");	
	if(count == -1)
	{
		LOGD("getTextAroundCursor -1 ");
		SelectionController* selectionContrler = m_mainFrame->selection();
		if(selectionContrler != NULL && !(selectionContrler->isNone()))	 
		{	
			Position pos = selectionContrler->selection().visibleEnd().deepEquivalent();
			IntPoint pt = IntPoint(0, 0);	
			Position startPos = m_mainFrame->visiblePositionForPoint(pt).deepEquivalent();
			
			VisibleSelection newSelection;
			SelectionController newSelectionControler; 		
			//if (comparePositions(pos , startPos) <= 0)
        	    	//	newSelection = VisibleSelection(pos, startPos);
    			//else
        	    		newSelection = VisibleSelection(startPos, pos);
			
			newSelectionControler.setSelection(newSelection);
			PassRefPtr<Range> rangePtr = newSelectionControler.toNormalizedRange(); 
			WebCore::Range* range = rangePtr.get();
			if(range != NULL)
			{	
				WebCore::String plainText = range->text();
				plainText.replace(NonBreakingSpaceCharacter, SpaceCharacter);
				LOGD("getTextAroundCursor -1 ret");				
				return plainText;
			}
	
		}
	}	
	else
	{
		SelectionController* frameSelectionContrler = m_mainFrame->selection(); 
		if(frameSelectionContrler == NULL)	
			return "";
		LOGD("getTextAroundCursor setSelection ent");		
		SelectionController newSelection; 
		newSelection.setSelection(frameSelectionContrler->selection());
		LOGD("getTextAroundCursor setSelection exit");		
		//newSelection.expandUsingGranularity(CharacterGranularity);
		for(int cnt = 0 ; cnt < count ; cnt++)
		{
			if(isBefore)
				newSelection.modify(SelectionController::EXTEND, SelectionController::BACKWARD, CharacterGranularity);
			else
				newSelection.modify(SelectionController::EXTEND, SelectionController::FORWARD, CharacterGranularity);
		}
		
		PassRefPtr<Range> rangePtr = newSelection.toNormalizedRange(); //Range::create(m_mainFrame->document(), newSelection.start(), newSelection.end());	
		WebCore::Range* range = rangePtr.get();
		LOGD("getTextAroundCursor range->text ent");	
		if(range != NULL)
		{	
			WebCore::String plainText = range->text();
			plainText.replace(NonBreakingSpaceCharacter, SpaceCharacter);
			LOGD("getTextAroundCursor range->text exit");		
			LOGD("getTextAroundCursor exit");
			return plainText ;
		}
	}
	LOGD("getTextAroundCursor exit"); 			
	return "";    	
}


int WebViewCore::checkSelectionAtBoundry()
{
	SelectionController* frameSelectionContrler = m_mainFrame->selection(); 
	if(frameSelectionContrler == NULL)	
		return -1;
	

	IntPoint pt = IntPoint(0, 0);
	VisiblePosition startPos = m_mainFrame->visiblePositionForPoint(pt);
		
	VisibleSelection newSelection;
	newSelection = VisibleSelection(startPos);
	SelectionController newSelectionControler;
	newSelectionControler.setSelection(newSelection); 
		

	if( (newSelectionControler.selection().visibleStart() == frameSelectionContrler->selection().visibleStart()) && 
			(newSelectionControler.selection().visibleEnd() == frameSelectionContrler->selection().visibleEnd()) )
		return 0;

	newSelectionControler.modify(SelectionController::MOVE, SelectionController::FORWARD, DocumentBoundary);	
	if( (newSelectionControler.selection().visibleStart() == frameSelectionContrler->selection().visibleStart()) && 
			(newSelectionControler.selection().visibleEnd() == frameSelectionContrler->selection().visibleEnd()) )
		return 1;	 	

	return -1;
}

WebCore::String WebViewCore::getBodyText()
{
	return m_mainFrame->document()->body()->innerText();
}

WebCore::String WebViewCore::getBodyHTML()
{
	return m_mainFrame->document()->body()->outerHTML();
}

WebCore::IntRect WebViewCore::getCursorRect(bool giveContentRect)
{
	if(giveContentRect) 
		return m_mainFrame->selection()->absoluteCaretBounds();
	else 
	{
		WebCore::IntRect caretRect;	
		caretRect = m_mainFrame->selection()->absoluteCaretBounds();//localCaretRect();
		LOGD("getCursorRect %d %d %d %d " , caretRect.x() , caretRect.y() , caretRect.right() , caretRect.bottom());	
		WebCore::IntPoint locInWindow = m_mainFrame->view()->contentsToWindow(caretRect.location()); 	
		//caretRect = m_mainFrame->view()->convertToContainingView(caretRect);	
		caretRect.setLocation(locInWindow);
		LOGD("getCursorRect %d %d %d %d " , caretRect.x() , caretRect.y() , caretRect.right() , caretRect.bottom());	
		LOGD("getCursorRect exit");
		return caretRect;
	}
	
}

void WebViewCore::setSelectionNone()
{
	m_mainFrame->selection()->setSelection(VisibleSelection());	
}

void WebViewCore::deleteSurroundingText(int left , int right)
{	
	LOGD("deleteSurroundingText enter");
	int cnt;
	for(cnt = 0 ; cnt < left ; cnt++)
	{	
		m_mainFrame->selection()->modify(SelectionController::EXTEND, SelectionController::BACKWARD, CharacterGranularity);
	}
	for(cnt = 0 ; cnt < right ; cnt++)
	{
		m_mainFrame->selection()->modify(SelectionController::EXTEND, SelectionController::FORWARD, CharacterGranularity);
	}
	simulateDelKeyForCount(1);
	LOGD("deleteSurroundingText exit");
		
}

void WebViewCore::getSelectionOffset(int& startOffset , int& endOffset)
{
	SelectionController* selectionContrler = m_mainFrame->selection();
	if(selectionContrler != NULL && !(selectionContrler->isNone()))	 
	{	
		Position pos;
			pos = selectionContrler->selection().visibleStart().deepEquivalent();
		
		IntPoint pt = IntPoint(0, 0);
		Position startPos = m_mainFrame->visiblePositionForPoint(pt).deepEquivalent();
		
		VisibleSelection newSelection;
		SelectionController newSelectionControler; 		
		//if (comparePositions(pos , startPos) <= 0)
            	//	newSelection = VisibleSelection(pos, startPos);
    		//else
            		newSelection = VisibleSelection(startPos, pos);
		
		newSelectionControler.setSelection(newSelection);
		PassRefPtr<Range> rangePtr = newSelectionControler.toNormalizedRange(); 
		WebCore::Range* range = rangePtr.get();
		if(range != NULL)
		{	
			WebCore::String plainText = range->text();
			plainText.replace(NonBreakingSpaceCharacter, SpaceCharacter);

			LOGD("getSelectionOffset %s len %d" , plainText.utf8().data() , plainText.length());		

			//return plainText.length();
			startOffset = plainText.length();
			if(selectionContrler->isRange())
			{
				endOffset = startOffset + m_mainFrame->selectedText().length(); 			
			}	
			else 
			{
				endOffset = startOffset; 	
		}
			LOGD("getSelectionOffset str %d end %d" , startOffset ,endOffset);
	}
	}


}



bool WebViewCore::execCommand(WebCore::String& commandName ,  WebCore::String& value)
{
    //ANDROID_LOG_PRINT(ANDROID_LOG_DEBUG, "HTML_EDIT", " WebViewCore.cpp ::execCommand command: dfgdsf sdgfgs:%s", commandName.utf8().data());
    //LOGD("execCommand entered %s value %s " , commandName.utf8().data() , value.utf8().data());
    bool retval = false;
    if(commandName == "MoveToBeginningOfDocument" || commandName == "MoveToEndOfDocument")
    {
        SelectionController* selectionContrler = m_mainFrame->selection();
        if(selectionContrler->isNone())
        {
            LOGD("execCommand selection none");
            //VisiblePosition vPos = VisiblePosition(m_mainFrame->document()->body() , 0 );
            Position startPos = Position(m_mainFrame->document()->body() , 0 );//vPos.deepEquivalent();

            VisibleSelection newSelection;
            newSelection = VisibleSelection(startPos);
            selectionContrler->setSelection(newSelection);
        }
        else
            LOGD("execCommand selection is Not none");
    }
    retval = m_mainFrame->editor()->command(commandName).execute(value);
    return retval;	
}

bool WebViewCore::canUndo()
{
	return m_mainFrame->editor()->canUndo();
}

bool WebViewCore::canRedo()
{
	return m_mainFrame->editor()->canRedo();
}
void WebViewCore::undoRedoStateReset()
{
	return m_mainFrame->editor()->client()->clearUndoRedoOperations();
}


bool WebViewCore::copyAndSaveImage(WebCore::String& imageUrl)
{
	WebCore::String filePath;
	filePath = WebCore::createLocalResource(m_mainFrame , imageUrl);
	if(filePath.isEmpty())
		return false;
	
	WebCore::copyImagePathToClipboard(filePath); 
	return true;
	//return false;		
}

// SAMSUNG CHANGE >>
bool WebViewCore::saveCachedImageToFile(WebCore::String& imageUrl, WebCore::String& filePath)
{
	// $TODO Null check
	return WebCore::saveCachedImageToFile( m_mainFrame, imageUrl, filePath );
	/*
	CachedResource* cachedResource = m_mainFrame->document()->docLoader()->cachedResource(imageUrl);
	if (cachedResource && cachedResource->isImage())
	{
		RefPtr<SharedBuffer> imageBuffer = cachedResource->data();
		if ( NULL == imageBuffer )
			return false;

		PlatformFileHandle fileHandle = open(filePath, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
		if (handle != -1)
			return false;

		const char* segment = NULL;
		unsigned pos = 0;
		while (unsigned length = imageBuffer->getSomeData(segment, pos)) {
			writeToFile(fileHandle, segment, length);
			pos += length;
		}
		closeFile(fileHandle);
	}
	
	return true;
	*/
}
// SAMSUNG CHANGE <<

WebHTMLMarkupData* WebViewCore::getFullMarkupData(){
    //ANDROID_LOG_PRINT(ANDROID_LOG_DEBUG, "HTML_EDIT", " WebViewCore::getFullMarkupData() Called");  

    setEditable(false);
    WebHTMLMarkupData* webMarkupData = WebCore::createFullMarkup(m_mainFrame->document());
    setEditable(true);

    return webMarkupData;
}
void WebViewCore::setSelectionEditable(int start, int end)
{
	SelectionController* selectionContrler = m_mainFrame->selection();
	if(selectionContrler != NULL)	 
	{	
		if(selectionContrler->isNone())
        {
            Position startPos = Position(m_mainFrame->document()->body() , 0 );//vPos.deepEquivalent();
			VisibleSelection newSelection;
            newSelection = VisibleSelection(startPos);
            selectionContrler->setSelection(newSelection);
        }
		SelectionController newSelectionControler;
		newSelectionControler.setSelection(selectionContrler->selection());
		//selectionContrler->modify(SelectionController::MOVE, SelectionController::FORWARD, CharacterGranularity);
		
		int strtOffset = -1;
		int endOffset = -1;
		getSelectionOffset(strtOffset , endOffset);
		if(strtOffset != endOffset)
		{
			newSelectionControler.modify(SelectionController::MOVE, SelectionController::BACKWARD, CharacterGranularity);
		}
 		int offsetDiff = start - strtOffset;
		int uOffDiff = offsetDiff > 0 ? offsetDiff : -offsetDiff; 
		int cnt;
		if(offsetDiff > 0 )
		{
			for(cnt = 0 ; cnt < uOffDiff ; cnt++)
			{
				newSelectionControler.modify(SelectionController::MOVE, SelectionController::FORWARD, CharacterGranularity);
			}
		}
		else
		{
			for(cnt = 0 ; cnt < uOffDiff ; cnt++)
			{
				newSelectionControler.modify(SelectionController::MOVE, SelectionController::BACKWARD, CharacterGranularity);
			}
		}
		for(cnt = 0 ; cnt < (end - start) ; cnt++)
		{
			newSelectionControler.modify(SelectionController::EXTEND, SelectionController::FORWARD, CharacterGranularity);
		}
		selectionContrler->setSelection(newSelectionControler.selection());
	}
	/*SelectionController* selectionContrler = m_mainFrame->selection();
	if(selectionContrler != NULL)	 
	{	
		if(start == -1 && end == -1 )
		{
			if(selectionContrler->isNone())
        		{
            			Position startPos = Position(m_mainFrame->document()->body() , 0 );//vPos.deepEquivalent();
				VisibleSelection newSelection;
            			newSelection = VisibleSelection(startPos);
            			selectionContrler->setSelection(newSelection);
        		}
			return; 	
		}	
		int cnt;
		IntPoint pt = IntPoint(0, 0);
		VisiblePosition startPos = m_mainFrame->visiblePositionForPoint(pt);
		
		VisibleSelection newSelection;
		newSelection = VisibleSelection(startPos);
		SelectionController newSelectionControler;
		newSelectionControler.setSelection(newSelection); 
		
		for(cnt = 0 ; cnt < start ; cnt++)
		{
			newSelectionControler.modify(SelectionController::MOVE, SelectionController::FORWARD, CharacterGranularity);
		}
		for(cnt = 0 ; cnt < (end - start) ; cnt++)
		{
			newSelectionControler.modify(SelectionController::EXTEND, SelectionController::FORWARD, CharacterGranularity);
		}		
		selectionContrler->setSelection(newSelectionControler.selection());
	}*/
}
void WebViewCore::setEditable(bool enableEditing)
{
	if(enableEditing)
		m_mainFrame->document()->body()->setContentEditable("true");	
	else 
		m_mainFrame->document()->body()->setContentEditable("false");		
}


////////////////SISO_HTMLCOMPOSER
bool WebViewCore::isEditableSupport()
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
	
    bool result = env->CallBooleanMethod(m_javaGlue->object(env).get(),
        m_javaGlue->m_isEditableSupport);

    checkException(env);
    return result;
}
///////////////////////////////////////////


///////////////SISO_HTMLCOMPOSER end
//SAMSUNG_THAI_EDITOR_FIX >>
void WebViewCore::setSelectionWithoutValidation(int start, int end)
{
    WebCore::Node* focus = currentFocus();
    if (!focus)
        return;
    WebCore::RenderObject* renderer = focus->renderer();
    if (!renderer || (!renderer->isTextField() && !renderer->isTextArea()))
        return;
    WebCore::RenderTextControl* rtc = static_cast<WebCore::RenderTextControl*>(renderer);
    if (start > end) {
        int temp = start;
        start = end;
        end = temp;
    }
    // Tell our EditorClient that this change was generated from the UI, so it
    // does not need to echo it to the UI.
    EditorClientAndroid* client = static_cast<EditorClientAndroid*>(
            m_mainFrame->editor()->client());
    client->setUiGeneratedSelectionChange(true);
    rtc->setSelectionRange(start, end);
    client->setUiGeneratedSelectionChange(false);
    WebCore::Frame* focusedFrame = focus->document()->frame();
    VisibleSelection selection = focusedFrame->selection()->selection();
    if (start != end 
        && selection.end().deprecatedEditingOffset() == selection.start().deprecatedEditingOffset()
        && selection.end().node() == selection.start().node()) {
        int e = selection.end().deprecatedEditingOffset() ;
        int s = e - (end - start) ;
        Position base(selection.end().node(), s) ;
        Position extent(selection.end().node(), e) ;
        if (!base.isNull() && !extent.isNull() && base != extent) {
            selection.setWithoutValidation(base, extent);
            focusedFrame->selection()->setSelection(selection);        
        }
    }

    focusedFrame->revealSelection();
    setFocusControllerActive(focusedFrame, true);
}
//SAMSUNG_THAI_EDITOR_FIX <<

void WebViewCore::setSelection(int start, int end)
{
    WebCore::Node* focus = currentFocus();
    if (!focus)
        return;
    WebCore::RenderObject* renderer = focus->renderer();
    if (!renderer || (!renderer->isTextField() && !renderer->isTextArea()))
        return;
    WebCore::RenderTextControl* rtc = static_cast<WebCore::RenderTextControl*>(renderer);
    if (start > end) {
        int temp = start;
        start = end;
        end = temp;
    }
    // Tell our EditorClient that this change was generated from the UI, so it
    // does not need to echo it to the UI.
    EditorClientAndroid* client = static_cast<EditorClientAndroid*>(
            m_mainFrame->editor()->client());
    client->setUiGeneratedSelectionChange(true);
    rtc->setSelectionRange(start, end);
    client->setUiGeneratedSelectionChange(false);
    WebCore::Frame* focusedFrame = focus->document()->frame();
    focusedFrame->revealSelection();
    setFocusControllerActive(focusedFrame, true);
}

void WebViewCore::deleteSelection(int start, int end, int textGeneration)
{
    setSelection(start, end);
    if (start == end)
        return;
    WebCore::Node* focus = currentFocus();
    if (!focus)
        return;
    // Prevent our editor client from passing a message to change the
    // selection.
    EditorClientAndroid* client = static_cast<EditorClientAndroid*>(
            m_mainFrame->editor()->client());
    client->setUiGeneratedSelectionChange(true);
    PlatformKeyboardEvent down(AKEYCODE_DEL, 0, 0, true, false, false, false);
    PlatformKeyboardEvent up(AKEYCODE_DEL, 0, 0, false, false, false, false);
    key(down);
    key(up);
    client->setUiGeneratedSelectionChange(false);
    m_textGeneration = textGeneration;
}

void WebViewCore::replaceTextfieldText(int oldStart,
        int oldEnd, const WebCore::String& replace, int start, int end,
        int textGeneration)
{
    WebCore::Node* focus = currentFocus();
    if (!focus)
        return;
    //setSelection(oldStart, oldEnd); - SAMSUNG_THAI_EDITOR_FIX
    setSelectionWithoutValidation(oldStart, oldEnd);
    // Prevent our editor client from passing a message to change the
    // selection.
    EditorClientAndroid* client = static_cast<EditorClientAndroid*>(
            m_mainFrame->editor()->client());
    client->setUiGeneratedSelectionChange(true);
    WebCore::TypingCommand::insertText(focus->document(), replace,
        false);
    client->setUiGeneratedSelectionChange(false);
    setSelection(start, end);
    m_textGeneration = textGeneration;
}

void WebViewCore::passToJs(int generation, const WebCore::String& current,
    const PlatformKeyboardEvent& event)
{
    // SAMSUNG_FIX_GA0100080596 + 
    m_textGeneration = generation;
    // SAMSUNG_FIX_GA0100080596 -
    WebCore::Node* focus = currentFocus();
    if (!focus) {
        DBG_NAV_LOG("!focus");
        clearTextEntry();
        return;
    }
    WebCore::RenderObject* renderer = focus->renderer();
    if (!renderer || (!renderer->isTextField() && !renderer->isTextArea())) {
        DBG_NAV_LOGD("renderer==%p || not text", renderer);
        clearTextEntry();
        return;
    }
    // Block text field updates during a key press.
    m_blockTextfieldUpdates = true;
    // Also prevent our editor client from passing a message to change the
    // selection.
    EditorClientAndroid* client = static_cast<EditorClientAndroid*>(
            m_mainFrame->editor()->client());
    client->setUiGeneratedSelectionChange(true);
    key(event);
    client->setUiGeneratedSelectionChange(false);
    m_blockTextfieldUpdates = false;
    m_textGeneration = generation;
    setFocusControllerActive(focus->document()->frame(), true);
    WebCore::RenderTextControl* renderText =
        static_cast<WebCore::RenderTextControl*>(renderer);
    WebCore::String test = renderText->text();
    if (test == current) {
        DBG_NAV_LOG("test == current");
        return;
    }
    // If the text changed during the key event, update the UI text field.
    updateTextfield(focus, false, test);
}

void WebViewCore::scrollFocusedTextInput(float xPercent, int y)
{
    WebCore::Node* focus = currentFocus();
    if (!focus) {
        DBG_NAV_LOG("!focus");
        clearTextEntry();
        return;
    }
    WebCore::RenderObject* renderer = focus->renderer();
    if (!renderer || (!renderer->isTextField() && !renderer->isTextArea())) {
        DBG_NAV_LOGD("renderer==%p || not text", renderer);
        clearTextEntry();
        return;
    }
    WebCore::RenderTextControl* renderText =
        static_cast<WebCore::RenderTextControl*>(renderer);
    int x = (int) (xPercent * (renderText->scrollWidth() -
        renderText->clientWidth()));
    DBG_NAV_LOGD("x=%d y=%d xPercent=%g scrollW=%d clientW=%d", x, y,
        xPercent, renderText->scrollWidth(), renderText->clientWidth());
    renderText->setScrollLeft(x);
    renderText->setScrollTop(y);
}

void WebViewCore::setFocusControllerActive(WebCore::Frame* frame, bool active)
{
    if (!frame) {
        WebCore::Node* focus = currentFocus();
        if (focus)
            frame = focus->document()->frame();
        else
            frame = m_mainFrame;
    }
    WebCore::FocusController* controller = frame->page()->focusController();
    controller->setActive(active);
    controller->setFocused(active);
}

void WebViewCore::saveDocumentState(WebCore::Frame* frame)
{
    if (!CacheBuilder::validNode(m_mainFrame, frame, 0))
        frame = m_mainFrame;
    WebCore::HistoryItem *item = frame->loader()->history()->currentItem();

    // item can be null when there is no offical URL for the current page. This happens
    // when the content is loaded using with WebCoreFrameBridge::LoadData() and there
    // is no failing URL (common case is when content is loaded using data: scheme)
    if (item) {
        item->setDocumentState(frame->document()->formElementsState());
    }
}

// Convert a WebCore::String into an array of characters where the first
// character represents the length, for easy conversion to java.
static uint16_t* stringConverter(const WebCore::String& text)
{
    size_t length = text.length();
    uint16_t* itemName = new uint16_t[length+1];
    itemName[0] = (uint16_t)length;
    uint16_t* firstChar = &(itemName[1]);
    memcpy((void*)firstChar, text.characters(), sizeof(UChar)*length);
    return itemName;
}

// Response to dropdown created for a listbox.
class ListBoxReply : public WebCoreReply {
public:
#if ENABLE(WML)
    ListBoxReply(WebCore::Element* select, WebCore::Frame* frame, WebViewCore* view)
#else
    ListBoxReply(WebCore::HTMLSelectElement* select, WebCore::Frame* frame, WebViewCore* view)
#endif    
        : m_select(select)
        , m_frame(frame)
        , m_viewImpl(view)
    {}

    // Response used if the listbox only allows single selection.
    // index is listIndex of the selected item, or -1 if nothing is selected.
    virtual void replyInt(int index)
    {
        if (-2 == index) {
            // Special value for cancel. Do nothing.
            return;
        }
        // If the select element no longer exists, due to a page change, etc,
        // silently return.
        if (!m_select || !CacheBuilder::validNode(m_viewImpl->m_mainFrame,
                m_frame, m_select))
            return;
        // Use a pointer to HTMLSelectElement's superclass, where
        // listToOptionIndex is public.
#if ENABLE(WML)
        WebCore::SelectElement* selectElement = WebCore::toSelectElement(m_select);
        int optionIndex = selectElement->listToOptionIndex(index);
        selectElement->setSelectedIndex(optionIndex, true);
#else
        SelectElement* selectElement = m_select;
        int optionIndex = selectElement->listToOptionIndex(index);
        m_select->setSelectedIndex(optionIndex, true);
#endif
        m_select->dispatchFormControlChangeEvent();
        m_viewImpl->contentInvalidate(m_select->getRect());
    }

    // Response if the listbox allows multiple selection.  array stores the listIndices
    // of selected positions.
    virtual void replyIntArray(const int* array, int count)
    {
        // If the select element no longer exists, due to a page change, etc,
        // silently return.
        if (!m_select || !CacheBuilder::validNode(m_viewImpl->m_mainFrame,
                m_frame, m_select))
            return;

        // If count is 1 or 0, use replyInt.
        SkASSERT(count > 1);
#if ENABLE(WML)
        const WTF::Vector<Element*>& items = WebCore::toSelectElement(m_select)->listItems();
#else
        const WTF::Vector<Element*>& items = m_select->listItems();
#endif
        int totalItems = static_cast<int>(items.size());
        // Keep track of the position of the value we are comparing against.
        int arrayIndex = 0;
        // The value we are comparing against.
        int selection = array[arrayIndex];
#if ENABLE(WML)
        // SAMSUNG_WML_FIXES+
        // http://spe.mobilephone.net/wit/wmlv2/formsubpost.wml
        // http://spe.mobilephone.net/wit/wmlv2/formselect.wml
        WebCore::OptionElement* option   = NULL;
        // SAMSUNG_WML_FIXES-
#else
        WebCore::HTMLOptionElement* option;
#endif
        for (int listIndex = 0; listIndex < totalItems; listIndex++) {
            if (items[listIndex]->hasLocalName(WebCore::HTMLNames::optionTag)) {
#if ENABLE(WML)
                // SAMSUNG_WML_FIXES+
                option = WebCore::toOptionElement(items[listIndex]);
                // SAMSUNG_WML_FIXES-   
#else
                option = static_cast<WebCore::HTMLOptionElement*>(
                        items[listIndex]);
#endif
                if (listIndex == selection) {
                    option->setSelectedState(true);
                    arrayIndex++;
                    if (arrayIndex == count)
                        selection = -1;
                    else
                        selection = array[arrayIndex];
                } else
                    option->setSelectedState(false);
            }
        }
        m_select->dispatchFormControlChangeEvent();
        m_viewImpl->contentInvalidate(m_select->getRect());
    }
private:
    // The select element associated with this listbox.
#if ENABLE(WML)
    WebCore::Element* m_select;
#else
    WebCore::HTMLSelectElement* m_select;
#endif
    // The frame of this select element, to verify that it is valid.
    WebCore::Frame* m_frame;
    // For calling invalidate and checking the select element's validity
    WebViewCore* m_viewImpl;
};

// Create an array of java Strings.
static jobjectArray makeLabelArray(JNIEnv* env, const uint16_t** labels, size_t count)
{
    jclass stringClass = env->FindClass("java/lang/String");
    LOG_ASSERT(stringClass, "Could not find java/lang/String");
    jobjectArray array = env->NewObjectArray(count, stringClass, 0);
    LOG_ASSERT(array, "Could not create new string array");

    for (size_t i = 0; i < count; i++) {
        jobject newString = env->NewString(&labels[i][1], labels[i][0]);
        env->SetObjectArrayElement(array, i, newString);
        env->DeleteLocalRef(newString);
        checkException(env);
    }
    env->DeleteLocalRef(stringClass);
    return array;
}

void WebViewCore::openFileChooser(PassRefPtr<WebCore::FileChooser> chooser) {
    if (!chooser)
        return;
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    jstring jName = (jstring) env->CallObjectMethod(
            m_javaGlue->object(env).get(), m_javaGlue->m_openFileChooser);
    checkException(env);
    const UChar* string = (const UChar*) env->GetStringChars(jName, NULL);
    if (!string)
        return;
    WebCore::String webcoreString = to_string(env, jName);
    env->ReleaseStringChars(jName, string);
    chooser->chooseFile(webcoreString);
}

void WebViewCore::listBoxRequest(WebCoreReply* reply, const uint16_t** labels, size_t count, const int enabled[], size_t enabledCount,
        bool multiple, const int selected[], size_t selectedCountOrSelection, const WebCore::Element * select/*SAMSUNG CHANGE*/ )
{
    // If m_popupReply is not null, then we already have a list showing.
    if (m_popupReply != 0)
        return;

    LOG_ASSERT(m_javaGlue->m_obj, "No java widget associated with this view!");

    // Create an array of java Strings for the drop down.
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    jobjectArray labelArray = makeLabelArray(env, labels, count);

    // Create an array determining whether each item is enabled.
    jintArray enabledArray = env->NewIntArray(enabledCount);
    checkException(env);
    jint* ptrArray = env->GetIntArrayElements(enabledArray, 0);
    checkException(env);
    for (size_t i = 0; i < enabledCount; i++) {
        ptrArray[i] = enabled[i];
    }
    env->ReleaseIntArrayElements(enabledArray, ptrArray, 0);
    checkException(env);
	//SAMSUNG CHANGE >>
    String nodeName = select->formControlName() ;
    jstring jName = env->NewString((jchar*) nodeName.characters(), nodeName.length());
	//SAMSUNG CHANGE <<
    if (multiple) {
        // Pass up an array representing which items are selected.
        jintArray selectedArray = env->NewIntArray(selectedCountOrSelection);
        checkException(env);
        jint* selArray = env->GetIntArrayElements(selectedArray, 0);
        checkException(env);
        for (size_t i = 0; i < selectedCountOrSelection; i++) {
            selArray[i] = selected[i];
        }
        env->ReleaseIntArrayElements(selectedArray, selArray, 0);

        env->CallVoidMethod(m_javaGlue->object(env).get(),
                m_javaGlue->m_requestListBox, labelArray, jName/*SAMSUNG CHANGE*/, enabledArray,
                selectedArray, (jint) select/*SAMSUNG CHANGE*/);
        env->DeleteLocalRef(selectedArray);
    } else {
        // Pass up the single selection.
        env->CallVoidMethod(m_javaGlue->object(env).get(),
                m_javaGlue->m_requestSingleListBox, labelArray, jName/*SAMSUNG CHANGE*/, enabledArray,
                selectedCountOrSelection, (jint) select/*SAMSUNG CHANGE*/);
    }

    env->DeleteLocalRef(labelArray);
    env->DeleteLocalRef(enabledArray);
    checkException(env);

    Retain(reply);
    m_popupReply = reply;
}

bool WebViewCore::key(const PlatformKeyboardEvent& event)
{
    WebCore::EventHandler* eventHandler = m_mainFrame->eventHandler();
    WebCore::Node* focusNode = currentFocus();
    if (focusNode)
        eventHandler = focusNode->document()->frame()->eventHandler();
    DBG_NAV_LOGD("keyCode=%s unichar=%d focusNode=%p",
        event.keyIdentifier().utf8().data(), event.unichar(), focusNode);
    return eventHandler->keyEvent(event);
}

// For when the user clicks the trackball
void WebViewCore::click(WebCore::Frame* frame, WebCore::Node* node) {
    if (!node) {
        WebCore::IntPoint pt = m_mousePos;
        pt.move(m_scrollOffsetX, m_scrollOffsetY);
        WebCore::HitTestResult hitTestResult = m_mainFrame->eventHandler()->
                hitTestResultAtPoint(pt, false);
        node = hitTestResult.innerNode();
        frame = node->document()->frame();
        DBG_NAV_LOGD("m_mousePos=(%d,%d) m_scrollOffset=(%d,%d) pt=(%d,%d)"
            " node=%p", m_mousePos.x(), m_mousePos.y(),
            m_scrollOffsetX, m_scrollOffsetY, pt.x(), pt.y(), node);
    }
    if (node) {
        EditorClientAndroid* client
                = static_cast<EditorClientAndroid*>(
                m_mainFrame->editor()->client());
        client->setShouldChangeSelectedRange(false);
        handleMouseClick(frame, node);
        client->setShouldChangeSelectedRange(true);
    }
}

#if USE(ACCELERATED_COMPOSITING)
GraphicsLayerAndroid* WebViewCore::graphicsRootLayer() const
{
    RenderView* contentRenderer = m_mainFrame->contentRenderer();
    if (!contentRenderer)
        return 0;
    return static_cast<GraphicsLayerAndroid*>(
          contentRenderer->compositor()->rootPlatformLayer());
}
#endif

bool WebViewCore::handleTouchEvent(int action, int x, int y, int metaState)
{
    bool preventDefault = false;

#if USE(ACCELERATED_COMPOSITING)
    GraphicsLayerAndroid* rootLayer = graphicsRootLayer();
    if (rootLayer)
      rootLayer->pauseDisplay(true);
#endif

#if ENABLE(TOUCH_EVENTS) // Android
    WebCore::TouchEventType type = WebCore::TouchStart;
    WebCore::PlatformTouchPoint::State touchState = WebCore::PlatformTouchPoint::TouchPressed;
    switch (action) {
    case 0: // MotionEvent.ACTION_DOWN
        type = WebCore::TouchStart;
        break;
    case 1: // MotionEvent.ACTION_UP
        type = WebCore::TouchEnd;
        touchState = WebCore::PlatformTouchPoint::TouchReleased;
        break;
    case 2: // MotionEvent.ACTION_MOVE
        type = WebCore::TouchMove;
        touchState = WebCore::PlatformTouchPoint::TouchMoved;
        break;
    case 3: // MotionEvent.ACTION_CANCEL
        type = WebCore::TouchCancel;
        touchState = WebCore::PlatformTouchPoint::TouchCancelled;
        break;
    case 0x100: // WebViewCore.ACTION_LONGPRESS
        type = WebCore::TouchLongPress;
        touchState = WebCore::PlatformTouchPoint::TouchPressed;
        break;
    case 0x200: // WebViewCore.ACTION_DOUBLETAP
        type = WebCore::TouchDoubleTap;
        touchState = WebCore::PlatformTouchPoint::TouchPressed;
        break;
    default:
        // We do not support other kinds of touch event inside WebCore
        // at the moment.
        LOGW("Java passed a touch event type that we do not support in WebCore: %d", action);
        return 0;
    }

    // Track previous touch and if stationary set the state.
    WebCore::IntPoint pt(x - m_scrollOffsetX, y - m_scrollOffsetY);

//  handleTouchEvent() in EventHandler.cpp doesn't handle TouchStationary, which
//  causes preventDefault be false when it returns. As our Java side may continue
//  process the events if WebKit doesn't, it can cause unexpected result.
//    if (type == WebCore::TouchMove && pt == m_lastTouchPoint)
//        touchState = WebCore::PlatformTouchPoint::TouchStationary;

    m_lastTouchPoint = pt;

    WebCore::PlatformTouchEvent te(pt, type, touchState, metaState);
    preventDefault = m_mainFrame->eventHandler()->handleTouchEvent(te);
#endif

#if USE(ACCELERATED_COMPOSITING)
    if (rootLayer)
      rootLayer->pauseDisplay(false);
#endif
    return preventDefault;
}

void WebViewCore::touchUp(int touchGeneration,
    WebCore::Frame* frame, WebCore::Node* node, int x, int y)
{
    if (m_touchGeneration > touchGeneration) {
        DBG_NAV_LOGD("m_touchGeneration=%d > touchGeneration=%d"
            " x=%d y=%d", m_touchGeneration, touchGeneration, x, y);
        return; // short circuit if a newer touch has been generated
    }
    // This moves m_mousePos to the correct place, and handleMouseClick uses
    // m_mousePos to determine where the click happens.
    moveMouse(frame, x, y);
    m_lastGeneration = touchGeneration;
    if (frame && CacheBuilder::validNode(m_mainFrame, frame, 0)) {
        frame->loader()->resetMultipleFormSubmissionProtection();
    }
    DBG_NAV_LOGD("touchGeneration=%d handleMouseClick frame=%p node=%p"
        " x=%d y=%d", touchGeneration, frame, node, x, y);
    handleMouseClick(frame, node);
}

// Common code for both clicking with the trackball and touchUp
bool WebViewCore::handleMouseClick(WebCore::Frame* framePtr, WebCore::Node* nodePtr)
{
    bool valid = framePtr == NULL
            || CacheBuilder::validNode(m_mainFrame, framePtr, nodePtr);
    WebFrame* webFrame = WebFrame::getWebFrame(m_mainFrame);
    if (valid && nodePtr) {
    // Need to special case area tags because an image map could have an area element in the middle
    // so when attempting to get the default, the point chosen would be follow the wrong link.
        if (nodePtr->hasTagName(WebCore::HTMLNames::areaTag)) {
            webFrame->setUserInitiatedClick(true);
            nodePtr->dispatchSimulatedClick(0, true, true);
            webFrame->setUserInitiatedClick(false);
            DBG_NAV_LOG("area");
            return true;
        }
        WebCore::RenderObject* renderer = nodePtr->renderer();
        if (renderer && (renderer->isMenuList() || renderer->isListBox())) {
#if ENABLE(WML)
            WebCore::Element *elementPtr = static_cast<WebCore::Element*>(nodePtr) ;
            WebCore::SelectElement* select = WebCore::toSelectElement(elementPtr);
#else
            WebCore::HTMLSelectElement* select = static_cast<WebCore::HTMLSelectElement*>(nodePtr);
#endif
            const WTF::Vector<WebCore::Element*>& listItems = select->listItems();
            SkTDArray<const uint16_t*> names;
            // Possible values for enabledArray.  Keep in Sync with values in
            // InvokeListBox.Container in WebView.java
            enum OptionStatus {
                OPTGROUP = -1,
                OPTION_DISABLED = 0,
                OPTION_ENABLED = 1,
            };
            SkTDArray<int> enabledArray;
            SkTDArray<int> selectedArray;
            int size = listItems.size();
            bool multiple = select->multiple();
            for (int i = 0; i < size; i++) {
#if ENABLE(WML)
                if(WebCore::isOptionElement(listItems[i]) ){
                    WebCore::OptionElement *op = WebCore::toOptionElement(listItems[i]);
                    if(listItems[i]->isWMLElement()) {
                        WebCore::WMLOptionElement* option = static_cast<WebCore::WMLOptionElement*>(listItems[i]);

                        *enabledArray.append() = option->disabled() ? OPTION_DISABLED : OPTION_ENABLED;
                    }
                    else {
                        WebCore::HTMLOptionElement* option = static_cast<WebCore::HTMLOptionElement*>(listItems[i]);

                        *enabledArray.append() = option->disabled() ? OPTION_DISABLED : OPTION_ENABLED;
                    }
                    WebCore::String label = op->textIndentedToRespectGroupLabel() ;
                    
                    *names.append() = stringConverter(label);

                    if (multiple && op->selected())
                        *selectedArray.append() = i;
#else
                if (listItems[i]->hasTagName(WebCore::HTMLNames::optionTag)) {
                    WebCore::HTMLOptionElement* option = static_cast<WebCore::HTMLOptionElement*>(listItems[i]);
                    *names.append() = stringConverter(option->textIndentedToRespectGroupLabel());
                    *enabledArray.append() = option->disabled() ? OPTION_DISABLED : OPTION_ENABLED;
                    if (multiple && option->selected())
                        *selectedArray.append() = i;
                } else if (listItems[i]->hasTagName(WebCore::HTMLNames::optgroupTag)) {
#endif
#if ENABLE(WML)
                } else if ( WebCore::isOptionGroupElement(listItems[i]) ) {
                    WebCore::OptionGroupElement* optGroup = WebCore::toOptionGroupElement(listItems[i]);
#else
                    WebCore::HTMLOptGroupElement* optGroup = static_cast<WebCore::HTMLOptGroupElement*>(listItems[i]);
#endif
                    *names.append() = stringConverter(optGroup->groupLabelText());
                    *enabledArray.append() = OPTGROUP;
                }
            }
#if ENABLE(WML)
            WebCoreReply* reply = new ListBoxReply(elementPtr, elementPtr->document()->frame(), this);
#else
            WebCoreReply* reply = new ListBoxReply(select, select->document()->frame(), this);
#endif
            // Use a pointer to HTMLSelectElement's superclass, where
            // optionToListIndex is public.
            SelectElement* selectElement = select;
            listBoxRequest(reply, names.begin(), size, enabledArray.begin(), enabledArray.count(),
                    multiple, selectedArray.begin(), multiple ? selectedArray.count() :
                    selectElement->optionToListIndex(select->selectedIndex()), static_cast<WebCore::Element*>(nodePtr)/*SAMSUNG CHANGE*/);
            DBG_NAV_LOG("menu list");
            return true;
        }
    }
    if (!valid || !framePtr)
        framePtr = m_mainFrame;
    webFrame->setUserInitiatedClick(true);
    WebCore::PlatformMouseEvent mouseDown(m_mousePos, m_mousePos, WebCore::LeftButton,
            WebCore::MouseEventPressed, 1, false, false, false, false,
            WTF::currentTime());
    // ignore the return from as it will return true if the hit point can trigger selection change
    framePtr->eventHandler()->handleMousePressEvent(mouseDown);
    WebCore::PlatformMouseEvent mouseUp(m_mousePos, m_mousePos, WebCore::LeftButton,
            WebCore::MouseEventReleased, 1, false, false, false, false,
            WTF::currentTime());
    bool handled = framePtr->eventHandler()->handleMouseReleaseEvent(mouseUp);
    webFrame->setUserInitiatedClick(false);

    // If the user clicked on a textfield, make the focusController active
    // so we show the blinking cursor.
    WebCore::Node* focusNode = currentFocus();
    DBG_NAV_LOGD("m_mousePos={%d,%d} focusNode=%p handled=%s", m_mousePos.x(),
        m_mousePos.y(), focusNode, handled ? "true" : "false");
    if (focusNode) {
        WebCore::RenderObject* renderer = focusNode->renderer();
        if (renderer && (renderer->isTextField() || renderer->isTextArea())) {
            //SAMSUNG FIX >>
#if ENABLE(WML)
            bool ime = false;
            if (renderer->isTextArea()) {
                ime = !(static_cast<WebCore::HTMLTextAreaElement*>(focusNode))->readOnly();
            } else {
                WebCore::InputElement *ie = WebCore::toInputElement(static_cast<WebCore::Element*>(focusNode));
                ime = ie ? !ie->readOnly() : true ;
            }
#else
            bool ime = false;
            if (renderer->isTextField()) {
                ime = !(static_cast<WebCore::HTMLInputElement*>(focusNode))->readOnly();
            } else {
                ime = !(static_cast<WebCore::HTMLTextAreaElement*>(focusNode))->readOnly();
            }
#endif
            //SAMSUNG FIX GA0100076825
            m_frameCacheOutOfDate = true;
            updateFrameCache();
            //SAMSUNG FIX <<
            setFocusControllerActive(framePtr, ime);
            if (ime) {
                RenderTextControl* rtc
                        = static_cast<RenderTextControl*> (renderer);
                requestKeyboardWithSelection(focusNode, rtc->selectionStart(),
                        rtc->selectionEnd());
            } else {
                requestKeyboard(false);
            }
        }
    }
    return handled;
}

void WebViewCore::popupReply(int index)
{
    if (m_popupReply) {
        m_popupReply->replyInt(index);
        //SAMSUNG CHANGE >>
        if (index == -2) {
            Release(m_popupReply);
            m_popupReply = 0;
        }
        //SAMSUNG CHANGE <<
    }
}

void WebViewCore::popupReply(const int* array, int count)
{
    if (m_popupReply) {
        m_popupReply->replyIntArray(array, count);
        Release(m_popupReply);
        m_popupReply = NULL;
    }
}

void WebViewCore::addMessageToConsole(const WebCore::String& message, unsigned int lineNumber, const WebCore::String& sourceID, int msgLevel) {
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    jstring jMessageStr = env->NewString((unsigned short *)message.characters(), message.length());
    jstring jSourceIDStr = env->NewString((unsigned short *)sourceID.characters(), sourceID.length());
    env->CallVoidMethod(m_javaGlue->object(env).get(),
            m_javaGlue->m_addMessageToConsole, jMessageStr, lineNumber,
            jSourceIDStr, msgLevel);
    env->DeleteLocalRef(jMessageStr);
    env->DeleteLocalRef(jSourceIDStr);
    checkException(env);
}

void WebViewCore::jsAlert(const WebCore::String& url, const WebCore::String& text)
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    jstring jInputStr = env->NewString((unsigned short *)text.characters(), text.length());
    jstring jUrlStr = env->NewString((unsigned short *)url.characters(), url.length());
    env->CallVoidMethod(m_javaGlue->object(env).get(), m_javaGlue->m_jsAlert, jUrlStr, jInputStr);
    env->DeleteLocalRef(jInputStr);
    env->DeleteLocalRef(jUrlStr);
    checkException(env);
}

void WebViewCore::exceededDatabaseQuota(const WebCore::String& url, const WebCore::String& databaseIdentifier, const unsigned long long currentQuota, unsigned long long estimatedSize)
{
#if ENABLE(DATABASE)
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    jstring jDatabaseIdentifierStr = env->NewString((unsigned short *)databaseIdentifier.characters(), databaseIdentifier.length());
    jstring jUrlStr = env->NewString((unsigned short *)url.characters(), url.length());
    env->CallVoidMethod(m_javaGlue->object(env).get(),
            m_javaGlue->m_exceededDatabaseQuota, jUrlStr,
            jDatabaseIdentifierStr, currentQuota, estimatedSize);
    env->DeleteLocalRef(jDatabaseIdentifierStr);
    env->DeleteLocalRef(jUrlStr);
    checkException(env);
#endif
}

void WebViewCore::reachedMaxAppCacheSize(const unsigned long long spaceNeeded)
{
#if ENABLE(OFFLINE_WEB_APPLICATIONS)
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(),
            m_javaGlue->m_reachedMaxAppCacheSize, spaceNeeded);
    checkException(env);
#endif
}

void WebViewCore::populateVisitedLinks(WebCore::PageGroup* group)
{
    m_groupForVisitedLinks = group;
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(), m_javaGlue->m_populateVisitedLinks);
    checkException(env);
}

void WebViewCore::geolocationPermissionsShowPrompt(const WebCore::String& origin)
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    jstring originString = env->NewString((unsigned short *)origin.characters(), origin.length());
    env->CallVoidMethod(m_javaGlue->object(env).get(),
                        m_javaGlue->m_geolocationPermissionsShowPrompt,
                        originString);
    env->DeleteLocalRef(originString);
    checkException(env);
}

void WebViewCore::geolocationPermissionsHidePrompt()
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(),
                        m_javaGlue->m_geolocationPermissionsHidePrompt);
    checkException(env);
}

bool WebViewCore::jsConfirm(const WebCore::String& url, const WebCore::String& text)
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    jstring jInputStr = env->NewString((unsigned short *)text.characters(), text.length());
    jstring jUrlStr = env->NewString((unsigned short *)url.characters(), url.length());
    jboolean result = env->CallBooleanMethod(m_javaGlue->object(env).get(), m_javaGlue->m_jsConfirm, jUrlStr, jInputStr);
    env->DeleteLocalRef(jInputStr);
    env->DeleteLocalRef(jUrlStr);
    checkException(env);
    return result;
}

bool WebViewCore::jsPrompt(const WebCore::String& url, const WebCore::String& text, const WebCore::String& defaultValue, WebCore::String& result)
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    jstring jInputStr = env->NewString((unsigned short *)text.characters(), text.length());
    jstring jDefaultStr = env->NewString((unsigned short *)defaultValue.characters(), defaultValue.length());
    jstring jUrlStr = env->NewString((unsigned short *)url.characters(), url.length());
    jstring returnVal = (jstring) env->CallObjectMethod(m_javaGlue->object(env).get(), m_javaGlue->m_jsPrompt, jUrlStr, jInputStr, jDefaultStr);
    // If returnVal is null, it means that the user cancelled the dialog.
    if (!returnVal)
        return false;

    result = to_string(env, returnVal);
    env->DeleteLocalRef(jInputStr);
    env->DeleteLocalRef(jDefaultStr);
    env->DeleteLocalRef(jUrlStr);
    checkException(env);
    return true;
}

bool WebViewCore::jsUnload(const WebCore::String& url, const WebCore::String& message)
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    jstring jInputStr = env->NewString((unsigned short *)message.characters(), message.length());
    jstring jUrlStr = env->NewString((unsigned short *)url.characters(), url.length());
    jboolean result = env->CallBooleanMethod(m_javaGlue->object(env).get(), m_javaGlue->m_jsUnload, jUrlStr, jInputStr);
    env->DeleteLocalRef(jInputStr);
    env->DeleteLocalRef(jUrlStr);
    checkException(env);
    return result;
}

bool WebViewCore::jsInterrupt()
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    jboolean result = env->CallBooleanMethod(m_javaGlue->object(env).get(), m_javaGlue->m_jsInterrupt);
    checkException(env);
    return result;
}

AutoJObject
WebViewCore::getJavaObject()
{
    return getRealObject(JSC::Bindings::getJNIEnv(), m_javaGlue->m_obj);
}

jobject
WebViewCore::getWebViewJavaObject()
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    return env->GetObjectField(m_javaGlue->object(env).get(), gWebViewCoreFields.m_webView);
}

void WebViewCore::updateTextSelection() {
    WebCore::Node* focusNode = currentFocus();
    if (!focusNode)
        return;
    RenderObject* renderer = focusNode->renderer();
    if (!renderer || (!renderer->isTextArea() && !renderer->isTextField()))
        return;
    RenderTextControl* rtc = static_cast<RenderTextControl*>(renderer);
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(),
            m_javaGlue->m_updateTextSelection, reinterpret_cast<int>(focusNode),
            rtc->selectionStart(), rtc->selectionEnd(), m_textGeneration);
    checkException(env);
}

void WebViewCore::updateTextfield(WebCore::Node* ptr, bool changeToPassword,
        const WebCore::String& text)
{
    if (m_blockTextfieldUpdates)
        return;
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    if (changeToPassword) {
        env->CallVoidMethod(m_javaGlue->object(env).get(), m_javaGlue->m_updateTextfield,
                (int) ptr, true, 0, m_textGeneration);
        checkException(env);
        return;
    }
    int length = text.length();
    jstring string = env->NewString((unsigned short *) text.characters(), length);
    env->CallVoidMethod(m_javaGlue->object(env).get(), m_javaGlue->m_updateTextfield,
            (int) ptr, false, string, m_textGeneration);
    env->DeleteLocalRef(string);
    checkException(env);
}

void WebViewCore::clearTextEntry()
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(),
        m_javaGlue->m_clearTextEntry);
}

void WebViewCore::setBackgroundColor(SkColor c)
{
    WebCore::FrameView* view = m_mainFrame->view();
    if (!view)
        return;

    // need (int) cast to find the right constructor
    WebCore::Color bcolor((int)SkColorGetR(c), (int)SkColorGetG(c),
                          (int)SkColorGetB(c), (int)SkColorGetA(c));
    view->setBaseBackgroundColor(bcolor);

    // Background color of 0 indicates we want a transparent background
    if (c == 0)
        view->setTransparent(true);
}

jclass WebViewCore::getPluginClass(const WebCore::String& libName, const char* className)
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();

    jstring libString = env->NewString(libName.characters(), libName.length());
    jstring classString = env->NewStringUTF(className);
    jobject pluginClass = env->CallObjectMethod(m_javaGlue->object(env).get(),
                                           m_javaGlue->m_getPluginClass,
                                           libString, classString);
    checkException(env);

    // cleanup unneeded local JNI references
    env->DeleteLocalRef(libString);
    env->DeleteLocalRef(classString);

    if (pluginClass != NULL) {
        return static_cast<jclass>(pluginClass);
    } else {
        return NULL;
    }
}

void WebViewCore::showFullScreenPlugin(jobject childView, NPP npp)
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    AutoJObject obj = m_javaGlue->object(env);

    env->CallVoidMethod(obj.get(),
                        m_javaGlue->m_showFullScreenPlugin, childView, (int)npp);
    checkException(env);
}

void WebViewCore::hideFullScreenPlugin()
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(),
                        m_javaGlue->m_hideFullScreenPlugin);
    checkException(env);
}

jobject WebViewCore::addSurface(jobject view, int x, int y, int width, int height)
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    jobject result = env->CallObjectMethod(m_javaGlue->object(env).get(),
                                           m_javaGlue->m_addSurface,
                                           view, x, y, width, height);
    checkException(env);
    return result;
}

void WebViewCore::updateSurface(jobject childView, int x, int y, int width, int height)
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(),
                        m_javaGlue->m_updateSurface, childView,
                        x, y, width, height);
    checkException(env);
}

void WebViewCore::destroySurface(jobject childView)
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(), m_javaGlue->m_destroySurface, childView);
    checkException(env);
}

jobject WebViewCore::getContext()
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    AutoJObject obj = m_javaGlue->object(env);

    jobject result = env->CallObjectMethod(obj.get(), m_javaGlue->m_getContext);
    checkException(env);
    return result;
}

bool WebViewCore::validNodeAndBounds(Frame* frame, Node* node,
    const IntRect& originalAbsoluteBounds)
{
    bool valid = CacheBuilder::validNode(m_mainFrame, frame, node);
    if (!valid)
        return false;
    RenderObject* renderer = node->renderer();
    if (!renderer)
        return false;
    IntRect absBounds = node->hasTagName(HTMLNames::areaTag)
        ? CacheBuilder::getAreaRect(static_cast<HTMLAreaElement*>(node))
        : renderer->absoluteBoundingBoxRect();
    return absBounds == originalAbsoluteBounds;
}

void WebViewCore::showRect(int left, int top, int width, int height,
        int contentWidth, int contentHeight, float xPercentInDoc,
        float xPercentInView, float yPercentInDoc, float yPercentInView,
//SAMSUNG CHANGES+
		bool hasAnchorDiff, int oldAnchorX, int newAnchorX, int oldAnchorY, int newAnchorY)
//SAMSUNG CHANGES-
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(), m_javaGlue->m_showRect,
            left, top, width, height, contentWidth, contentHeight,
            xPercentInDoc, xPercentInView, yPercentInDoc, yPercentInView,
//SAMSUNG CHANGES+
			hasAnchorDiff, oldAnchorX, newAnchorX, oldAnchorY, newAnchorY);
//SAMSUNG CHANGES-
    checkException(env);
}

void WebViewCore::centerFitRect(int x, int y, int width, int height)
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(),
            m_javaGlue->m_centerFitRect, x, y, width, height);
    checkException(env);
}


void WebViewCore::setScrollbarModes(ScrollbarMode horizontalMode, ScrollbarMode verticalMode)
{
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    env->CallVoidMethod(m_javaGlue->object(env).get(), m_javaGlue->m_setScrollbarModes,
            horizontalMode, verticalMode);
    checkException(env);
}


//SAMSUNG CHANGE BEGIN : ADVANCED_COPY_PASTE


void WebViewCore::webTextSelectionAll(int x1, int y1, int x2, int y2 )
{
	int granularity  = -1;
	//select closet word 
	selectClosestWord(x1,y1,0.4,granularity);
	
	// First case
	int controller =5;
	bool smartGranularity = true;
	bool selectionMove = false;
       float zoomLevel  = 0.4f;
 
	
	//then move the selection	
	copyMoveSelection(x2,y2, controller, smartGranularity, selectionMove, zoomLevel, granularity);
	
	selectionMove = true;
	copyMoveSelection(x2,y2, controller, smartGranularity, selectionMove, zoomLevel, granularity);

	LOGD("webTextSelectionAll calles   %d  , %d",  x1, y1);
}

int mSelectionDirection = WebCore::SelectionController::FORWARD;


void WebViewCore::copyMoveSelection(int x, int y,  int controller, bool smartGranularity, bool selectionMove, float zoomLevel , int granularity)
{
    DEBUG_NAV_UI_LOGD("%s: x,y position: %d, %d", __FUNCTION__, x, y);
    WebCore::Frame* frame = m_mainFrame;
    WebCore::IntPoint contentsPoint = WebCore::IntPoint(x, y);
    WebCore::IntPoint wndPoint = frame->view()->contentsToWindow(contentsPoint);

        DEBUG_NAV_UI_LOGD("%s: second time click", __FUNCTION__);

		//Set Direction 
        if (selectionMove == false)
        {
            copySetSelectionDirection(controller);
            if(frame->selectionGranularity() == WebCore::WordGranularity )
            {
            frame->setSelectionGranularity(WebCore::CharacterGranularity);
            frame->selection()->expandUsingGranularity(WebCore::CharacterGranularity);
            DEBUG_NAV_UI_LOGD("Changed from Word to character Granularity");
            }else   if(frame->selectionGranularity() == WebCore::CharacterGranularity )
            {
          	  frame->selection()->expandUsingGranularity(WebCore::CharacterGranularity);
           	  LOGD("Set the character Granularity");
            }
        }

		if(smartGranularity == true){
	        if(!inSameParagraph(WebCore::VisiblePosition(frame->selection()->base()), 
	            WebCore::VisiblePosition(frame->selection()->extent())) 
	             && frame->selectionGranularity() != WebCore::ParagraphGranularity)
	        {
	            frame->setSelectionGranularity(WebCore::ParagraphGranularity);
	            DEBUG_NAV_UI_LOGD("Set Paragraph Granularity");
	        } 
	        else if(inSameParagraph(WebCore::VisiblePosition(frame->selection()->base()),
	                WebCore::VisiblePosition(frame->selection()->extent())) 
	                && frame->selectionGranularity() == WebCore::ParagraphGranularity)
	        {
	            frame->setSelectionGranularity(WebCore::CharacterGranularity);
	            frame->selection()->expandUsingGranularity(WebCore::CharacterGranularity);
	            DEBUG_NAV_UI_LOGD("Change from Paragraph to Character Granularity");
	        }
	         //Set In Paragraph mode when zoom level is less than 0.8
	        if (zoomLevel < 0.8 && frame->selectionGranularity() != WebCore::ParagraphGranularity)
	        {
	            frame->setSelectionGranularity(WebCore::ParagraphGranularity);
	            DEBUG_NAV_UI_LOGD("Set Paragraph Granularity for Less Zoom Level");
	        }
		}

				
	 WebCore::TextGranularity CurrGranulaity = frame->selectionGranularity() ;

	//User Granularity Apply if Set 
	if(granularity != -1 && CurrGranulaity == WebCore::CharacterGranularity){
		frame->setSelectionGranularity((WebCore::TextGranularity) granularity );
	   LOGD("Set  Granularity by client  %d",  granularity);
	   webkitCopyMoveSelection(wndPoint, contentsPoint,  controller);
	   frame->setSelectionGranularity((WebCore::TextGranularity) CurrGranulaity );
	}  else{      
           webkitCopyMoveSelection(wndPoint, contentsPoint, controller);
	}

    DEBUG_NAV_UI_LOGD("%s: End", __FUNCTION__);
}

void WebViewCore::clearTextSelection(int contentX, int contentY)
{
    DEBUG_NAV_UI_LOGD("%s: x,y position: %d, %d", __FUNCTION__, contentX, contentY);
    WebCore::Frame* frame = m_mainFrame;
    WebCore::FrameView *frameview = frame->view();
    WebCore::IntPoint contentPoint = WebCore::IntPoint(contentX, contentY);
    WebCore::IntPoint wndPoint = frame->view()->contentsToWindow(contentPoint);
    
    WebCore::PlatformMouseEvent pme(wndPoint, contentPoint, LeftButton, 
                     WebCore::MouseEventPressed, 1, false, false, false, false, 0);

    frameview->frame()->eventHandler()->handleMousePressEvent(pme);
    DEBUG_NAV_UI_LOGD("%s: End", __FUNCTION__);
}

void WebViewCore::copySetSelectionDirection(int controller)
{
    DEBUG_NAV_UI_LOGD("%s: Set the Selection Direction", __FUNCTION__);

    WebCore::Frame* frame = m_mainFrame;
    frame->eventHandler()->setMousePressed(true);
    switch(controller)
    {
        case 2:
        case 5:
            mSelectionDirection = SelectionController::FORWARD;
            frame->selection()->willBeModified(SelectionController::EXTEND, SelectionController::FORWARD);
        break;
        
        case 3:
            mSelectionDirection = SelectionController::LEFT;
            frame->selection()->willBeModified(SelectionController::EXTEND, SelectionController::LEFT);
        break;
        case 4:
            mSelectionDirection = SelectionController::RIGHT;
            frame->selection()->willBeModified(SelectionController::EXTEND, SelectionController::RIGHT);
       break;       

    case 1:
    case 6:
         mSelectionDirection = SelectionController::BACKWARD;
            frame->selection()->willBeModified(SelectionController::EXTEND, SelectionController::BACKWARD);
    break;
       default:
            DEBUG_NAV_UI_LOGD("%s: Invalid Direction: %d", __FUNCTION__, controller);
            frame->eventHandler()->setMousePressed(false);
        break;
    }
}

void WebViewCore::webkitCopyMoveSelection(WebCore::IntPoint wndPoint, WebCore::IntPoint contentPoint, int controller)
{
    DEBUG_NAV_UI_LOGD("%s", __FUNCTION__);
    WebCore::Frame* frame = m_mainFrame;
    DEBUG_NAV_UI_LOGD("%s: Frame=%s", __FUNCTION__, frame);
    WebCore::FrameView *frameview = frame->view();

    if(frame->selectionGranularity() == WebCore::ParagraphGranularity)
    {
        DEBUG_NAV_UI_LOGD("%s: Moving in Paragraph Granularity", __FUNCTION__);
        WebCore::IntRect box = WebCore::IntRect(0,0,0,0);
        int left = 0, top = 0, right = 0, bottom = 0;

         if (RefPtr<Range> range = frame->selection()->toNormalizedRange()) 
        {
            ExceptionCode ec = 0;
            RefPtr<Range> tempRange = range->cloneRange(ec);
            // consider image selection also while getting the bounds.
            box = tempRange->boundingBoxEx();
            left = box.x();
            top = box.y();
            right = left + box.width();
            bottom = top + box.height();
            DEBUG_NAV_UI_LOGD("%s: BoundingRect:[%d, %d, %d, %d]", __FUNCTION__, box.x(), box.y(), box.width(), box.height());
        } 
        else
        {
            DEBUG_NAV_UI_LOGD("%s: Exception in getting Selection Region", __FUNCTION__);
            return;
        }
        switch(mSelectionDirection)
        {
            case WebCore::SelectionController::FORWARD:
            contentPoint=contentPoint.shrunkTo(WebCore::IntPoint(right, contentPoint.y()));
            contentPoint=contentPoint.expandedTo(WebCore::IntPoint(left, top));
            break;
            
            case WebCore::SelectionController::BACKWARD:
                contentPoint=contentPoint.expandedTo(WebCore::IntPoint(left, contentPoint.y()));
                contentPoint=contentPoint.shrunkTo(WebCore::IntPoint(right, bottom));
            break;
            
            case WebCore::SelectionController::LEFT:
                contentPoint=contentPoint.expandedTo(WebCore::IntPoint(contentPoint.x(), top));
                contentPoint=contentPoint.shrunkTo(WebCore::IntPoint(right, bottom));
            break;
            
            case WebCore::SelectionController::RIGHT:
                contentPoint=contentPoint.shrunkTo(WebCore::IntPoint(contentPoint.x(), bottom));
                contentPoint=contentPoint.expandedTo(WebCore::IntPoint(left, top));
            break;
            
            default:
            break;
        }
    }
    else
    {
     DEBUG_NAV_UI_LOGD("%s: Character Granularity", __FUNCTION__);
    }

    DEBUG_NAV_UI_LOGD("%s: Point after expansion: %d, %d", __FUNCTION__, contentPoint.x(), contentPoint.y());
    DEBUG_NAV_UI_LOGD("%s: WindowPoint: %d, %d", __FUNCTION__, wndPoint.x(), wndPoint.y());
    WebCore::PlatformMouseEvent pme(wndPoint, contentPoint, LeftButton, 
                            WebCore::MouseEventMoved, 0, false, true, false, false, 0);

    frameview->frame()->eventHandler()->mouseMoved(pme);
    frameview->frame()->eventHandler()->stopAutoscrollTimer();

    DEBUG_NAV_UI_LOGD("%s: End", __FUNCTION__);
    return;
}

bool WebViewCore::recordSelectionCopiedData(SkRegion* region, SkIRect* startRect,
                                                        SkIRect* endRect, int granularity ){

    DBG_SET_LOG("start");
    WebCore::Frame* frame =  m_mainFrame;
    WebCore::IntRect box, start, end;
    int boxX, boxY, boxWidth, boxHeight, endX, endY, temp;
    bool result = false;

    RefPtr<Range> range;
    m_contentMutex.lock();
    if ( /*SISO_HTMLCOMPOSER*/ frame->selection()->isRange() && /*SISO_HTMLCOMPOSER*/ (range = frame->selection()->toNormalizedRange()) ) 
    {
        ExceptionCode ec = 0;
        RefPtr<Range> tempRange = range->cloneRange(ec);
//        if(isEditableSupport())	 
        	box = tempRange->boundingBoxEx(); //Fix for html editor image selection
//		else
//        box = tempRange->boundingBox();
        DEBUG_NAV_UI_LOGD("%s: BoundingRect:[%d, %d, %d, %d]", __FUNCTION__, box.x(), box.y(), box.width(), box.height());

        if (!box.isEmpty())
        {
            region->setRect(box.x(), box.y(), box.x() + box.width(), box.y() + box.height());
            start = frame->firstRectForRange(tempRange.get());
            DEBUG_NAV_UI_LOGD("%s: StartRect:[%d, %d, %d, %d]", __FUNCTION__, start.x(), start.y(), start.width(), start.height());
            startRect->set(start.x(), start.y(), start.x() + start.width(),  start.y() + start.height());
            
            end = frame->lastRectForRange(tempRange.get());
            DEBUG_NAV_UI_LOGD("%s: EndRect:[%d, %d, %d, %d]", __FUNCTION__, end.x(), end.y(), end.width(), end.height());
            endRect->set(end.x(), end.y(), end.x() + end.width(),  end.y() + end.height());

	     // Validation of BOUND RECT X and Y
	     // Validate START and END RECTs assuming that BOUND RECT is correct
	     boxX = box.x();
	     boxY = box.y();
	     boxWidth = box.width();
	     boxHeight = box.height();
            if (boxX < 0)
            {
                boxX = 0;
            }
	     if (boxY < 0)
            {
                boxY = 0;
            }
	     if (box.x() < 0 || box.y() < 0)
	     {
		  	region->setRect(boxX, boxY, boxX + boxWidth, boxY + boxHeight);
	     }
	   // Remove the validation : have side effect in selection bound rect 
	   /*
	     // If START RECT is not within BOUND REC,T push the START RECT to LEFT TOP corner of BOUND RECT
	     // Also START RECT width and height should not be more than BOUND RECT width and height
	     if (!(region->contains(*startRect)))
     	     {
     	         temp = start.height();
     	     	  if (temp > boxHeight)
     	  	  {
     	  	  	temp = boxHeight;
     	  	  }
		  endX = start.width();
	         if (endX > boxWidth)
	         {
	         	endX = boxWidth;
	         }
           	  startRect->set(boxX, boxY, boxX + boxWidth, boxY + temp);
     	     }

	     // If END RECT is not within BOUND RECT, push the END RECT to RIGHT BOTTOM corner of BOUND RECT
	     // Also END RECT width and height should not be more than BOUND RECT width and height
	     if (!(region->contains(*endRect)))
     	     {
     	     	   endX = boxX + boxWidth;
		   endY = boxY + boxHeight;
		   temp = end.height();
		   if (temp > boxHeight)
		   {
		   	temp = boxHeight;
		   }
		   if (end.width() < boxWidth)
	   	   {
	   	   	endX = endX - end.width();
	   	   }
	   	   endY = endY - temp;
     	          endRect->set(endX, endY, boxX + boxWidth, boxY + boxHeight);
     	     }
	*/
		//Validation : Text selection is not happend,though engine have selection region bound. 
	      WebCore::String str = getSelectedText();		
          if(NULL == str || str.isEmpty()){
             		DEBUG_NAV_UI_LOGD("%s: text Selection is not happend", __FUNCTION__);
		  }else{
			result = true;		
		  }
        }
        else
        {
            DEBUG_NAV_UI_LOGD("%s: Selection Bound Rect is Empty", __FUNCTION__);
            startRect->set(0, 0, 0, 0);
            endRect->set(0, 0, 0, 0);
        }
    }
    else{
        DEBUG_NAV_UI_LOGD("%s: recordSelectionCopiedData  is false", __FUNCTION__);
    }

    granularity = frame->selectionGranularity();
    DEBUG_NAV_UI_LOGD("%s: Granularity: %d", __FUNCTION__, granularity);

    m_contentMutex.unlock();

    DBG_SET_LOG("end");
    return result;

}


int WebViewCore::getSelectionGranularity()
{
    WebCore::Frame* frame =  m_mainFrame;
    return frame->selectionGranularity();
}

bool WebViewCore::selectClosestWord(int x , int y , float zoomLevel, bool flagGranularity){
    DEBUG_NAV_UI_LOGD("%s: x,y position: %d, %d", __FUNCTION__, x, y);
    WebCore::Frame* frame = m_mainFrame;
    WebCore::IntPoint contentsPoint = WebCore::IntPoint(x, y);
    WebCore::IntPoint wndPoint = frame->view()->contentsToWindow(contentsPoint);

    if (!frame->eventHandler())
    {
        DEBUG_NAV_UI_LOGD("%s: Eventhandler is NULL", __FUNCTION__);
        return false;
    }
    DEBUG_NAV_UI_LOGD("First time selection: Zoom Level: %f", zoomLevel);

    if (zoomLevel >= 0.8)
    {
        WebCore::MouseEventType met1 = WebCore::MouseEventMoved;
        WebCore::PlatformMouseEvent pme1(wndPoint, contentsPoint, NoButton, met1,
                        false, false, false, false, false, 0);
        return frame->eventHandler()->sendContextMenuEventForWordSelection(pme1, flagGranularity);
    }
    else
    {
        WebCore::MouseEventType met = WebCore::MouseEventPressed;
        WebCore::PlatformMouseEvent pme(wndPoint, contentsPoint, LeftButton, met, 3, false, false, false, false, 0);
        return frame->eventHandler()->handleMousePressEvent(pme);
    }
}

//SAMSUNG CHANGE END : ADVANCED_COPY_PASTE


//SAMSUNG CHANGE >>
void WebViewCore::getWebFeedLinks ( Vector<WebFeedLink*>& out )
{
    WebCore::String typeRss ( "application/rss+xml" );
    WebCore::String typeRdf ( "application/rdf+xml" );
    WebCore::String typeAtom ( "application/atom+xml" );
    WebCore::String relAlternate ( "alternate" );

    WebCore::Frame* frame = m_mainFrame ;

    LOGV ( "WebViewCore::getWebFeedLinks()" );

    while ( frame != NULL )
    {

        Document* doc = frame->document() ;

        RefPtr<WebCore::NodeList> links = doc->getElementsByTagName ( "link" ) ;
        int length = links->length() ;

        for ( int i = 0; i < length; i ++ )
        {

            WebCore::HTMLLinkElement *linkElement = static_cast<WebCore::HTMLLinkElement *> ( links->item ( i ) );
            String type = linkElement->type() ;
            String rel = linkElement->rel() ;

            if ( ( ( type == typeRss ) || ( type == typeRdf ) || ( type == typeAtom ) ) && ( rel == relAlternate ) )
            {
                String url = linkElement->href() ;
                String title = linkElement->getAttribute ( WebCore::HTMLNames::titleAttr ) ;

                LOGV ( "WebViewCore::getWebFeedLinks() type=%s, url=%s, title = %s", type.latin1().data(), url.latin1().data(), title.latin1().data() );

                out.append ( new WebFeedLink ( url, title, type) ) ;
            }

        }

        frame = frame->tree()->traverseNext() ;
    }
}

WebCore::IntRect toContainingView(const WebCore::RenderObject* renderer, const WebCore::IntRect& rendererRect)
{
    WebCore::IntRect result = rendererRect;
    WebCore::RenderView *view = renderer->view() ;
    LOGD("toContainingView: rendererRect(%d, %d, %d, %d)", result.x(), result.y(), result.width(), result.height());

    if (view && view->frameView() ) {
        WebCore::FrameView * frameView = view->frameView() ;
        if (const WebCore::ScrollView* parentScrollView = frameView->parent()) {
            if (parentScrollView->isFrameView()) {

                const FrameView* parentView = static_cast<const WebCore::FrameView*>(parentScrollView);

                // Get our renderer in the parent view
                WebCore::RenderPart* renderer = frameView->frame()->ownerRenderer();
                if (renderer) {
                    WebCore::IntPoint point(rendererRect.location());

                    // Add borders and padding
                    point.move(renderer->borderLeft() + renderer->paddingLeft(),
                        renderer->borderTop() + renderer->paddingTop());
                    WebCore::IntPoint pt = WebCore::roundedIntPoint(renderer->localToAbsolute(point, false, true /* use transforms */));
                    result.setLocation(pt);
                }

                //Let us verify the calculated location
                WebCore::IntPoint test(rendererRect.location());
                LOGD("toContainingView: test(%d, %d)", test.x(), test.y());                    
                ScrollView* view = frameView;
                //while (view) {
                    LOGD("toContainingView: frame position(%d, %d)", view->x(), view->y());                    
                    //test.move(view->x(), view->y());
                    //test = _convertToContainingWindow(frameView, test) ;
                    test = view->convertToContainingWindow(test);
                    //view = view->parent();
                //}
                IntPoint scroll ;
                while (view) {
                    scroll.move(view->scrollX(), view->scrollY());
                    view = view->parent();
                }
                test.move(scroll.x(), scroll.y()) ;

                if (test.x() > result.x() || test.y() > result.y()) {
                    LOGD("toContainingView: Inconsistant result(%d, %d, %d, %d), recalculating using frame positions...", result.x(), result.y(), result.width(), result.height());
                    result.setLocation(test);
                }
            }
            else {
                result = frameView->Widget::convertToContainingView(result);
            }
        }
    }

    LOGD("toContainingView: result(%d, %d, %d, %d)", result.x(), result.y(), result.width(), result.height());
    return result ;
}

//SAMSUNG CHANGES+
WebCore::IntRect WebViewCore::getRenderBlockBounds(WebCore::Node* node)
{
    WebCore::IntRect result;
    if (!node) {
    	DBG_NAV_LOG("getRenderBlockBounds : HitTest Result Node is NULL!");
        return result;
    }
    WebCore::String nodeName = node->nodeName() ;
    WebCore::CString nodeNameLatin1 = nodeName.latin1() ;
    DBG_NAV_LOGD("getRenderBlockBounds: node name = %s", nodeNameLatin1.data());
    

    WebCore::RenderObject *renderer = NULL ;
    WebCore::RenderObject* nodeRenderer = node->renderer();
    if (nodeRenderer != NULL) {
    	DBG_NAV_LOGD("getRenderBlockBounds: nodeRenderer = %s", nodeRenderer->renderName());

        if (nodeRenderer->isRenderPart()){
            renderer = nodeRenderer ;
        }
        else if (!nodeRenderer->isRenderBlock() && !nodeRenderer->isRenderImage()) {
            WebCore::RenderBlock *block = nodeRenderer->containingBlock() ;
            if (block) {
                renderer = block ;
                //result = block->absoluteBoundingBoxRect() ;
            }
        }
        else {
            renderer = nodeRenderer ;
            //result = nodeRenderer->absoluteBoundingBoxRect() ;
        }
    }
    else if (node->hasTagName(HTMLNames::areaTag) ){
        HTMLAreaElement *area = static_cast<HTMLAreaElement*>(node) ;

        if (area->shape() == HTMLAreaElement::Rect
            && node->parent() 
            && node->parent()->hasTagName(HTMLNames::mapTag)) {

            Node *map = node->parent() ;
            if ( map->parent()) {
                WebCore::RenderObject *r = map->parent()->renderer() ;
                if (r->isRenderBlock()) {
                    IntRect parentRect = r->absoluteBoundingBoxRect() ;
                    result = area->rect() ;
                    result.move(parentRect.x(), parentRect.y()) ;

                    if (r->view() && r->view()->frameView()) 
                        result = toContainingView(r, result) ;
                        //result = static_cast<WebCore::Widget*>(r->view()->frameView())->convertToContainingView(result) ;
                }
            }
        }
    }    
    
    if (renderer) {
        result = renderer->absoluteBoundingBoxRect() ;
        result = toContainingView(renderer, result) ;
    }    
    
    if ( renderer == NULL)
    	DBG_NAV_LOG("getRenderBlockBounds: No render block found!");
    else
    	DBG_NAV_LOGD("getRenderBlockBounds: node=%p result(%d, %d, %d, %d)", node, result.x(), result.y(), result.width(), result.height());
        
    return result;

}

WebCore::IntRect WebViewCore::getRenderBlockBounds(const WebCore::IntPoint &pt)
{
    WebCore::IntRect result;
    LOGD("getRenderBlockBounds: point=(%d, %d)", pt.x(), pt.y() );

    WebCore::HitTestResult hitTestResult = m_mainFrame->eventHandler()->hitTestResultAtPoint(pt, false, true);

    //dumpHitTestResult(hitTestResult) ;

    WebCore::Node* node = hitTestResult.innerNode();
    if (!node) {
    	DBG_NAV_LOG("getRenderBlockBounds : HitTest Result Node is NULL!");
        return result;
    }else{
    	return getRenderBlockBounds(node);
    }
}
//SAMSUNG CHANGES-

static jobject nativeGetBlockBounds(JNIEnv *env, jobject obj, jint x, jint y, jfloat scale)
{
#ifdef ANDROID_INSTRUMENT
   TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in nativeSetSize");

    WebCore::IntPoint pt(x,y) ;
    WebCore::IntRect webRect = viewImpl->getRenderBlockBounds(pt);

    jclass rectClass = env->FindClass("android/graphics/Rect");
    LOG_ASSERT(rectClass, "Could not find Rect class!");

    jmethodID init = env->GetMethodID(rectClass, "<init>", "(IIII)V");
    LOG_ASSERT(init, "Could not find constructor for Rect");

    jobject rect = env->NewObject(rectClass, init, webRect.x(),
                        webRect.y(), webRect.right(), webRect.bottom());
    return rect ;
}

//SAMSUNG CHANGE <<

//----------------------------------------------------------------------
// Native JNI methods
//----------------------------------------------------------------------
static jstring WebCoreStringToJString(JNIEnv *env, WebCore::String string)
{
    int length = string.length();
    if (!length)
        return 0;
    jstring ret = env->NewString((jchar *)string.characters(), length);
    env->DeleteLocalRef(ret);
    return ret;
}

static jstring RequestLabel(JNIEnv *env, jobject obj, int framePointer,
        int nodePointer)
{
    return WebCoreStringToJString(env, GET_NATIVE_VIEW(env, obj)->requestLabel(
            (WebCore::Frame*) framePointer, (WebCore::Node*) nodePointer));
}

static void UpdateFrameCacheIfLoading(JNIEnv *env, jobject obj)
{
    GET_NATIVE_VIEW(env, obj)->updateFrameCacheIfLoading();
}

static void SetSize(JNIEnv *env, jobject obj, jint width, jint height,
        jint screenWidth, jfloat scale, jint realScreenWidth, jint screenHeight,
        jint anchorX, jint anchorY, jboolean ignoreHeight,
//SAMSUNG CHANGES+
		jboolean needAnchorDiff, jint viewportLeft, jint viewportTop)
//SAMSUNG CHANGES-
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOGV("webviewcore::nativeSetSize(%u %u)\n viewImpl: %p", (unsigned)width, (unsigned)height, viewImpl);
    LOG_ASSERT(viewImpl, "viewImpl not set in nativeSetSize");
    viewImpl->setSizeScreenWidthAndScale(width, height, screenWidth, scale,
        realScreenWidth, screenHeight, anchorX, anchorY, ignoreHeight,
//SAMSUNG CHANGES+
		needAnchorDiff, viewportLeft, viewportTop);
//SAMSUNG CHANGES-
}

static void UpdatePlugins(JNIEnv *env, jobject obj, int mode, int viewleft, int viewtop, int viewright, int viewbottom, float scale, int anchorX, int anchorY )
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    viewImpl->updatePlugins(  mode, viewleft, viewtop, viewright, viewbottom, scale, anchorX, anchorY );
}
static void SetScrollOffset(JNIEnv *env, jobject obj, jint gen, jint x, jint y)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "need viewImpl");

    viewImpl->setScrollOffset(gen, x, y);
}

static void SetGlobalBounds(JNIEnv *env, jobject obj, jint x, jint y, jint h,
                            jint v)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "need viewImpl");

    viewImpl->setGlobalBounds(x, y, h, v);
}

static jboolean Key(JNIEnv *env, jobject obj, jint keyCode, jint unichar,
        jint repeatCount, jboolean isShift, jboolean isAlt, jboolean isSym,
        jboolean isDown)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    return GET_NATIVE_VIEW(env, obj)->key(PlatformKeyboardEvent(keyCode,
        unichar, repeatCount, isDown, isShift, isAlt, isSym));
}

static void Click(JNIEnv *env, jobject obj, int framePtr, int nodePtr)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in Click");

    viewImpl->click(reinterpret_cast<WebCore::Frame*>(framePtr),
        reinterpret_cast<WebCore::Node*>(nodePtr));
}

//SAMSUNG FIX >>
static void SelectAndClick(JNIEnv *env, jobject obj, int start, int end, int framePtr, int nodePtr)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in Click");

    viewImpl->click(reinterpret_cast<WebCore::Frame*>(framePtr),
        reinterpret_cast<WebCore::Node*>(nodePtr));
    viewImpl->setSelection(start, end);
    viewImpl->updateTextSelection();
}
//SAMSUNG FIX <<

static void DeleteSelection(JNIEnv *env, jobject obj, jint start, jint end,
        jint textGeneration)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    viewImpl->deleteSelection(start, end, textGeneration);
}

static void SetSelection(JNIEnv *env, jobject obj, jint start, jint end)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    viewImpl->setSelection(start, end);
}


static void ReplaceTextfieldText(JNIEnv *env, jobject obj,
    jint oldStart, jint oldEnd, jstring replace, jint start, jint end,
    jint textGeneration)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    WebCore::String webcoreString = to_string(env, replace);
    viewImpl->replaceTextfieldText(oldStart,
            oldEnd, webcoreString, start, end, textGeneration);
}

static void PassToJs(JNIEnv *env, jobject obj,
    jint generation, jstring currentText, jint keyCode,
    jint keyValue, jboolean down, jboolean cap, jboolean fn, jboolean sym)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebCore::String current = to_string(env, currentText);
    GET_NATIVE_VIEW(env, obj)->passToJs(generation, current,
        PlatformKeyboardEvent(keyCode, keyValue, 0, down, cap, fn, sym));
}

static void ScrollFocusedTextInput(JNIEnv *env, jobject obj, jfloat xPercent,
    jint y)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    viewImpl->scrollFocusedTextInput(xPercent, y);
}

static void SetFocusControllerActive(JNIEnv *env, jobject obj, jboolean active)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    LOGV("webviewcore::nativeSetFocusControllerActive()\n");
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in nativeSetFocusControllerActive");
    viewImpl->setFocusControllerActive(0, active);
}

static void SaveDocumentState(JNIEnv *env, jobject obj, jint frame)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    LOGV("webviewcore::nativeSaveDocumentState()\n");
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in nativeSaveDocumentState");
    viewImpl->saveDocumentState((WebCore::Frame*) frame);
}

void WebViewCore::addVisitedLink(const UChar* string, int length)
{
    if (m_groupForVisitedLinks)
        m_groupForVisitedLinks->addVisitedLink(string, length);
}

static bool RecordContent(JNIEnv *env, jobject obj, jobject region, jobject pt)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    SkRegion* nativeRegion = GraphicsJNI::getNativeRegion(env, region);
    SkIPoint nativePt;
    bool result = viewImpl->recordContent(nativeRegion, &nativePt);
    GraphicsJNI::ipoint_to_jpoint(nativePt, env, pt);
    return result;
}

static void SplitContent(JNIEnv *env, jobject obj)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    viewImpl->splitContent();
}

static void SendListBoxChoice(JNIEnv* env, jobject obj, jint choice)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in nativeSendListBoxChoice");
    viewImpl->popupReply(choice);
}

// Set aside a predetermined amount of space in which to place the listbox
// choices, to avoid unnecessary allocations.
// The size here is arbitrary.  We want the size to be at least as great as the
// number of items in the average multiple-select listbox.
#define PREPARED_LISTBOX_STORAGE 10

static void SendListBoxChoices(JNIEnv* env, jobject obj, jbooleanArray jArray,
        jint size)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in nativeSendListBoxChoices");
    jboolean* ptrArray = env->GetBooleanArrayElements(jArray, 0);
    SkAutoSTMalloc<PREPARED_LISTBOX_STORAGE, int> storage(size);
    int* array = storage.get();
    int count = 0;
    for (int i = 0; i < size; i++) {
        if (ptrArray[i]) {
            array[count++] = i;
        }
    }
    env->ReleaseBooleanArrayElements(jArray, ptrArray, JNI_ABORT);
    viewImpl->popupReply(array, count);
}

static jstring FindAddress(JNIEnv *env, jobject obj, jstring addr,
    jboolean caseInsensitive)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    if (!addr)
        return 0;
    int length = env->GetStringLength(addr);
    if (!length)
        return 0;
    const jchar* addrChars = env->GetStringChars(addr, 0);
    int start, end;
    bool success = CacheBuilder::FindAddress(addrChars, length,
        &start, &end, caseInsensitive) == CacheBuilder::FOUND_COMPLETE;
    jstring ret = 0;
    if (success) {
        ret = env->NewString((jchar*) addrChars + start, end - start);
        env->DeleteLocalRef(ret);
    }
    env->ReleaseStringChars(addr, addrChars);
    return ret;
}

static jboolean HandleTouchEvent(JNIEnv *env, jobject obj, jint action, jint x, jint y, jint metaState)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in %s", __FUNCTION__);
    return viewImpl->handleTouchEvent(action, x, y, metaState);
}

static void TouchUp(JNIEnv *env, jobject obj, jint touchGeneration,
        jint frame, jint node, jint x, jint y)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in %s", __FUNCTION__);
    viewImpl->touchUp(touchGeneration,
        (WebCore::Frame*) frame, (WebCore::Node*) node, x, y);
}

static jstring RetrieveHref(JNIEnv *env, jobject obj, jint frame,
        jint node)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in %s", __FUNCTION__);
    WebCore::String result = viewImpl->retrieveHref((WebCore::Frame*) frame,
            (WebCore::Node*) node);
    if (!result.isEmpty())
        return WebCoreStringToJString(env, result);
    return 0;
}

static jstring RetrieveAnchorText(JNIEnv *env, jobject obj, jint frame,
        jint node)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in %s", __FUNCTION__);
    WebCore::String result = viewImpl->retrieveAnchorText((WebCore::Frame*) frame,
            (WebCore::Node*) node);
    if (!result.isEmpty())
        return WebCoreStringToJString(env, result);
    return 0;
}


static void MoveFocus(JNIEnv *env, jobject obj, jint framePtr, jint nodePtr)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in %s", __FUNCTION__);
    viewImpl->moveFocus((WebCore::Frame*) framePtr, (WebCore::Node*) nodePtr);
}

static void MoveMouse(JNIEnv *env, jobject obj, jint frame,
        jint x, jint y)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in %s", __FUNCTION__);
    viewImpl->moveMouse((WebCore::Frame*) frame, x, y);
}

//SAMSUNG CHANGE >>
static void MouseClick(JNIEnv *env, jobject obj, jint frame, jint node)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in %s", __FUNCTION__);
    viewImpl->click((WebCore::Frame*) frame, (WebCore::Node*) node);
}
//SAMSUNG CHANGE <<

static void MoveMouseIfLatest(JNIEnv *env, jobject obj, jint moveGeneration,
        jint frame, jint x, jint y)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in %s", __FUNCTION__);
    viewImpl->moveMouseIfLatest(moveGeneration,
        (WebCore::Frame*) frame, x, y);
}

static void UpdateFrameCache(JNIEnv *env, jobject obj)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in %s", __FUNCTION__);
    viewImpl->updateFrameCache();
}

static jint GetContentMinPrefWidth(JNIEnv *env, jobject obj)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in %s", __FUNCTION__);

    WebCore::Frame* frame = viewImpl->mainFrame();
    if (frame) {
        WebCore::Document* document = frame->document();
        if (document) {
            WebCore::RenderObject* renderer = document->renderer();
            if (renderer && renderer->isRenderView()) {
                return renderer->minPrefWidth();
            }
        }
    }
    return 0;
}

static void SetViewportSettingsFromNative(JNIEnv *env, jobject obj)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in %s", __FUNCTION__);

    WebCore::Settings* s = viewImpl->mainFrame()->page()->settings();
    if (!s)
        return;

#ifdef ANDROID_META_SUPPORT
    env->SetIntField(obj, gWebViewCoreFields.m_viewportWidth, s->viewportWidth());
    env->SetIntField(obj, gWebViewCoreFields.m_viewportHeight, s->viewportHeight());
    env->SetIntField(obj, gWebViewCoreFields.m_viewportInitialScale, s->viewportInitialScale());
    env->SetIntField(obj, gWebViewCoreFields.m_viewportMinimumScale, s->viewportMinimumScale());
    env->SetIntField(obj, gWebViewCoreFields.m_viewportMaximumScale, s->viewportMaximumScale());
    env->SetBooleanField(obj, gWebViewCoreFields.m_viewportUserScalable, s->viewportUserScalable());
    env->SetIntField(obj, gWebViewCoreFields.m_viewportDensityDpi, s->viewportTargetDensityDpi());
#endif
}

static void SetBackgroundColor(JNIEnv *env, jobject obj, jint color)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in %s", __FUNCTION__);

    viewImpl->setBackgroundColor((SkColor) color);
}


//SAMSUNG CHANGE BEGIN : ADVANCED_COPY_PASTE
static jstring GetSelectedText(JNIEnv *env, jobject obj)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in %s", __FUNCTION__);
    WebCore::String result = viewImpl->getSelectedText();
    if (!result.isEmpty())
        return WebCoreStringToJString(env, result);
    return 0;
}
//SAMSUNG CHANGE END : ADVANCED_COPY_PASTE
////////////////////SISO_HTMLCOMPOSER begin
static void SimulateDelKeyForCount(JNIEnv *env, jobject obj, jint count)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    viewImpl->simulateDelKeyForCount(count);	
}

static jstring GetTextAroundCursor(JNIEnv *env, jobject obj, jint count , jboolean isBefore)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    WebCore::String result = viewImpl->getTextAroundCursor(count, isBefore);
    if (!result.isEmpty())
        return WebCoreStringToJString(env, result);
    return 0;		
}


static void DeleteSurroundingText(JNIEnv *env, jobject obj, jint left , jint right)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    viewImpl->deleteSurroundingText(left , right);	
}


static jobject GetSelectionOffset(JNIEnv *env, jobject obj)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    int startOffset = -1;
    int endOffset = -1;	 	
    viewImpl->getSelectionOffset(startOffset , endOffset);
    
    jclass selectionOffset_clazz = 0;
    jmethodID selectionOffset_initID = 0;
       
    selectionOffset_clazz = env->FindClass ( "android/graphics/Point" );
    selectionOffset_initID = env->GetMethodID ( selectionOffset_clazz, "<init>", "(II)V" );
    jobject jselectionOffset_Object ;
    jselectionOffset_Object = env->NewObject ( selectionOffset_clazz, selectionOffset_initID , startOffset , endOffset);
    env->DeleteLocalRef(selectionOffset_clazz);
    return jselectionOffset_Object;	
}

static jstring GetBodyText(JNIEnv *env, jobject obj)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    WebCore::String result = viewImpl->getBodyText();
    if (!result.isEmpty())
        return WebCoreStringToJString(env, result);
    return 0;		
}

static jstring GetBodyHTML(JNIEnv *env, jobject obj)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    WebCore::String result = viewImpl->getBodyHTML();
    if (!result.isEmpty())
        return WebCoreStringToJString(env, result);
    return 0;		
}



static bool ExecCommand(JNIEnv *env, jobject obj, jstring command , jstring value)
{
    WebCore::String commandString = to_string(env, command);
    WebCore::String valueString = to_string(env, value);
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    return viewImpl->execCommand(commandString , valueString);	
    
}

static bool CanUndo(JNIEnv *env, jobject obj)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    return viewImpl->canUndo();	
}

static bool CanRedo(JNIEnv *env, jobject obj)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    return viewImpl->canRedo();	
}

static void UndoRedoStateReset(JNIEnv *env, jobject obj)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    viewImpl->undoRedoStateReset();	
}


static bool CopyAndSaveImage(JNIEnv *env, jobject obj, jstring url)
{
    WebCore::String urlString = to_string(env, url);
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    return viewImpl->copyAndSaveImage(urlString);	
    
}

// SAMSUNG CHANGE >>
static bool SaveCachedImageToFile(JNIEnv *env, jobject obj, jstring url, jstring filePath)
{
    WebCore::String strUrl = to_string(env, url);
    WebCore::String strPath = to_string(env, filePath);

    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    return viewImpl->saveCachedImageToFile( strUrl, strPath );
}
// SAMSUNG CHANGE <<

static jobject GetFullMarkupData(JNIEnv* env, jobject obj )
{
    jclass markupData_clazz = 0;
    jmethodID markupData_initID = 0;
    jobject jmarkup_Object ;
	
    jclass subPart_clazz = 0;
    jmethodID subPart_initID = 0;
	
    jclass vector_clazz = 0;
    jmethodID vector_initID = 0;
    jobject vector_Object;

    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    WebHTMLMarkupData* markupData = viewImpl->getFullMarkupData();

    //ANDROID_LOG_PRINT(ANDROID_LOG_DEBUG, "HTML_EDIT", " WebViewCore::nativeGetFullMarkupData() markupData = %p", markupData);

    //Create MarkupData Object
    markupData_clazz = env->FindClass ( "android/webkit/WebHTMLMarkupData" );
    markupData_initID = env->GetMethodID ( markupData_clazz, "<init>", "()V" );
    jmarkup_Object = env->NewObject ( markupData_clazz, markupData_initID);
    LOGV ( "WebViewCore::nativeGetFullMarkupData() MarkupData Object Created ");

    //Create Vector Class
    vector_clazz = env->FindClass ( "java/util/Vector" );
    vector_initID = env->GetMethodID ( vector_clazz, "<init>", "()V" );
    vector_Object = env->NewObject ( vector_clazz, vector_initID);
    LOGV ( "WebViewCore::nativeGetFullMarkupData() Vector Object Created ");

    //Find Class and Method ID for SubPart
    subPart_clazz = env->FindClass ( "android/webkit/WebSubPart" );
    subPart_initID = env->GetMethodID ( subPart_clazz, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;J)V" ); //String,String,String,String,String,long
	
     if(markupData != 0){

        //Fill the Vector Object with the SupPart Data
        Vector<WebSubPart> supPartList = markupData->subPartList();
        for(int i =0;i<supPartList.size();i++){
            jobject subPart_Object;
            WebSubPart subPart = supPartList[i];

            //Create SubPart Object
            jstring j_cid = env->NewString( subPart.cid().characters(), subPart.cid().length());
            jstring j_file =  env->NewString( subPart.fileName().characters(), subPart.fileName().length());
            jstring j_mime = env->NewString( subPart.mimeType().characters(), subPart.mimeType().length());
            jstring j_path = env->NewString( subPart.path().characters(), subPart.path().length());
            jstring j_uri = env->NewString( subPart.contentUri().characters(), subPart.contentUri().length());

            subPart_Object = env->NewObject ( subPart_clazz, subPart_initID, j_cid, j_file, j_mime, j_path, j_uri, subPart.fileSize());

            //Add SubPart Object to the Vector,  Adds the specified component to the end of the vector, increasing its size by one.
            env->CallVoidMethod(vector_Object, env->GetMethodID(vector_clazz, "addElement", "(Ljava/lang/Object;)V"), subPart_Object);

            env->DeleteLocalRef(j_cid);
            env->DeleteLocalRef(j_file);
            env->DeleteLocalRef(j_mime);
            env->DeleteLocalRef(j_path);
            env->DeleteLocalRef(j_uri);
            env->DeleteLocalRef(subPart_Object);
        }

        //Set the Vector Object to the WebHTMLMarkupData
        env->CallVoidMethod(jmarkup_Object, env->GetMethodID(markupData_clazz, "setSubPartList", "(Ljava/util/Vector;)V"), vector_Object);
        env->DeleteLocalRef(vector_Object);

        //Set the HTMLFragment to the WebHTMLMarkupData 
        jstring j_html = env->NewString(  markupData->htmlFragment().characters(), markupData->htmlFragment().length());
        env->CallVoidMethod(jmarkup_Object, env->GetMethodID(markupData_clazz, "setHTMLFragment", "(Ljava/lang/String;)V"), j_html);
        env->DeleteLocalRef(j_html);

        jstring j_plain= env->NewString(  markupData->plainText().characters(), markupData->plainText().length());
        env->CallVoidMethod(jmarkup_Object, env->GetMethodID(markupData_clazz, "setPlainText", "(Ljava/lang/String;)V"), j_plain);        
        env->DeleteLocalRef(j_plain);

        env->DeleteLocalRef(markupData_clazz);
        env->DeleteLocalRef(vector_clazz);
        env->DeleteLocalRef(subPart_clazz);

        //Currently CharSet is ignored, will be added after confirmation
        delete markupData;
	}

    return jmarkup_Object;

}

static jobject GetCursorRect(JNIEnv *env, jobject obj , jboolean giveContentRect)
{
#ifdef ANDROID_INSTRUMENT
   TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in nativeSetSize");

    WebCore::IntRect webRect = viewImpl->getCursorRect(giveContentRect);

    jclass rectClass = env->FindClass("android/graphics/Rect");
    LOG_ASSERT(rectClass, "Could not find Rect class!");

    jmethodID init = env->GetMethodID(rectClass, "<init>", "(IIII)V");
    LOG_ASSERT(init, "Could not find constructor for Rect");

    jobject rect = env->NewObject(rectClass, init, webRect.x(),
                        webRect.y(), webRect.right(), webRect.bottom());
    return rect ;
}


static void SetSelectionNone(JNIEnv *env, jobject obj)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    viewImpl->setSelectionNone();	
}

static void SetEditable(JNIEnv *env, jobject obj , jboolean isEditable)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    viewImpl->setEditable(isEditable);	
}

static void SetSelectionEditable(JNIEnv *env, jobject obj, jint start , jint end)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    viewImpl->setSelectionEditable(start , end);	
}


static int CheckSelectionAtBoundry(JNIEnv *env, jobject obj)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    return viewImpl->checkSelectionAtBoundry();	
}

/////////////////////SISO_HTMLCOMPOSER end

static void DumpDomTree(JNIEnv *env, jobject obj, jboolean useFile)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in %s", __FUNCTION__);

    viewImpl->dumpDomTree(useFile);
}

static void DumpRenderTree(JNIEnv *env, jobject obj, jboolean useFile)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in %s", __FUNCTION__);

    viewImpl->dumpRenderTree(useFile);
}

static void DumpNavTree(JNIEnv *env, jobject obj)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in %s", __FUNCTION__);

    viewImpl->dumpNavTree();
}

static void DumpV8Counters(JNIEnv*, jobject)
{
#if USE(V8)
#ifdef ANDROID_INSTRUMENT
    V8Counters::dumpCounters();
#endif
#endif
}

static void SetJsFlags(JNIEnv *env, jobject obj, jstring flags)
{
#if USE(V8)
    WebCore::String flagsString = to_string(env, flags);
    WebCore::CString utf8String = flagsString.utf8();
    WebCore::ScriptController::setFlags(utf8String.data(), utf8String.length());
#endif
}


// Called from the Java side to set a new quota for the origin or new appcache
// max size in response to a notification that the original quota was exceeded or
// that the appcache has reached its maximum size.
static void SetNewStorageLimit(JNIEnv* env, jobject obj, jlong quota) {
#if ENABLE(DATABASE) || ENABLE(OFFLINE_WEB_APPLICATIONS)
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    Frame* frame = viewImpl->mainFrame();

    // The main thread is blocked awaiting this response, so now we can wake it
    // up.
    ChromeClientAndroid* chromeC = static_cast<ChromeClientAndroid*>(frame->page()->chrome()->client());
    chromeC->wakeUpMainThreadWithNewQuota(quota);
#endif
}

// Called from Java to provide a Geolocation permission state for the specified origin.
static void GeolocationPermissionsProvide(JNIEnv* env, jobject obj, jstring origin, jboolean allow, jboolean remember) {
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    Frame* frame = viewImpl->mainFrame();

    ChromeClientAndroid* chromeClient = static_cast<ChromeClientAndroid*>(frame->page()->chrome()->client());
    chromeClient->provideGeolocationPermissions(to_string(env, origin), allow, remember);
}

static void RegisterURLSchemeAsLocal(JNIEnv* env, jobject obj, jstring scheme) {
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebCore::SecurityOrigin::registerURLSchemeAsLocal(to_string(env, scheme));
}

static void ClearContent(JNIEnv *env, jobject obj)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    viewImpl->clearContent();
}

static void CopyContentToPicture(JNIEnv *env, jobject obj, jobject pict)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    if (!viewImpl)
        return;
    SkPicture* picture = GraphicsJNI::getNativePicture(env, pict);
    viewImpl->copyContentToPicture(picture);
}

static bool DrawContent(JNIEnv *env, jobject obj, jobject canv, jint color)
{
    // Note: this is called from UI thread, don't count it for WebViewCoreTimeCounter
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    SkCanvas* canvas = GraphicsJNI::getNativeCanvas(env, canv);
    return viewImpl->drawContent(canvas, color);
}

static bool FocusBoundsChanged(JNIEnv* env, jobject obj)
{
    return GET_NATIVE_VIEW(env, obj)->focusBoundsChanged();
}

static bool PictureReady(JNIEnv* env, jobject obj)
{
    return GET_NATIVE_VIEW(env, obj)->pictureReady();
}

static void Pause(JNIEnv* env, jobject obj)
{
    // This is called for the foreground tab when the browser is put to the
    // background (and also for any tab when it is put to the background of the
    // browser). The browser can only be killed by the system when it is in the
    // background, so saving the Geolocation permission state now ensures that
    // is maintained when the browser is killed.
    ChromeClient* chromeClient = GET_NATIVE_VIEW(env, obj)->mainFrame()->page()->chrome()->client();
    ChromeClientAndroid* chromeClientAndroid = static_cast<ChromeClientAndroid*>(chromeClient);
    chromeClientAndroid->storeGeolocationPermissions();

    Frame* mainFrame = GET_NATIVE_VIEW(env, obj)->mainFrame();
    for (Frame* frame = mainFrame; frame; frame = frame->tree()->traverseNext()) {
        Geolocation* geolocation = frame->domWindow()->navigator()->optionalGeolocation();
        if (geolocation)
            geolocation->suspend();
    }

    ANPEvent event;
    SkANP::InitEvent(&event, kLifecycle_ANPEventType);
    event.data.lifecycle.action = kPause_ANPLifecycleAction;
    GET_NATIVE_VIEW(env, obj)->sendPluginEvent(event);

    GET_NATIVE_VIEW(env, obj)->setIsPaused(true);
}

static void Resume(JNIEnv* env, jobject obj)
{
    Frame* mainFrame = GET_NATIVE_VIEW(env, obj)->mainFrame();
    for (Frame* frame = mainFrame; frame; frame = frame->tree()->traverseNext()) {
        Geolocation* geolocation = frame->domWindow()->navigator()->optionalGeolocation();
        if (geolocation)
            geolocation->resume();
    }

    ANPEvent event;
    SkANP::InitEvent(&event, kLifecycle_ANPEventType);
    event.data.lifecycle.action = kResume_ANPLifecycleAction;
    GET_NATIVE_VIEW(env, obj)->sendPluginEvent(event);

    GET_NATIVE_VIEW(env, obj)->setIsPaused(false);
}

static void FreeMemory(JNIEnv* env, jobject obj)
{
    ANPEvent event;
    SkANP::InitEvent(&event, kLifecycle_ANPEventType);
    event.data.lifecycle.action = kFreeMemory_ANPLifecycleAction;
    GET_NATIVE_VIEW(env, obj)->sendPluginEvent(event);
}

static void ProvideVisitedHistory(JNIEnv *env, jobject obj, jobject hist)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in %s", __FUNCTION__);

    jobjectArray array = static_cast<jobjectArray>(hist);

    jsize len = env->GetArrayLength(array);
    for (jsize i = 0; i < len; i++) {
        jstring item = static_cast<jstring>(env->GetObjectArrayElement(array, i));
        const UChar* str = static_cast<const UChar*>(env->GetStringChars(item, NULL));
        jsize len = env->GetStringLength(item);
        viewImpl->addVisitedLink(str, len);
        env->ReleaseStringChars(item, str);
        env->DeleteLocalRef(item);
    }
}

//SanJose++
static void SetPluginRefreshDelay(JNIEnv* env, jobject obj, jint ms)
{
    ANPSurfaceInterfaceSetDelay(ms);
}
//SanJose--

// Notification from the UI thread that the plugin's full-screen surface has been discarded
static void FullScreenPluginHidden(JNIEnv* env, jobject obj, jint npp)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    PluginWidgetAndroid* plugin = viewImpl->getPluginWidget((NPP)npp);
    if (plugin)
        plugin->exitFullScreen(false);
}

static WebCore::IntRect jrect_to_webrect(JNIEnv* env, jobject obj)
{
    int L, T, R, B;
    GraphicsJNI::get_jrect(env, obj, &L, &T, &R, &B);
    return WebCore::IntRect(L, T, R - L, B - T);
}

static bool ValidNodeAndBounds(JNIEnv *env, jobject obj, int frame, int node,
    jobject rect)
{
    IntRect nativeRect = jrect_to_webrect(env, rect);
    return GET_NATIVE_VIEW(env, obj)->validNodeAndBounds(
            reinterpret_cast<Frame*>(frame),
            reinterpret_cast<Node*>(node), nativeRect);
}

//SAMSUNG CHANGE BEGIN : ADVANCED_COPY_PASTE

static void WebTextSelectionAll(JNIEnv *env, jobject obj, int x1, int y1, int x2, int y2)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "view not set in %s", __FUNCTION__);
    viewImpl->webTextSelectionAll(x1, y1, x2, y2);
}

static void CopyMoveSelection(JNIEnv *env, jobject obj, int x, int y, int controller, 
										    bool ex, bool selectionMove, float zoomLevel, int granularity)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "view not set in %s", __FUNCTION__);
    viewImpl->copyMoveSelection(x, y, controller, ex, selectionMove, zoomLevel, granularity);
}

static void ClearTextSelection(JNIEnv *env, jobject obj, int contentX, int contentY)
{
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "view not set in %s", __FUNCTION__);
    viewImpl->clearTextSelection(contentX, contentY);
}
static bool RecordSelectionCopiedData(JNIEnv *env, jobject obj, jobject region, jobject sRect,jobject eRect, jint value)
{
#ifdef ANDROID_INSTRUMENT
    TimeCounterAuto counter(TimeCounter::WebViewCoreTimeCounter);
#endif
    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
    LOG_ASSERT(viewImpl, "viewImpl not set in RecordSelectionCopiedData");

    SkRegion* nativeRegion = GraphicsJNI::getNativeRegion(env, region);
    SkIRect nativeSRect;
    SkIRect nativeERect;

    bool result = viewImpl->recordSelectionCopiedData(nativeRegion, &nativeSRect,&nativeERect,value);
    GraphicsJNI::set_jrect(env,sRect,nativeSRect.fLeft, nativeSRect.fTop,nativeSRect.fRight, nativeSRect.fBottom );
    GraphicsJNI::set_jrect(env,eRect,nativeERect.fLeft, nativeERect.fTop,nativeERect.fRight, nativeERect.fBottom );


    //GraphicsJNI::iRect_to_jRect(&nativeSRect, env,sRect);
    //GraphicsJNI::iRect_to_jRect(&nativeERect, env,eRect);
    return result;
}

static jint GetSelectionGranularity(JNIEnv *env, jobject obj)
{
	WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
	LOG_ASSERT(viewImpl, "viewImpl not set in GetSelectionGranularity");
	return viewImpl->getSelectionGranularity();
}

static bool SelectClosestWord(JNIEnv *env, jobject obj,int x, int y, float zoomLevel, bool flag)
{
	WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);
	LOG_ASSERT(viewImpl, "viewImpl not set in SelectClosestWord");
	return viewImpl->selectClosestWord(x,y, zoomLevel, flag);

}


//SAMSUNG CHANGE END : ADVANCED_COPY_PASTE

//SAMSUNG CHANGE >>
static jobjectArray nativeGetWebFeedLinks ( JNIEnv* env, jobject obj )
{
    jclass fi_clazz = 0;
    jmethodID initID = 0;
    jobjectArray infos ;
    int start = 0;
    int limit = 0;
    jobject urlobj,titleobj, typeobj;//, pathobj ;
    Vector<WebFeedLink*> feedInfoList ;

    WebViewCore* viewImpl = GET_NATIVE_VIEW(env, obj);

    viewImpl->getWebFeedLinks ( feedInfoList ) ;

    LOGV ( "WebViewCore::nativeGetWebFeedLinks() links count = %d", feedInfoList.size() );

    fi_clazz = env->FindClass ( "android/webkit/WebFeedLink" );
    initID = env->GetMethodID ( fi_clazz, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V" );

    infos = env->NewObjectArray ( feedInfoList.size(), fi_clazz, NULL );

    for ( int i = 0; i <  feedInfoList.size(); i++ )
    {
        urlobj =env->NewString ( feedInfoList[i]->url().characters(), feedInfoList[i]->url().length() );
        titleobj = env->NewString ( feedInfoList[i]->title().characters(), feedInfoList[i]->title().length() );
        typeobj = env->NewString ( feedInfoList[i]->type().characters(), feedInfoList[i]->type().length() );
        //pathobj = env->NewString(feedInfoList[i]->path().characters(), feedInfoList[i]->path().length());

        jobject fi = env->NewObject ( fi_clazz, initID, urlobj, titleobj, typeobj );

        env->SetObjectArrayElement ( infos, i, fi );

        delete feedInfoList[i] ;

        start = limit;
    }

    feedInfoList.clear() ;

    return infos ;
}
//SAMSUNG CHANGE <<

// ----------------------------------------------------------------------------

/*
 * JNI registration.
 */
static JNINativeMethod gJavaWebViewCoreMethods[] = {
    { "nativeClearContent", "()V",
        (void*) ClearContent },
    { "nativeCopyContentToPicture", "(Landroid/graphics/Picture;)V",
        (void*) CopyContentToPicture },
    { "nativeDrawContent", "(Landroid/graphics/Canvas;I)Z",
        (void*) DrawContent } ,
    { "nativeFocusBoundsChanged", "()Z",
        (void*) FocusBoundsChanged } ,
    { "nativeKey", "(IIIZZZZ)Z",
        (void*) Key },
    { "nativeClick", "(II)V",
        (void*) Click },
    { "nativePictureReady", "()Z",
        (void*) PictureReady } ,
    { "nativeSendListBoxChoices", "([ZI)V",
        (void*) SendListBoxChoices },
    { "nativeSendListBoxChoice", "(I)V",
        (void*) SendListBoxChoice },
//SAMSUNG CHANGES+
	{ "nativeSetSize", "(IIIFIIIIZZII)V",
		(void*) SetSize },
    { "nativeUpdatePlugins", "(IIIIIFII)V",
        (void*) UpdatePlugins }, 		
//SAMSUNG CHANGES-
    { "nativeSetScrollOffset", "(III)V",
        (void*) SetScrollOffset },
    { "nativeSetGlobalBounds", "(IIII)V",
        (void*) SetGlobalBounds },
    { "nativeSetSelection", "(II)V",
        (void*) SetSelection } ,
    { "nativeDeleteSelection", "(III)V",
        (void*) DeleteSelection } ,
    { "nativeReplaceTextfieldText", "(IILjava/lang/String;III)V",
        (void*) ReplaceTextfieldText } ,
    { "nativeMoveFocus", "(II)V",
        (void*) MoveFocus },
    { "nativeMoveMouse", "(III)V",
        (void*) MoveMouse },
    { "nativeMoveMouseIfLatest", "(IIII)V",
        (void*) MoveMouseIfLatest },
    { "passToJs", "(ILjava/lang/String;IIZZZZ)V",
        (void*) PassToJs },
    { "nativeScrollFocusedTextInput", "(FI)V",
        (void*) ScrollFocusedTextInput },
    { "nativeSetFocusControllerActive", "(Z)V",
        (void*) SetFocusControllerActive },
    { "nativeSaveDocumentState", "(I)V",
        (void*) SaveDocumentState },
    { "nativeFindAddress", "(Ljava/lang/String;Z)Ljava/lang/String;",
        (void*) FindAddress },
    { "nativeHandleTouchEvent", "(IIII)Z",
            (void*) HandleTouchEvent },
    { "nativeTouchUp", "(IIIII)V",
        (void*) TouchUp },
    { "nativeRetrieveHref", "(II)Ljava/lang/String;",
        (void*) RetrieveHref },
    { "nativeRetrieveAnchorText", "(II)Ljava/lang/String;",
        (void*) RetrieveAnchorText },
    { "nativeUpdateFrameCache", "()V",
        (void*) UpdateFrameCache },
    { "nativeGetContentMinPrefWidth", "()I",
        (void*) GetContentMinPrefWidth },
    { "nativeRecordContent", "(Landroid/graphics/Region;Landroid/graphics/Point;)Z",
        (void*) RecordContent },
    { "setViewportSettingsFromNative", "()V",
        (void*) SetViewportSettingsFromNative },
    { "nativeSplitContent", "()V",
        (void*) SplitContent },
    { "nativeSetBackgroundColor", "(I)V",
        (void*) SetBackgroundColor },
    { "nativeGetSelectedText", "()Ljava/lang/String;",
        (void*) GetSelectedText },
/////////////////////SISO_HTMLCOMPOSER begin
	{ "nativeSimulateDelKeyForCount", "(I)V",
	 (void*) SimulateDelKeyForCount },

	{ "nativeGetTextAroundCursor", "(IZ)Ljava/lang/String;",
	 (void*) GetTextAroundCursor },

 	{ "nativeDeleteSurroundingText", "(II)V",
        (void*) DeleteSurroundingText },


	{ "nativeGetSelectionOffset", "()Landroid/graphics/Point;",
        (void*) GetSelectionOffset },
	
	{ "nativeGetBodyText", "()Ljava/lang/String;",
        (void*) GetBodyText },

	{ "nativeExecCommand", "(Ljava/lang/String;Ljava/lang/String;)Z",
	 (void*) ExecCommand },

	{ "nativeCanUndo", "()Z",
	 (void*) CanUndo },

	{ "nativeCanRedo", "()Z",
	 (void*) CanRedo },

	{ "nativeUndoRedoStateReset", "()V",
	 (void*) UndoRedoStateReset },


	{ "nativeCopyAndSaveImage", "(Ljava/lang/String;)Z",
	 (void*) CopyAndSaveImage },

	{ "nativeSaveCachedImageToFile", "(Ljava/lang/String;Ljava/lang/String;)Z",
	 (void*) SaveCachedImageToFile },

	
	{ "nativeGetBodyHTML", "()Ljava/lang/String;",
        (void*) GetBodyHTML },

	{ "nativeGetFullMarkupData", "()Landroid/webkit/WebHTMLMarkupData;",
        (void*) GetFullMarkupData },

	{ "nativeSetEditable", "(Z)V",
        (void*) SetEditable },

	{ "nativeSetSelectionEditable", "(II)V",
        (void*) SetSelectionEditable },

	{ "nativeGetCursorRect", "(Z)Landroid/graphics/Rect;",
        (void*) GetCursorRect },

	{ "nativeSetSelectionNone", "()V",
        (void*) SetSelectionNone },

	{ "nativeCheckSelectionAtBoundry", "()I",
	 (void*) CheckSelectionAtBoundry },
	
///////////////////SISO_HTMLCOMPOSER end
    { "nativeRegisterURLSchemeAsLocal", "(Ljava/lang/String;)V",
        (void*) RegisterURLSchemeAsLocal },
    { "nativeDumpDomTree", "(Z)V",
        (void*) DumpDomTree },
    { "nativeDumpRenderTree", "(Z)V",
        (void*) DumpRenderTree },
    { "nativeDumpNavTree", "()V",
        (void*) DumpNavTree },
    { "nativeDumpV8Counters", "()V",
        (void*) DumpV8Counters },
    { "nativeSetNewStorageLimit", "(J)V",
        (void*) SetNewStorageLimit },
    { "nativeGeolocationPermissionsProvide", "(Ljava/lang/String;ZZ)V",
        (void*) GeolocationPermissionsProvide },
    { "nativePause", "()V", (void*) Pause },
    { "nativeResume", "()V", (void*) Resume },
    { "nativeFreeMemory", "()V", (void*) FreeMemory },
    { "nativeSetJsFlags", "(Ljava/lang/String;)V", (void*) SetJsFlags },
    { "nativeRequestLabel", "(II)Ljava/lang/String;",
        (void*) RequestLabel },
    { "nativeUpdateFrameCacheIfLoading", "()V",
        (void*) UpdateFrameCacheIfLoading },
    { "nativeProvideVisitedHistory", "([Ljava/lang/String;)V",
        (void*) ProvideVisitedHistory },
//SanJose++
    { "nativeSetPluginRefreshDelay", "(I)V",
        (void*) SetPluginRefreshDelay },
//SanJose--
    { "nativeFullScreenPluginHidden", "(I)V",
        (void*) FullScreenPluginHidden },
    { "nativeValidNodeAndBounds", "(IILandroid/graphics/Rect;)Z",
        (void*) ValidNodeAndBounds },   
    //SAMSUNG CHANGES >>
        { "nativeWebTextSelectionAll", "(IIII)V",
	 (void*) WebTextSelectionAll },
    { "nativeCopyMoveSelection", "(IIIZZFI)V",
	 (void*) CopyMoveSelection },
    { "nativeClearTextSelection", "(II)V",
	 (void*) ClearTextSelection },
    { "nativeRecordSelectionCopiedData", "(Landroid/graphics/Region;Landroid/graphics/Rect;Landroid/graphics/Rect;I)Z",
	 (void*) RecordSelectionCopiedData },
    { "nativeGetSelectionGranularity", "()I",
	 (void*) GetSelectionGranularity },
    { "nativeSelectClosestWord", "(IIFZ)Z",
	 (void*) SelectClosestWord },
    { "nativeGetWebFeedLinks", "()[Landroid/webkit/WebFeedLink;",
        (void*) nativeGetWebFeedLinks },
    { "nativeMouseClick", "(II)V",
        (void*) MouseClick },
    { "nativeGetBlockBounds", "(IIF)Landroid/graphics/Rect;",
        (void*) nativeGetBlockBounds }
    ,{ "nativeSelectAndClick", "(IIII)V",
        (void*) SelectAndClick }
    //SAMSUNG CHANGES <<
};

int register_webviewcore(JNIEnv* env)
{
    jclass widget = env->FindClass("android/webkit/WebViewCore");
    LOG_ASSERT(widget,
            "Unable to find class android/webkit/WebViewCore");
    gWebViewCoreFields.m_nativeClass = env->GetFieldID(widget, "mNativeClass",
            "I");
    LOG_ASSERT(gWebViewCoreFields.m_nativeClass,
            "Unable to find android/webkit/WebViewCore.mNativeClass");
    gWebViewCoreFields.m_viewportWidth = env->GetFieldID(widget,
            "mViewportWidth", "I");
    LOG_ASSERT(gWebViewCoreFields.m_viewportWidth,
            "Unable to find android/webkit/WebViewCore.mViewportWidth");
    gWebViewCoreFields.m_viewportHeight = env->GetFieldID(widget,
            "mViewportHeight", "I");
    LOG_ASSERT(gWebViewCoreFields.m_viewportHeight,
            "Unable to find android/webkit/WebViewCore.mViewportHeight");
    gWebViewCoreFields.m_viewportInitialScale = env->GetFieldID(widget,
            "mViewportInitialScale", "I");
    LOG_ASSERT(gWebViewCoreFields.m_viewportInitialScale,
            "Unable to find android/webkit/WebViewCore.mViewportInitialScale");
    gWebViewCoreFields.m_viewportMinimumScale = env->GetFieldID(widget,
            "mViewportMinimumScale", "I");
    LOG_ASSERT(gWebViewCoreFields.m_viewportMinimumScale,
            "Unable to find android/webkit/WebViewCore.mViewportMinimumScale");
    gWebViewCoreFields.m_viewportMaximumScale = env->GetFieldID(widget,
            "mViewportMaximumScale", "I");
    LOG_ASSERT(gWebViewCoreFields.m_viewportMaximumScale,
            "Unable to find android/webkit/WebViewCore.mViewportMaximumScale");
    gWebViewCoreFields.m_viewportUserScalable = env->GetFieldID(widget,
            "mViewportUserScalable", "Z");
    LOG_ASSERT(gWebViewCoreFields.m_viewportUserScalable,
            "Unable to find android/webkit/WebViewCore.mViewportUserScalable");
    gWebViewCoreFields.m_viewportDensityDpi = env->GetFieldID(widget,
            "mViewportDensityDpi", "I");
    LOG_ASSERT(gWebViewCoreFields.m_viewportDensityDpi,
            "Unable to find android/webkit/WebViewCore.mViewportDensityDpi");
    gWebViewCoreFields.m_webView = env->GetFieldID(widget,
            "mWebView", "Landroid/webkit/WebView;");
    LOG_ASSERT(gWebViewCoreFields.m_webView,
            "Unable to find android/webkit/WebViewCore.mWebView");

    gWebViewCoreStaticMethods.m_supportsMimeType =
        env->GetStaticMethodID(widget, "supportsMimeType", "(Ljava/lang/String;)Z");
    LOG_ASSERT(gWebViewCoreStaticMethods.m_supportsMimeType == NULL,
        "Could not find static method supportsMimeType from WebViewCore");

    return jniRegisterNativeMethods(env, "android/webkit/WebViewCore",
            gJavaWebViewCoreMethods, NELEM(gJavaWebViewCoreMethods));
}

} /* namespace android */
