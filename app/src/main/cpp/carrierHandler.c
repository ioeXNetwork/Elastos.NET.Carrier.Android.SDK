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

#include <jni.h>
#include <assert.h>
#include <stdlib.h>
#include "log.h"
#include "utils.h"
#include "IOEX_carrier.h"
#include "carrierUtils.h"
#include "carrierHandler.h"

static
void cbOnIdle(IOEXCarrier* carrier, void* context)
{
    assert(carrier);
    assert(context);

    HandlerContext* hc = (HandlerContext*)context;
    assert(carrier == hc->nativeCarrier);
    assert(hc->env);

    if (!callVoidMethod(hc->env, hc->clazz, hc->callbacks,
                        "onIdle", "("_W("Carrier;)V"),
                        hc->carrier)) {
        logE("Call Carrier.Callbacks.OnIdle error");
    }
}

static
void cbOnConnection(IOEXCarrier* carrier, IOEXConnectionStatus status, void* context)
{
    HandlerContext *hc = (HandlerContext *) context;
    jobject jstatus = NULL;

    assert(carrier);
    assert(context);

    assert(carrier == hc->nativeCarrier);
    assert(hc->env);

    if (!newJavaConnectionStatus(hc->env, status, &jstatus)) {
        logE("Construct java Connection object error");
        return;
    }

    if (!callVoidMethod(hc->env, hc->clazz, hc->callbacks,
                        "onConnection",
                        "("_W("Carrier;")_W("ConnectionStatus;)V"),
                        hc->carrier, jstatus)) {
        logE("Call Carrier.Callbacks.OnConnection error");
    }

    (*hc->env)->DeleteLocalRef(hc->env, jstatus);
}

static
void cbOnReady(IOEXCarrier* carrier, void* context)
{
    assert(carrier);
    assert(context);

    HandlerContext* hc = (HandlerContext*)context;
    assert(carrier == hc->nativeCarrier);
    assert(hc->env);

    if (!callVoidMethod(hc->env, hc->clazz, hc->callbacks,
                        "onReady",
                        "("_W("Carrier;)V"),
                        hc->carrier)) {
        logE("Call Carrier.Callbacks.OnReady error");
    }
}

static
void cbOnSelfInfoChanged(IOEXCarrier* carrier, const IOEXUserInfo* userInfo, void* context)
{
    HandlerContext* hc = (HandlerContext*)context;
    jobject juserInfo;

    assert(carrier);
    assert(userInfo);
    assert(context);

    assert(carrier == hc->nativeCarrier);
    assert(hc->env);

    if (!newJavaUserInfo(hc->env, userInfo, &juserInfo)) {
        logE("Construct Java UserInfo object error");
        return;
    }

    if (!callVoidMethod(hc->env, hc->clazz, hc->callbacks,
                        "onSelfInfoChanged",
                        "("_W("Carrier;")_W("UserInfo;)V"),
                        hc->carrier, juserInfo)) {
        logE("Call Carrier.Callbacks.OnSelfInfoChanged error");
    }
    (*hc->env)->DeleteLocalRef(hc->env, juserInfo);
}

static
bool cbFriendsIterated(IOEXCarrier* carrier, const IOEXFriendInfo* friendInfo, void* context)
{
    HandlerContext* hc = (HandlerContext*)context;
    jobject jfriendInfo = NULL;
    jboolean result;

    assert(carrier);
    assert(context);

    assert(carrier == hc->nativeCarrier);
    assert(hc->env);

    if (friendInfo) {
        if (!newJavaFriendInfo(hc->env, friendInfo, &jfriendInfo)) {
            logE("Construct Java FriendInfo object error");
            return false;
        }
    }

    if (!callBooleanMethod(hc->env, hc->clazz, hc->callbacks,
                           "onFriendsIterated",
                           "("_W("Carrier;")_W("FriendInfo;)Z"),
                           &result, hc->carrier, jfriendInfo)) {
        logE("Call Carrier.Callbacks.OnFriendIterated error");
    }

    if (jfriendInfo)
        (*hc->env)->DeleteLocalRef(hc->env, jfriendInfo);
    return (bool)result;
}

