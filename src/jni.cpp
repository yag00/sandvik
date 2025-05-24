#include "jni.hpp"

#include <string.h>

#include <fmt/format.h>

#include <stdexcept>

#include "jni/jni.h"

#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

NativeInterface::NativeInterface() {
	_interface = std::make_unique<JNINativeInterface_>();
	functions = _interface.get();

	_interface->reserved0 = nullptr;
	_interface->reserved1 = nullptr;
	_interface->reserved2 = nullptr;
	_interface->reserved3 = nullptr;
	// Initialize the function pointers with the implemented methods
	_interface->GetVersion = &NativeInterface::NativeInterface::GetVersion;
	_interface->DefineClass = &NativeInterface::DefineClass;
	_interface->FindClass = &NativeInterface::FindClass;
	_interface->FromReflectedMethod = &NativeInterface::FromReflectedMethod;
	_interface->FromReflectedField = &NativeInterface::FromReflectedField;
	_interface->ToReflectedMethod = &NativeInterface::ToReflectedMethod;
	_interface->GetSuperclass = &NativeInterface::GetSuperclass;
	_interface->IsAssignableFrom = &NativeInterface::IsAssignableFrom;
	_interface->ToReflectedField = &NativeInterface::ToReflectedField;
	_interface->Throw = &NativeInterface::Throw;
	_interface->ThrowNew = &NativeInterface::ThrowNew;
	_interface->ExceptionOccurred = &NativeInterface::ExceptionOccurred;
	_interface->ExceptionDescribe = &NativeInterface::ExceptionDescribe;
	_interface->ExceptionClear = &NativeInterface::ExceptionClear;
	_interface->FatalError = &NativeInterface::FatalError;
	_interface->PushLocalFrame = &NativeInterface::PushLocalFrame;
	_interface->PopLocalFrame = &NativeInterface::PopLocalFrame;
	_interface->NewGlobalRef = &NativeInterface::NewGlobalRef;
	_interface->DeleteGlobalRef = &NativeInterface::DeleteGlobalRef;
	_interface->DeleteLocalRef = &NativeInterface::DeleteLocalRef;
	_interface->IsSameObject = &NativeInterface::IsSameObject;
	_interface->NewLocalRef = &NativeInterface::NewLocalRef;
	_interface->EnsureLocalCapacity = &NativeInterface::EnsureLocalCapacity;
	_interface->AllocObject = &NativeInterface::AllocObject;
	_interface->NewObject = &NativeInterface::NewObject;
	_interface->NewObjectV = &NativeInterface::NewObjectV;
	_interface->NewObjectA = &NativeInterface::NewObjectA;
	_interface->GetObjectClass = &NativeInterface::GetObjectClass;
	_interface->IsInstanceOf = &NativeInterface::IsInstanceOf;
	_interface->GetMethodID = &NativeInterface::GetMethodID;
	_interface->CallObjectMethod = &NativeInterface::CallObjectMethod;
	_interface->CallObjectMethodV = &NativeInterface::CallObjectMethodV;
	_interface->CallObjectMethodA = &NativeInterface::CallObjectMethodA;
	_interface->CallBooleanMethod = &NativeInterface::CallBooleanMethod;
	_interface->CallBooleanMethodV = &NativeInterface::CallBooleanMethodV;
	_interface->CallBooleanMethodA = &NativeInterface::CallBooleanMethodA;
	_interface->CallByteMethod = &NativeInterface::CallByteMethod;
	_interface->CallByteMethodV = &NativeInterface::CallByteMethodV;
	_interface->CallByteMethodA = &NativeInterface::CallByteMethodA;
	_interface->CallCharMethod = &NativeInterface::CallCharMethod;
	_interface->CallCharMethodV = &NativeInterface::CallCharMethodV;
	_interface->CallCharMethodA = &NativeInterface::CallCharMethodA;
	_interface->CallShortMethod = &NativeInterface::CallShortMethod;
	_interface->CallShortMethodV = &NativeInterface::CallShortMethodV;
	_interface->CallShortMethodA = &NativeInterface::CallShortMethodA;
	_interface->CallIntMethod = &NativeInterface::CallIntMethod;
	_interface->CallIntMethodV = &NativeInterface::CallIntMethodV;
	_interface->CallIntMethodA = &NativeInterface::CallIntMethodA;
	_interface->CallLongMethod = &NativeInterface::CallLongMethod;
	_interface->CallLongMethodV = &NativeInterface::CallLongMethodV;
	_interface->CallLongMethodA = &NativeInterface::CallLongMethodA;
	_interface->CallFloatMethod = &NativeInterface::CallFloatMethod;
	_interface->CallFloatMethodV = &NativeInterface::CallFloatMethodV;
	_interface->CallFloatMethodA = &NativeInterface::CallFloatMethodA;
	_interface->CallDoubleMethod = &NativeInterface::CallDoubleMethod;
	_interface->CallDoubleMethodV = &NativeInterface::CallDoubleMethodV;
	_interface->CallDoubleMethodA = &NativeInterface::CallDoubleMethodA;
	_interface->CallVoidMethod = &NativeInterface::CallVoidMethod;
	_interface->CallVoidMethodV = &NativeInterface::CallVoidMethodV;
	_interface->CallVoidMethodA = &NativeInterface::CallVoidMethodA;
	_interface->CallNonvirtualObjectMethod = &NativeInterface::CallNonvirtualObjectMethod;
	_interface->CallNonvirtualObjectMethodV = &NativeInterface::CallNonvirtualObjectMethodV;
	_interface->CallNonvirtualObjectMethodA = &NativeInterface::CallNonvirtualObjectMethodA;
	_interface->CallNonvirtualBooleanMethod = &NativeInterface::CallNonvirtualBooleanMethod;
	_interface->CallNonvirtualBooleanMethodV = &NativeInterface::CallNonvirtualBooleanMethodV;
	_interface->CallNonvirtualBooleanMethodA = &NativeInterface::CallNonvirtualBooleanMethodA;
	_interface->CallNonvirtualByteMethod = &NativeInterface::CallNonvirtualByteMethod;
	_interface->CallNonvirtualByteMethodV = &NativeInterface::CallNonvirtualByteMethodV;
	_interface->CallNonvirtualByteMethodA = &NativeInterface::CallNonvirtualByteMethodA;
	_interface->CallNonvirtualCharMethod = &NativeInterface::CallNonvirtualCharMethod;
	_interface->CallNonvirtualCharMethodV = &NativeInterface::CallNonvirtualCharMethodV;
	_interface->CallNonvirtualCharMethodA = &NativeInterface::CallNonvirtualCharMethodA;
	_interface->CallNonvirtualShortMethod = &NativeInterface::CallNonvirtualShortMethod;
	_interface->CallNonvirtualShortMethodV = &NativeInterface::CallNonvirtualShortMethodV;
	_interface->CallNonvirtualShortMethodA = &NativeInterface::CallNonvirtualShortMethodA;
	_interface->CallNonvirtualIntMethod = &NativeInterface::CallNonvirtualIntMethod;
	_interface->CallNonvirtualIntMethodV = &NativeInterface::CallNonvirtualIntMethodV;
	_interface->CallNonvirtualIntMethodA = &NativeInterface::CallNonvirtualIntMethodA;
	_interface->CallNonvirtualLongMethod = &NativeInterface::CallNonvirtualLongMethod;
	_interface->CallNonvirtualLongMethodV = &NativeInterface::CallNonvirtualLongMethodV;
	_interface->CallNonvirtualLongMethodA = &NativeInterface::CallNonvirtualLongMethodA;
	_interface->CallNonvirtualFloatMethod = &NativeInterface::CallNonvirtualFloatMethod;
	_interface->CallNonvirtualFloatMethodV = &NativeInterface::CallNonvirtualFloatMethodV;
	_interface->CallNonvirtualFloatMethodA = &NativeInterface::CallNonvirtualFloatMethodA;
	_interface->CallNonvirtualDoubleMethod = &NativeInterface::CallNonvirtualDoubleMethod;
	_interface->CallNonvirtualDoubleMethodV = &NativeInterface::CallNonvirtualDoubleMethodV;
	_interface->CallNonvirtualDoubleMethodA = &NativeInterface::CallNonvirtualDoubleMethodA;
	_interface->CallNonvirtualVoidMethod = &NativeInterface::CallNonvirtualVoidMethod;
	_interface->CallNonvirtualVoidMethodV = &NativeInterface::CallNonvirtualVoidMethodV;
	_interface->CallNonvirtualVoidMethodA = &NativeInterface::CallNonvirtualVoidMethodA;
	_interface->GetFieldID = &NativeInterface::GetFieldID;
	_interface->GetObjectField = &NativeInterface::GetObjectField;
	_interface->GetBooleanField = &NativeInterface::GetBooleanField;
	_interface->GetByteField = &NativeInterface::GetByteField;
	_interface->GetCharField = &NativeInterface::GetCharField;
	_interface->GetShortField = &NativeInterface::GetShortField;
	_interface->GetIntField = &NativeInterface::GetIntField;
	_interface->GetLongField = &NativeInterface::GetLongField;
	_interface->GetFloatField = &NativeInterface::GetFloatField;
	_interface->GetDoubleField = &NativeInterface::GetDoubleField;
	_interface->SetObjectField = &NativeInterface::SetObjectField;
	_interface->SetBooleanField = &NativeInterface::SetBooleanField;
	_interface->SetByteField = &NativeInterface::SetByteField;
	_interface->SetCharField = &NativeInterface::SetCharField;
	_interface->SetShortField = &NativeInterface::SetShortField;
	_interface->SetIntField = &NativeInterface::SetIntField;
	_interface->SetLongField = &NativeInterface::SetLongField;
	_interface->SetFloatField = &NativeInterface::SetFloatField;
	_interface->SetDoubleField = &NativeInterface::SetDoubleField;
	_interface->GetStaticMethodID = &NativeInterface::GetStaticMethodID;
	_interface->CallStaticObjectMethod = &NativeInterface::CallStaticObjectMethod;
	_interface->CallStaticObjectMethodV = &NativeInterface::CallStaticObjectMethodV;
	_interface->CallStaticObjectMethodA = &NativeInterface::CallStaticObjectMethodA;
	_interface->CallStaticBooleanMethod = &NativeInterface::CallStaticBooleanMethod;
	_interface->CallStaticBooleanMethodV = &NativeInterface::CallStaticBooleanMethodV;
	_interface->CallStaticBooleanMethodA = &NativeInterface::CallStaticBooleanMethodA;
	_interface->CallStaticByteMethod = &NativeInterface::CallStaticByteMethod;
	_interface->CallStaticByteMethodV = &NativeInterface::CallStaticByteMethodV;
	_interface->CallStaticByteMethodA = &NativeInterface::CallStaticByteMethodA;
	_interface->CallStaticCharMethod = &NativeInterface::CallStaticCharMethod;
	_interface->CallStaticCharMethodV = &NativeInterface::CallStaticCharMethodV;
	_interface->CallStaticCharMethodA = &NativeInterface::CallStaticCharMethodA;
	_interface->CallStaticShortMethod = &NativeInterface::CallStaticShortMethod;
	_interface->CallStaticShortMethodV = &NativeInterface::CallStaticShortMethodV;
	_interface->CallStaticShortMethodA = &NativeInterface::CallStaticShortMethodA;
	_interface->CallStaticIntMethod = &NativeInterface::CallStaticIntMethod;
	_interface->CallStaticIntMethodV = &NativeInterface::CallStaticIntMethodV;
	_interface->CallStaticIntMethodA = &NativeInterface::CallStaticIntMethodA;
	_interface->CallStaticLongMethod = &NativeInterface::CallStaticLongMethod;
	_interface->CallStaticLongMethodV = &NativeInterface::CallStaticLongMethodV;
	_interface->CallStaticLongMethodA = &NativeInterface::CallStaticLongMethodA;
	_interface->CallStaticFloatMethod = &NativeInterface::CallStaticFloatMethod;
	_interface->CallStaticFloatMethodV = &NativeInterface::CallStaticFloatMethodV;
	_interface->CallStaticFloatMethodA = &NativeInterface::CallStaticFloatMethodA;
	_interface->CallStaticDoubleMethod = &NativeInterface::CallStaticDoubleMethod;
	_interface->CallStaticDoubleMethodV = &NativeInterface::CallStaticDoubleMethodV;
	_interface->CallStaticDoubleMethodA = &NativeInterface::CallStaticDoubleMethodA;
	_interface->CallStaticVoidMethod = &NativeInterface::CallStaticVoidMethod;
	_interface->CallStaticVoidMethodV = &NativeInterface::CallStaticVoidMethodV;
	_interface->CallStaticVoidMethodA = &NativeInterface::CallStaticVoidMethodA;
	_interface->GetStaticFieldID = &NativeInterface::GetStaticFieldID;
	_interface->GetStaticObjectField = &NativeInterface::GetStaticObjectField;
	_interface->GetStaticBooleanField = &NativeInterface::GetStaticBooleanField;
	_interface->GetStaticByteField = &NativeInterface::GetStaticByteField;
	_interface->GetStaticCharField = &NativeInterface::GetStaticCharField;
	_interface->GetStaticShortField = &NativeInterface::GetStaticShortField;
	_interface->GetStaticIntField = &NativeInterface::GetStaticIntField;
	_interface->GetStaticLongField = &NativeInterface::GetStaticLongField;
	_interface->GetStaticFloatField = &NativeInterface::GetStaticFloatField;
	_interface->GetStaticDoubleField = &NativeInterface::GetStaticDoubleField;
	_interface->SetStaticObjectField = &NativeInterface::SetStaticObjectField;
	_interface->SetStaticBooleanField = &NativeInterface::SetStaticBooleanField;
	_interface->SetStaticByteField = &NativeInterface::SetStaticByteField;
	_interface->SetStaticCharField = &NativeInterface::SetStaticCharField;
	_interface->SetStaticShortField = &NativeInterface::SetStaticShortField;
	_interface->SetStaticIntField = &NativeInterface::SetStaticIntField;
	_interface->SetStaticLongField = &NativeInterface::SetStaticLongField;
	_interface->SetStaticFloatField = &NativeInterface::SetStaticFloatField;
	_interface->SetStaticDoubleField = &NativeInterface::SetStaticDoubleField;
	_interface->NewString = &NativeInterface::NewString;
	_interface->GetStringLength = &NativeInterface::GetStringLength;
	_interface->GetStringChars = &NativeInterface::GetStringChars;
	_interface->ReleaseStringChars = &NativeInterface::ReleaseStringChars;
	_interface->NewStringUTF = &NativeInterface::NewStringUTF;
	_interface->GetStringUTFLength = &NativeInterface::GetStringUTFLength;
	_interface->GetStringUTFChars = &NativeInterface::GetStringUTFChars;
	_interface->ReleaseStringUTFChars = &NativeInterface::ReleaseStringUTFChars;
	_interface->GetArrayLength = &NativeInterface::GetArrayLength;
	_interface->NewObjectArray = &NativeInterface::NewObjectArray;
	_interface->GetObjectArrayElement = &NativeInterface::GetObjectArrayElement;
	_interface->SetObjectArrayElement = &NativeInterface::SetObjectArrayElement;
	_interface->NewBooleanArray = &NativeInterface::NewBooleanArray;
	_interface->NewByteArray = &NativeInterface::NewByteArray;
	_interface->NewCharArray = &NativeInterface::NewCharArray;
	_interface->NewShortArray = &NativeInterface::NewShortArray;
	_interface->NewIntArray = &NativeInterface::NewIntArray;
	_interface->NewLongArray = &NativeInterface::NewLongArray;
	_interface->NewFloatArray = &NativeInterface::NewFloatArray;
	_interface->NewDoubleArray = &NativeInterface::NewDoubleArray;
	_interface->GetBooleanArrayElements = &NativeInterface::GetBooleanArrayElements;
	_interface->GetByteArrayElements = &NativeInterface::GetByteArrayElements;
	_interface->GetCharArrayElements = &NativeInterface::GetCharArrayElements;
	_interface->GetShortArrayElements = &NativeInterface::GetShortArrayElements;
	_interface->GetIntArrayElements = &NativeInterface::GetIntArrayElements;
	_interface->GetLongArrayElements = &NativeInterface::GetLongArrayElements;
	_interface->GetFloatArrayElements = &NativeInterface::GetFloatArrayElements;
	_interface->GetDoubleArrayElements = &NativeInterface::GetDoubleArrayElements;
	_interface->ReleaseBooleanArrayElements = &NativeInterface::ReleaseBooleanArrayElements;
	_interface->ReleaseByteArrayElements = &NativeInterface::ReleaseByteArrayElements;
	_interface->ReleaseCharArrayElements = &NativeInterface::ReleaseCharArrayElements;
	_interface->ReleaseShortArrayElements = &NativeInterface::ReleaseShortArrayElements;
	_interface->ReleaseIntArrayElements = &NativeInterface::ReleaseIntArrayElements;
	_interface->ReleaseLongArrayElements = &NativeInterface::ReleaseLongArrayElements;
	_interface->ReleaseFloatArrayElements = &NativeInterface::ReleaseFloatArrayElements;
	_interface->ReleaseDoubleArrayElements = &NativeInterface::ReleaseDoubleArrayElements;
	_interface->GetBooleanArrayRegion = &NativeInterface::GetBooleanArrayRegion;
	_interface->GetByteArrayRegion = &NativeInterface::GetByteArrayRegion;
	_interface->GetCharArrayRegion = &NativeInterface::GetCharArrayRegion;
	_interface->GetShortArrayRegion = &NativeInterface::GetShortArrayRegion;
	_interface->GetIntArrayRegion = &NativeInterface::GetIntArrayRegion;
	_interface->GetLongArrayRegion = &NativeInterface::GetLongArrayRegion;
	_interface->GetFloatArrayRegion = &NativeInterface::GetFloatArrayRegion;
	_interface->GetDoubleArrayRegion = &NativeInterface::GetDoubleArrayRegion;
	_interface->SetBooleanArrayRegion = &NativeInterface::SetBooleanArrayRegion;
	_interface->SetByteArrayRegion = &NativeInterface::SetByteArrayRegion;
	_interface->SetCharArrayRegion = &NativeInterface::SetCharArrayRegion;
	_interface->SetShortArrayRegion = &NativeInterface::SetShortArrayRegion;
	_interface->SetIntArrayRegion = &NativeInterface::SetIntArrayRegion;
	_interface->SetLongArrayRegion = &NativeInterface::SetLongArrayRegion;
	_interface->SetFloatArrayRegion = &NativeInterface::SetFloatArrayRegion;
	_interface->SetDoubleArrayRegion = &NativeInterface::SetDoubleArrayRegion;
	_interface->RegisterNatives = &NativeInterface::RegisterNatives;
	_interface->UnregisterNatives = &NativeInterface::UnregisterNatives;
	_interface->MonitorEnter = &NativeInterface::MonitorEnter;
	_interface->MonitorExit = &NativeInterface::MonitorExit;
	_interface->GetJavaVM = &NativeInterface::GetJavaVM;
	_interface->GetStringRegion = &NativeInterface::GetStringRegion;
	_interface->GetStringUTFRegion = &NativeInterface::GetStringUTFRegion;
	_interface->GetPrimitiveArrayCritical = &NativeInterface::GetPrimitiveArrayCritical;
	_interface->ReleasePrimitiveArrayCritical = &NativeInterface::ReleasePrimitiveArrayCritical;
	_interface->GetStringCritical = &NativeInterface::GetStringCritical;
	_interface->ReleaseStringCritical = &NativeInterface::ReleaseStringCritical;
	_interface->NewWeakGlobalRef = &NativeInterface::NewWeakGlobalRef;
	_interface->DeleteWeakGlobalRef = &NativeInterface::DeleteWeakGlobalRef;
	_interface->ExceptionCheck = &NativeInterface::ExceptionCheck;
	_interface->NewDirectByteBuffer = &NativeInterface::NewDirectByteBuffer;
	_interface->GetDirectBufferAddress = &NativeInterface::GetDirectBufferAddress;
	_interface->GetDirectBufferCapacity = &NativeInterface::GetDirectBufferCapacity;
	_interface->GetObjectRefType = &NativeInterface::GetObjectRefType;
}
NativeInterface::~NativeInterface() {
}

