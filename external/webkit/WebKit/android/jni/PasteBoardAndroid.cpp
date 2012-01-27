#include "config.h"

///////////////
#undef LOG
#define LOG_TAG "WML_SISO"
#include <utils/Log.h>
////////////////////
#include "Pasteboard.h"
#include "NotImplemented.h"
#include "Frame.h"
#include "Node.h"
#include "Range.h"
#include "KURL.h"
#include "DocumentFragment.h"
#include "markup.h"
#include "RenderImage.h"
#include "Image.h"
#include "SharedBuffer.h"
#include "CachedResource.h"
#include "DocLoader.h"

#include <JNIHelp.h>  // For jniRegisterNativeMethods
#include <JNIUtility.h>
#include <jni.h>
#include "WebCoreJni.h"  // For to_string

namespace android {
	
static jmethodID GetJMethod(JNIEnv* env, jclass clazz, const char name[], const char signature[])
{
    jmethodID m = env->GetMethodID(clazz, name, signature);
    LOG_ASSERT(m, "Could not find method %s", name);
    return m;
}

struct JavaGlueForPasteBoard {
    jweak       m_obj;
    jmethodID   m_getText;
    jmethodID   m_getHTML;
    jmethodID   m_setDataToClipBoard;
    AutoJObject object(JNIEnv* env) {
        return getRealObject(env, m_obj);
    }
} m_javaGlueForPasteBoard;

static void InitPasteboardJni(JNIEnv *env, jobject obj)
{
	LOGD("InitPasteboardJni");
	jclass clazz = env->FindClass("android/webkit/WebClipboard");
   	m_javaGlueForPasteBoard.m_obj = env->NewWeakGlobalRef(obj);

	m_javaGlueForPasteBoard.m_getText = GetJMethod(env, clazz, "getText", "()Ljava/lang/String;");
	m_javaGlueForPasteBoard.m_getHTML = GetJMethod(env, clazz, "getHTML", "()Ljava/lang/String;");

	m_javaGlueForPasteBoard.m_setDataToClipBoard = GetJMethod(env, clazz, "setDataToClipboard", "(Ljava/lang/String;Ljava/lang/String;)V");
}

WebCore::String getText(){
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    jstring returnVal = (jstring) env->CallObjectMethod(m_javaGlueForPasteBoard.object(env).get(),
        m_javaGlueForPasteBoard.m_getText);
	
    WebCore::String result = to_string(env, returnVal);	
    checkException(env);
    return result;	
}

WebCore::String getHTML(){
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    jstring returnVal = (jstring) env->CallObjectMethod(m_javaGlueForPasteBoard.object(env).get(),
        m_javaGlueForPasteBoard.m_getHTML);
	
    WebCore::String result = to_string(env, returnVal);	
    checkException(env);
    return result;
}

void setDataToClipBoard(WebCore::String& format , WebCore::String& data){
    JNIEnv* env = JSC::Bindings::getJNIEnv();
    jstring jFormat = env->NewString((jchar*) format.characters(), format.length());	
    jstring jData = env->NewString((jchar*) data.characters(), data.length());
    env->CallVoidMethod(m_javaGlueForPasteBoard.object(env).get(),
        m_javaGlueForPasteBoard.m_setDataToClipBoard , jFormat , jData);
    env->DeleteLocalRef(jFormat); 
    env->DeleteLocalRef(jData);
    checkException(env);
}



static JNINativeMethod gPasteboardMethods[] = {
    	{ "nativeInitPasteboardJni", "()V",
       	(void*) InitPasteboardJni 	}
};



int register_pasteboard(JNIEnv* env)
{
    const char* kPasteBoardClass = "android/webkit/WebClipboard";
    jclass pasteBoardClass = env->FindClass(kPasteBoardClass);
    LOG_ASSERT(geolocationPermissions, "Unable to find class");

    return jniRegisterNativeMethods(env, kPasteBoardClass ,
            gPasteboardMethods, NELEM(gPasteboardMethods));
}
		

}




