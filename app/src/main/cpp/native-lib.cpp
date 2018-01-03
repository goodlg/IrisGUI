#define LOG_TAG "Irisgui"
#define LOG_NDEBUG 0

#include <jni.h>
#include <string>
#include <android/log.h>
#include <dlfcn.h>
#include <assert.h>

#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define USE_MY 1
#if defined(USE_MY) && (USE_MY == 1)
#define IRIS_LIB_PATH "/system/lib/libHWMI.so"
#else
#define IRIS_LIB_PATH "/system/lib/libraw_interface.so"
#endif

#define UNUSED(x) (void)(x)

static void *s_handle = NULL;

typedef int (*CAC_FUNC)();
#if defined(USE_MY) && (USE_MY == 1)
typedef int (*CAC_FUNC1)(int);
#endif

/* API */
#if defined(USE_MY) && (USE_MY == 1)
CAC_FUNC Init = NULL;
CAC_FUNC1 Open = NULL;
#else
CAC_FUNC Open = NULL;
#endif
CAC_FUNC Close = NULL;
CAC_FUNC StartStream = NULL;
CAC_FUNC StopStream = NULL;

extern "C"
JNIEXPORT jint JNICALL
Java_org_ftd_gyn_IrisguiActicity_ApiInit(
        JNIEnv *env,
        jobject /* this */) {
    char *error;
    LOGI("[IR_JNI]: %s", __func__);

    s_handle = dlopen(IRIS_LIB_PATH, RTLD_NOW);
    if (NULL == s_handle) {
        LOGE("[IR_JNI]: Failed to get IRIS handle in %s()! (Reason=%s)\n", __FUNCTION__, dlerror());
        return FALSE;
    }

    //clear prev error
    dlerror();

#if defined(USE_MY) && (USE_MY == 1)
    *(void **) (&Init) = dlsym(s_handle, "hwmi_init");
    if ((error = dlerror()) != NULL)  {
        LOGE("[IR_JNI]: Failed to get hwmi_init handle in %s()! (Reason=%s)\n", __FUNCTION__, error);
        return FALSE;
    }

    *(void **) (&Open) = dlsym(s_handle, "mm_camera_open");
    if ((error = dlerror()) != NULL)  {
        LOGE("[IR_JNI]: Failed to get mm_camera_open handle in %s()! (Reason=%s)\n", __FUNCTION__, error);
        return FALSE;
    }

    *(void **) (&Close) = dlsym(s_handle, "mm_camera_close");
    if ((error = dlerror()) != NULL)  {
        LOGE("[IR_JNI]: Failed to get mm_camera_close handle in %s()! (Reason=%s)\n", __FUNCTION__, error);
        return FALSE;
    }

    if (Init() != 0) {
        LOGE("[IR_JNI]: hal init failed in %s()! (Reason=%s)\n", __FUNCTION__, error);
        return FALSE;
    }
#else
        *(void **) (&Open) = dlsym(s_handle, "RawCam_Open");
    if ((error = dlerror()) != NULL)  {
        LOGE("[IR_JNI]: Failed to get RawCam_Open handle in %s()! (Reason=%s)\n", __FUNCTION__, error);
        return FALSE;
    }

    *(void **) (&Close) = dlsym(s_handle, "RawCam_Close");
    if ((error = dlerror()) != NULL)  {
        LOGE("[IR_JNI]: Failed to get RawCam_Close handle in %s()! (Reason=%s)\n", __FUNCTION__, error);
        return FALSE;
    }

    *(void **) (&StartStream) = dlsym(s_handle, "RawCam_StartStream");
    if ((error = dlerror()) != NULL)  {
        LOGE("[IR_JNI]: Failed to get RawCam_StartStream handle in %s()! (Reason=%s)\n", __FUNCTION__, error);
        return FALSE;
    }

    *(void **) (&StopStream) = dlsym(s_handle, "RawCam_StopStream");
    if ((error = dlerror()) != NULL)  {
        LOGE("[IR_JNI]: Failed to get RawCam_StopStream handle in %s()! (Reason=%s)\n", __FUNCTION__, error);
        return FALSE;
    }
#endif
    LOGI("[IR_JNI]: %s INIT done!!!", __func__);
    return TRUE;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ftd_gyn_IrisguiActicity_ApiDeinit(
        JNIEnv *env,
        jobject /* this */) {
    if (s_handle != NULL) {
        dlclose(s_handle);
        LOGI("[IR_JNI]: ApiDeinit done!!!");
    }
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ftd_gyn_IrisguiActicity_RawCamOpen(
        JNIEnv *env,
        jobject obj,
        jint id) {
#if defined(USE_MY) && (USE_MY == 1)
    LOGI("[IR_JNI]: RawCamOpen (id %d)", id);
    return Open(id);
#else
    LOGI("[IR_JNI]: RawCamOpen");
    return Open();
#endif
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ftd_gyn_IrisguiActicity_RawCamClose(
        JNIEnv *env,
        jobject /* this */) {
    LOGI("[IR_JNI]: RawCamClose");
    return Close();
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ftd_gyn_IrisguiActicity_RawCamStartStream(
        JNIEnv *env,
        jobject /* this */) {
#if defined(USE_MY) && (USE_MY == 1)
#else
    LOGI("[IR_JNI]: RawCamStartStream");
    StartStream();
#endif
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_org_ftd_gyn_IrisguiActicity_RawCamStopStream(
        JNIEnv *env,
        jobject /* this */) {
#if defined(USE_MY) && (USE_MY == 1)
#else
    LOGI("[IR_JNI]: RawCamStopStream");
    StopStream();
#endif
    return 0;
}