// ----------------------------------------------------------------------------
// Implementations of the JNI functions
// ----------------------------------------------------------------------------

jint NativeInterface::GetVersion(JNIEnv *env) {
	return JNI_VERSION_1_6;
}
jclass NativeInterface::DefineClass(JNIEnv *env, const char *name, jobject loader, const jbyte *buf, jsize len) {
	throw std::runtime_error("DefineClass not implemented");
}
jclass NativeInterface::FindClass(JNIEnv *env, const char *name) {
	throw std::runtime_error("FindClass not implemented");
}
jmethodID NativeInterface::FromReflectedMethod(JNIEnv *env, jobject method) {
	throw std::runtime_error("FromReflectedMethod not implemented");
}
jfieldID NativeInterface::FromReflectedField(JNIEnv *env, jobject field) {
	throw std::runtime_error("FromReflectedField not implemented");
}
jobject NativeInterface::ToReflectedMethod(JNIEnv *env, jclass cls, jmethodID methodID, jboolean isStatic) {
	throw std::runtime_error("ToReflectedMethod not implemented");
}
jclass NativeInterface::GetSuperclass(JNIEnv *env, jclass sub) {
	throw std::runtime_error("GetSuperclass not implemented");
}
jboolean NativeInterface::IsAssignableFrom(JNIEnv *env, jclass sub, jclass sup) {
	throw std::runtime_error("IsAssignableFrom not implemented");
}
jobject NativeInterface::ToReflectedField(JNIEnv *env, jclass cls, jfieldID fieldID, jboolean isStatic) {
	throw std::runtime_error("ToReflectedField not implemented");
}

