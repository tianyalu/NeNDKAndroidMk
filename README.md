# NeAndroid.mk Android.mk基础

## 一、概念
### 1.1 Android.mk简介
Android.mk是一个向Android NDK构建系统描述NDK项目的GNU Makefile片段，主要用来编译生成以下几种：
> APK程序：一般的Android应用程序，系统级别的直接push即可。      
> JAVA库：JAVA类库，编译打包生成JAR文件。  
> C/C++应用程序：可执行的C/C++应用程序。  
> C/C++静态库：编译生成C/C++静态库，并打包成.a文件。  
> C/C++共享库：编译生成共享库，并打包生成.so文件。  

### 1.2 Android.mk基本格式
以下是一个简单的Android.mk文件的内容:
```bash
# 定义模块当前路径（必须定义在文件开头，只需要定义一次）
LOCAL_PATH := $(call my-dir)

# 清空当前环境变量（LOCAL_PATH除外）
include $(CLEAR_VARS)

# 当前模块名（这里会生成libhello-jni.so）
LOCAL_MODULE := hello-jni.c

# 当前模块包含的源代码文件
LOCAL_SRC_FILES := hello-jni.c

# 表示当前模块将被编译成一个共享库
include $(BUILD_SHARED_LIBRARY)
```

### 1.3 编译多个共享库
一个Android.mk可能编译产生多个共享库模块。这里会产生libmodule1.so和libmodule2.so两个动态库
```bash
LOCAL_PATH := $(call my-dir)

# 模块1
include $(CLEAR_VARS)
LOCAL_MODULE := module1
LOCAL_SRC_FILES := module1.c
include $(BUILD_SHARED_LIBRARY)

# 模块2
include $(CLEAR_VARS)
LOCAL_MODULE := module2
LOCAL_SRC_FILES := module2.c
include $(BUILD_SHARDE_LIBRARY)
```

### 1.4 编译静态库
虽然Android应用程序不能直接使用静态库，静态库可以用来编译动态库。比如将第三方代码添加到原生项目中时，可以不用直接将第三方源码包括在原生项目中，而是将第三方源码编译成静态库，然后并入共享库。
```bash
LOCAL_PATH := $(call my-dir)

# 第三方AVI库
include $(CLEAR_VARS)
LOCAL_MODULE := avilib
LOCAL_SRC_FILES := avilib.c platform_posix.c
include $(BUILD_STATIC_LIBRARY)

# 原生模块
include $(CLEAR_VARS)
LOCAL_MODULE := module
LOCAL_SRC_FILES := module.c
# 将静态库模块名添加到LOCAL_STATIC_LIBRARIES变量
LOCAL_STATIC_LIBRARIES := avilib
include $(BUILD_SHARED_LIBRARY)
```

### 1.5 使用共享库共享通用模块
静态库可以保证源代码模块化，但是当静态库与共享库相连时，它就变成了共享库的一部分。在多个共享库的情况下，多个共享库与静态库连接时，需要将通用模块的多个副本与不同的共享库重复相连，这样就增加了APP的大小。这种情况，可以将通用模块作为共享库。
```bash
LOCAL_PATH := $(call my-dir)

# 第三方AVI库
include $(CLEAR_VARS)
LOCAL_MODULE := avilib
LOCAL_SRC_FILES := avilib.c platform_posix.c
include $(BUILD_SHARED_LIBRARY)

# 原生模块1
include $(CLEAR_VARS)
LOCAL_MODULE := module1
LOCAL_SRC_FILES := module1.c
LOCAL_SHARED_LIBRARIES := avilib
include $(BUILD_SHARED_LIBRARY)

# 原生模块2
include $(CLEAR_VARS)
LOCAL_MODULE := module2
LOCAL_SRC_FILES := module2.c
LOCAL_SHARED_LIBRARIES := avilib
include $(BUILD_SHARED_LIBRARY)
```

### 1.6 在多个NDK项目间共享模块
> 首先将avilib源代码移动到NDK项目以外的位置，比如：C:\android\shared-modules\transcode\avilib.  
> 作为共享块，avilib需要有自己的Android.mk文件.  
> 以transcode/avilib为参数调用函数宏import-module添加到NDK项目的Android.mk文档末尾.  

