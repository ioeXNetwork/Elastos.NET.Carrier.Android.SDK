/*
 * Copyright (c) 2018 Elastos Foundation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
* Copyright (c) 2019 ioeXNetwork
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include <jni.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <IOEX_carrier.h>
#include <IOEX_session.h>

#include "log.h"
#include "utils.h"
#include "carrierCookie.h"
#include "sessionUtils.h"

typedef struct CallbackContext {
    JNIEnv* env;
    jclass  clazz;
    jobject carrier;
    jobject handler;
} CallbackContext;

static CallbackContext callbackContext;

static
void onSessionRequestCallback(IOEXCarrier* carrier, const char* from, const char* sdp,
                              size_t len, void* context)
{
    CallbackContext* cc = (CallbackContext*)context;
    int needDetach = 0;
    JNIEnv* env;
    jstring jfrom;
    jstring jsdp;

    assert(carrier);
    assert(from);
    assert(sdp);

    (void)carrier;
    (void)len;

    env = attachJvm(&needDetach);
    if (!env) {
        logE("Attach JVM error");
        return;
    }

    jfrom = (*env)->NewStringUTF(env, from);
    if (!jfrom) {
        detachJvm(env, needDetach);
        return;
    }

    jsdp = (*env)->NewStringUTF(env, sdp);
    if (!jsdp) {
        (*env)->DeleteLocalRef(env, jfrom);
        detachJvm(env, needDetach);
        return;
    }

    if (!callVoidMethod(env, cc->clazz, cc->handler, "onSessionRequest",
                        "("_W("Carrier;")_J("String;")_J("String;)V"),
                        cc->carrier, jfrom, jsdp)) {
        logE("Can not call method:\n\tvoid onSessionRequest(Carrier, String, String)");
    }

    (*env)->DeleteLocalRef(env, jsdp);
    (*env)->DeleteLocalRef(env, jfrom);

    detachJvm(env, needDetach);
}

static
bool callbackCtxtSet(CallbackContext* hc, JNIEnv* env, jobject jcarrier, jobject jhandler)
{

    jclass lclazz;
    jclass  gclazz;
    jobject gjcarrier;
    jobject gjhandler;

    lclazz = (*env)->GetObjectClass(env, jhandler);
    if (!lclazz) {
        setErrorCode(IOEX_GENERAL_ERROR(IOEXERR_LANGUAGE_BINDING));
        return false;
    }

    gclazz    = (*env)->NewGlobalRef(env, lclazz);
    gjcarrier = (*env)->NewGlobalRef(env, jcarrier);
    gjhandler = (*env)->NewGlobalRef(env, jhandler);

    if (!gclazz || !gjcarrier || !gjhandler) {
        setErrorCode(IOEX_GENERAL_ERROR(IOEXERR_OUT_OF_MEMORY));
        goto errorExit;
    }

    hc->env     = NULL;
    hc->clazz   = gclazz;
    hc->carrier = gjcarrier;
    hc->handler = gjhandler;
    return true;

errorExit:
    if (gjhandler) (*env)->DeleteGlobalRef(env, gjhandler);
    if (gjcarrier) (*env)->DeleteGlobalRef(env, gjcarrier);
    if (gclazz)    (*env)->DeleteGlobalRef(env, gclazz);

    return false;
}

static
void callbackCtxtCleanup(CallbackContext* cc, JNIEnv* env)
{
    assert(cc);

    if (cc->clazz)
        (*env)->DeleteGlobalRef(env, cc->clazz);
    if (cc->carrier)
        (*env)->DeleteGlobalRef(env, cc->carrier);
    if (cc->handler)
        (*env)->DeleteGlobalRef(env, cc->handler);
}

static
jboolean sessionMgrInit(JNIEnv* env, jclass clazz, jobject jcarrier, jobject jhandler)
{
    CallbackContext *hc = NULL;
    int rc;

    assert(jcarrier);

    (void)clazz;

    memset(&callbackContext, 0, sizeof(callbackContext));


    if (jhandler) {
        hc = (CallbackContext*)&callbackContext;
        if (!callbackCtxtSet(hc, env, jcarrier, jhandler)) {
            setErrorCode(IOEX_GENERAL_ERROR(IOEXERR_LANGUAGE_BINDING));
            return JNI_FALSE;
        }
    }

    rc = IOEX_session_init(getCarrier(env, jcarrier), onSessionRequestCallback, hc);
    if (rc < 0) {
        logE("Call IOEX_session_init API error");
        setErrorCode(IOEX_get_error());
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

static
void sessionMgrCleanup(JNIEnv* env, jclass clazz, jobject jcarrier)
{
    assert(jcarrier);

    (void)clazz;

    callbackCtxtCleanup(&callbackContext, env);
    IOEX_session_cleanup(getCarrier(env, jcarrier));
}

static
jobject createSession(JNIEnv* env, jobject thiz, jobject jcarrier, jstring jto)
{
    const char *to;
    IOEXSession *session;
    jobject jsession;

    assert(jcarrier);
    assert(jto);

    (void)thiz;

    to = (*env)->GetStringUTFChars(env, jto, NULL);
    if (!to) {
        setErrorCode(IOEX_GENERAL_ERROR(IOEXERR_LANGUAGE_BINDING));
        return NULL;
    }

    session = IOEX_session_new(getCarrier(env, jcarrier), to);
    (*env)->ReleaseStringUTFChars(env, jto, to);
    if (!session) {
        logE("Call IOEX_session_new API error");
        setErrorCode(IOEX_get_error());
        return NULL;
    }

    if (!newJavaSession(env, session, jto, &jsession)) {
        IOEX_session_close(session);
        setErrorCode(IOEX_GENERAL_ERROR(IOEXERR_LANGUAGE_BINDING));
        return NULL;
    }

    return jsession;
}

static
jint getErrorCode(JNIEnv* env, jclass clazz)
{
    (void)env;
    (void)clazz;

    return _getErrorCode();
}

static const char* gClassName = "org/ioex/carrier/session/Manager";
static JNINativeMethod gMethods[] = {
        {"native_init",      "("_W("Carrier;")_S("ManagerHandler;)Z"),  (void*)sessionMgrInit   },
        {"native_cleanup",   "("_W("Carrier;)V"),                       (void*)sessionMgrCleanup},
        {"create_session",   "("_W("Carrier;")_J("String;)")_S("Session;"),
                                                                        (void*)createSession    },
        {"get_error_code",   "()I",                                     (void*)getErrorCode     },
};

int registerCarrierSessionManagerMethods(JNIEnv* env)
{
    return registerNativeMethods(env, gClassName,
                                 gMethods,
                                 sizeof(gMethods) / sizeof(gMethods[0]));
}

void unregisterCarrierSessionManagerMethods(JNIEnv* env)
{
    jclass clazz = (*env)->FindClass(env, gClassName);
    if (clazz)
        (*env)->UnregisterNatives(env, clazz);
}