jint NativeInterface::Throw(JNIEnv *env, jthrowable obj) {
	throw std::runtime_error("Throw not implemented");
}
jint NativeInterface::ThrowNew(JNIEnv *env, jclass clazz, const char *msg) {
	throw std::runtime_error("ThrowNew not implemented");
}
jthrowable NativeInterface::ExceptionOccurred(JNIEnv *env) {
	throw std::runtime_error("ExceptionOccurred not implemented");
}
void NativeInterface::ExceptionDescribe(JNIEnv *env) {
	throw std::runtime_error("ExceptionDescribe not implemented");
}
void NativeInterface::ExceptionClear(JNIEnv *env) {
	throw std::runtime_error("ExceptionClear not implemented");
}
void NativeInterface::FatalError(JNIEnv *env, const char *msg) {
	throw std::runtime_error("FatalError not implemented");
}

jint NativeInterface::PushLocalFrame(JNIEnv *env, jint capacity) {
	throw std::runtime_error("PushLocalFrame not implemented");
}
jobject NativeInterface::PopLocalFrame(JNIEnv *env, jobject result) {
	throw std::runtime_error("PopLocalFrame not implemented");
}
jobject NativeInterface::NewGlobalRef(JNIEnv *env, jobject lobj) {
	throw std::runtime_error("NewGlobalRef not implemented");
}
void NativeInterface::DeleteGlobalRef(JNIEnv *env, jobject gref) {
	throw std::runtime_error("DeleteGlobalRef not implemented");
}
void NativeInterface::DeleteLocalRef(JNIEnv *env, jobject obj) {
	throw std::runtime_error("DeleteLocalRef not implemented");
}
jboolean NativeInterface::IsSameObject(JNIEnv *env, jobject obj1, jobject obj2) {
	throw std::runtime_error("IsSameObject not implemented");
}
jobject NativeInterface::NewLocalRef(JNIEnv *env, jobject ref) {
	throw std::runtime_error("NewLocalRef not implemented");
}
jint NativeInterface::EnsureLocalCapacity(JNIEnv *env, jint capacity) {
	throw std::runtime_error("EnsureLocalCapacity not implemented");
}
jobject NativeInterface::AllocObject(JNIEnv *env, jclass clazz) {
	throw std::runtime_error("AllocObject not implemented");
}