static
void cbOnFriendConnectionChanged(IOEXCarrier *carrier, const char *friendId,
                                 IOEXConnectionStatus status, void *context)
{
    HandlerContext *hc = (HandlerContext *) context;
    jstring jfriendId;
    jobject jstatus;

    assert(carrier);
    assert(context);

    assert(carrier == hc->nativeCarrier);
    assert(hc->env);

    jfriendId = (*hc->env)->NewStringUTF(hc->env, friendId);
    if (!jfriendId) {
        logE("New Java String object error");
        return;
    }

    if (!newJavaConnectionStatus(hc->env, status, &jstatus)) {
        logE("Construct java Connection object error");
        (*hc->env)->DeleteLocalRef(hc->env, jfriendId);
        return;
    }

    if (!callVoidMethod(hc->env, hc->clazz, hc->callbacks,
                        "onFriendConnection",
                        "("_W("Carrier;")_J("String;")_W("ConnectionStatus;)V"),
            hc->carrier, jfriendId, jstatus)) {
        logE("Call Carrier.Callbacks.OnFriendConnection error");
    }

    (*hc->env)->DeleteLocalRef(hc->env, jstatus);
    (*hc->env)->DeleteLocalRef(hc->env, jfriendId);
}

static
void cbOnFriendInfoChanged(IOEXCarrier* carrier, const char* friendId,
                           const IOEXFriendInfo* friendInfo, void* context)
{
    HandlerContext* hc = (HandlerContext*)context;
    jstring jfriendId;
    jobject jfriendInfo;

    assert(carrier);
    assert(friendId);
    assert(friendInfo);
    assert(context);

    assert(carrier == hc->nativeCarrier);
    assert(hc->env);

    jfriendId = (*hc->env)->NewStringUTF(hc->env, friendId);
    if (!jfriendId) {
        logE("New Java String object error");
        return;
    }

    if (!newJavaFriendInfo(hc->env, friendInfo, &jfriendInfo)) {
        logE("Construct Java FriendInfo object error");
        (*hc->env)->DeleteLocalRef(hc->env, jfriendId);
        return;
    }

    if (!callVoidMethod(hc->env, hc->clazz, hc->callbacks,
                        "onFriendInfoChanged",
                        "("_W("Carrier;")_J("String;")_W("FriendInfo;)V"),
                        hc->carrier, jfriendId, jfriendInfo)) {
        logE("Call Carrier.Callbacks.OnFriendInfoChanged error");
    }
    (*hc->env)->DeleteLocalRef(hc->env, jfriendId);
    (*hc->env)->DeleteLocalRef(hc->env, jfriendInfo);
}

static
void cbOnFriendPresence(IOEXCarrier* carrier, const char* friendId,
                        IOEXPresenceStatus status, void* context)
{
    HandlerContext* hc = (HandlerContext*)context;
    jstring jfriendId;
    jobject jpresence;

    assert(carrier);
    assert(friendId);
    assert(context);

    assert(carrier == hc->nativeCarrier);
    assert(hc->env);

    jfriendId = (*hc->env)->NewStringUTF(hc->env, friendId);
    if (!jfriendId) {
        logE("New Java String object error");
        return;
    }

    if (!newJavaPresenceStatus(hc->env, status, &jpresence)) {
        logE("Construct java PresenceStatus object error");
        (*hc->env)->DeleteLocalRef(hc->env, jfriendId);
        return;
    }

    if (!callVoidMethod(hc->env, hc->clazz, hc->callbacks,
                        "onFriendPresence",
                        "("_W("Carrier;")_J("String;")_W("PresenceStatus;)V"),
                        hc->carrier, jfriendId, jpresence)){
        logE("Call Carrier.Callbacks.onFriendPresence error");
    }

    (*hc->env)->DeleteLocalRef(hc->env, jfriendId);
    (*hc->env)->DeleteLocalRef(hc->env, jpresence);
}

static
void cbOnFriendAdded(IOEXCarrier* carrier, const IOEXFriendInfo* friendInfo, void* context)
{
    HandlerContext* hc = (HandlerContext*)context;
    jobject jfriendInfo;

    assert(carrier);
    assert(friendInfo);
    assert(context);

    assert(carrier == hc->nativeCarrier);
    assert(hc->env);

    if (!newJavaFriendInfo(hc->env, friendInfo, &jfriendInfo)){
        logE("Construct Java UserInfo object error");
        return;
    }

    if (!callVoidMethod(hc->env, hc->clazz, hc->callbacks,
                        "onFriendAdded",
                        "("_W("Carrier;")_W("FriendInfo;)V"),
                        hc->carrier, jfriendInfo)) {
        logE("Call Carrier.Callbacks.onFriendAdded error");
    }
    (*hc->env)->DeleteLocalRef(hc->env, jfriendInfo);
}

