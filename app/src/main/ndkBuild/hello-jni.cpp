#include <jni.h>
#include <string>

int test() {
    return 124;
}

//androidmk.ndk.ne.sty.com.nendkandroidmk

extern "C"
JNIEXPORT jstring JNICALL
 Java_androidmk_ndk_ne_sty_com_nendkandroidmk_MainActivity_stringFromJNI(JNIEnv *env, jobject thiz) {
    std::string hello = "hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT jint JNICALL
Java_androidmk_ndk_ne_sty_com_nendkandroidmk_MainActivity_nativeTest(JNIEnv *env, jobject thiz) {
    return test();
}