jobject NativeInterface::NewObject(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	throw std::runtime_error("NewObject not implemented");
}
jobject NativeInterface::NewObjectV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
	throw std::runtime_error("NewObjectV not implemented");
}
jobject NativeInterface::NewObjectA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("NewObjectA not implemented");
}

jclass NativeInterface::GetObjectClass(JNIEnv *env, jobject obj) {
	throw std::runtime_error("GetObjectClass not implemented");
}
jboolean NativeInterface::IsInstanceOf(JNIEnv *env, jobject obj, jclass clazz) {
	throw std::runtime_error("IsInstanceOf not implemented");
}
jmethodID NativeInterface::GetMethodID(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
	throw std::runtime_error("GetMethodID not implemented");
}

jobject NativeInterface::CallObjectMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	throw std::runtime_error("CallObjectMethod not implemented");
}
jobject NativeInterface::CallObjectMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallObjectMethodV not implemented");
}
jobject NativeInterface::CallObjectMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallObjectMethodA not implemented");
}
jboolean NativeInterface::CallBooleanMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	throw std::runtime_error("CallBooleanMethod not implemented");
}
jboolean NativeInterface::CallBooleanMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallBooleanMethodV not implemented");
}
jboolean NativeInterface::CallBooleanMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallBooleanMethodA not implemented");
}
jbyte NativeInterface::CallByteMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	throw std::runtime_error("CallByteMethod not implemented");
}
jbyte NativeInterface::CallByteMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallByteMethodV not implemented");
}
jbyte NativeInterface::CallByteMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallByteMethodA not implemented");
}
jchar NativeInterface::CallCharMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	throw std::runtime_error("CallCharMethod not implemented");
}
jchar NativeInterface::CallCharMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallCharMethodV not implemented");
}
jchar NativeInterface::CallCharMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallCharMethodA not implemented");
}
jshort NativeInterface::CallShortMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	throw std::runtime_error("CallShortMethod not implemented");
}
jshort NativeInterface::CallShortMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallShortMethodV not implemented");
}
jshort NativeInterface::CallShortMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallShortMethodA not implemented");
}
jint NativeInterface::CallIntMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	throw std::runtime_error("CallIntMethod not implemented");
}
jint NativeInterface::CallIntMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallIntMethodV not implemented");
}
jint NativeInterface::CallIntMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallIntMethodA not implemented");
}
jlong NativeInterface::CallLongMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	throw std::runtime_error("CallLongMethod not implemented");
}
jlong NativeInterface::CallLongMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallLongMethodV not implemented");
}
jlong NativeInterface::CallLongMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallLongMethodA not implemented");
}
jfloat NativeInterface::CallFloatMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	throw std::runtime_error("CallFloatMethod not implemented");
}
jfloat NativeInterface::CallFloatMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallFloatMethodV not implemented");
}
jfloat NativeInterface::CallFloatMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallFloatMethodA not implemented");
}
jdouble NativeInterface::CallDoubleMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	throw std::runtime_error("CallDoubleMethod not implemented");
}
jdouble NativeInterface::CallDoubleMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallDoubleMethodV not implemented");
}
jdouble NativeInterface::CallDoubleMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallDoubleMethodA not implemented");
}
void NativeInterface::CallVoidMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	throw std::runtime_error("CallVoidMethod not implemented");
}
void NativeInterface::CallVoidMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallVoidMethodV not implemented");
}
void NativeInterface::CallVoidMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallVoidMethodA not implemented");
}

