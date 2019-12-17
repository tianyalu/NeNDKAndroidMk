#include <jni.h>
//#include <string> //引入<string>头文件会报错，目前不知道问题所在

int test() {
    return 124;
}

//androidmk.ndk.ne.sty.com.nendkandroidmk

extern "C"
JNIEXPORT jint JNICALL
Java_androidmk_ndk_ne_sty_com_nendkandroidmk_MainActivity_nativeTest(JNIEnv *env, jobject thiz) {
    return test();
}

extern "C"
JNIEXPORT jstring JNICALL
Java_androidmk_ndk_ne_sty_com_nendkandroidmk_MainActivity_stringFromJNI(JNIEnv *env, jobject thiz) {
//    std::string hello = "hello from C++";
//    return env->NewStringUTF(hello.c_str());
    return env->NewStringUTF("hello from C++");
}