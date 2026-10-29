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

#ifndef __ANDROID_CONTENT_RES_RESOURCESJNI_HPP__
#define __ANDROID_CONTENT_RES_RESOURCESJNI_HPP__

#include <jni/jni.h>

#include <memory>
#include <string>
#include <vector>

#include "exceptions.hpp"
#include "resources/asset_manager.hpp"
#include "resources/handle_registry.hpp"

// Glue between the android.content.res natives and the Sandvik resources implementation (src/resources)
namespace sandvik::resources::jni {
	/** @brief A theme keeps its AssetManager alive */
	struct ThemeHandle {
			std::shared_ptr<AssetManager> assetManager;
			Theme theme;
			explicit ThemeHandle(std::shared_ptr<AssetManager> assetManager_) : assetManager(std::move(assetManager_)), theme(*assetManager) {
			}
	};

	using ApkAssetsRegistry = HandleRegistry<ApkAssets>;
	using AssetManagerRegistry = HandleRegistry<AssetManager>;
	using StringPoolRegistry = HandleRegistry<StringPool>;
	using ThemeRegistry = HandleRegistry<ThemeHandle>;

	/** @brief Object of a handle, throws if the handle is unknown */
	template <typename T> std::shared_ptr<T> lookup(int64_t handle_, const char* what_) {
		auto object = HandleRegistry<T>::instance().get(handle_);
		if (!object) {
			throw VmException("{}: invalid native handle {}", what_, handle_);
		}
		return object;
	}

	/** @brief Java cookies are 1 based, -1 for no ApkAssets */
	inline jint toJavaCookie(Cookie cookie_) {
		return cookie_ == INVALID_COOKIE ? -1 : cookie_ + 1;
	}

	/** @brief Fills a TypedValue (string is set by the Java side from the cookie string pool), returns the Java cookie */
	inline jint copyValue(JNIEnv* env_, const SelectedValue& value_, jobject typedValue_) {
		jclass clazz = env_->GetObjectClass(typedValue_);
		env_->SetIntField(typedValue_, env_->GetFieldID(clazz, "type", "I"), value_.value.dataType);
		env_->SetIntField(typedValue_, env_->GetFieldID(clazz, "data", "I"), static_cast<jint>(value_.value.data));
		env_->SetObjectField(typedValue_, env_->GetFieldID(clazz, "string", "Ljava/lang/CharSequence;"), nullptr);
		env_->SetIntField(typedValue_, env_->GetFieldID(clazz, "resourceId", "I"), static_cast<jint>(value_.resid));
		env_->SetIntField(typedValue_, env_->GetFieldID(clazz, "changingConfigurations", "I"), static_cast<jint>(value_.flags));
		env_->SetIntField(typedValue_, env_->GetFieldID(clazz, "density", "I"), value_.density);
		const jint cookie = toJavaCookie(value_.cookie);
		env_->SetIntField(typedValue_, env_->GetFieldID(clazz, "assetCookie", "I"), cookie);
		return cookie;
	}

	inline jstring newString(JNIEnv* env_, const std::u16string& str_) {
		return env_->NewString(reinterpret_cast<const jchar*>(str_.data()), static_cast<jsize>(str_.size()));
	}

	inline jstring newString(JNIEnv* env_, const std::string& str_) {
		return env_->NewStringUTF(str_.c_str());
	}

	/** @brief Java string to UTF-8, empty for null */
	inline std::string toString(JNIEnv* env_, jstring str_) {
		if (str_ == nullptr) {
			return "";
		}
		const char* utf8 = env_->GetStringUTFChars(str_, nullptr);
		std::string result(utf8);
		env_->ReleaseStringUTFChars(str_, utf8);
		return result;
	}

	inline std::vector<uint32_t> toVector(JNIEnv* env_, jintArray array_) {
		std::vector<uint32_t> result;
		if (array_ == nullptr) {
			return result;
		}
		std::vector<jint> values(env_->GetArrayLength(array_));
		env_->GetIntArrayRegion(array_, 0, static_cast<jsize>(values.size()), values.data());
		result.assign(values.begin(), values.end());
		return result;
	}

	inline jintArray newIntArray(JNIEnv* env_, const std::vector<int32_t>& values_) {
		jintArray array = env_->NewIntArray(static_cast<jsize>(values_.size()));
		env_->SetIntArrayRegion(array, 0, static_cast<jsize>(values_.size()), values_.data());
		return array;
	}
}  // namespace sandvik::resources::jni

#endif  // __ANDROID_CONTENT_RES_RESOURCESJNI_HPP__
