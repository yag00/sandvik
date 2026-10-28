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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <fmt/format.h>
#include <jni/jni.h>

#include <cstdint>
#include <vector>

#include "class.hpp"
#include "classloader.hpp"
#include "frame.hpp"
#include "jni.hpp"
#include "jthread.hpp"
#include "method.hpp"
#include "native_utils.hpp"
#include "vm.hpp"

using namespace sandvik;

extern "C" {

	// The stack state is a long[] of (Method*, dex pc) pairs, innermost first: StackTraceElements are only built when
	// the trace is read (as ART does), most exceptions are caught without it
	JNIEXPORT jobject JNICALL Java_java_lang_Throwable_nativeFillInStackTrace(JNIEnv* env, jclass) {
		auto jenv = native::getNativeInterface(env);
		const auto frames = jenv->getVm().currentThread().getStackFrames();
		const auto& throwableClass = jenv->getClassLoader().getOrLoad("java.lang.Throwable");
		std::vector<jlong> state;
		state.reserve(frames.size() * 2);
		bool constructing = true;
		for (const auto* frame : frames) {
			const auto& method = frame->getMethod();
			// skip fillInStackTrace() and the constructors of the throwable
			if (constructing) {
				if (method.getName() == "fillInStackTrace" || (method.getName() == "<init>" && throwableClass.isInstanceOf(method.getClass()))) {
					continue;
				}
				constructing = false;
			}
			state.push_back(static_cast<jlong>(reinterpret_cast<uintptr_t>(&method)));
			state.push_back(frame->pc());
		}
		jlongArray result = env->NewLongArray(static_cast<jsize>(state.size()));
		env->SetLongArrayRegion(result, 0, static_cast<jsize>(state.size()), state.data());
		return result;
	}

	JNIEXPORT jobjectArray JNICALL Java_java_lang_Throwable_nativeGetStackTrace(JNIEnv* env, jclass, jobject stackState) {
		jclass steClass = env->FindClass("java/lang/StackTraceElement");
		if (stackState == nullptr) {
			return env->NewObjectArray(0, steClass, nullptr);
		}
		auto stateArray = static_cast<jlongArray>(stackState);
		const jsize size = env->GetArrayLength(stateArray);
		std::vector<jlong> state(static_cast<size_t>(size));
		env->GetLongArrayRegion(stateArray, 0, size, state.data());

		// StackTraceElement(String declaringClass, String methodName, String fileName, int lineNumber)
		jmethodID constructor = env->GetMethodID(steClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
		jobjectArray result = env->NewObjectArray(size / 2, steClass, nullptr);
		for (jsize i = 0; i + 1 < size; i += 2) {
			const auto* method = reinterpret_cast<const Method*>(static_cast<uintptr_t>(state[i]));
			jstring className = env->NewStringUTF(method->getClass().getFullname().c_str());
			jstring methodName = env->NewStringUTF(method->getName().c_str());
			// no debug info (line numbers, source file) is read from the dex: -1 unknown, -2 native method
			const jint lineNumber = method->isNative() ? -2 : -1;
			env->SetObjectArrayElement(result, i / 2, env->NewObject(steClass, constructor, className, methodName, nullptr, lineNumber));
		}
		return result;
	}

}  // extern "C"