jobject NativeInterface::CallNonvirtualObjectMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) {
	throw std::runtime_error("CallNonvirtualObjectMethod not implemented");
}
jobject NativeInterface::CallNonvirtualObjectMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallNonvirtualObjectMethodV not implemented");
}
jobject NativeInterface::CallNonvirtualObjectMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallNonvirtualObjectMethodA not implemented");
}

jboolean NativeInterface::CallNonvirtualBooleanMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) {
	throw std::runtime_error("CallNonvirtualBooleanMethod not implemented");
}
jboolean NativeInterface::CallNonvirtualBooleanMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallNonvirtualBooleanMethodV not implemented");
}
jboolean NativeInterface::CallNonvirtualBooleanMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallNonvirtualBooleanMethodA not implemented");
}

jbyte NativeInterface::CallNonvirtualByteMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) {
	throw std::runtime_error("CallNonvirtualByteMethod not implemented");
}
jbyte NativeInterface::CallNonvirtualByteMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallNonvirtualByteMethodV not implemented");
}
jbyte NativeInterface::CallNonvirtualByteMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallNonvirtualByteMethodA not implemented");
}

jchar NativeInterface::CallNonvirtualCharMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) {
	throw std::runtime_error("CallNonvirtualCharMethod not implemented");
}
jchar NativeInterface::CallNonvirtualCharMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallNonvirtualCharMethodV not implemented");
}
jchar NativeInterface::CallNonvirtualCharMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallNonvirtualCharMethodA not implemented");
}

jshort NativeInterface::CallNonvirtualShortMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) {
	throw std::runtime_error("CallNonvirtualShortMethod not implemented");
}
jshort NativeInterface::CallNonvirtualShortMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallNonvirtualShortMethodV not implemented");
}
jshort NativeInterface::CallNonvirtualShortMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallNonvirtualShortMethodA not implemented");
}

jint NativeInterface::CallNonvirtualIntMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) {
	throw std::runtime_error("CallNonvirtualIntMethod not implemented");
}
jint NativeInterface::CallNonvirtualIntMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallNonvirtualIntMethodV not implemented");
}
jint NativeInterface::CallNonvirtualIntMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallNonvirtualIntMethodA not implemented");
}

jlong NativeInterface::CallNonvirtualLongMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) {
	throw std::runtime_error("CallNonvirtualLongMethod not implemented");
}
jlong NativeInterface::CallNonvirtualLongMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallNonvirtualLongMethodV not implemented");
}
jlong NativeInterface::CallNonvirtualLongMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallNonvirtualLongMethodA not implemented");
}

jfloat NativeInterface::CallNonvirtualFloatMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) {
	throw std::runtime_error("CallNonvirtualFloatMethod not implemented");
}
jfloat NativeInterface::CallNonvirtualFloatMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallNonvirtualFloatMethodV not implemented");
}
jfloat NativeInterface::CallNonvirtualFloatMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallNonvirtualFloatMethodA not implemented");
}

jdouble NativeInterface::CallNonvirtualDoubleMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) {
	throw std::runtime_error("CallNonvirtualDoubleMethod not implemented");
}
jdouble NativeInterface::CallNonvirtualDoubleMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallNonvirtualDoubleMethodV not implemented");
}
jdouble NativeInterface::CallNonvirtualDoubleMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallNonvirtualDoubleMethodA not implemented");
}

void NativeInterface::CallNonvirtualVoidMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) {
	throw std::runtime_error("CallNonvirtualVoidMethod not implemented");
}
void NativeInterface::CallNonvirtualVoidMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallNonvirtualVoidMethodV not implemented");
}
void NativeInterface::CallNonvirtualVoidMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallNonvirtualVoidMethodA not implemented");
}
jfieldID NativeInterface::GetFieldID(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
	throw std::runtime_error("GetFieldID not implemented");
}

jobject NativeInterface::GetObjectField(JNIEnv *env, jobject obj, jfieldID fieldID) {
	throw std::runtime_error("GetObjectField not implemented");
}
jboolean NativeInterface::GetBooleanField(JNIEnv *env, jobject obj, jfieldID fieldID) {
	throw std::runtime_error("GetBooleanField not implemented");
}
jbyte NativeInterface::GetByteField(JNIEnv *env, jobject obj, jfieldID fieldID) {
	throw std::runtime_error("GetByteField not implemented");
}
jchar NativeInterface::GetCharField(JNIEnv *env, jobject obj, jfieldID fieldID) {
	throw std::runtime_error("GetCharField not implemented");
}
jshort NativeInterface::GetShortField(JNIEnv *env, jobject obj, jfieldID fieldID) {
	throw std::runtime_error("GetShortField not implemented");
}
jint NativeInterface::GetIntField(JNIEnv *env, jobject obj, jfieldID fieldID) {
	throw std::runtime_error("GetIntField not implemented");
}
jlong NativeInterface::GetLongField(JNIEnv *env, jobject obj, jfieldID fieldID) {
	throw std::runtime_error("GetLongField not implemented");
}
jfloat NativeInterface::GetFloatField(JNIEnv *env, jobject obj, jfieldID fieldID) {
	throw std::runtime_error("GetFloatField not implemented");
}
jdouble NativeInterface::GetDoubleField(JNIEnv *env, jobject obj, jfieldID fieldID) {
	throw std::runtime_error("GetDoubleField not implemented");
}

void NativeInterface::SetObjectField(JNIEnv *env, jobject obj, jfieldID fieldID, jobject val) {
	throw std::runtime_error("SetObjectField not implemented");
}
void NativeInterface::SetBooleanField(JNIEnv *env, jobject obj, jfieldID fieldID, jboolean val) {
	throw std::runtime_error("SetBooleanField not implemented");
}
void NativeInterface::SetByteField(JNIEnv *env, jobject obj, jfieldID fieldID, jbyte val) {
	throw std::runtime_error("SetByteField not implemented");
}
void NativeInterface::SetCharField(JNIEnv *env, jobject obj, jfieldID fieldID, jchar val) {
	throw std::runtime_error("SetCharField not implemented");
}
void NativeInterface::SetShortField(JNIEnv *env, jobject obj, jfieldID fieldID, jshort val) {
	throw std::runtime_error("SetShortField not implemented");
}
void NativeInterface::SetIntField(JNIEnv *env, jobject obj, jfieldID fieldID, jint val) {
	throw std::runtime_error("SetIntField not implemented");
}
void NativeInterface::SetLongField(JNIEnv *env, jobject obj, jfieldID fieldID, jlong val) {
	throw std::runtime_error("SetLongField not implemented");
}
void NativeInterface::SetFloatField(JNIEnv *env, jobject obj, jfieldID fieldID, jfloat val) {
	throw std::runtime_error("SetFloatField not implemented");
}
void NativeInterface::SetDoubleField(JNIEnv *env, jobject obj, jfieldID fieldID, jdouble val) {
	throw std::runtime_error("SetDoubleField not implemented");
}
jmethodID NativeInterface::GetStaticMethodID(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
	throw std::runtime_error("GetStaticMethodID not implemented");
}

