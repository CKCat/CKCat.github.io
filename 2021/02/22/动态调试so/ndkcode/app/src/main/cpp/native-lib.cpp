#include <jni.h>
#include <string>
#include <stdio.h>
#include <android/log.h>

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "MainActivity", __VA_ARGS__);

class A{
public:
    A(int i){
        LOGD("A::A() %d", i);
    }

    ~A(){
        LOGD("~A::A()");
    }
};

//
//A a(1); __attribute__((init_priority(2000)));
//A b(2); __attribute__((init_priority(543)));
//A aa(3); __attribute__((constructor));
//
//__attribute__ ((visibility ("hidden"))) int test(){
//    LOGD("test");
//}
//int n = test();


//__attribute__((constructor (-1))) void test_constructor1(){
//    LOGD("test_constructor-1");
//}
//
//__attribute__((constructor (0))) void test_constructor2(){
//    LOGD("test_constructor0");
//}
//
//__attribute__((constructor (3))) void test_constructor3(){
//    LOGD("test_constructor3");
//}

//visibility ("hidden")


__attribute__((constructor ))
void test_constructor(){
    LOGD("test_constructor");
}

#ifdef __cplusplus
extern "C" {
#endif

void _init(void){
    LOGD("_init enter");
}

#ifdef __cplusplus
}
#endif

bool IsException(JNIEnv *pEnv)
{
    if (pEnv->ExceptionCheck())
    {
        pEnv->ExceptionDescribe();
        pEnv->ExceptionClear();
        return true;
    }
    return false;
}

#ifdef __cplusplus
extern "C" {
#endif

__attribute__ ((visibility ("hidden"))) jint JNICALL MyAdd_INT  //函数名可以随意
        (JNIEnv *pEnv, jobject obj, jint n1, jint n2)
{
    return n1 + n2;
}
#ifdef __cplusplus
}
#endif



__attribute__ ((visibility ("hidden"))) JNINativeMethod g_NativeMethod[] = {
        "MyAdd", "(II)I", (void*)MyAdd_INT     //java中定义的函数名， 函数原型， 对应native层的函数指针
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    LOGD("JNI_OnLoad");


    JNIEnv *pEnv = NULL;
    int version = vm->GetEnv((void**)&pEnv, JNI_VERSION_1_4);
    LOGD("GetEnv:%d", version);
    if (pEnv == NULL)
    {
        LOGD("GetEnv error");
        return JNI_VERSION_1_4;
    }
    LOGD("JNI_OnLoad GetEnv=%p", pEnv);
    jclass clsMainActivity = pEnv->FindClass("com/example/ndkcode/MainActivity");
    if (IsException(pEnv))
        return JNI_VERSION_1_4;

    pEnv->RegisterNatives(clsMainActivity, g_NativeMethod,
                          sizeof(g_NativeMethod) / sizeof(g_NativeMethod[0]));
    if (IsException(pEnv))
        return JNI_VERSION_1_4;

    return JNI_VERSION_1_4;
}


extern "C" JNIEXPORT jstring JNICALL
Java_com_example_ndkcode_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {

    std::string hello = "Hello from C++";

    return env->NewStringUTF(hello.c_str());
}
