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

#include <fmt/format.h>
#include <jni/jni.h>
#include <unicode/uregex.h>

#include <cstdint>
#include <string>

#include "exceptions.hpp"

// java.util.regex is backed by ICU4C uregex, as on Android (libcore_bridge PatternNative)

namespace {
	void closePattern(void* pattern_) {
		uregex_close(static_cast<URegularExpression*>(pattern_));
	}

	std::u16string toU16(JNIEnv* env_, jstring str_) {
		std::u16string result(env_->GetStringLength(str_), u'\0');
		env_->GetStringRegion(str_, 0, (jsize)result.size(), reinterpret_cast<jchar*>(result.data()));
		return result;
	}
}  // namespace

extern "C" {

	JNIEXPORT jlong JNICALL Java_com_android_icu_util_regex_PatternNative_compileImpl(JNIEnv* env, jclass, jstring regex, jint flags) {
		const auto pattern = toU16(env, regex);
		UParseError parseError{};
		UErrorCode status = U_ZERO_ERROR;
		// Java Pattern flags passed here have the same values as the UREGEX_* flags
		URegularExpression* result = uregex_open(pattern.data(), (int32_t)pattern.size(), (uint32_t)flags, &parseError, &status);
		if (U_FAILURE(status)) {
			uregex_close(result);
			const char* utf8 = env->GetStringUTFChars(regex, nullptr);
			std::string message = fmt::format("{} near index {}: {}", u_errorName(status), parseError.offset, utf8);
			env->ReleaseStringUTFChars(regex, utf8);
			throw sandvik::JavaException("java.util.regex.PatternSyntaxException", message);
		}
		return static_cast<jlong>(reinterpret_cast<uintptr_t>(result));
	}

	JNIEXPORT jlong JNICALL Java_com_android_icu_util_regex_PatternNative_getNativeFinalizer(JNIEnv*, jclass) {
		return static_cast<jlong>(reinterpret_cast<uintptr_t>(&closePattern));
	}

	JNIEXPORT jint JNICALL Java_com_android_icu_util_regex_PatternNative_getMatchedGroupIndexImpl(JNIEnv* env, jclass, jlong patternAddr, jstring groupName) {
		const auto name = toU16(env, groupName);
		UErrorCode status = U_ZERO_ERROR;
		auto* pattern = reinterpret_cast<URegularExpression*>(static_cast<uintptr_t>(patternAddr));
		const int32_t result = uregex_groupNumberFromName(pattern, name.data(), (int32_t)name.size(), &status);
		return U_SUCCESS(status) ? result : -1;
	}

}  // extern "C"
