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

#include <jni/jni.h>
#include <unicode/uregex.h>

#include <cstdint>
#include <string>
#include <vector>

#include "exceptions.hpp"

// java.util.regex matcher backed by ICU4C uregex, as on Android (libcore_bridge MatcherNative)

namespace {
	// A matcher owns a clone of the compiled pattern and the UTF-16 input it searches
	class MatcherState {
		public:
			explicit MatcherState(URegularExpression* pattern_) : _pattern(pattern_) {
			}
			~MatcherState() {
				uregex_close(_pattern);
			}
			MatcherState(const MatcherState&) = delete;
			MatcherState& operator=(const MatcherState&) = delete;

			URegularExpression* pattern() const {
				return _pattern;
			}

			void setInput(JNIEnv* env_, jstring text_, jint start_, jint end_) {
				_text.resize(env_->GetStringLength(text_));
				env_->GetStringRegion(text_, 0, (jsize)_text.size(), reinterpret_cast<jchar*>(_text.data()));
				UErrorCode status = U_ZERO_ERROR;
				uregex_setText(_pattern, _text.data(), (int32_t)_text.size(), &status);
				uregex_setRegion(_pattern, start_, end_, &status);
				check(status);
			}

			// offsets[2 * group] = start, offsets[2 * group + 1] = end, for groups 0..groupCount
			jboolean updateOffsets(JNIEnv* env_, jintArray offsets_, UBool found_, UErrorCode status_) {
				check(status_);
				if (!found_) {
					return JNI_FALSE;
				}
				UErrorCode status = U_ZERO_ERROR;
				const int32_t groupCount = uregex_groupCount(_pattern, &status);
				std::vector<jint> offsets(2 * (groupCount + 1));
				for (int32_t i = 0; i <= groupCount; ++i) {
					offsets[2 * i] = uregex_start(_pattern, i, &status);
					offsets[2 * i + 1] = uregex_end(_pattern, i, &status);
				}
				check(status);
				env_->SetIntArrayRegion(offsets_, 0, (jsize)offsets.size(), offsets.data());
				return JNI_TRUE;
			}

			static void check(UErrorCode status_) {
				if (U_FAILURE(status_)) {
					throw sandvik::JavaException("java.lang.RuntimeException", std::string("ICU regex error: ") + u_errorName(status_));
				}
			}

		private:
			URegularExpression* _pattern;
			std::u16string _text;
	};

	MatcherState* toMatcher(jlong address_) {
		return reinterpret_cast<MatcherState*>(static_cast<uintptr_t>(address_));
	}

	void closeMatcher(void* matcher_) {
		delete static_cast<MatcherState*>(matcher_);
	}
}  // namespace

extern "C" {

	JNIEXPORT jlong JNICALL Java_com_android_icu_util_regex_PatternNative_openMatcherImpl(JNIEnv*, jclass, jlong patternAddr) {
		UErrorCode status = U_ZERO_ERROR;
		URegularExpression* clone = uregex_clone(reinterpret_cast<URegularExpression*>(static_cast<uintptr_t>(patternAddr)), &status);
		MatcherState::check(status);
		return static_cast<jlong>(reinterpret_cast<uintptr_t>(new MatcherState(clone)));
	}

	JNIEXPORT jlong JNICALL Java_com_android_icu_util_regex_MatcherNative_getNativeFinalizer(JNIEnv*, jclass) {
		return static_cast<jlong>(reinterpret_cast<uintptr_t>(&closeMatcher));
	}

	JNIEXPORT jboolean JNICALL Java_com_android_icu_util_regex_MatcherNative_findImpl(JNIEnv* env, jclass, jlong addr, jint startIndex, jintArray offsets) {
		auto* matcher = toMatcher(addr);
		UErrorCode status = U_ZERO_ERROR;
		const UBool found = uregex_find(matcher->pattern(), startIndex, &status);
		return matcher->updateOffsets(env, offsets, found, status);
	}

	JNIEXPORT jboolean JNICALL Java_com_android_icu_util_regex_MatcherNative_findNextImpl(JNIEnv* env, jclass, jlong addr, jintArray offsets) {
		auto* matcher = toMatcher(addr);
		UErrorCode status = U_ZERO_ERROR;
		const UBool found = uregex_findNext(matcher->pattern(), &status);
		return matcher->updateOffsets(env, offsets, found, status);
	}

	JNIEXPORT jint JNICALL Java_com_android_icu_util_regex_MatcherNative_groupCountImpl(JNIEnv*, jclass, jlong addr) {
		UErrorCode status = U_ZERO_ERROR;
		const int32_t result = uregex_groupCount(toMatcher(addr)->pattern(), &status);
		MatcherState::check(status);
		return result;
	}

	JNIEXPORT jboolean JNICALL Java_com_android_icu_util_regex_MatcherNative_hitEndImpl(JNIEnv*, jclass, jlong addr) {
		UErrorCode status = U_ZERO_ERROR;
		const UBool result = uregex_hitEnd(toMatcher(addr)->pattern(), &status);
		MatcherState::check(status);
		return result ? JNI_TRUE : JNI_FALSE;
	}

	JNIEXPORT jboolean JNICALL Java_com_android_icu_util_regex_MatcherNative_lookingAtImpl(JNIEnv* env, jclass, jlong addr, jintArray offsets) {
		auto* matcher = toMatcher(addr);
		UErrorCode status = U_ZERO_ERROR;
		const UBool found = uregex_lookingAt(matcher->pattern(), -1, &status);
		return matcher->updateOffsets(env, offsets, found, status);
	}

	JNIEXPORT jboolean JNICALL Java_com_android_icu_util_regex_MatcherNative_matchesImpl(JNIEnv* env, jclass, jlong addr, jintArray offsets) {
		auto* matcher = toMatcher(addr);
		UErrorCode status = U_ZERO_ERROR;
		const UBool found = uregex_matches(matcher->pattern(), -1, &status);
		return matcher->updateOffsets(env, offsets, found, status);
	}

	JNIEXPORT jboolean JNICALL Java_com_android_icu_util_regex_MatcherNative_requireEndImpl(JNIEnv*, jclass, jlong addr) {
		UErrorCode status = U_ZERO_ERROR;
		const UBool result = uregex_requireEnd(toMatcher(addr)->pattern(), &status);
		MatcherState::check(status);
		return result ? JNI_TRUE : JNI_FALSE;
	}

	JNIEXPORT void JNICALL Java_com_android_icu_util_regex_MatcherNative_setInputImpl(JNIEnv* env, jclass, jlong addr, jstring text, jint start, jint end) {
		toMatcher(addr)->setInput(env, text, start, end);
	}

	JNIEXPORT void JNICALL Java_com_android_icu_util_regex_MatcherNative_useAnchoringBoundsImpl(JNIEnv*, jclass, jlong addr, jboolean value) {
		UErrorCode status = U_ZERO_ERROR;
		uregex_useAnchoringBounds(toMatcher(addr)->pattern(), value == JNI_TRUE, &status);
		MatcherState::check(status);
	}

	JNIEXPORT void JNICALL Java_com_android_icu_util_regex_MatcherNative_useTransparentBoundsImpl(JNIEnv*, jclass, jlong addr, jboolean value) {
		UErrorCode status = U_ZERO_ERROR;
		uregex_useTransparentBounds(toMatcher(addr)->pattern(), value == JNI_TRUE, &status);
		MatcherState::check(status);
	}

}  // extern "C"
