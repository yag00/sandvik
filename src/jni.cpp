/*
 * This file is part of Sandvik project.
 * Copyright (C) 2025 Christophe Duvernois
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "jni.hpp"

#include <string.h>

#include <fmt/format.h>

#include <stdexcept>

#include "jni/jni.h"

#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "frame.hpp"
#include "interpreter.hpp"
#include "jnihandlemap.hpp"
#include "jthread.hpp"
#include "method.hpp"
#include "native/native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"
#include "vm.hpp"

using namespace sandvik;

NativeInterface::NativeInterface(Vm &vm_) : _vm(vm_), _classloader(vm_.getClassLoader()), _handles(std::make_unique<JNIHandleMap>()) {
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

Vm &NativeInterface::getVm() const {
	return _vm;
}

ClassLoader &NativeInterface::getClassLoader() const {
	return _classloader;
}

JNIHandleMap &NativeInterface::getHandles() const {
	return *_handles;
}

// ----------------------------------------------------------------------------
// Implementations of the JNI functions
// ----------------------------------------------------------------------------

jint NativeInterface::GetVersion(JNIEnv *env) {
	return JNI_VERSION_1_6;
}
jclass NativeInterface::DefineClass(JNIEnv *env, const char *name, jobject loader, const jbyte *buf, jsize len) {
	throw VmException("DefineClass not implemented");
}
jclass NativeInterface::FindClass(JNIEnv *env, const char *name) {
	NativeInterface *this_ptr = static_cast<NativeInterface *>(env);
	ClassLoader &classloader = this_ptr->getClassLoader();
	Class &cls = classloader.getOrLoad(name);
	return (jclass)&cls;
}
jmethodID NativeInterface::FromReflectedMethod(JNIEnv *env, jobject method) {
	throw VmException("FromReflectedMethod not implemented");
}
jfieldID NativeInterface::FromReflectedField(JNIEnv *env, jobject field) {
	throw VmException("FromReflectedField not implemented");
}
jobject NativeInterface::ToReflectedMethod(JNIEnv *env, jclass cls, jmethodID methodID, jboolean isStatic) {
	throw VmException("ToReflectedMethod not implemented");
}
jclass NativeInterface::GetSuperclass(JNIEnv *env, jclass sub) {
	throw VmException("GetSuperclass not implemented");
}
jboolean NativeInterface::IsAssignableFrom(JNIEnv *env, jclass sub, jclass sup) {
	throw VmException("IsAssignableFrom not implemented");
}
jobject NativeInterface::ToReflectedField(JNIEnv *env, jclass cls, jfieldID fieldID, jboolean isStatic) {
	throw VmException("ToReflectedField not implemented");
}

jint NativeInterface::Throw(JNIEnv *env, jthrowable obj) {
	throw VmException("Throw not implemented");
}
jint NativeInterface::ThrowNew(JNIEnv *env, jclass clazz, const char *msg) {
	throw VmException("ThrowNew not implemented");
}
jthrowable NativeInterface::ExceptionOccurred(JNIEnv *env) {
	throw VmException("ExceptionOccurred not implemented");
}
void NativeInterface::ExceptionDescribe(JNIEnv *env) {
	throw VmException("ExceptionDescribe not implemented");
}
void NativeInterface::ExceptionClear(JNIEnv *env) {
	throw VmException("ExceptionClear not implemented");
}
void NativeInterface::FatalError(JNIEnv *env, const char *msg) {
	throw VmException("FatalError not implemented");
}

jint NativeInterface::PushLocalFrame(JNIEnv *env, jint capacity) {
	throw VmException("PushLocalFrame not implemented");
}
jobject NativeInterface::PopLocalFrame(JNIEnv *env, jobject result) {
	throw VmException("PopLocalFrame not implemented");
}
jobject NativeInterface::NewGlobalRef(JNIEnv *env, jobject lobj) {
	throw VmException("NewGlobalRef not implemented");
}
void NativeInterface::DeleteGlobalRef(JNIEnv *env, jobject gref) {
	throw VmException("DeleteGlobalRef not implemented");
}
void NativeInterface::DeleteLocalRef(JNIEnv *env, jobject obj) {
	throw VmException("DeleteLocalRef not implemented");
}
jboolean NativeInterface::IsSameObject(JNIEnv *env, jobject obj1, jobject obj2) {
	throw VmException("IsSameObject not implemented");
}
jobject NativeInterface::NewLocalRef(JNIEnv *env, jobject ref) {
	throw VmException("NewLocalRef not implemented");
}
jint NativeInterface::EnsureLocalCapacity(JNIEnv *env, jint capacity) {
	throw VmException("EnsureLocalCapacity not implemented");
}
jobject NativeInterface::AllocObject(JNIEnv *env, jclass clazz) {
	throw VmException("AllocObject not implemented");
}

jobject NativeInterface::NewObject(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	throw VmException("NewObject not implemented");
}
jobject NativeInterface::NewObjectV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
	throw VmException("NewObjectV not implemented");
}
jobject NativeInterface::NewObjectA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw VmException("NewObjectA not implemented");
}

jclass NativeInterface::GetObjectClass(JNIEnv *env, jobject obj) {
	if (obj == nullptr) {
		throw NullPointerException("GetObjectClass on null object");
	}
	auto jenv = static_cast<NativeInterface *>(env);
	auto jobj = jenv->getHandles().fromJObject(obj);
	auto &clazz = jobj->getClass();
	return (jclass)jenv->getHandles().toJObject(Object::make(clazz));
}
jboolean NativeInterface::IsInstanceOf(JNIEnv *env, jobject obj, jclass clazz) {
	throw VmException("IsInstanceOf not implemented");
}
jmethodID NativeInterface::GetMethodID(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
	auto jenv = static_cast<NativeInterface *>(env);
	if (clazz == nullptr || name == nullptr || sig == nullptr) {
		throw NullPointerException("GetMethodID: class, name, or sig is null");
	}
	auto clsObj = jenv->getHandles().fromJObject(clazz);
	if (!clsObj || !clsObj->isClass()) {
		throw ClassCastException("GetMethodID: class is not a class object");
	}
	try {
		auto &cls = clsObj->getClass();
		auto &method = cls.getMethod(name, sig);
		return (jmethodID)(uintptr_t)method.getIndex();
	} catch (const std::invalid_argument &) {
		return nullptr;
	}
}

JThread &NativeInterface::__CallObjectMethod(JNIEnv *env, jobject obj, jmethodID methodID, va_list &args) {
	if (obj == nullptr) {
		throw NullPointerException("CallIntMethod on null object");
	}
	auto jenv = static_cast<NativeInterface *>(env);
	auto &vm = jenv->getVm();
	auto jobj = jenv->getHandles().fromJObject(obj);
	// Find the method in the object's class
	Class &clazz = jobj->getClass();
	if (!clazz.hasMethod((uint32_t)(uintptr_t)methodID)) {
		throw NoSuchMethodException(fmt::format("CallIntMethod: methodID {} not found in class {}", (size_t)methodID, clazz.getName()));
	}
	Method &method = clazz.getMethod((uint32_t)(uintptr_t)methodID);
	// Call the method
	auto &thread = vm.newThread(fmt::format("{}.{}", clazz.getFullname(), method.getName()));
	auto &frame = thread.newFrame(method);
	auto sig = method.getSignature();

	// @todo: check that the return type is 'I' (int)
	auto regidx = method.getNbRegisters() - 1 - method.getNbArguments();
	frame.setObjRegister(regidx++, jobj);  // this
	// Set method arguments in the frame registers
	for (auto arg : method.arguments()) {
		switch (arg[0]) {
			case 'Z':  // boolean
			case 'B':  // byte
			case 'C':  // char
			case 'S':  // short
			case 'I':  // int
				frame.setObjRegister(regidx++, Object::make((int32_t)va_arg(args, int)));
				break;
			case 'J':  // long
				// frame.setObjRegister(nbRegisters, (int64_t)va_arg(args, int64_t));
				logger.fwarning("CallIntMethod: long arguments not yet supported");
				break;
			case 'F':  // float
				// frame.setObjRegister(nbRegisters, (float)va_arg(args, float));
				logger.fwarning("CallIntMethod: float arguments not yet supported");
				break;
			case 'D':  // double
				// frame.setObjRegister(nbRegisters, (double)va_arg(args, double));
				logger.fwarning("CallIntMethod: double arguments not yet supported");
				break;
			case 'L':  // object
			case '[':  // array
			{
				logger.fwarning("CallIntMethod: object/array arguments not yet supported");
				break;
			}
			default:
				throw VmException(fmt::format("CallIntMethod: unknown argument type '{}'", arg));
		}
	}
	thread.run(true);
	// Get the return value
	return thread;
}

jobject NativeInterface::CallObjectMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	va_list args;
	va_start(args, methodID);
	const auto &result = __CallObjectMethod(env, obj, methodID, args);
	va_end(args);
	auto jenv = static_cast<NativeInterface *>(env);
	return jenv->getHandles().toJObject(result.getReturnObject());
}
jobject NativeInterface::CallObjectMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
	throw VmException("CallObjectMethodV not implemented");
}
jobject NativeInterface::CallObjectMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) {
	throw VmException("CallObjectMethodA not implemented");
}
jboolean NativeInterface::CallBooleanMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	va_list args;
	va_start(args, methodID);
	const auto &result = __CallObjectMethod(env, obj, methodID, args);
	va_end(args);
	return (jboolean)result.getReturnValue();
}
jboolean NativeInterface::CallBooleanMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
	throw VmException("CallBooleanMethodV not implemented");
}
jboolean NativeInterface::CallBooleanMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) {
	throw VmException("CallBooleanMethodA not implemented");
}
jbyte NativeInterface::CallByteMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	va_list args;
	va_start(args, methodID);
	const auto &result = __CallObjectMethod(env, obj, methodID, args);
	va_end(args);
	return (jbyte)result.getReturnValue();
}
jbyte NativeInterface::CallByteMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
	throw VmException("CallByteMethodV not implemented");
}
jbyte NativeInterface::CallByteMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) {
	throw VmException("CallByteMethodA not implemented");
}
jchar NativeInterface::CallCharMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	va_list args;
	va_start(args, methodID);
	const auto &result = __CallObjectMethod(env, obj, methodID, args);
	va_end(args);
	return (jchar)result.getReturnValue();
}
jchar NativeInterface::CallCharMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
	throw VmException("CallCharMethodV not implemented");
}
jchar NativeInterface::CallCharMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) {
	throw VmException("CallCharMethodA not implemented");
}
jshort NativeInterface::CallShortMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	va_list args;
	va_start(args, methodID);
	const auto &result = __CallObjectMethod(env, obj, methodID, args);
	va_end(args);
	return (jshort)result.getReturnValue();
}
jshort NativeInterface::CallShortMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
	throw VmException("CallShortMethodV not implemented");
}
jshort NativeInterface::CallShortMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) {
	throw VmException("CallShortMethodA not implemented");
}
jint NativeInterface::CallIntMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	va_list args;
	va_start(args, methodID);
	const auto &result = __CallObjectMethod(env, obj, methodID, args);
	va_end(args);
	return result.getReturnValue();
}
jint NativeInterface::CallIntMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
	throw VmException("CallIntMethodV not implemented");
}
jint NativeInterface::CallIntMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) {
	throw VmException("CallIntMethodA not implemented");
}
jlong NativeInterface::CallLongMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	va_list args;
	va_start(args, methodID);
	const auto &result = __CallObjectMethod(env, obj, methodID, args);
	va_end(args);
	return (jlong)result.getReturnDoubleValue();
}
jlong NativeInterface::CallLongMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
	throw VmException("CallLongMethodV not implemented");
}
jlong NativeInterface::CallLongMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) {
	throw VmException("CallLongMethodA not implemented");
}
jfloat NativeInterface::CallFloatMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	va_list args;
	va_start(args, methodID);
	const auto &result = __CallObjectMethod(env, obj, methodID, args);
	va_end(args);
	return (jfloat)result.getReturnValue();
}
jfloat NativeInterface::CallFloatMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
	throw VmException("CallFloatMethodV not implemented");
}
jfloat NativeInterface::CallFloatMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) {
	throw VmException("CallFloatMethodA not implemented");
}
jdouble NativeInterface::CallDoubleMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	va_list args;
	va_start(args, methodID);
	const auto &result = __CallObjectMethod(env, obj, methodID, args);
	va_end(args);
	return (jdouble)result.getReturnDoubleValue();
}
jdouble NativeInterface::CallDoubleMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
	throw VmException("CallDoubleMethodV not implemented");
}
jdouble NativeInterface::CallDoubleMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) {
	throw VmException("CallDoubleMethodA not implemented");
}
void NativeInterface::CallVoidMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...) {
	va_list args;
	va_start(args, methodID);
	__CallObjectMethod(env, obj, methodID, args);
	va_end(args);
}
void NativeInterface::CallVoidMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args) {
	throw VmException("CallVoidMethodV not implemented");
}
void NativeInterface::CallVoidMethodA(JNIEnv *env, jobject obj, jmethodID methodID, const jvalue *args) {
	throw VmException("CallVoidMethodA not implemented");
}

jobject NativeInterface::CallNonvirtualObjectMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) {
	throw VmException("CallNonvirtualObjectMethod not implemented");
}
jobject NativeInterface::CallNonvirtualObjectMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) {
	throw VmException("CallNonvirtualObjectMethodV not implemented");
}
jobject NativeInterface::CallNonvirtualObjectMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw VmException("CallNonvirtualObjectMethodA not implemented");
}

jboolean NativeInterface::CallNonvirtualBooleanMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) {
	throw VmException("CallNonvirtualBooleanMethod not implemented");
}
jboolean NativeInterface::CallNonvirtualBooleanMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) {
	throw VmException("CallNonvirtualBooleanMethodV not implemented");
}
jboolean NativeInterface::CallNonvirtualBooleanMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw VmException("CallNonvirtualBooleanMethodA not implemented");
}

jbyte NativeInterface::CallNonvirtualByteMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) {
	throw VmException("CallNonvirtualByteMethod not implemented");
}
jbyte NativeInterface::CallNonvirtualByteMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) {
	throw VmException("CallNonvirtualByteMethodV not implemented");
}
jbyte NativeInterface::CallNonvirtualByteMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw VmException("CallNonvirtualByteMethodA not implemented");
}

jchar NativeInterface::CallNonvirtualCharMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) {
	throw VmException("CallNonvirtualCharMethod not implemented");
}
jchar NativeInterface::CallNonvirtualCharMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) {
	throw VmException("CallNonvirtualCharMethodV not implemented");
}
jchar NativeInterface::CallNonvirtualCharMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw VmException("CallNonvirtualCharMethodA not implemented");
}

jshort NativeInterface::CallNonvirtualShortMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) {
	throw VmException("CallNonvirtualShortMethod not implemented");
}
jshort NativeInterface::CallNonvirtualShortMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) {
	throw VmException("CallNonvirtualShortMethodV not implemented");
}
jshort NativeInterface::CallNonvirtualShortMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw VmException("CallNonvirtualShortMethodA not implemented");
}

jint NativeInterface::CallNonvirtualIntMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) {
	throw VmException("CallNonvirtualIntMethod not implemented");
}
jint NativeInterface::CallNonvirtualIntMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) {
	throw VmException("CallNonvirtualIntMethodV not implemented");
}
jint NativeInterface::CallNonvirtualIntMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw VmException("CallNonvirtualIntMethodA not implemented");
}

jlong NativeInterface::CallNonvirtualLongMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) {
	throw VmException("CallNonvirtualLongMethod not implemented");
}
jlong NativeInterface::CallNonvirtualLongMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) {
	throw VmException("CallNonvirtualLongMethodV not implemented");
}
jlong NativeInterface::CallNonvirtualLongMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw VmException("CallNonvirtualLongMethodA not implemented");
}

jfloat NativeInterface::CallNonvirtualFloatMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) {
	throw VmException("CallNonvirtualFloatMethod not implemented");
}
jfloat NativeInterface::CallNonvirtualFloatMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) {
	throw VmException("CallNonvirtualFloatMethodV not implemented");
}
jfloat NativeInterface::CallNonvirtualFloatMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw VmException("CallNonvirtualFloatMethodA not implemented");
}

jdouble NativeInterface::CallNonvirtualDoubleMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) {
	throw VmException("CallNonvirtualDoubleMethod not implemented");
}
jdouble NativeInterface::CallNonvirtualDoubleMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) {
	throw VmException("CallNonvirtualDoubleMethodV not implemented");
}
jdouble NativeInterface::CallNonvirtualDoubleMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw VmException("CallNonvirtualDoubleMethodA not implemented");
}

void NativeInterface::CallNonvirtualVoidMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...) {
	throw VmException("CallNonvirtualVoidMethod not implemented");
}
void NativeInterface::CallNonvirtualVoidMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args) {
	throw VmException("CallNonvirtualVoidMethodV not implemented");
}
void NativeInterface::CallNonvirtualVoidMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw VmException("CallNonvirtualVoidMethodA not implemented");
}
jfieldID NativeInterface::GetFieldID(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
	throw VmException("GetFieldID not implemented");
}

jobject NativeInterface::GetObjectField(JNIEnv *env, jobject obj, jfieldID fieldID) {
	throw VmException("GetObjectField not implemented");
}
jboolean NativeInterface::GetBooleanField(JNIEnv *env, jobject obj, jfieldID fieldID) {
	if (obj == nullptr) {
		throw NullPointerException("GetBooleanField on null object");
	}
	auto jenv = static_cast<NativeInterface *>(env);
	auto jobj = jenv->getHandles().fromJObject(obj);
	if (!jobj) {
		throw ClassCastException("GetBooleanField: invalid object");
	}
	auto &clazz = jobj->getClass();
	if (!clazz.hasField((uint32_t)(uintptr_t)fieldID)) {
		throw NoSuchFieldException(fmt::format("GetBooleanField: fieldID {} not found in class {}", (size_t)fieldID, clazz.getName()));
	}
	const auto &field = clazz.getField((uint32_t)(uintptr_t)fieldID);
	if (field.getType() != "Z") {
		throw ClassCastException("GetBooleanField: field is not boolean");
	}
	return (jboolean)field.getIntValue();
}

jbyte NativeInterface::GetByteField(JNIEnv *env, jobject obj, jfieldID fieldID) {
	if (obj == nullptr) {
		throw NullPointerException("GetByteField on null object");
	}
	auto jenv = static_cast<NativeInterface *>(env);
	auto jobj = jenv->getHandles().fromJObject(obj);
	if (!jobj) {
		throw ClassCastException("GetByteField: invalid object");
	}
	auto &clazz = jobj->getClass();
	if (!clazz.hasField((uint32_t)(uintptr_t)fieldID)) {
		throw NoSuchFieldException(fmt::format("GetByteField: fieldID {} not found in class {}", (size_t)fieldID, clazz.getName()));
	}
	const auto &field = clazz.getField((uint32_t)(uintptr_t)fieldID);
	if (field.getType() != "B") {
		throw ClassCastException("GetByteField: field is not byte");
	}
	return (jbyte)field.getIntValue();
}

jchar NativeInterface::GetCharField(JNIEnv *env, jobject obj, jfieldID fieldID) {
	if (obj == nullptr) {
		throw NullPointerException("GetCharField on null object");
	}
	auto jenv = static_cast<NativeInterface *>(env);
	auto jobj = jenv->getHandles().fromJObject(obj);
	if (!jobj) {
		throw ClassCastException("GetCharField: invalid object");
	}
	auto &clazz = jobj->getClass();
	if (!clazz.hasField((uint32_t)(uintptr_t)fieldID)) {
		throw NoSuchFieldException(fmt::format("GetCharField: fieldID {} not found in class {}", (size_t)fieldID, clazz.getName()));
	}
	const auto &field = clazz.getField((uint32_t)(uintptr_t)fieldID);
	if (field.getType() != "C") {
		throw ClassCastException("GetCharField: field is not char");
	}
	return (jchar)field.getIntValue();
}

jshort NativeInterface::GetShortField(JNIEnv *env, jobject obj, jfieldID fieldID) {
	if (obj == nullptr) {
		throw NullPointerException("GetShortField on null object");
	}
	auto jenv = static_cast<NativeInterface *>(env);
	auto jobj = jenv->getHandles().fromJObject(obj);
	if (!jobj) {
		throw ClassCastException("GetShortField: invalid object");
	}
	auto &clazz = jobj->getClass();
	if (!clazz.hasField((uint32_t)(uintptr_t)fieldID)) {
		throw NoSuchFieldException(fmt::format("GetShortField: fieldID {} not found in class {}", (size_t)fieldID, clazz.getName()));
	}
	const auto &field = clazz.getField((uint32_t)(uintptr_t)fieldID);
	if (field.getType() != "S") {
		throw ClassCastException("GetShortField: field is not short");
	}
	return (jshort)field.getIntValue();
}

jint NativeInterface::GetIntField(JNIEnv *env, jobject obj, jfieldID fieldID) {
	if (obj == nullptr) {
		throw NullPointerException("GetIntField on null object");
	}
	auto jenv = static_cast<NativeInterface *>(env);
	auto jobj = jenv->getHandles().fromJObject(obj);
	if (!jobj) {
		throw ClassCastException("GetIntField: invalid object");
	}
	auto &clazz = jobj->getClass();
	if (!clazz.hasField((uint32_t)(uintptr_t)fieldID)) {
		throw NoSuchFieldException(fmt::format("GetIntField: fieldID {} not found in class {}", (size_t)fieldID, clazz.getName()));
	}
	const auto &field = clazz.getField((uint32_t)(uintptr_t)fieldID);
	if (field.getType() != "I") {
		throw ClassCastException("GetIntField: field is not int");
	}
	return (jint)field.getIntValue();
}

jlong NativeInterface::GetLongField(JNIEnv *env, jobject obj, jfieldID fieldID) {
	if (obj == nullptr) {
		throw NullPointerException("GetLongField on null object");
	}
	auto jenv = static_cast<NativeInterface *>(env);
	auto jobj = jenv->getHandles().fromJObject(obj);
	if (!jobj) {
		throw ClassCastException("GetLongField: invalid object");
	}
	auto &clazz = jobj->getClass();
	if (!clazz.hasField((uint32_t)(uintptr_t)fieldID)) {
		throw NoSuchFieldException(fmt::format("GetLongField: fieldID {} not found in class {}", (size_t)fieldID, clazz.getName()));
	}
	const auto &field = clazz.getField((uint32_t)(uintptr_t)fieldID);
	if (field.getType() != "J") {
		throw ClassCastException("GetLongField: field is not long");
	}
	return (jlong)field.getLongValue();
}

jfloat NativeInterface::GetFloatField(JNIEnv *env, jobject obj, jfieldID fieldID) {
	if (obj == nullptr) {
		throw NullPointerException("GetFloatField on null object");
	}
	auto jenv = static_cast<NativeInterface *>(env);
	auto jobj = jenv->getHandles().fromJObject(obj);
	if (!jobj) {
		throw ClassCastException("GetFloatField: invalid object");
	}
	auto &clazz = jobj->getClass();
	if (!clazz.hasField((uint32_t)(uintptr_t)fieldID)) {
		throw NoSuchFieldException(fmt::format("GetFloatField: fieldID {} not found in class {}", (size_t)fieldID, clazz.getName()));
	}
	const auto &field = clazz.getField((uint32_t)(uintptr_t)fieldID);
	if (field.getType() != "F") {
		throw ClassCastException("GetFloatField: field is not float");
	}
	return (jfloat)field.getIntValue();
}

jdouble NativeInterface::GetDoubleField(JNIEnv *env, jobject obj, jfieldID fieldID) {
	if (obj == nullptr) {
		throw NullPointerException("GetDoubleField on null object");
	}
	auto jenv = static_cast<NativeInterface *>(env);
	auto jobj = jenv->getHandles().fromJObject(obj);
	if (!jobj) {
		throw ClassCastException("GetDoubleField: invalid object");
	}
	auto &clazz = jobj->getClass();
	if (!clazz.hasField((uint32_t)(uintptr_t)fieldID)) {
		throw NoSuchFieldException(fmt::format("GetDoubleField: fieldID {} not found in class {}", (size_t)fieldID, clazz.getName()));
	}
	const auto &field = clazz.getField((uint32_t)(uintptr_t)fieldID);
	if (field.getType() != "D") {
		throw ClassCastException("GetDoubleField: field is not double");
	}
	return (jdouble)field.getLongValue();
}

void NativeInterface::SetObjectField(JNIEnv *env, jobject obj, jfieldID fieldID, jobject val) {
	throw VmException("SetObjectField not implemented");
}
void NativeInterface::SetBooleanField(JNIEnv *env, jobject obj, jfieldID fieldID, jboolean val) {
	throw VmException("SetBooleanField not implemented");
}
void NativeInterface::SetByteField(JNIEnv *env, jobject obj, jfieldID fieldID, jbyte val) {
	throw VmException("SetByteField not implemented");
}
void NativeInterface::SetCharField(JNIEnv *env, jobject obj, jfieldID fieldID, jchar val) {
	throw VmException("SetCharField not implemented");
}
void NativeInterface::SetShortField(JNIEnv *env, jobject obj, jfieldID fieldID, jshort val) {
	throw VmException("SetShortField not implemented");
}
void NativeInterface::SetIntField(JNIEnv *env, jobject obj, jfieldID fieldID, jint val) {
	throw VmException("SetIntField not implemented");
}
void NativeInterface::SetLongField(JNIEnv *env, jobject obj, jfieldID fieldID, jlong val) {
	throw VmException("SetLongField not implemented");
}
void NativeInterface::SetFloatField(JNIEnv *env, jobject obj, jfieldID fieldID, jfloat val) {
	throw VmException("SetFloatField not implemented");
}
void NativeInterface::SetDoubleField(JNIEnv *env, jobject obj, jfieldID fieldID, jdouble val) {
	throw VmException("SetDoubleField not implemented");
}
jmethodID NativeInterface::GetStaticMethodID(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
	throw VmException("GetStaticMethodID not implemented");
}

jobject NativeInterface::CallStaticObjectMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	throw VmException("CallStaticObjectMethod not implemented");
}
jobject NativeInterface::CallStaticObjectMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
	throw VmException("CallStaticObjectMethodV not implemented");
}
jobject NativeInterface::CallStaticObjectMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw VmException("CallStaticObjectMethodA not implemented");
}

jboolean NativeInterface::CallStaticBooleanMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	throw VmException("CallStaticBooleanMethod not implemented");
}
jboolean NativeInterface::CallStaticBooleanMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
	throw VmException("CallStaticBooleanMethodV not implemented");
}
jboolean NativeInterface::CallStaticBooleanMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw VmException("CallStaticBooleanMethodA not implemented");
}

jbyte NativeInterface::CallStaticByteMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	throw VmException("CallStaticByteMethod not implemented");
}
jbyte NativeInterface::CallStaticByteMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
	throw VmException("CallStaticByteMethodV not implemented");
}
jbyte NativeInterface::CallStaticByteMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw VmException("CallStaticByteMethodA not implemented");
}

jchar NativeInterface::CallStaticCharMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	throw VmException("CallStaticCharMethod not implemented");
}
jchar NativeInterface::CallStaticCharMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
	throw VmException("CallStaticCharMethodV not implemented");
}
jchar NativeInterface::CallStaticCharMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw VmException("CallStaticCharMethodA not implemented");
}

jshort NativeInterface::CallStaticShortMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	throw VmException("CallStaticShortMethod not implemented");
}
jshort NativeInterface::CallStaticShortMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
	throw VmException("CallStaticShortMethodV not implemented");
}
jshort NativeInterface::CallStaticShortMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw VmException("CallStaticShortMethodA not implemented");
}

jint NativeInterface::CallStaticIntMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	throw VmException("CallStaticIntMethod not implemented");
}
jint NativeInterface::CallStaticIntMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
	throw VmException("CallStaticIntMethodV not implemented");
}
jint NativeInterface::CallStaticIntMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw VmException("CallStaticIntMethodA not implemented");
}

jlong NativeInterface::CallStaticLongMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	throw VmException("CallStaticLongMethod not implemented");
}
jlong NativeInterface::CallStaticLongMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
	throw VmException("CallStaticLongMethodV not implemented");
}
jlong NativeInterface::CallStaticLongMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw VmException("CallStaticLongMethodA not implemented");
}

jfloat NativeInterface::CallStaticFloatMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	throw VmException("CallStaticFloatMethod not implemented");
}
jfloat NativeInterface::CallStaticFloatMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
	throw VmException("CallStaticFloatMethodV not implemented");
}
jfloat NativeInterface::CallStaticFloatMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw VmException("CallStaticFloatMethodA not implemented");
}

jdouble NativeInterface::CallStaticDoubleMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...) {
	throw VmException("CallStaticDoubleMethod not implemented");
}
jdouble NativeInterface::CallStaticDoubleMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args) {
	throw VmException("CallStaticDoubleMethodV not implemented");
}
jdouble NativeInterface::CallStaticDoubleMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, const jvalue *args) {
	throw VmException("CallStaticDoubleMethodA not implemented");
}

void NativeInterface::CallStaticVoidMethod(JNIEnv *env, jclass cls, jmethodID methodID, ...) {
	throw VmException("CallStaticVoidMethod not implemented");
}
void NativeInterface::CallStaticVoidMethodV(JNIEnv *env, jclass cls, jmethodID methodID, va_list args) {
	throw VmException("CallStaticVoidMethodV not implemented");
}
void NativeInterface::CallStaticVoidMethodA(JNIEnv *env, jclass cls, jmethodID methodID, const jvalue *args) {
	throw VmException("CallStaticVoidMethodA not implemented");
}

jfieldID NativeInterface::GetStaticFieldID(JNIEnv *env, jclass clazz, const char *name, const char *sig) {
	throw VmException("GetStaticFieldID not implemented");
}
jobject NativeInterface::GetStaticObjectField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
	throw VmException("GetStaticObjectField not implemented");
}
jboolean NativeInterface::GetStaticBooleanField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
	throw VmException("GetStaticBooleanField not implemented");
}
jbyte NativeInterface::GetStaticByteField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
	throw VmException("GetStaticByteField not implemented");
}
jchar NativeInterface::GetStaticCharField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
	throw VmException("GetStaticCharField not implemented");
}
jshort NativeInterface::GetStaticShortField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
	throw VmException("GetStaticShortField not implemented");
}
jint NativeInterface::GetStaticIntField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
	throw VmException("GetStaticIntField not implemented");
}
jlong NativeInterface::GetStaticLongField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
	throw VmException("GetStaticLongField not implemented");
}
jfloat NativeInterface::GetStaticFloatField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
	throw VmException("GetStaticFloatField not implemented");
}
jdouble NativeInterface::GetStaticDoubleField(JNIEnv *env, jclass clazz, jfieldID fieldID) {
	throw VmException("GetStaticDoubleField not implemented");
}

void NativeInterface::SetStaticObjectField(JNIEnv *env, jclass clazz, jfieldID fieldID, jobject value) {
	throw VmException("SetStaticObjectField not implemented");
}
void NativeInterface::SetStaticBooleanField(JNIEnv *env, jclass clazz, jfieldID fieldID, jboolean value) {
	throw VmException("SetStaticBooleanField not implemented");
}
void NativeInterface::SetStaticByteField(JNIEnv *env, jclass clazz, jfieldID fieldID, jbyte value) {
	throw VmException("SetStaticByteField not implemented");
}
void NativeInterface::SetStaticCharField(JNIEnv *env, jclass clazz, jfieldID fieldID, jchar value) {
	throw VmException("SetStaticCharField not implemented");
}
void NativeInterface::SetStaticShortField(JNIEnv *env, jclass clazz, jfieldID fieldID, jshort value) {
	throw VmException("SetStaticShortField not implemented");
}
void NativeInterface::SetStaticIntField(JNIEnv *env, jclass clazz, jfieldID fieldID, jint value) {
	throw VmException("SetStaticIntField not implemented");
}
void NativeInterface::SetStaticLongField(JNIEnv *env, jclass clazz, jfieldID fieldID, jlong value) {
	throw VmException("SetStaticLongField not implemented");
}
void NativeInterface::SetStaticFloatField(JNIEnv *env, jclass clazz, jfieldID fieldID, jfloat value) {
	throw VmException("SetStaticFloatField not implemented");
}
void NativeInterface::SetStaticDoubleField(JNIEnv *env, jclass clazz, jfieldID fieldID, jdouble value) {
	throw VmException("SetStaticDoubleField not implemented");
}

jstring NativeInterface::NewString(JNIEnv *env, const jchar *unicode, jsize len) {
	throw VmException("NewString not implemented");
}
jsize NativeInterface::GetStringLength(JNIEnv *env, jstring str) {
	throw VmException("GetStringLength not implemented");
}
const jchar *NativeInterface::GetStringChars(JNIEnv *env, jstring str, jboolean *isCopy) {
	throw VmException("GetStringChars not implemented");
}
void NativeInterface::ReleaseStringChars(JNIEnv *env, jstring str, const jchar *chars) {
	throw VmException("ReleaseStringChars not implemented");
}

jstring NativeInterface::NewStringUTF(JNIEnv *env, const char *utf) {
	throw VmException("NewStringUTF not implemented");
}
jsize NativeInterface::GetStringUTFLength(JNIEnv *env, jstring str) {
	throw VmException("GetStringUTFLength not implemented");
}

const char *NativeInterface::GetStringUTFChars(JNIEnv *env, jstring str, jboolean *isCopy) {
	Object *obj = (Object *)str;
	if (obj == nullptr) {
		throw NullPointerException("GetStringUTFChars on null object");
	}
	if (!obj->isString()) {
		throw ClassCastException("Not a string");
	}
	logger.fdebug("env->GetStringUTFChars {}", obj->toString());
	char *utf = new char[obj->str().length() + 1];
	std::strncpy(utf, obj->str().c_str(), obj->str().length());
	utf[obj->str().length()] = '\0';  // Null-terminate the string
	if (isCopy != nullptr) {
		*isCopy = JNI_FALSE;  // Assuming we don't need to copy the string
	}
	return utf;
}
void NativeInterface::ReleaseStringUTFChars(JNIEnv *env, jstring str, const char *chars) {
	Object *obj = (Object *)str;
	if (obj == nullptr) {
		throw NullPointerException("ReleaseStringUTFChars on null object");
	}
	logger.fdebug("env->ReleaseStringUTFChars {}", obj->toString());
	if (chars) {
		// If we allocated memory for chars, we should delete it
		delete[] chars;
	}
}

jsize NativeInterface::GetArrayLength(JNIEnv *env, jarray array) {
	auto obj = sandvik::native::getArray(array);
	return (jsize)obj->getArrayLength();
}

jobjectArray NativeInterface::NewObjectArray(JNIEnv *env, jsize len, jclass clazz, jobject init) {
	throw VmException("NewObjectArray not implemented");
}
jobject NativeInterface::GetObjectArrayElement(JNIEnv *env, jobjectArray array, jsize index) {
	throw VmException("GetObjectArrayElement not implemented");
}
void NativeInterface::SetObjectArrayElement(JNIEnv *env, jobjectArray array, jsize index, jobject val) {
	throw VmException("SetObjectArrayElement not implemented");
}
jbooleanArray NativeInterface::NewBooleanArray(JNIEnv *env, jsize len) {
	auto jenv = static_cast<NativeInterface *>(env);
	ClassLoader &classloader = jenv->getClassLoader();
	const auto &type = classloader.getOrLoad("boolean");
	auto arrayObj = Array::make(type, len);
	return (jbooleanArray)jenv->getHandles().toJObject(arrayObj);
}
jbyteArray NativeInterface::NewByteArray(JNIEnv *env, jsize len) {
	auto jenv = static_cast<NativeInterface *>(env);
	ClassLoader &classloader = jenv->getClassLoader();
	const auto &type = classloader.getOrLoad("byte");
	auto arrayObj = Array::make(type, len);
	return (jbyteArray)jenv->getHandles().toJObject(arrayObj);
}
jcharArray NativeInterface::NewCharArray(JNIEnv *env, jsize len) {
	auto jenv = static_cast<NativeInterface *>(env);
	ClassLoader &classloader = jenv->getClassLoader();
	const auto &type = classloader.getOrLoad("char");
	auto arrayObj = Array::make(type, len);
	return (jcharArray)jenv->getHandles().toJObject(arrayObj);
}
jshortArray NativeInterface::NewShortArray(JNIEnv *env, jsize len) {
	auto jenv = static_cast<NativeInterface *>(env);
	ClassLoader &classloader = jenv->getClassLoader();
	const auto &type = classloader.getOrLoad("short");
	auto arrayObj = Array::make(type, len);
	return (jshortArray)jenv->getHandles().toJObject(arrayObj);
}
jintArray NativeInterface::NewIntArray(JNIEnv *env, jsize len) {
	auto jenv = static_cast<NativeInterface *>(env);
	ClassLoader &classloader = jenv->getClassLoader();
	const auto &type = classloader.getOrLoad("int");
	auto arrayObj = Array::make(type, len);
	return (jintArray)jenv->getHandles().toJObject(arrayObj);
}
jlongArray NativeInterface::NewLongArray(JNIEnv *env, jsize len) {
	auto jenv = static_cast<NativeInterface *>(env);
	ClassLoader &classloader = jenv->getClassLoader();
	const auto &type = classloader.getOrLoad("long");
	auto arrayObj = Array::make(type, len);
	return (jlongArray)jenv->getHandles().toJObject(arrayObj);
}
jfloatArray NativeInterface::NewFloatArray(JNIEnv *env, jsize len) {
	auto jenv = static_cast<NativeInterface *>(env);
	ClassLoader &classloader = jenv->getClassLoader();
	const auto &type = classloader.getOrLoad("float");
	auto arrayObj = Array::make(type, len);
	return (jfloatArray)jenv->getHandles().toJObject(arrayObj);
}
jdoubleArray NativeInterface::NewDoubleArray(JNIEnv *env, jsize len) {
	auto jenv = static_cast<NativeInterface *>(env);
	ClassLoader &classloader = jenv->getClassLoader();
	const auto &type = classloader.getOrLoad("double");
	auto arrayObj = Array::make(type, len);
	return (jdoubleArray)jenv->getHandles().toJObject(arrayObj);
}

jboolean *NativeInterface::GetBooleanArrayElements(JNIEnv *env, jbooleanArray array, jboolean *isCopy) {
	throw VmException("GetBooleanArrayElements not implemented");
}
jbyte *NativeInterface::GetByteArrayElements(JNIEnv *env, jbyteArray array, jboolean *isCopy) {
	throw VmException("GetByteArrayElements not implemented");
}
jchar *NativeInterface::GetCharArrayElements(JNIEnv *env, jcharArray array, jboolean *isCopy) {
	throw VmException("GetCharArrayElements not implemented");
}
jshort *NativeInterface::GetShortArrayElements(JNIEnv *env, jshortArray array, jboolean *isCopy) {
	throw VmException("GetShortArrayElements not implemented");
}
jint *NativeInterface::GetIntArrayElements(JNIEnv *env, jintArray array, jboolean *isCopy) {
	throw VmException("GetIntArrayElements not implemented");
}
jlong *NativeInterface::GetLongArrayElements(JNIEnv *env, jlongArray array, jboolean *isCopy) {
	throw VmException("GetLongArrayElements not implemented");
}
jfloat *NativeInterface::GetFloatArrayElements(JNIEnv *env, jfloatArray array, jboolean *isCopy) {
	throw VmException("GetFloatArrayElements not implemented");
}
jdouble *NativeInterface::GetDoubleArrayElements(JNIEnv *env, jdoubleArray array, jboolean *isCopy) {
	throw VmException("GetDoubleArrayElements not implemented");
}

void NativeInterface::ReleaseBooleanArrayElements(JNIEnv *env, jbooleanArray array, jboolean *elems, jint mode) {
	throw VmException("ReleaseBooleanArrayElements not implemented");
}
void NativeInterface::ReleaseByteArrayElements(JNIEnv *env, jbyteArray array, jbyte *elems, jint mode) {
	throw VmException("ReleaseByteArrayElements not implemented");
}
void NativeInterface::ReleaseCharArrayElements(JNIEnv *env, jcharArray array, jchar *elems, jint mode) {
	throw VmException("ReleaseCharArrayElements not implemented");
}
void NativeInterface::ReleaseShortArrayElements(JNIEnv *env, jshortArray array, jshort *elems, jint mode) {
	throw VmException("ReleaseShortArrayElements not implemented");
}
void NativeInterface::ReleaseIntArrayElements(JNIEnv *env, jintArray array, jint *elems, jint mode) {
	throw VmException("ReleaseIntArrayElements not implemented");
}
void NativeInterface::ReleaseLongArrayElements(JNIEnv *env, jlongArray array, jlong *elems, jint mode) {
	throw VmException("ReleaseLongArrayElements not implemented");
}
void NativeInterface::ReleaseFloatArrayElements(JNIEnv *env, jfloatArray array, jfloat *elems, jint mode) {
	throw VmException("ReleaseFloatArrayElements not implemented");
}
void NativeInterface::ReleaseDoubleArrayElements(JNIEnv *env, jdoubleArray array, jdouble *elems, jint mode) {
	throw VmException("ReleaseDoubleArrayElements not implemented");
}
void NativeInterface::GetBooleanArrayRegion(JNIEnv *env, jbooleanArray array, jsize start, jsize l, jboolean *buf) {
	throw VmException("GetBooleanArrayRegion not implemented");
}
void NativeInterface::GetByteArrayRegion(JNIEnv *env, jbyteArray array, jsize start, jsize len, jbyte *buf) {
	throw VmException("GetByteArrayRegion not implemented");
}
void NativeInterface::GetCharArrayRegion(JNIEnv *env, jcharArray array, jsize start, jsize len, jchar *buf) {
	auto jenv = static_cast<NativeInterface *>(env);
	auto arrObj = jenv->getHandles().fromJObject(array);
	if (!arrObj || !arrObj->isArray()) {
		throw ClassCastException("GetCharArrayRegion: not an array");
	}
	Array &arr = static_cast<Array &>(*arrObj);
	if (start < 0 || len < 0 || (uint32_t)(start + len) > arr.getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("GetCharArrayRegion: invalid start/len");
	}
	if (!buf) {
		throw NullPointerException("GetCharArrayRegion: buf is null");
	}
	for (jsize i = 0; i < len; ++i) {
		buf[i] = (jchar)arr.getElement(start + i)->getValue();
	}
}
void NativeInterface::GetShortArrayRegion(JNIEnv *env, jshortArray array, jsize start, jsize len, jshort *buf) {
	throw VmException("GetShortArrayRegion not implemented");
}
void NativeInterface::GetIntArrayRegion(JNIEnv *env, jintArray array, jsize start, jsize len, jint *buf) {
	throw VmException("GetIntArrayRegion not implemented");
}
void NativeInterface::GetLongArrayRegion(JNIEnv *env, jlongArray array, jsize start, jsize len, jlong *buf) {
	throw VmException("GetLongArrayRegion not implemented");
}
void NativeInterface::GetFloatArrayRegion(JNIEnv *env, jfloatArray array, jsize start, jsize len, jfloat *buf) {
	throw VmException("GetFloatArrayRegion not implemented");
}
void NativeInterface::GetDoubleArrayRegion(JNIEnv *env, jdoubleArray array, jsize start, jsize len, jdouble *buf) {
	throw VmException("GetDoubleArrayRegion not implemented");
}

void NativeInterface::SetBooleanArrayRegion(JNIEnv *env, jbooleanArray array, jsize start, jsize l, const jboolean *buf) {
	throw VmException("SetBooleanArrayRegion not implemented");
}
void NativeInterface::SetByteArrayRegion(JNIEnv *env, jbyteArray array, jsize start, jsize len, const jbyte *buf) {
	throw VmException("SetByteArrayRegion not implemented");
}
void NativeInterface::SetCharArrayRegion(JNIEnv *env, jcharArray array, jsize start, jsize len, const jchar *buf) {
	auto jenv = static_cast<NativeInterface *>(env);
	auto arrObj = jenv->getHandles().fromJObject(array);
	if (!arrObj->isArray()) {
		throw ClassCastException("SetCharArrayRegion: not an array");
	}
	Array &arr = static_cast<Array &>(*arrObj);
	if (start < 0 || len < 0 || (uint32_t)(start + len) > arr.getArrayLength()) {
		throw ArrayIndexOutOfBoundsException("SetCharArrayRegion: invalid start/len");
	}
	if (!buf) {
		throw NullPointerException("SetCharArrayRegion: buf is null");
	}
	for (jsize i = 0; i < len; ++i) {
		arr.setElement(start + i, Object::make((uint64_t)buf[i]));
	}
}
void NativeInterface::SetShortArrayRegion(JNIEnv *env, jshortArray array, jsize start, jsize len, const jshort *buf) {
	throw VmException("SetShortArrayRegion not implemented");
}
void NativeInterface::SetIntArrayRegion(JNIEnv *env, jintArray array, jsize start, jsize len, const jint *buf) {
	throw VmException("SetIntArrayRegion not implemented");
}
void NativeInterface::SetLongArrayRegion(JNIEnv *env, jlongArray array, jsize start, jsize len, const jlong *buf) {
	throw VmException("SetLongArrayRegion not implemented");
}
void NativeInterface::SetFloatArrayRegion(JNIEnv *env, jfloatArray array, jsize start, jsize len, const jfloat *buf) {
	throw VmException("SetFloatArrayRegion not implemented");
}
void NativeInterface::SetDoubleArrayRegion(JNIEnv *env, jdoubleArray array, jsize start, jsize len, const jdouble *buf) {
	throw VmException("SetDoubleArrayRegion not implemented");
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
		logger.fdebug("JNI: RegisterNatives {}{} -> {}", method->name, method->signature, method->fnPtr);
	}
	return JNI_OK;
}
jint NativeInterface::UnregisterNatives(JNIEnv *env, jclass clazz) {
	throw VmException("UnregisterNatives not implemented");
}

jint NativeInterface::MonitorEnter(JNIEnv *env, jobject obj) {
	throw VmException("MonitorEnter not implemented");
}
jint NativeInterface::MonitorExit(JNIEnv *env, jobject obj) {
	throw VmException("MonitorExit not implemented");
}

jint NativeInterface::GetJavaVM(JNIEnv *env, JavaVM **vm) {
	throw VmException("GetJavaVM not implemented");
}

void NativeInterface::GetStringRegion(JNIEnv *env, jstring str, jsize start, jsize len, jchar *buf) {
	throw VmException("GetStringRegion not implemented");
}
void NativeInterface::GetStringUTFRegion(JNIEnv *env, jstring str, jsize start, jsize len, char *buf) {
	throw VmException("GetStringUTFRegion not implemented");
}

void *NativeInterface::GetPrimitiveArrayCritical(JNIEnv *env, jarray array, jboolean *isCopy) {
	throw VmException("GetPrimitiveArrayCritical not implemented");
}
void NativeInterface::ReleasePrimitiveArrayCritical(JNIEnv *env, jarray array, void *carray, jint mode) {
	throw VmException("ReleasePrimitiveArrayCritical not implemented");
}

const jchar *NativeInterface::GetStringCritical(JNIEnv *env, jstring string, jboolean *isCopy) {
	throw VmException("GetStringCritical not implemented");
}
void NativeInterface::ReleaseStringCritical(JNIEnv *env, jstring string, const jchar *cstring) {
	throw VmException("ReleaseStringCritical not implemented");
}

jweak NativeInterface::NewWeakGlobalRef(JNIEnv *env, jobject obj) {
	throw VmException("NewWeakGlobalRef not implemented");
}
void NativeInterface::DeleteWeakGlobalRef(JNIEnv *env, jweak ref) {
	throw VmException("DeleteWeakGlobalRef not implemented");
}

jboolean NativeInterface::ExceptionCheck(JNIEnv *env) {
	throw VmException("ExceptionCheck not implemented");
}

jobject NativeInterface::NewDirectByteBuffer(JNIEnv *env, void *address, jlong capacity) {
	throw VmException("NewDirectByteBuffer not implemented");
}
void *NativeInterface::GetDirectBufferAddress(JNIEnv *env, jobject buf) {
	throw VmException("GetDirectBufferAddress not implemented");
}
jlong NativeInterface::GetDirectBufferCapacity(JNIEnv *env, jobject buf) {
	throw VmException("GetDirectBufferCapacity not implemented");
}

jobjectRefType NativeInterface::GetObjectRefType(JNIEnv *env, jobject obj) {
	throw VmException("GetObjectRefType not implemented");
}