jobject NativeInterface::CallStaticObjectMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	throw std::runtime_error("CallStaticObjectMethod not implemented");
}
jobject NativeInterface::CallStaticObjectMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallStaticObjectMethodV not implemented");
}
jobject NativeInterface::CallStaticObjectMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallStaticObjectMethodA not implemented");
}

jboolean NativeInterface::CallStaticBooleanMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	throw std::runtime_error("CallStaticBooleanMethod not implemented");
}
jboolean NativeInterface::CallStaticBooleanMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallStaticBooleanMethodV not implemented");
}
jboolean NativeInterface::CallStaticBooleanMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallStaticBooleanMethodA not implemented");
}

jbyte NativeInterface::CallStaticByteMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	throw std::runtime_error("CallStaticByteMethod not implemented");
}
jbyte NativeInterface::CallStaticByteMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallStaticByteMethodV not implemented");
}
jbyte NativeInterface::CallStaticByteMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallStaticByteMethodA not implemented");
}

jchar NativeInterface::CallStaticCharMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	throw std::runtime_error("CallStaticCharMethod not implemented");
}
jchar NativeInterface::CallStaticCharMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallStaticCharMethodV not implemented");
}
jchar NativeInterface::CallStaticCharMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallStaticCharMethodA not implemented");
}

jshort NativeInterface::CallStaticShortMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	throw std::runtime_error("CallStaticShortMethod not implemented");
}
jshort NativeInterface::CallStaticShortMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallStaticShortMethodV not implemented");
}
jshort NativeInterface::CallStaticShortMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallStaticShortMethodA not implemented");
}

jint NativeInterface::CallStaticIntMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	throw std::runtime_error("CallStaticIntMethod not implemented");
}
jint NativeInterface::CallStaticIntMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallStaticIntMethodV not implemented");
}
jint NativeInterface::CallStaticIntMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallStaticIntMethodA not implemented");
}

jlong NativeInterface::CallStaticLongMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	throw std::runtime_error("CallStaticLongMethod not implemented");
}
jlong NativeInterface::CallStaticLongMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallStaticLongMethodV not implemented");
}
jlong NativeInterface::CallStaticLongMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallStaticLongMethodA not implemented");
}

jfloat NativeInterface::CallStaticFloatMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	throw std::runtime_error("CallStaticFloatMethod not implemented");
}
jfloat NativeInterface::CallStaticFloatMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallStaticFloatMethodV not implemented");
}
jfloat NativeInterface::CallStaticFloatMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallStaticFloatMethodA not implemented");
}

jdouble NativeInterface::CallStaticDoubleMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	throw std::runtime_error("CallStaticDoubleMethod not implemented");
}
jdouble NativeInterface::CallStaticDoubleMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallStaticDoubleMethodV not implemented");
}
jdouble NativeInterface::CallStaticDoubleMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallStaticDoubleMethodA not implemented");
}

void NativeInterface::CallStaticVoidMethod(JNIEnv *env, jclass cls, jmethodID methodID, ...) {
	throw std::runtime_error("CallStaticVoidMethod not implemented");
}
void NativeInterface::CallStaticVoidMethodV(JNIEnv *env, jclass cls, jmethodID methodID, va_list args) {
	throw std::runtime_error("CallStaticVoidMethodV not implemented");
}
void NativeInterface::CallStaticVoidMethodA(JNIEnv *env, jclass cls, jmethodID methodID, const jvalue *args) {
	throw std::runtime_error("CallStaticVoidMethodA not implemented");
}

jfieldID NativeInterface::GetStaticFieldID(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
	throw std::runtime_error("GetStaticFieldID not implemented");
}
jobject NativeInterface::GetStaticObjectField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
	throw std::runtime_error("GetStaticObjectField not implemented");
}
jboolean NativeInterface::GetStaticBooleanField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
	throw std::runtime_error("GetStaticBooleanField not implemented");
}
jbyte NativeInterface::GetStaticByteField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
	throw std::runtime_error("GetStaticByteField not implemented");
}
jchar NativeInterface::GetStaticCharField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
	throw std::runtime_error("GetStaticCharField not implemented");
}
jshort NativeInterface::GetStaticShortField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
	throw std::runtime_error("GetStaticShortField not implemented");
}
jint NativeInterface::GetStaticIntField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
	throw std::runtime_error("GetStaticIntField not implemented");
}
jlong NativeInterface::GetStaticLongField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
	throw std::runtime_error("GetStaticLongField not implemented");
}
jfloat NativeInterface::GetStaticFloatField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
	throw std::runtime_error("GetStaticFloatField not implemented");
}
jdouble NativeInterface::GetStaticDoubleField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
	throw std::runtime_error("GetStaticDoubleField not implemented");
}

void NativeInterface::SetStaticObjectField(JNIEnv *env, jclass clazz, jfieldID fieldID, jobject value) {
	throw std::runtime_error("SetStaticObjectField not implemented");
}
void NativeInterface::SetStaticBooleanField(JNIEnv *env, jclass clazz, jfieldID fieldID, jboolean value) {
	throw std::runtime_error("SetStaticBooleanField not implemented");
}
void NativeInterface::SetStaticByteField(JNIEnv *env, jclass clazz, jfieldID fieldID, jbyte value) {
	throw std::runtime_error("SetStaticByteField not implemented");
}
void NativeInterface::SetStaticCharField(JNIEnv *env, jclass clazz, jfieldID fieldID, jchar value) {
	throw std::runtime_error("SetStaticCharField not implemented");
}
void NativeInterface::SetStaticShortField(JNIEnv *env, jclass clazz, jfieldID fieldID, jshort value) {
	throw std::runtime_error("SetStaticShortField not implemented");
}
void NativeInterface::SetStaticIntField(JNIEnv *env, jclass clazz, jfieldID fieldID, jint value) {
	throw std::runtime_error("SetStaticIntField not implemented");
}
void NativeInterface::SetStaticLongField(JNIEnv *env, jclass clazz, jfieldID fieldID, jlong value) {
	throw std::runtime_error("SetStaticLongField not implemented");
}
void NativeInterface::SetStaticFloatField(JNIEnv *env, jclass clazz, jfieldID fieldID, jfloat value) {
	throw std::runtime_error("SetStaticFloatField not implemented");
}
void NativeInterface::SetStaticDoubleField(JNIEnv *env, jclass clazz, jfieldID fieldID, jdouble value) {
	throw std::runtime_error("SetStaticDoubleField not implemented");
}

