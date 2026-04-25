#include <jni.h>
#include <string>
#include <algorithm>
#include <climits>
#include <cstring>
#include <cstdio>
#include <android/log.h>
#include <sys/ptrace.h>

#define LOG_TAG "ANTI_DEBUG"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static bool isBeingTraced() {
    long resultat = ptrace(PTRACE_TRACEME, 0, 0, 0);
    if (resultat == -1) {
        LOGE("isBeingTraced : trace active detectee");
        return true;
    }
    LOGI("isBeingTraced : aucun traceur detecte");
    return false;
}

static bool containsSuspiciousLibraryNames() {
    FILE* maps = fopen("/proc/self/maps", "r");
    if (!maps) {
        LOGW("containsSuspiciousLibraryNames : lecture impossible");
        return false;
    }

    char ligne[512];
    const char* signatures[] = {
            "frida", "xposed", "libfrida",
            "gdbserver", "libgdb", "magisk"
    };
    int nbSignatures = 6;

    while (fgets(ligne, sizeof(ligne), maps)) {
        for (int i = 0; i < nbSignatures; i++) {
            if (strstr(ligne, signatures[i])) {
                LOGE("containsSuspiciousLibraryNames : [%s] detecte", signatures[i]);
                fclose(maps);
                return true;
            }
        }
    }

    fclose(maps);
    LOGI("containsSuspiciousLibraryNames : aucune signature suspecte");
    return false;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_jnidemo_MainActivity_isDebugDetected(
        JNIEnv* env,
        jobject /* thiz */) {

    bool traced         = isBeingTraced();
    bool suspiciousMaps = containsSuspiciousLibraryNames();

    if (traced || suspiciousMaps) {
        LOGE("isDebugDetected : environnement suspect confirme");
        return JNI_TRUE;
    }

    LOGI("isDebugDetected : environnement OK");
    return JNI_FALSE;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_jnidemo_MainActivity_helloFromJNI(
        JNIEnv* env,
        jobject /* thiz */) {

    LOGI("helloFromJNI : execution autorisee");
    return env->NewStringUTF("Bonjour depuis le C++ natif via JNI !");
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_jnidemo_MainActivity_factorial(
        JNIEnv* env,
        jobject /* thiz */,
        jint n) {

    if (n < 0) {
        LOGE("factorial : valeur negative refusee (%d)", n);
        return -1;
    }

    long long resultat = 1;
    for (int i = 2; i <= n; i++) {
        resultat *= i;
        if (resultat > INT_MAX) {
            LOGE("factorial : depassement pour n=%d", n);
            return -2;
        }
    }

    LOGI("factorial(%d) = %lld", n, resultat);
    return static_cast<jint>(resultat);
}