static
void cbOnFriendRemoved(IOEXCarrier* carrier, const char* friendId, void* context)
{
    HandlerContext* hc = (HandlerContext*)context;
    jstring jfriendId;

    assert(carrier);
    assert(friendId);
    assert(context);

    assert(carrier == hc->nativeCarrier);
    assert(hc->env);

    jfriendId = (*hc->env)->NewStringUTF(hc->env, friendId);
    if (!jfriendId) {
        logE("New Java String object error");
        return;
    }

    if (!callVoidMethod(hc->env, hc->clazz, hc->callbacks,
                        "onFriendRemoved",
                        "("_W("Carrier;")_J("String;)V"),
                        hc->carrier, jfriendId)) {
        logE("Call Carrier.Callbacks.onFriendRemoved error");
    }
    (*hc->env)->DeleteLocalRef(hc->env, jfriendId);
}

static
void cbOnFriendRequest(IOEXCarrier* carrier, const char* userId, const IOEXUserInfo* userInfo,
                       const char* hello, void* context)
{
    HandlerContext* hc = (HandlerContext*)context;
    jstring juserId;
    jobject juserInfo;
    jstring jhello;

    assert(carrier);
    assert(userId);
    assert(userInfo);
    assert(context);

    assert(carrier == hc->nativeCarrier);
    assert(hc->env);

    juserId = (*hc->env)->NewStringUTF(hc->env, userId);
    if (!juserId) {
        logE("New Java String object error");
        return;
    }
    if (!newJavaUserInfo(hc->env, userInfo, &juserInfo)) {
        logE("Construct Java UserInfo object error");
        (*hc->env)->DeleteLocalRef(hc->env, juserId);
        return;
    }
    jhello = (*hc->env)->NewStringUTF(hc->env, hello);
    if (!jhello) {
        logE("New Java String object error");
        (*hc->env)->DeleteLocalRef(hc->env, juserId);
        (*hc->env)->DeleteLocalRef(hc->env, juserInfo);
        return;
    }

    if (!callVoidMethod(hc->env, hc->clazz, hc->callbacks,
                        "onFriendRequest",
                        "("_W("Carrier;")_J("String;")_W("UserInfo;")_J("String;)V"),
                        hc->carrier, juserId, juserInfo, jhello)) {
        logE("Call Carrier.Callbacks.OnFriendRequest error");
    }

    (*hc->env)->DeleteLocalRef(hc->env, juserId);
    (*hc->env)->DeleteLocalRef(hc->env, juserInfo);
    (*hc->env)->DeleteLocalRef(hc->env, jhello);
}

static
void cbOnFriendMessage(IOEXCarrier* carrier, const char* friendId, const void* message, size_t length,
                       void* context)
{
    HandlerContext* hc = (HandlerContext*)context;
    jstring jfriendId;
    jstring jmessage;

    assert(carrier);
    assert(friendId);
    assert(message);
    assert(length > 0);

    assert(carrier == hc->nativeCarrier);
    assert(hc->env);

    jfriendId = (*hc->env)->NewStringUTF(hc->env, friendId);
    if (!jfriendId) {
        logE("New Java String object error");
        return;
    }
    jmessage = (*hc->env)->NewStringUTF(hc->env, (const char *)message);
    if (!jmessage) {
        logE("New Java String object error");
        (*hc->env)->DeleteLocalRef(hc->env, jfriendId);
        return;
    }

    if (!callVoidMethod(hc->env, hc->clazz, hc->callbacks,
                        "onFriendMessage",
                        "("_W("Carrier;")_J("String;")_J("String;)V"),
                        hc->carrier, jfriendId, jmessage)) {
        logE("Call Carrier.Callbacks.onFriendMessage error");
    }

    (*hc->env)->DeleteLocalRef(hc->env, jfriendId);
    (*hc->env)->DeleteLocalRef(hc->env, jmessage);
}

static
void cbOnFriendInviteRquest(IOEXCarrier* carrier, const char* from, const void* hello,
                            size_t length, void* context)
{
    HandlerContext* hc = (HandlerContext*)context;
    jstring jfrom;
    jstring jhello;

    (void)length;

    assert(carrier);
    assert(from);
    assert(hello);

    assert(carrier == hc->nativeCarrier);
    assert(hc->env);

    jfrom = (*hc->env)->NewStringUTF(hc->env, from);
    if (!jfrom) {
        logE("New java String object error");
        return;
    }
    jhello = (*hc->env)->NewStringUTF(hc->env, (const char *)hello);
    if (!jhello) {
        logE("New java String object error");
        (*hc->env)->DeleteLocalRef(hc->env, jfrom);
        return;
    }

    if (!callVoidMethod(hc->env, hc->clazz, hc->callbacks,
                           "onFriendInviteRequest",
                           "("_W("Carrier;")_J("String;")_J("String;)V"),
                           hc->carrier, jfrom, jhello)) {
        logE("Call Carrier.Callbacks.onFriendInviteRequest error");
    }
    (*hc->env)->DeleteLocalRef(hc->env, jfrom);
    (*hc->env)->DeleteLocalRef(hc->env, jhello);
}

