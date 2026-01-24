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

#include "array.hpp"
#include "class.hpp"
#include "classloader.hpp"
#include "exceptions.hpp"
#include "field.hpp"
#include "jni.hpp"
#include "native_utils.hpp"
#include "object.hpp"
#include "system/logger.hpp"

using namespace sandvik;

extern "C" {

	JNIEXPORT jint JNICALL Java_sun_misc_Unsafe_getArrayBaseOffsetForComponentType(JNIEnv* env, jobject obj, jclass componentType_) {
		auto componentType = native::getObject(componentType_);
		if (componentType->isNull()) {
			throw NullPointerException("Component type is null");
		}
		return 0;
	}

	JNIEXPORT jint JNICALL Java_sun_misc_Unsafe_getArrayIndexScaleForComponentType(JNIEnv* env, jobject obj, jclass componentType_) {
		auto componentType = native::getObject(componentType_);
		if (componentType->isNull()) {
			throw NullPointerException("Component type is null");
		}
		return 1;
	}

	JNIEXPORT jboolean JNICALL Java_sun_misc_Unsafe_compareAndSwapInt(JNIEnv*, jobject, jobject obj, jlong offset, jint expected, jint newValue) {
		auto object = sandvik::native::getObject(obj);
		size_t index = static_cast<size_t>(offset);
		if (object->isArray()) {
			auto array = static_cast<ArrayRef>(object);
			if (index >= array->getArrayLength()) {
				return JNI_FALSE;
			}
			auto val = array->getElement(index);
			if (!val->isNumberObject()) {
				return JNI_FALSE;
			}
			return val->compareAndSet(expected, newValue);
		} else {
			auto val = object->getField(index);
			if (!val->isNumberObject()) {
				return JNI_FALSE;
			}
			return val->compareAndSet(expected, newValue);
		}
	}

	JNIEXPORT jboolean JNICALL Java_sun_misc_Unsafe_compareAndSwapLong(JNIEnv*, jobject, jobject obj, jlong offset, jlong expected, jlong newValue) {
		auto object = sandvik::native::getObject(obj);
		size_t index = static_cast<size_t>(offset);
		if (object->isArray()) {
			auto array = static_cast<ArrayRef>(object);
			if (index >= array->getArrayLength()) {
				return JNI_FALSE;
			}
			auto val = array->getElement(index);
			if (!val->isNumberObject()) {
				return JNI_FALSE;
			}
			return val->compareAndSet(expected, newValue);
		} else {
			auto val = object->getField(index);
			if (!val->isNumberObject()) {
				return JNI_FALSE;
			}
			return val->compareAndSet(expected, newValue);
		}
	}

	JNIEXPORT jboolean JNICALL Java_sun_misc_Unsafe_compareAndSwapObject(JNIEnv*, jobject, jobject obj, jlong offset, jobject expected, jobject newValue) {
		auto object = native::getObject(obj);
		size_t index = static_cast<size_t>(offset);
		if (object->isArray()) {
			auto array = static_cast<ArrayRef>(object);
			return array->compareAndSwapElement(index, native::getObject(expected), native::getObject(newValue));

		} else {
			return object->compareAndSwapField(index, native::getObject(expected), native::getObject(newValue));
		}
	}

#if 0
JNIEXPORT void JNICALL Java_sun_misc_Unsafe_getIntVolatile(JNIEnv* env, jobject obj) {
	logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_sun_misc_Unsafe_putIntVolatile(JNIEnv* env, jobject obj) {
	logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_sun_misc_Unsafe_getLongVolatile(JNIEnv* env, jobject obj) {
	logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_sun_misc_Unsafe_putLongVolatile(JNIEnv* env, jobject obj) {
	logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_sun_misc_Unsafe_putOrderedInt(JNIEnv* env, jobject obj) {
	logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_sun_misc_Unsafe_putOrderedLong(JNIEnv* env, jobject obj) {
	logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

	JNIEXPORT jobject JNICALL Java_sun_misc_Unsafe_getObject(JNIEnv* env, jobject, jobject obj, jlong offset) {
		auto object = native::getObject(obj);
		size_t index = static_cast<size_t>(offset);
		if (object->isArray()) {
			auto array = static_cast<ArrayRef>(object);
			if (index >= array->getArrayLength()) {
				throw ArrayIndexOutOfBoundsException(fmt::format("index {} out of bounds for length {}", index, array->getArrayLength()));
			}
			return (jobject)array->getElement(index);
		} else {
			return (jobject)object->getField(index);
		}
	}

	JNIEXPORT void JNICALL Java_sun_misc_Unsafe_putObject(JNIEnv* env, jobject, jobject obj, jlong offset, jobject value) {
		auto object = native::getObject(obj);
		auto val = native::getObject(value);
		size_t index = static_cast<size_t>(offset);

		if (object->isArray()) {
			auto array = static_cast<ArrayRef>(object);
			if (index >= array->getArrayLength()) {
				throw ArrayIndexOutOfBoundsException(fmt::format("index {} out of bounds for length {}", index, array->getArrayLength()));
			}
			array->setElement(index, val);
		} else {
			object->setField(index, val);
		}
	}

	JNIEXPORT jobject JNICALL Java_sun_misc_Unsafe_getObjectVolatile(JNIEnv* env, jobject, jobject obj, jlong offset) {
		return Java_sun_misc_Unsafe_getObject(env, nullptr, obj, offset);
	}

	JNIEXPORT void JNICALL Java_sun_misc_Unsafe_putObjectVolatile(JNIEnv* env, jobject, jobject obj, jlong offset, jobject value) {
		Java_sun_misc_Unsafe_putObject(env, nullptr, obj, offset, value);
	}

#if 0
JNIEXPORT void JNICALL Java_sun_misc_Unsafe_putOrderedObject(JNIEnv* env, jobject obj) {
	logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif
#if 0
JNIEXPORT void JNICALL Java_sun_misc_Unsafe_park(JNIEnv* env, jobject obj) {
	logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_sun_misc_Unsafe_unpark(JNIEnv* env, jobject obj) {
	logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_sun_misc_Unsafe_allocateInstance(JNIEnv* env, jobject obj) {
	logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_sun_misc_Unsafe_addressSize(JNIEnv* env, jobject obj) {
	logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_sun_misc_Unsafe_pageSize(JNIEnv* env, jobject obj) {
	logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_sun_misc_Unsafe_allocateMemory(JNIEnv* env, jobject obj) {
	logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_sun_misc_Unsafe_freeMemory(JNIEnv* env, jobject obj) {
	logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_sun_misc_Unsafe_setMemory(JNIEnv* env, jobject obj) {
	logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

	JNIEXPORT jint JNICALL Java_sun_misc_Unsafe_getInt(JNIEnv* env, jobject, jobject obj, jlong offset) {
		auto object = sandvik::native::getObject(obj);
		size_t index = static_cast<size_t>(offset);
		if (object->isArray()) {
			auto array = static_cast<ArrayRef>(object);
			if (index >= array->getArrayLength()) {
				throw ArrayIndexOutOfBoundsException(fmt::format("index {} out of bounds for length {}", index, array->getArrayLength()));
			}
			auto val = array->getElement(index);
			if (!val->isNumberObject()) {
				throw IllegalArgumentException("Expected int array element");
			}
			return val->getValue();
		} else {
			auto val = object->getField(index);
			if (!val->isNumberObject()) {
				throw IllegalArgumentException("Expected int array element");
			}
			return val->getValue();
		}
	}

	JNIEXPORT void JNICALL Java_sun_misc_Unsafe_putInt(JNIEnv* env, jobject, jobject obj, jlong offset, jint value) {
		auto object = sandvik::native::getObject(obj);
		size_t index = static_cast<size_t>(offset);

		if (object->isArray()) {
			auto array = static_cast<ArrayRef>(object);
			if (index >= array->getArrayLength()) {
				throw ArrayIndexOutOfBoundsException(fmt::format("index {} out of bounds for length {}", index, array->getArrayLength()));
			}
			array->setElement(index, Object::make((int32_t)value));
		} else {
			object->setField(index, Object::make((int32_t)value));
		}
	}

	JNIEXPORT jlong JNICALL Java_sun_misc_Unsafe_getLong(JNIEnv* env, jobject, jobject obj, jlong offset) {
		auto object = sandvik::native::getObject(obj);
		size_t index = static_cast<size_t>(offset);
		if (object->isArray()) {
			auto array = static_cast<ArrayRef>(object);
			if (index >= array->getArrayLength()) {
				throw ArrayIndexOutOfBoundsException(fmt::format("index {} out of bounds for length {}", index, array->getArrayLength()));
			}
			auto val = array->getElement(index);
			if (!val->isNumberObject()) {
				throw IllegalArgumentException("Expected long array element");
			}
			return val->getLongValue();
		} else {
			auto val = object->getField(index);
			if (!val->isNumberObject()) {
				throw IllegalArgumentException("Expected long field value");
			}
			return val->getLongValue();
		}
	}

	JNIEXPORT void JNICALL Java_sun_misc_Unsafe_putLong(JNIEnv* env, jobject, jobject obj, jlong offset, jlong value) {
		auto object = sandvik::native::getObject(obj);
		size_t index = static_cast<size_t>(offset);

		if (object->isArray()) {
			auto array = static_cast<ArrayRef>(object);
			if (index >= array->getArrayLength()) {
				throw ArrayIndexOutOfBoundsException(fmt::format("index {} out of bounds for length {}", index, array->getArrayLength()));
			}
			array->setElement(index, Object::make((int64_t)value));
		} else {
			object->setField(index, Object::make((int64_t)value));
		}
	}

	JNIEXPORT jboolean JNICALL Java_sun_misc_Unsafe_getBoolean(JNIEnv* env, jobject, jobject obj, jlong offset) {
		return static_cast<jboolean>(Java_sun_misc_Unsafe_getInt(env, nullptr, obj, offset));
	}

	JNIEXPORT void JNICALL Java_sun_misc_Unsafe_putBoolean(JNIEnv* env, jobject, jobject obj, jlong offset, jboolean value) {
		Java_sun_misc_Unsafe_putInt(env, nullptr, obj, offset, static_cast<jint>(value));
	}

	JNIEXPORT jbyte JNICALL Java_sun_misc_Unsafe_getByte(JNIEnv* env, jobject, jobject obj, jlong offset) {
		return static_cast<jbyte>(Java_sun_misc_Unsafe_getInt(env, nullptr, obj, offset));
	}

	JNIEXPORT void JNICALL Java_sun_misc_Unsafe_putByte(JNIEnv* env, jobject, jobject obj, jlong offset, jbyte value) {
		Java_sun_misc_Unsafe_putInt(env, nullptr, obj, offset, static_cast<jint>(value));
	}

	JNIEXPORT jshort JNICALL Java_sun_misc_Unsafe_getShort(JNIEnv* env, jobject, jobject obj, jlong offset) {
		return static_cast<jshort>(Java_sun_misc_Unsafe_getInt(env, nullptr, obj, offset));
	}

	JNIEXPORT void JNICALL Java_sun_misc_Unsafe_putShort(JNIEnv* env, jobject, jobject obj, jlong offset, jshort value) {
		Java_sun_misc_Unsafe_putInt(env, nullptr, obj, offset, static_cast<jint>(value));
	}

	JNIEXPORT jchar JNICALL Java_sun_misc_Unsafe_getChar(JNIEnv* env, jobject, jobject obj, jlong offset) {
		return static_cast<jchar>(Java_sun_misc_Unsafe_getInt(env, nullptr, obj, offset));
	}

	JNIEXPORT void JNICALL Java_sun_misc_Unsafe_putChar(JNIEnv* env, jobject, jobject obj, jlong offset, jchar value) {
		Java_sun_misc_Unsafe_putInt(env, nullptr, obj, offset, static_cast<jint>(value));
	}

	JNIEXPORT jfloat JNICALL Java_sun_misc_Unsafe_getFloat(JNIEnv* env, jobject, jobject obj, jlong offset) {
		uint32_t v = Java_sun_misc_Unsafe_getInt(env, nullptr, obj, offset);
		return *reinterpret_cast<jfloat*>(&v);
	}

	JNIEXPORT void JNICALL Java_sun_misc_Unsafe_putFloat(JNIEnv* env, jobject, jobject obj, jlong offset, jfloat value) {
		Java_sun_misc_Unsafe_putInt(env, nullptr, obj, offset, *reinterpret_cast<jint*>(&value));
	}

	JNIEXPORT jdouble JNICALL Java_sun_misc_Unsafe_getDouble(JNIEnv* env, jobject, jobject obj, jlong offset) {
		uint64_t v = Java_sun_misc_Unsafe_getLong(env, nullptr, obj, offset);
		return *reinterpret_cast<jdouble*>(&v);
	}

	JNIEXPORT void JNICALL Java_sun_misc_Unsafe_putDouble(JNIEnv* env, jobject, jobject obj, jlong offset, jdouble value) {
		Java_sun_misc_Unsafe_putLong(env, nullptr, obj, offset, *reinterpret_cast<jlong*>(&value));
	}

#if 0
JNIEXPORT void JNICALL Java_sun_misc_Unsafe_copyMemoryToPrimitiveArray(JNIEnv* env, jobject obj) {
	logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_sun_misc_Unsafe_copyMemoryFromPrimitiveArray(JNIEnv* env, jobject obj) {
	logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

#if 0
JNIEXPORT void JNICALL Java_sun_misc_Unsafe_copyMemory(JNIEnv* env, jobject obj) {
	logger.fwarning("{} not implemented!", __FUNCTION__);
}
#endif

	JNIEXPORT void JNICALL Java_sun_misc_Unsafe_loadFence(JNIEnv* env, jobject obj) {
		logger.fwarning("{} no-op.", __FUNCTION__);
	}

	JNIEXPORT void JNICALL Java_sun_misc_Unsafe_storeFence(JNIEnv* env, jobject obj) {
		logger.fwarning("{} no-op.", __FUNCTION__);
	}

	JNIEXPORT void JNICALL Java_sun_misc_Unsafe_fullFence(JNIEnv* env, jobject obj) {
		logger.fwarning("{} no-op.", __FUNCTION__);
	}

}  // extern "C"