jstring NativeInterface::NewString(JNIEnv *env, const jchar *unicode, jsize len) {
	throw std::runtime_error("NewString not implemented");
}
jsize NativeInterface::GetStringLength(JNIEnv *env, jstring str) {
	throw std::runtime_error("GetStringLength not implemented");
}
const jchar *NativeInterface::GetStringChars(JNIEnv *env, jstring str, jboolean *isCopy) {
	throw std::runtime_error("GetStringChars not implemented");
}
void NativeInterface::ReleaseStringChars(JNIEnv *env, jstring str, const jchar *chars) {
	throw std::runtime_error("ReleaseStringChars not implemented");
}

jstring NativeInterface::NewStringUTF(JNIEnv *env, const char *utf) {
	throw std::runtime_error("NewStringUTF not implemented");
}
jsize NativeInterface::GetStringUTFLength(JNIEnv *env, jstring str) {
	throw std::runtime_error("GetStringUTFLength not implemented");
}

const char *NativeInterface::GetStringUTFChars(JNIEnv *env, jstring str, jboolean *isCopy) {
	StringObject *obj = (StringObject *)str;
	if (obj == nullptr) {
		throw std::runtime_error("NullPointerException");
	}
	logger.debug(fmt::format("env->GetStringUTFChars {}", obj->debug()));
	char *utf = new char[obj->str().length() + 1];
	std::strncpy(utf, obj->str().c_str(), obj->str().length());
	utf[obj->str().length()] = '\0';  // Null-terminate the string
	if (isCopy != nullptr) {
		*isCopy = JNI_FALSE;  // Assuming we don't need to copy the string
	}
	return utf;
}
void NativeInterface::ReleaseStringUTFChars(JNIEnv *env, jstring str, const char *chars) {
	StringObject *obj = (StringObject *)str;
	if (obj == nullptr) {
		throw std::runtime_error("NullPointerException");
	}
	logger.debug(fmt::format("env->ReleaseStringUTFChars {}", obj->debug()));
	if (chars) {
		// If we allocated memory for chars, we should delete it
		delete[] chars;
	}
}

jsize NativeInterface::GetArrayLength(JNIEnv *env, jarray array) {
	throw std::runtime_error("GetArrayLength not implemented");
}

jobjectArray NativeInterface::NewObjectArray(JNIEnv *env, jsize len, jclass clazz, jobject init) {
	throw std::runtime_error("NewObjectArray not implemented");
}
jobject NativeInterface::GetObjectArrayElement(JNIEnv *env, jobjectArray array, jsize index) {
	throw std::runtime_error("GetObjectArrayElement not implemented");
}
void NativeInterface::SetObjectArrayElement(JNIEnv *env, jobjectArray array, jsize index, jobject val) {
	throw std::runtime_error("SetObjectArrayElement not implemented");
}
jbooleanArray NativeInterface::NewBooleanArray(JNIEnv *env, jsize len) {
	throw std::runtime_error("NewBooleanArray not implemented");
}
jbyteArray NativeInterface::NewByteArray(JNIEnv *env, jsize len) {
	throw std::runtime_error("NewByteArray not implemented");
}
jcharArray NativeInterface::NewCharArray(JNIEnv *env, jsize len) {
	throw std::runtime_error("NewCharArray not implemented");
}
jshortArray NativeInterface::NewShortArray(JNIEnv *env, jsize len) {
	throw std::runtime_error("NewShortArray not implemented");
}
jintArray NativeInterface::NewIntArray(JNIEnv *env, jsize len) {
	throw std::runtime_error("NewIntArray not implemented");
}
jlongArray NativeInterface::NewLongArray(JNIEnv *env, jsize len) {
	throw std::runtime_error("NewLongArray not implemented");
}
jfloatArray NativeInterface::NewFloatArray(JNIEnv *env, jsize len) {
	throw std::runtime_error("NewFloatArray not implemented");
}
jdoubleArray NativeInterface::NewDoubleArray(JNIEnv *env, jsize len) {
	throw std::runtime_error("NewDoubleArray not implemented");
}

jboolean *NativeInterface::GetBooleanArrayElements(JNIEnv *env, jbooleanArray array, jboolean *isCopy) {
	throw std::runtime_error("GetBooleanArrayElements not implemented");
}
jbyte *NativeInterface::GetByteArrayElements(JNIEnv *env, jbyteArray array, jboolean *isCopy) {
	throw std::runtime_error("GetByteArrayElements not implemented");
}
jchar *NativeInterface::GetCharArrayElements(JNIEnv *env, jcharArray array, jboolean *isCopy) {
	throw std::runtime_error("GetCharArrayElements not implemented");
}
jshort *NativeInterface::GetShortArrayElements(JNIEnv *env, jshortArray array, jboolean *isCopy) {
	throw std::runtime_error("GetShortArrayElements not implemented");
}
jint *NativeInterface::GetIntArrayElements(JNIEnv *env, jintArray array, jboolean *isCopy) {
	throw std::runtime_error("GetIntArrayElements not implemented");
}
jlong *NativeInterface::GetLongArrayElements(JNIEnv *env, jlongArray array, jboolean *isCopy) {
	throw std::runtime_error("GetLongArrayElements not implemented");
}
jfloat *NativeInterface::GetFloatArrayElements(JNIEnv *env, jfloatArray array, jboolean *isCopy) {
	throw std::runtime_error("GetFloatArrayElements not implemented");
}
jdouble *NativeInterface::GetDoubleArrayElements(JNIEnv *env, jdoubleArray array, jboolean *isCopy) {
	throw std::runtime_error("GetDoubleArrayElements not implemented");
}

