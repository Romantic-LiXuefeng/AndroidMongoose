#include <jni.h>
#include <mongoose.h>

#ifdef NATIVE_LOG
#define LOG_TAG "NATIVE.LOG"

#include <android/log.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,LOG_TAG,__VA_ARGS__)
#else
#define LOGD(...) do{}while(0)
#define LOGI(...) do{}while(0)
#define LOGW(...) do{}while(0)
#define LOGE(...) do{}while(0)
#define LOGF(...) do{}while(0)
#endif

#define NATIVE_FUNC(f) Java_com_hzy_mongoose_lib_MongooseNative_##f

JNIEXPORT jstring JNICALL
NATIVE_FUNC(getVersionInfo)(JNIEnv *env, jclass type) {
    return (*env)->NewStringUTF(env, "Mongoose/" MG_VERSION);
}

static bool serverRunning = false;
static char documentRoot[2048];

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
    struct http_message *hm = (struct http_message *) ev_data;
    struct mg_serve_http_opts opts = {.document_root = documentRoot};
    switch (ev) {
        case MG_EV_HTTP_REQUEST:
            mg_serve_http(nc, hm, opts);
            break;
        default:
            break;
    }
}

JNIEXPORT jint JNICALL
NATIVE_FUNC(nStartServer)(JNIEnv *env, jclass type, jstring port_, jstring rootPath_) {
    const char *port = (*env)->GetStringUTFChars(env, port_, 0);
    const char *rootPath = (*env)->GetStringUTFChars(env, rootPath_, 0);
    LOGD("Start Server [%s] [%s]", rootPath, port);
    strcpy(documentRoot, rootPath);
    int ret = 0;
    struct mg_mgr mgr;
    struct mg_connection *c;
    mg_mgr_init(&mgr, NULL);
    c = mg_bind(&mgr, port, ev_handler);
    mg_set_protocol_http_websocket(c);
    serverRunning = true;
    while (serverRunning) {
        mg_mgr_poll(&mgr, 500);
    }
    mg_mgr_free(&mgr);
    (*env)->ReleaseStringUTFChars(env, port_, port);
    (*env)->ReleaseStringUTFChars(env, rootPath_, rootPath);
    return ret;
}


JNIEXPORT jint JNICALL
NATIVE_FUNC(nStopServer)(JNIEnv *env, jclass type) {
    int ret = 0;
    serverRunning = false;
    LOGD("Stop Server!");
    return ret;
}