static
void cbOnFileRequest(IOEXCarrier *carrier, const char *fileid, const char *from,
                     const char *filename, size_t filesize, void *context)
{
    HandlerContext* hc = (HandlerContext*)context;
    jstring jfileid, jfrom, jfilename;
    jlong jfilesize;

    (void)filesize;
    assert(carrier);
    assert(fileid);
    assert(from);
    assert(filename);
    assert(carrier == hc->nativeCarrier);
    assert(hc->env);



    jfrom = (*hc->env)->NewStringUTF(hc->env, from);
    if (!jfrom) {
        logE("New java String(jfrom) object error");
        return;
    }

    jfileid = (*hc->env)->NewStringUTF(hc->env, fileid);
    if (!jfileid) {
        logE("New java String(jfileid) object error");
        (*hc->env)->DeleteLocalRef(hc->env, jfrom);
        return;
    }

    jfilename = (*hc->env)->NewStringUTF(hc->env, filename);
    if (!jfilename) {
        logE("New java String(jfilename) object error");
        (*hc->env)->DeleteLocalRef(hc->env, jfrom);
        (*hc->env)->DeleteLocalRef(hc->env, jfileid);
        return;
    }

    jfilesize = (jlong)filesize;
    if (!callVoidMethod(hc->env, hc->clazz, hc->callbacks,
                        "onFriendFileRequest",
                        "("_W("Carrier;")_J("String;")_J("String;")_J("String;")"J)V",
                        hc->carrier, jfrom, jfileid, jfilename, jfilesize)) {
        logE("Call Carrier.Callbacks.onFriendFileRequest error");
    }
    (*hc->env)->DeleteLocalRef(hc->env, jfrom);
    (*hc->env)->DeleteLocalRef(hc->env, jfileid);
    (*hc->env)->DeleteLocalRef(hc->env, jfilename);
}

static
void cbOnFileAccepted(IOEXCarrier *carrier, const char *fileid, const char *friendid,
                      const char *fullpath, size_t filesize, void *context)
{
    HandlerContext* hc = (HandlerContext*)context;

    jstring jfileid, jreceiver, jfilepath;
    jlong jfilesize;

    (void)filesize;

    jreceiver = (*hc->env)->NewStringUTF(hc->env, friendid);
    if(!jreceiver){
        logE("New java String(jreceiver) object error");
        return;
    }

    jfileid = (*hc->env)->NewStringUTF(hc->env, fileid);
    if(!jfileid){
        logE("New java String(jfileid) object error");
        (*hc->env)->DeleteLocalRef(hc->env, jreceiver);
        return;
    }

    jfilepath = (*hc->env)->NewStringUTF(hc->env, fullpath);
    if(!jfilepath){
        logE("New java String(jfilepath) object error");
        (*hc->env)->DeleteLocalRef(hc->env, jreceiver);
        (*hc->env)->DeleteLocalRef(hc->env, jfileid);
        return;
    }

    jfilesize = (jlong)filesize;

    if (!callVoidMethod(hc->env, hc->clazz, hc->callbacks,
                        "onFriendFileAccepted",
                        "("_W("Carrier;")_J("String;")_J("String;")_J("String;")"J)V",
                        hc->carrier, jreceiver, jfileid, jfilepath, jfilesize)) {
        logE("Call Carrier.Callbacks.onFriendFileAccepted error");
    }
    (*hc->env)->DeleteLocalRef(hc->env, jreceiver);
    (*hc->env)->DeleteLocalRef(hc->env, jfileid);
    (*hc->env)->DeleteLocalRef(hc->env, jfilepath);
}