```bash
# avilib模块自己的Android.mk文件
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := avilib
LOCAL_SRC_FILES := avilib.c platform_posix.c
include $(BUILD_SHARED_LIBRARY)
---------------------------------------------
# 使用共享模块的NDK项目1的Android.mk文件
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := module1
LOCAL_SRC_FILES := modules1.c
LOCAL_SHARED_LIBRARIES := avilib
include $(BUILD_SHARED_LIBRARY)
$(call import-module,transcode/avilib)
----------------------------------------------
# 使用共享模块的NDK项目2的Android.mk文件
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := module2
LOCAL_SRC_FILES := modules2.c
LOCAL_SHARED_LIBRARIES := avilib
include $(BUILD_SHARED_LIBRARY)
$(call import-module,transcode/avilib)
```

### 1.7 使用预编译库
> 想在不发布源代码的情况下将模块发布给他人。  
> 想使用共享模块的预编译版来加速编译过程。  
```bash
# 预编译共享模块的Android.mk文件
LOCAL_PATH := $(call my-dir)
# 第三方预编译的库
include $(CLEAR_VARS)
LOCAL_MODULE := avilib
LOCAL_SRC_FILES := libavilib.so
include $(PREBUILT_SHARED_LIBRARY)
```

### 1.7 使用预编译库
为了方便测试和进行快速开发，可以编译成可执行文件。不用打包成apk就可以复制到Android设备上直接执行  
```bash
# 度量可执行模块的Android.mk文件
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := module
LOCAL_SRC_FILES := module.c
LOCAL_STATIC_LIBRARIES := avilib
include $(BUILD_EXECUTABLE)
```

### 注意
> 假如我们本地库libhello-jni.so依赖于libTest.so(可以使用NDK下的ndk-depends查看so的依赖关系).    
> 在Android6.0版本之前，需要在加载本地库前先加载被依赖的so.  
> 在Android6.0版本之后，不能再使用预编译的动态库（静态库没问题）.  
```bash
# Android 6.0版本之前：
System.loadLibrary("Test");
System.loadLibrary("hello-jni");

# Andnoid 6.0之后
System.loadLibrary("hello-jni");
```

## 二、实操
2.1 创建hello-jni.c文件
AS新建项目，在app/src/main下新建ndkBuild目录，在该目录下新建hello-jni.c文件。
```c++
#include <jni.h>

int test() {
    return 124;
}

//androidmk.ndk.ne.sty.com.nendkandroidmk
jint Java_androidmk_ndk_ne_sty_com_nendkandroidmk_MainActivity_nativeTest() {
    return test();
}
```
2.1 MainActivity中添加本地方法
```java
native int nativeTest();
```
2.3 在ndkBuild目录下新建Android.mk文件
```bash
# 定义模块当前路径
LOCAL_PATH := $(call my-dir)

# 清空当前环境变量
include $(CLEAR_VARS)

# 当前模块名
LOCAL_MODULE := hello-jni

# 当前模块包含的源代码文件
LOCAL_SRC_FILES := hello-jni.c

# 生成一个动态库
include $(BUILD_SHARED_LIBRARY)
```
2.4 配置build.gradle
```bash
    defaultConfig {
        applicationId "androidmk.ndk.ne.sty.com.nendkandroidmk"
        minSdkVersion 17
        targetSdkVersion 26
        versionCode 1
        versionName "1.0"
        testInstrumentationRunner "android.support.test.runner.AndroidJUnitRunner"

        //指定源文件的编译方式以及配置编译选项
        externalNativeBuild {
            ndkBuild {
                abiFilters "armeabi-v7a", "x86"
            }
        }
    }
    //配置native的编译脚本路径
    externalNativeBuild {
        ndkBuild {
            path "src/main/ndkBuild/Android.mk"
        }
    }
```
完成后build APK，可以在app/build/outputs/apk/debug下发现生成的APK，打开APK，可以发现lib目录中生成了
libhello-jni.so动态库。  
2.5 在MainActivity中调用native方法
```java
public class MainActivity extends AppCompatActivity {

    {
        System.loadLibrary("hello-jni");
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        TextView textView = findViewById(R.id.tv_text);
        textView.setText("nativeTest: " + nativeTest());
    }

    native int nativeTest();
}
```