void NativeInterface::ReleaseBooleanArrayElements(JNIEnv *env, jbooleanArray array, jboolean *elems, jint mode) {
	throw std::runtime_error("ReleaseBooleanArrayElements not implemented");
}
void NativeInterface::ReleaseByteArrayElements(JNIEnv *env, jbyteArray array, jbyte *elems, jint mode) {
	throw std::runtime_error("ReleaseByteArrayElements not implemented");
}
void NativeInterface::ReleaseCharArrayElements(JNIEnv *env, jcharArray array, jchar *elems, jint mode) {
	throw std::runtime_error("ReleaseCharArrayElements not implemented");
}
void NativeInterface::ReleaseShortArrayElements(JNIEnv *env, jshortArray array, jshort *elems, jint mode) {
	throw std::runtime_error("ReleaseShortArrayElements not implemented");
}
void NativeInterface::ReleaseIntArrayElements(JNIEnv *env, jintArray array, jint *elems, jint mode) {
	throw std::runtime_error("ReleaseIntArrayElements not implemented");
}
void NativeInterface::ReleaseLongArrayElements(JNIEnv *env, jlongArray array, jlong *elems, jint mode) {
	throw std::runtime_error("ReleaseLongArrayElements not implemented");
}
void NativeInterface::ReleaseFloatArrayElements(JNIEnv *env, jfloatArray array, jfloat *elems, jint mode) {
	throw std::runtime_error("ReleaseFloatArrayElements not implemented");
}
void NativeInterface::ReleaseDoubleArrayElements(JNIEnv *env, jdoubleArray array, jdouble *elems, jint mode) {
	throw std::runtime_error("ReleaseDoubleArrayElements not implemented");
}
void NativeInterface::GetBooleanArrayRegion(JNIEnv *env, jbooleanArray array, jsize start, jsize l, jboolean *buf) {
	throw std::runtime_error("GetBooleanArrayRegion not implemented");
}
void NativeInterface::GetByteArrayRegion(JNIEnv *env, jbyteArray array, jsize start, jsize len, jbyte *buf) {
	throw std::runtime_error("GetByteArrayRegion not implemented");
}
void NativeInterface::GetCharArrayRegion(JNIEnv *env, jcharArray array, jsize start, jsize len, jchar *buf) {
	throw std::runtime_error("GetCharArrayRegion not implemented");
}
void NativeInterface::GetShortArrayRegion(JNIEnv *env, jshortArray array, jsize start, jsize len, jshort *buf) {
	throw std::runtime_error("GetShortArrayRegion not implemented");
}
void NativeInterface::GetIntArrayRegion(JNIEnv *env, jintArray array, jsize start, jsize len, jint *buf) {
	throw std::runtime_error("GetIntArrayRegion not implemented");
}
void NativeInterface::GetLongArrayRegion(JNIEnv *env, jlongArray array, jsize start, jsize len, jlong *buf) {
	throw std::runtime_error("GetLongArrayRegion not implemented");
}
void NativeInterface::GetFloatArrayRegion(JNIEnv *env, jfloatArray array, jsize start, jsize len, jfloat *buf) {
	throw std::runtime_error("GetFloatArrayRegion not implemented");
}
void NativeInterface::GetDoubleArrayRegion(JNIEnv *env, jdoubleArray array, jsize start, jsize len, jdouble *buf) {
	throw std::runtime_error("GetDoubleArrayRegion not implemented");
}

void NativeInterface::SetBooleanArrayRegion(JNIEnv *env, jbooleanArray array, jsize start, jsize l, const jboolean *buf) {
	throw std::runtime_error("SetBooleanArrayRegion not implemented");
}
void NativeInterface::SetByteArrayRegion(JNIEnv *env, jbyteArray array, jsize start, jsize len, const jbyte *buf) {
	throw std::runtime_error("SetByteArrayRegion not implemented");
}
void NativeInterface::SetCharArrayRegion(JNIEnv *env, jcharArray array, jsize start, jsize len, const jchar *buf) {
	throw std::runtime_error("SetCharArrayRegion not implemented");
}
void NativeInterface::SetShortArrayRegion(JNIEnv *env, jshortArray array, jsize start, jsize len, const jshort *buf) {
	throw std::runtime_error("SetShortArrayRegion not implemented");
}
void NativeInterface::SetIntArrayRegion(JNIEnv *env, jintArray array, jsize start, jsize len, const jint *buf) {
	throw std::runtime_error("SetIntArrayRegion not implemented");
}
void NativeInterface::SetLongArrayRegion(JNIEnv *env, jlongArray array, jsize start, jsize len, const jlong *buf) {
	throw std::runtime_error("SetLongArrayRegion not implemented");
}
void NativeInterface::SetFloatArrayRegion(JNIEnv *env, jfloatArray array, jsize start, jsize len, const jfloat *buf) {
	throw std::runtime_error("SetFloatArrayRegion not implemented");
}
void NativeInterface::SetDoubleArrayRegion(JNIEnv *env, jdoubleArray array, jsize start, jsize len, const jdouble *buf) {
	throw std::runtime_error("SetDoubleArrayRegion not implemented");
}
jint NativeInterface::RegisterNatives(JNIEnv *env, jclass clazz, const JNINativeMethod *methods, jint nMethods) {
	if (env == nullptr) {
		throw std::invalid_argument("env cannot be null");
	}
	if (methods == nullptr) {
		throw std::invalid_argument("methods cannot be null");
	}
	for (int i = 0; i < nMethods; i++) {
		const JNINativeMethod *method = &methods[i];
		logger.debug(fmt::format("JNI: RegisterNatives {}{} -> {}", method->name, method->signature, method->fnPtr));
	}
	return JNI_OK;
}
jint NativeInterface::UnregisterNatives(JNIEnv *env, jclass clazz) {
	throw std::runtime_error("UnregisterNatives not implemented");
}

jint NativeInterface::MonitorEnter(JNIEnv *env, jobject obj) {
	throw std::runtime_error("MonitorEnter not implemented");
}
jint NativeInterface::MonitorExit(JNIEnv *env, jobject obj) {
	throw std::runtime_error("MonitorExit not implemented");
}

jint NativeInterface::GetJavaVM(JNIEnv *env, JavaVM **vm) {
	throw std::runtime_error("GetJavaVM not implemented");
}

void NativeInterface::GetStringRegion(JNIEnv *env, jstring str, jsize start, jsize len, jchar *buf) {
	throw std::runtime_error("GetStringRegion not implemented");
}
void NativeInterface::GetStringUTFRegion(JNIEnv *env, jstring str, jsize start, jsize len, char *buf) {
	throw std::runtime_error("GetStringUTFRegion not implemented");
}

void *NativeInterface::GetPrimitiveArrayCritical(JNIEnv *env, jarray array, jboolean *isCopy) {
	throw std::runtime_error("GetPrimitiveArrayCritical not implemented");
}
void NativeInterface::ReleasePrimitiveArrayCritical(JNIEnv *env, jarray array, void *carray, jint mode) {
	throw std::runtime_error("ReleasePrimitiveArrayCritical not implemented");
}

const jchar *NativeInterface::GetStringCritical(JNIEnv *env, jstring string, jboolean *isCopy) {
	throw std::runtime_error("GetStringCritical not implemented");
}
void NativeInterface::ReleaseStringCritical(JNIEnv *env, jstring string, const jchar *cstring) {
	throw std::runtime_error("ReleaseStringCritical not implemented");
}

jweak NativeInterface::NewWeakGlobalRef(JNIEnv *env, jobject obj) {
	throw std::runtime_error("NewWeakGlobalRef not implemented");
}
void NativeInterface::DeleteWeakGlobalRef(JNIEnv *env, jweak ref) {
	throw std::runtime_error("DeleteWeakGlobalRef not implemented");
}

jboolean NativeInterface::ExceptionCheck(JNIEnv *env) {
	throw std::runtime_error("ExceptionCheck not implemented");
}

jobject NativeInterface::NewDirectByteBuffer(JNIEnv *env, void *address, jlong capacity) {
	throw std::runtime_error("NewDirectByteBuffer not implemented");
}
void *NativeInterface::GetDirectBufferAddress(JNIEnv *env, jobject buf) {
	throw std::runtime_error("GetDirectBufferAddress not implemented");
}
jlong NativeInterface::GetDirectBufferCapacity(JNIEnv *env, jobject buf) {
	throw std::runtime_error("GetDirectBufferCapacity not implemented");
}

jobjectRefType NativeInterface::GetObjectRefType(JNIEnv *env, jobject obj) {
	throw std::runtime_error("GetObjectRefType not implemented");
}