static
void cbOnFilePaused(IOEXCarrier *carrier, const char *fileid, const char *friendid, void *context)
{
    HandlerContext* hc = (HandlerContext*)context;
    jstring jfriendid, jfileid;

    jfriendid = (*hc->env)->NewStringUTF(hc->env ,friendid);
    if(!jfriendid){
        logE("New java String(jfriendid) object error");
        return;
    }

    jfileid = (*hc->env)->NewStringUTF(hc->env ,fileid);
    if(!jfileid){
        logE("New java String(jfileid) object error");
        (*hc->env)->DeleteLocalRef(hc->env, jfriendid);
        return;
    }

    if (!callVoidMethod(hc->env, hc->clazz, hc->callbacks,
                        "onFriendFilePaused",
                        "("_W("Carrier;")_J("String;")_J("String;)V"),
                        hc->carrier, jfriendid, jfileid)) {
        logE("Call Carrier.Callbacks.onFriendFilePaused error");
    }
    (*hc->env)->DeleteLocalRef(hc->env, jfriendid);
    (*hc->env)->DeleteLocalRef(hc->env, jfileid);
}

static
void cbOnFileResumed(IOEXCarrier *carrier, const char *fileid, const char *friendid, void *context)
{
    HandlerContext* hc = (HandlerContext*)context;
    jstring jfriendid, jfileid;

    jfriendid = (*hc->env)->NewStringUTF(hc->env ,friendid);
    if(!jfriendid){
        logE("New java String(jfriendid) object error");
        return;
    }

    jfileid = (*hc->env)->NewStringUTF(hc->env ,fileid);
    if(!jfileid){
        logE("New java String(jfileid) object error");
        (*hc->env)->DeleteLocalRef(hc->env, jfriendid);
        return;
    }

    if (!callVoidMethod(hc->env, hc->clazz, hc->callbacks,
                        "onFriendFileResumed",
                        "("_W("Carrier;")_J("String;")_J("String;)V"),
                        hc->carrier, jfriendid, jfileid)) {
        logE("Call Carrier.Callbacks.onFriendFileResumed error");
    }
    (*hc->env)->DeleteLocalRef(hc->env, jfriendid);
    (*hc->env)->DeleteLocalRef(hc->env, jfileid);
}

static
void cbOnFileCanceled(IOEXCarrier *carrier, const char *fileid, const char *friendid, void *context)
{
    HandlerContext* hc = (HandlerContext*)context;
    jstring jfriendid, jfileid;

    jfriendid = (*hc->env)->NewStringUTF(hc->env ,friendid);
    if(!jfriendid){
        logE("New java String(jfriendid) object error");
        return;
    }

    jfileid = (*hc->env)->NewStringUTF(hc->env ,fileid);
    if(!jfileid){
        logE("New java String(jfileid) object error");
        (*hc->env)->DeleteLocalRef(hc->env, jfriendid);
        return;
    }

    if (!callVoidMethod(hc->env, hc->clazz, hc->callbacks,
                        "onFriendFileCanceled",
                        "("_W("Carrier;")_J("String;")_J("String;)V"),
                        hc->carrier, jfriendid, jfileid)) {
        logE("Call Carrier.Callbacks.onFriendFileCanceled error");
    }
    (*hc->env)->DeleteLocalRef(hc->env, jfriendid);
    (*hc->env)->DeleteLocalRef(hc->env, jfileid);
}

static
void cbOnFileCompleted(IOEXCarrier *carrier, const char *fileid, const char *friendid, void *context)
{
    HandlerContext* hc = (HandlerContext*)context;
    jstring jfriendid, jfileid;

    jfriendid = (*hc->env)->NewStringUTF(hc->env ,friendid);
    if(!jfriendid){
        logE("New java String(jfriendid) object error");
        return;
    }

    jfileid = (*hc->env)->NewStringUTF(hc->env ,fileid);
    if(!jfileid){
        logE("New java String(jfileid) object error");
        (*hc->env)->DeleteLocalRef(hc->env, jfriendid);
        return;
    }

    if (!callVoidMethod(hc->env, hc->clazz, hc->callbacks,
                        "onFriendFileCompleted",
                        "("_W("Carrier;")_J("String;")_J("String;)V"),
                        hc->carrier, jfriendid, jfileid)) {
        logE("Call Carrier.Callbacks.onFriendFileCompleted error");
    }
    (*hc->env)->DeleteLocalRef(hc->env, jfriendid);
    (*hc->env)->DeleteLocalRef(hc->env, jfileid);

}

