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
2.1 创建hello-jni.cpp文件
AS新建项目，在app/src/main下新建ndkBuild目录，在该目录下新建hello-jni.cpp文件。
```c++
#include <jni.h>
//#include <string> //引入<string>头文件会报错，目前不知道问题所在

int test() {
    return 124;
}

//androidmk.ndk.ne.sty.com.nendkandroidmk

extern "C"
JNIEXPORT jint JNICALL
Java_androidmk_ndk_ne_sty_com_nendkandroidmk_MainActivity_nativeTest(JNIEnv *env, jobject thiz) {
//    std::string hello = "hello from C++";
    return test();
}

extern "C"
JNIEXPORT jstring JNICALL
Java_androidmk_ndk_ne_sty_com_nendkandroidmk_MainActivity_stringFromJNI(JNIEnv *env, jobject thiz) {
//    std::string hello = "hello from C++";
//    return env->NewStringUTF(hello.c_str());
    return env->NewStringUTF("hello from C++");
}
```
2.1 MainActivity中添加本地方法
```java
public native int nativeTest();
public native String stringFromJNI();
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
        textView.setText("nativeTest: " + nativeTest() + "-->" + stringFromJNI());
    }

    public native int nativeTest();

    public native String stringFromJNI();
}
```

## 三、补充-NDK基础
### 3.1 Android.mk构建系统
#### 3.1.1 其他构建系统变量
* TARGET_ARCH：目标CPU体系结构的名称，例如arm  
* TARGET_PLATFORM：目标Android平台的名称，例如android-22  
* TARGET_ARCH_ABI：目标CPU体系结构和ABI的名称，例如armeabi-v7a  
* TARGET_ABI：目标平台和ABI的串联，例如android-22-armeabi-v7a  

CPU and architecture  | Setting  
--------------------- | -------
ARMv5TE               | armeabi
ARMv7                 | armeabi-v7a
ARMv8 AArch64         | arm64-v8a
i686                  | x86
x86-64                | x86_64
mips32                | mips
mips64                | mips64
All                   | all  

* LOCAL_MODULE_FILENAME：可选变量，用来重新定义生成的输出文件名称  
* LOCAL_CPP_EXTENSION：C++源文件的默认扩展名为.cpp，这个变量可以用来为C++源文件指定一个或多个文件扩展名  
* LOCAL_CPP_FEATURES：可选变量，用来指明模块所依赖的具体C++特性，如RTTI、exceptions等  
* LOCAL_C_INCLUDES：可选目录列表，NDK安装目录的相对路径，用来搜素头文件  
* LOCAL_CFLAGS：一组可选的编译器标志，在编译C和C++源文件的时候会被传送给编译器  
* LOCAL_CPP_FLAGS：一组可选的编译标志，在只编译C++源文件的时候被传送给编译器  
* LOCAL_WHOLE_STATIC_LIBRARIES：LOCAL_STATIC_LIBRARIES的变体，用来指明应该被包含在生成的共享库中的所有静态库内容  
* LOCAL_ARM_MODE：可选参数，ARM机器体系结构特有变量，用于指定要生成的ARM二进制类型  

#### 3.1.2 定义新变量
开发人员可以定义其他变量来简化它们的构建文件。以LOCAL_和NDK_前缀开头的名称预留给Android NDK构建系统使用。  
```cmakecache
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := hello-jni

MY_SRC_FILES := src/main/cpp/hello-jni.cpp  

LOCAL_SRC_FILES := $(MY_SRC_FILES)
include $(BUILD_SHARED_LIBRARY)
```

#### 3.1.3 Application.mk
Application.mk 是 Android NDK 构建系统使用的一个可选构建文件。  
* APP_OPTION：改变量可以被设置为 release 或 debug 以改变生成的二进制文件的优化级别  
* APP_CFLAGS：该变量列出了一些编译器标志，在编译任何模块的 C 和 C++ 源文件时这些标志都会被传送给编译器  
* APP_CPPFLAGS：该变量列出了一些编译器标志，在编译任何模块的 C++ 源文件时这些标志都会被传送给编译器  
* APP_ABI：该变量指定编译的目标 ABI 文件  
* APP_STL：默认情况下，Android NDK 构建系统使用最小 STL 运行库，也被称为 system 库，可以用该变量选择不同的STL实现  
 
#### 3.1.4 C++ Library Support
* Standard C++ Library support  
* C++ exceptions support  
* RTTI support  
 
名称                 | 说明                                           | 功能  
------------------- | ---------------------------------------------- | ------------------------------------------  
libstdc++ (default) | The default minimal system C++ runtime library | N/A
gabi++_static       | The GAbi++ runtime (static)                    | C++ Exceptions and RTTI
gabi++_shared       | The GAbi++ runtime (shared)                    | C++ Exceptions and RTTI
stlport_static      | The STLport runtime (static)                   | C++ Exceptions and RTTI; Standard Library
stlport_shared      | The STLport runtime (shared)                   | C++ Exceptions and RTTI; Standard Library
gnustl_static       | The GNU STL (static)                           | C++ Exceptions and RTTI; Standard Library
gnustl_shared       | The GNU STL (shared)                           | C++ Exceptions and RTTI; Standard Library
c++_static          | The LLVM libc++ runtime (static)               | C++ Exceptions and RTTI; Standard Library
c++_shared          | The LLVM libc++ runtime (shared)               | C++ Exceptions and RTTI; Standard Library  

### 3.2 JNI 
#### 3.2.1 概念
JNI是Java Native Interface的缩写，它提供了若干API实现了Java和其他语言的通信。  
> Java代码如何调用原生方法  
> 声明原生方法  
> 加载共享库  
> 在C/C++中实现原生方法  
#### 3.2.1 Java代码调用原生方法 
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
        textView.setText("nativeTest: " + nativeTest() + "-->" + stringFromJNI());
    }
}
```
#### 3.2.2 声明原生方法
```java
/**
 * 原生方法由‘hello-jni' 原生库实现
 * 该原生库与应用程序一起打包
 */
public native int nativeTest();

public native String stringFromJNI();
```

#### 3.2.3 加载共享库
```java
/**
 * 这段代码用于在应用启动时加载‘hello_jni' 共享库
 * 该库在应用程序安装时由包管理器解压到 /data/data/androidmk.ndk.ne.sty.com.nendkandroidmk/lib/libhello_jni.so中
 */
static {
    System.loadLibrary("hello-jni");
}
```

#### 3.2.4 实现原生方法 
```c++
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
```