namespace WebCore {

const String savePath =  "/sdcard/temp";

void replaceNBSPWithSpace(String& str)
{
    static const UChar NonBreakingSpaceCharacter = 0xA0;
    static const UChar SpaceCharacter = ' ';
    str.replace(NonBreakingSpaceCharacter, SpaceCharacter);
}

void copyImagePathToClipboard(const String& imagePath)
{
    String dataFormat = "HTML";
    String htmlImageFragment = "<img src=";
    htmlImageFragment.append(imagePath);
    htmlImageFragment.append(" />");			
    android::setDataToClipBoard(dataFormat , htmlImageFragment);	
}

String createLocalResource(Frame* frame , String url)
{
    String filename;	
    CachedResource* cachedResource = frame->document()->docLoader()->cachedResource(url);
    //if (!cachedResource)
    //    cachedResource = cache()->resourceForURL(url);
    if (cachedResource && cachedResource->isImage())
    {
	return createLocalResource(cachedResource ,savePath); 
    }
    return String();	
}

bool saveCachedImageToFile(WebCore::Frame* frame, String imageUrl, String filePath)
{
	CachedResource* cachedResource = frame->document()->docLoader()->cachedResource(imageUrl);
	if (cachedResource && cachedResource->isImage())
		return saveCachedImageToFile(cachedResource, imageUrl, filePath);
	
	return false;
}

// FIXME, no support for spelling yet.
Pasteboard* Pasteboard::generalPasteboard()
{
    LOGD("generalPasteboard");	
    static Pasteboard* pasteboard = new Pasteboard();
    return pasteboard;
}

void Pasteboard::writeSelection(Range* selectedRange, bool canSmartCopyOrDelete, Frame* frame)
{
	LOGD("writeSelection");	
	String html = createMarkup(selectedRange, 0 , AnnotateForInterchange , false );
	String dataFormat = "HTML";
	ExceptionCode ec = 0;

    	android::setDataToClipBoard(dataFormat , html);
    
}

void Pasteboard::writePlainText(const String& plainText)
{
	LOGD("writePlainText");
	String dataFormat = "HTML";
	android::setDataToClipBoard(dataFormat , (String&)plainText);
}

void Pasteboard::writeURL(const KURL&, const String&, Frame*)
{
    LOGD("writeURL");
    notImplemented();
}

void Pasteboard::clear()
{
    LOGD("clear");
    notImplemented();
}

bool Pasteboard::canSmartReplace()
{
    notImplemented();
    return false;
}

PassRefPtr<DocumentFragment> Pasteboard::documentFragment(Frame* frame, PassRefPtr<Range> context, bool allowPlainText, bool& chosePlainText)
{
	LOGD("documentFragment");
	chosePlainText = false;
	String dataFormat = "HTML";
	String imageFormat = "IMAGE";
	String markup;
	KURL srcURL;
	
	markup = android::getHTML();
	if(!markup.isEmpty()) {	
		RefPtr<DocumentFragment> fragment =
			createFragmentFromMarkup(frame->document(), markup, srcURL, FragmentScriptingNotAllowed);
		if (fragment)
			return fragment.release();
	}
	else if (allowPlainText) {
		String markup = android::getText();//ChromiumBridge::clipboardReadPlainText(buffer);
		if (!markup.isEmpty()) {
			chosePlainText = true;

			RefPtr<DocumentFragment> fragment =
				createFragmentFromText(context.get(), markup);
			if (fragment)
				return fragment.release();
		}
	}

	return 0;
}

String Pasteboard::plainText(Frame*)
{
	LOGD("plainText");
    	return android::getText();
}

Pasteboard::Pasteboard()
{
	LOGD("Pasteboard");
    	notImplemented();
}

Pasteboard::~Pasteboard()
{
	LOGD("~Pasteboard");
    	notImplemented();
}


void Pasteboard::writeImage(WebCore::Node* node, WebCore::KURL const& , WebCore::String const& title)
{
    ASSERT(node);
    ASSERT(node->renderer());
    ASSERT(node->renderer()->isImage());
    RenderImage* renderer = toRenderImage(node->renderer());
    CachedImage* cachedImage = renderer->cachedImage();
    ASSERT(cachedImage);
    Image* image = cachedImage->image();
    ASSERT(image);

    // If the image is wrapped in a link, |url| points to the target of the
    // link.  This isn't useful to us, so get the actual image URL.
    AtomicString urlString;
    if (node->hasTagName(HTMLNames::imgTag) || node->hasTagName(HTMLNames::inputTag))
        urlString = static_cast<Element*>(node)->getAttribute(HTMLNames::srcAttr);
#if ENABLE(SVG)
    else if (node->hasTagName(SVGNames::imageTag))
        urlString = static_cast<Element*>(node)->getAttribute(XLinkNames::hrefAttr);
#endif
    else if (node->hasTagName(HTMLNames::embedTag) || node->hasTagName(HTMLNames::objectTag)) {
        Element* element = static_cast<Element*>(node);
        urlString = element->getAttribute(element->imageSourceAttributeName());
    }
    KURL url = urlString.isEmpty() ? KURL() : node->document()->completeURL(deprecatedParseURL(urlString));

    const char* imageEncodedBuffer = image->data()->data();
    	 		
    //NativeImagePtr bitmap = image->nativeImageForCurrentFrame();
    //ChromiumBridge::clipboardWriteImage(bitmap, url, title);
}

}