static
void cbOnFileProgress(IOEXCarrier *carrier, const char *fileid, const char *friendid,
                      const char *fullpath, uint64_t size, uint64_t transferred,
                      void *context)
{
    HandlerContext* hc = (HandlerContext*)context;
    jstring jfriendid, jfileid, jfilepath;
    jlong jtotalsize, jtransferredsize;

    (void)size;
    (void)transferred;

    assert(fileid);
    assert(friendid);
    assert(fullpath);
    assert(hc->env);

    jfriendid = (*hc->env)->NewStringUTF(hc->env, friendid);
    if(!jfriendid){
        logE("New java String(jfriendid) object error");
        return;
    }

    jfilepath = (*hc->env)->NewStringUTF(hc->env, fullpath);
    if(!jfilepath){
        logE("New java String(jfilepath) object error");
        (*hc->env)->DeleteLocalRef(hc->env, jfriendid);
        return;
    }

    jfileid = (*hc->env)->NewStringUTF(hc->env, fileid);
    if(!jfileid){
        logE("New java String(jfilepath) object error");
        (*hc->env)->DeleteLocalRef(hc->env, jfriendid);
        (*hc->env)->DeleteLocalRef(hc->env, jfilepath);
        return;
    }

    jtotalsize = (jlong) size;
    jtransferredsize = (jlong) transferred;

    if (!callVoidMethod(hc->env, hc->clazz, hc->callbacks,
                        "onFriendFileProgress",
                        "("_W("Carrier;")_J("String;")_J("String;")_J("String;")"JJ)V",
                        hc->carrier, jfriendid, jfilepath, jfileid, jtotalsize, jtransferredsize)) {
        logE("Call Carrier.Callbacks.onFriendFileChunkReceived error");
    }

    (*hc->env)->DeleteLocalRef(hc->env, jfriendid);
    (*hc->env)->DeleteLocalRef(hc->env, jfilepath);
    (*hc->env)->DeleteLocalRef(hc->env, jfileid);
}

IOEXCallbacks  carrierCallbacks = {
        .idle            = cbOnIdle,
        .connection_status = cbOnConnection,
        .ready           = cbOnReady,
        .self_info       = cbOnSelfInfoChanged,
        .friend_list     = cbFriendsIterated,
        .friend_connection = cbOnFriendConnectionChanged,
        .friend_info     = cbOnFriendInfoChanged,
        .friend_presence = cbOnFriendPresence,
        .friend_request  = cbOnFriendRequest,
        .friend_added    = cbOnFriendAdded,
        .friend_removed  = cbOnFriendRemoved,
        .friend_message  = cbOnFriendMessage,
        .friend_invite   = cbOnFriendInviteRquest,
        .file_request    = cbOnFileRequest,
        .file_accepted   = cbOnFileAccepted,
        .file_paused     = cbOnFilePaused,
        .file_resumed    = cbOnFileResumed,
        .file_canceled   = cbOnFileCanceled,
        .file_completed  = cbOnFileCompleted,
        .file_progress   = cbOnFileProgress,

};

int handlerCtxtSet(HandlerContext* hc, JNIEnv* env, jobject jcarrier, jobject jcallbacks)
{
    jclass lclazz = (*env)->GetObjectClass(env, jcallbacks);
    if (!lclazz) {
        logE("Java class implementing interface 'CarrierHandler' not found");
        return 0;
    }

    jclass  gclazz      = NULL;
    jobject gjcarrier   = NULL;
    jobject gjcallbacks = NULL;

    gclazz      = (*env)->NewGlobalRef(env, lclazz);
    gjcarrier   = (*env)->NewGlobalRef(env, jcarrier);
    gjcallbacks = (*env)->NewGlobalRef(env, jcallbacks);

    if (!gclazz || !gjcarrier || !gjcallbacks) {
        logE("New global reference to local object error");
        goto errorExit;
    }

    hc->clazz     = gclazz;
    hc->carrier   = gjcarrier;
    hc->callbacks = gjcallbacks;
    return 1;

errorExit:
    if (gclazz)      (*env)->DeleteGlobalRef(env, gclazz);
    if (gjcarrier)   (*env)->DeleteGlobalRef(env, gjcarrier);
    if (gjcallbacks) (*env)->DeleteGlobalRef(env, gjcallbacks);
    return 0;
}

void handlerCtxtCleanup(HandlerContext* hc, JNIEnv* env)
{
    assert(hc);
    assert(env);

    if (hc->clazz)
        (*env)->DeleteGlobalRef(env, hc->clazz);
    if (hc->carrier)
        (*env)->DeleteGlobalRef(env, hc->carrier);
    if (hc->callbacks)
        (*env)->DeleteGlobalRef(env, hc->callbacks);
}

