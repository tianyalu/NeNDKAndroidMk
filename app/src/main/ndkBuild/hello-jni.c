#include <jni.h>

int test() {
    return 124;
}

//androidmk.ndk.ne.sty.com.nendkandroidmk
jint Java_androidmk_ndk_ne_sty_com_nendkandroidmk_MainActivity_nativeTest() {
return test();
}