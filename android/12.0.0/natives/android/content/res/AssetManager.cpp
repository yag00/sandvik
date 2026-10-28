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

#include <atomic>
#include <string>
#include <vector>

#include "ResourcesJni.hpp"
#include "resources/attribute_resolution.hpp"
#include "system/logger.hpp"

using namespace sandvik;
using namespace sandvik::resources;
using namespace sandvik::resources::jni;

namespace {
	std::shared_ptr<AssetManager> getAssetManager(jlong ptr_) {
		return lookup<AssetManager>(ptr_, "AssetManager");
	}

	Theme& getTheme(jlong ptr_) {
		return lookup<ThemeHandle>(ptr_, "AssetManager.Theme")->theme;
	}

	// The theme free function is called with the theme handle, not a pointer
	void freeTheme(void* theme_) {
		ThemeRegistry::instance().remove(static_cast<int64_t>(reinterpret_cast<uintptr_t>(theme_)));
	}

	// TypedArray data arrays come from VMRuntime.addressOf(), which returns the array object itself
	jintArray fromAddress(jlong address_) {
		return reinterpret_cast<jintArray>(static_cast<uintptr_t>(address_));
	}

	void writeInts(JNIEnv* env_, jintArray array_, const std::vector<int32_t>& values_) {
		if (array_ == nullptr) {
			return;
		}
		const auto size = std::min<size_t>(values_.size(), static_cast<size_t>(env_->GetArrayLength(array_)));
		env_->SetIntArrayRegion(array_, 0, static_cast<jsize>(size), values_.data());
	}

	/** @brief Values of a bag item after reference resolution, in the TypedArray layout */
	SelectedValue resolvedItem(const AssetManager& assetManager_, const Bag& bag_, const Bag::Item& item_) {
		SelectedValue value;
		value.value = item_.value;
		value.cookie = item_.cookie;
		value.flags = bag_.typeSpecFlags;
		assetManager_.resolveReference(value);
		return value;
	}

	void warnXmlIgnored(jlong xmlParserPtr_, const char* function_) {
		if (xmlParserPtr_ != 0) {
			logger.fwarning("AssetManager.{}: XML attributes are not supported yet, ignored", function_);
		}
	}
}  // namespace

extern "C" {

	// AssetManager

	JNIEXPORT jlong JNICALL Java_android_content_res_AssetManager_nativeCreate(JNIEnv*, jclass) {
		return AssetManagerRegistry::instance().add(std::make_shared<AssetManager>());
	}

	JNIEXPORT void JNICALL Java_android_content_res_AssetManager_nativeDestroy(JNIEnv*, jclass, jlong ptr) {
		AssetManagerRegistry::instance().remove(ptr);
	}

	JNIEXPORT void JNICALL Java_android_content_res_AssetManager_nativeSetApkAssets(JNIEnv* env, jclass, jlong ptr, jobjectArray apkAssetsArray, jboolean) {
		std::vector<std::shared_ptr<ApkAssets>> apkAssets;
		const jsize count = env->GetArrayLength(apkAssetsArray);
		for (jsize i = 0; i < count; ++i) {
			jobject javaApkAssets = env->GetObjectArrayElement(apkAssetsArray, i);
			if (javaApkAssets == nullptr) {
				continue;
			}
			const jlong nativePtr = env->GetLongField(javaApkAssets, env->GetFieldID(env->GetObjectClass(javaApkAssets), "mNativePtr", "J"));
			apkAssets.push_back(lookup<ApkAssets>(nativePtr, "AssetManager.setApkAssets"));
		}
		getAssetManager(ptr)->setApkAssets(std::move(apkAssets));
	}

	JNIEXPORT void JNICALL Java_android_content_res_AssetManager_nativeSetConfiguration(JNIEnv* env, jclass, jlong ptr, jint mcc, jint mnc, jstring locale,
	                                                                                    jint orientation, jint touchscreen, jint density, jint keyboard,
	                                                                                    jint keyboardHidden, jint navigation, jint screenWidth,
	                                                                                    jint screenHeight, jint smallestScreenWidthDp, jint screenWidthDp,
	                                                                                    jint screenHeightDp, jint screenLayout, jint uiMode, jint colorMode,
	                                                                                    jint majorVersion) {
		ResConfig config;
		config.mcc = static_cast<uint16_t>(mcc);
		config.mnc = static_cast<uint16_t>(mnc);
		config.setBcp47Locale(toString(env, locale));
		config.orientation = static_cast<uint8_t>(orientation);
		config.touchscreen = static_cast<uint8_t>(touchscreen);
		config.density = static_cast<uint16_t>(density);
		config.keyboard = static_cast<uint8_t>(keyboard);
		config.inputFlags = static_cast<uint8_t>(keyboardHidden);
		config.navigation = static_cast<uint8_t>(navigation);
		config.screenWidth = static_cast<uint16_t>(screenWidth);
		config.screenHeight = static_cast<uint16_t>(screenHeight);
		config.smallestScreenWidthDp = static_cast<uint16_t>(smallestScreenWidthDp);
		config.screenWidthDp = static_cast<uint16_t>(screenWidthDp);
		config.screenHeightDp = static_cast<uint16_t>(screenHeightDp);
		config.screenLayout = static_cast<uint8_t>(screenLayout);
		// Configuration.SCREENLAYOUT_ROUND_MASK (0x300) goes to screenLayout2
		config.screenLayout2 = static_cast<uint8_t>((screenLayout & 0x300) >> 8);
		config.uiMode = static_cast<uint8_t>(uiMode);
		config.colorMode = static_cast<uint8_t>(colorMode);
		config.sdkVersion = static_cast<uint16_t>(majorVersion);
		logger.fdebug("AssetManager.setConfiguration: {}", config.toString());
		getAssetManager(ptr)->setConfiguration(config);
	}

	JNIEXPORT jobject JNICALL Java_android_content_res_AssetManager_nativeGetAssignedPackageIdentifiers(JNIEnv* env, jclass, jlong ptr, jboolean, jboolean) {
		jclass sparseArrayClass = env->FindClass("android/util/SparseArray");
		jobject result = env->NewObject(sparseArrayClass, env->GetMethodID(sparseArrayClass, "<init>", "()V"));
		jmethodID put = env->GetMethodID(sparseArrayClass, "put", "(ILjava/lang/Object;)V");
		for (const auto& [id, name] : getAssetManager(ptr)->getAssignedPackages()) {
			jvalue args[2];
			args[0].i = id;
			args[1].l = newString(env, name);
			env->CallVoidMethodA(result, put, args);
		}
		return result;
	}

	JNIEXPORT jboolean JNICALL Java_android_content_res_AssetManager_nativeContainsAllocatedTable(JNIEnv*, jclass, jlong) {
		return JNI_FALSE;
	}

	// Assets (assets/ files and non-asset files of the APK): not implemented yet

	JNIEXPORT jobjectArray JNICALL Java_android_content_res_AssetManager_nativeList(JNIEnv* env, jclass, jlong, jstring path) {
		logger.fwarning("AssetManager.list not implemented: {}", toString(env, path));
		return nullptr;
	}

	JNIEXPORT jlong JNICALL Java_android_content_res_AssetManager_nativeOpenAsset(JNIEnv* env, jclass, jlong, jstring fileName, jint) {
		logger.fwarning("AssetManager.openAsset not implemented: {}", toString(env, fileName));
		return 0;
	}

	JNIEXPORT jobject JNICALL Java_android_content_res_AssetManager_nativeOpenAssetFd(JNIEnv* env, jclass, jlong, jstring fileName, jlongArray) {
		throw IOException("AssetManager.openFd not implemented: " + toString(env, fileName));
	}

	JNIEXPORT jlong JNICALL Java_android_content_res_AssetManager_nativeOpenNonAsset(JNIEnv* env, jclass, jlong, jint, jstring fileName, jint) {
		logger.fwarning("AssetManager.openNonAsset not implemented: {}", toString(env, fileName));
		return 0;
	}

	JNIEXPORT jobject JNICALL Java_android_content_res_AssetManager_nativeOpenNonAssetFd(JNIEnv* env, jclass, jlong, jint, jstring fileName, jlongArray) {
		throw IOException("AssetManager.openNonAssetFd not implemented: " + toString(env, fileName));
	}

	JNIEXPORT jlong JNICALL Java_android_content_res_AssetManager_nativeOpenXmlAsset(JNIEnv* env, jclass, jlong, jint, jstring fileName) {
		logger.fwarning("AssetManager.openXmlAsset not implemented: {}", toString(env, fileName));
		return 0;
	}

	JNIEXPORT jlong JNICALL Java_android_content_res_AssetManager_nativeOpenXmlAssetFd(JNIEnv*, jclass, jlong, jint, jobject) {
		logger.fwarning("AssetManager.openXmlAssetFd not implemented");
		return 0;
	}

	// Values

	JNIEXPORT jint JNICALL Java_android_content_res_AssetManager_nativeGetResourceValue(JNIEnv* env, jclass, jlong ptr, jint resid, jshort density,
	                                                                                    jobject typedValue, jboolean resolveReferences) {
		auto assetManager = getAssetManager(ptr);
		auto value = assetManager->getResource(static_cast<uint32_t>(resid), false, static_cast<uint16_t>(density));
		if (!value || (resolveReferences && !assetManager->resolveReference(*value))) {
			return -1;
		}
		return copyValue(env, *value, typedValue);
	}

	JNIEXPORT jint JNICALL Java_android_content_res_AssetManager_nativeGetResourceBagValue(JNIEnv* env, jclass, jlong ptr, jint resid, jint bagEntryId,
	                                                                                       jobject typedValue) {
		auto assetManager = getAssetManager(ptr);
		auto bag = assetManager->getBag(static_cast<uint32_t>(resid));
		const auto* item = bag ? bag->find(static_cast<uint32_t>(bagEntryId)) : nullptr;
		if (item == nullptr) {
			return -1;
		}
		return copyValue(env, resolvedItem(*assetManager, *bag, *item), typedValue);
	}

	JNIEXPORT jintArray JNICALL Java_android_content_res_AssetManager_nativeGetStyleAttributes(JNIEnv* env, jclass, jlong ptr, jint resid) {
		auto bag = getAssetManager(ptr)->getBag(static_cast<uint32_t>(resid));
		if (!bag) {
			return nullptr;
		}
		std::vector<int32_t> keys;
		for (const auto& item : bag->items) {
			keys.push_back(static_cast<int32_t>(item.key));
		}
		return newIntArray(env, keys);
	}

	JNIEXPORT jobjectArray JNICALL Java_android_content_res_AssetManager_nativeGetResourceStringArray(JNIEnv* env, jclass, jlong ptr, jint resid) {
		auto assetManager = getAssetManager(ptr);
		auto bag = assetManager->getBag(static_cast<uint32_t>(resid));
		if (!bag) {
			return nullptr;
		}
		jobjectArray result = env->NewObjectArray(static_cast<jsize>(bag->items.size()), env->FindClass("java/lang/String"), nullptr);
		for (size_t i = 0; i < bag->items.size(); ++i) {
			auto value = resolvedItem(*assetManager, *bag, bag->items[i]);
			if (value.value.dataType != TYPE_STRING) {
				continue;
			}
			if (auto pool = assetManager->getStringPool(value.cookie)) {
				if (auto str = pool->stringAt(value.value.data)) {
					env->SetObjectArrayElement(result, static_cast<jsize>(i), newString(env, *str));
				}
			}
		}
		return result;
	}

	// (Java cookie, string index or -1) per item
	JNIEXPORT jintArray JNICALL Java_android_content_res_AssetManager_nativeGetResourceStringArrayInfo(JNIEnv* env, jclass, jlong ptr, jint resid) {
		auto assetManager = getAssetManager(ptr);
		auto bag = assetManager->getBag(static_cast<uint32_t>(resid));
		if (!bag) {
			return nullptr;
		}
		std::vector<int32_t> values;
		for (const auto& item : bag->items) {
			auto value = resolvedItem(*assetManager, *bag, item);
			values.push_back(toJavaCookie(value.cookie));
			values.push_back(value.value.dataType == TYPE_STRING ? static_cast<int32_t>(value.value.data) : -1);
		}
		return newIntArray(env, values);
	}

	JNIEXPORT jintArray JNICALL Java_android_content_res_AssetManager_nativeGetResourceIntArray(JNIEnv* env, jclass, jlong ptr, jint resid) {
		auto assetManager = getAssetManager(ptr);
		auto bag = assetManager->getBag(static_cast<uint32_t>(resid));
		if (!bag) {
			return nullptr;
		}
		std::vector<int32_t> values;
		for (const auto& item : bag->items) {
			auto value = resolvedItem(*assetManager, *bag, item);
			const bool isInt = value.value.dataType >= TYPE_INT_DEC && value.value.dataType <= TYPE_INT_COLOR_RGB4;
			values.push_back(isInt ? static_cast<int32_t>(value.value.data) : 0);
		}
		return newIntArray(env, values);
	}

	JNIEXPORT jint JNICALL Java_android_content_res_AssetManager_nativeGetResourceArraySize(JNIEnv*, jclass, jlong ptr, jint resid) {
		auto bag = getAssetManager(ptr)->getBag(static_cast<uint32_t>(resid));
		return bag ? static_cast<jint>(bag->items.size()) : -1;
	}

	// Items in the TypedArray layout (STYLE_NUM_ENTRIES ints each), returns the number of items
	JNIEXPORT jint JNICALL Java_android_content_res_AssetManager_nativeGetResourceArray(JNIEnv* env, jclass, jlong ptr, jint resid, jintArray outData) {
		auto assetManager = getAssetManager(ptr);
		auto bag = assetManager->getBag(static_cast<uint32_t>(resid));
		if (!bag) {
			return -1;
		}
		std::vector<int32_t> values(bag->items.size() * STYLE_NUM_ENTRIES, 0);
		for (size_t i = 0; i < bag->items.size(); ++i) {
			auto value = resolvedItem(*assetManager, *bag, bag->items[i]);
			if (value.value.dataType == TYPE_REFERENCE && value.value.data == 0) {
				value.value.dataType = TYPE_NULL;
				value.value.data = DATA_NULL_UNDEFINED;
				value.cookie = INVALID_COOKIE;
			}
			int32_t* out = &values[i * STYLE_NUM_ENTRIES];
			out[STYLE_TYPE] = value.value.dataType;
			out[STYLE_DATA] = static_cast<int32_t>(value.value.data);
			out[STYLE_ASSET_COOKIE] = toJavaCookie(value.cookie);
			out[STYLE_RESOURCE_ID] = static_cast<int32_t>(value.resid);
			out[STYLE_CHANGING_CONFIGURATIONS] = static_cast<int32_t>(value.flags);
			out[STYLE_DENSITY] = value.density;
		}
		writeInts(env, outData, values);
		return static_cast<jint>(bag->items.size());
	}

	// Names

	JNIEXPORT jint JNICALL Java_android_content_res_AssetManager_nativeGetResourceIdentifier(JNIEnv* env, jclass, jlong ptr, jstring name, jstring defType,
	                                                                                         jstring defPackage) {
		return static_cast<jint>(getAssetManager(ptr)->getResourceId(toString(env, name), toString(env, defType), toString(env, defPackage)));
	}

	JNIEXPORT jstring JNICALL Java_android_content_res_AssetManager_nativeGetResourceName(JNIEnv* env, jclass, jlong ptr, jint resid) {
		auto name = getAssetManager(ptr)->getResourceName(static_cast<uint32_t>(resid));
		return name ? newString(env, name->package + ":" + name->type + "/" + name->entry) : nullptr;
	}

	JNIEXPORT jstring JNICALL Java_android_content_res_AssetManager_nativeGetResourcePackageName(JNIEnv* env, jclass, jlong ptr, jint resid) {
		auto name = getAssetManager(ptr)->getResourceName(static_cast<uint32_t>(resid));
		return name ? newString(env, name->package) : nullptr;
	}

	JNIEXPORT jstring JNICALL Java_android_content_res_AssetManager_nativeGetResourceTypeName(JNIEnv* env, jclass, jlong ptr, jint resid) {
		auto name = getAssetManager(ptr)->getResourceName(static_cast<uint32_t>(resid));
		return name ? newString(env, name->type) : nullptr;
	}

	JNIEXPORT jstring JNICALL Java_android_content_res_AssetManager_nativeGetResourceEntryName(JNIEnv* env, jclass, jlong ptr, jint resid) {
		auto name = getAssetManager(ptr)->getResourceName(static_cast<uint32_t>(resid));
		return name ? newString(env, name->entry) : nullptr;
	}

	JNIEXPORT jobjectArray JNICALL Java_android_content_res_AssetManager_nativeGetLocales(JNIEnv* env, jclass, jlong ptr, jboolean) {
		const auto locales = getAssetManager(ptr)->getLocales();
		jobjectArray result = env->NewObjectArray(static_cast<jsize>(locales.size()), env->FindClass("java/lang/String"), nullptr);
		for (size_t i = 0; i < locales.size(); ++i) {
			env->SetObjectArrayElement(result, static_cast<jsize>(i), newString(env, locales[i]));
		}
		return result;
	}

	JNIEXPORT jobjectArray JNICALL Java_android_content_res_AssetManager_nativeGetSizeConfigurations(JNIEnv* env, jclass, jlong) {
		return env->NewObjectArray(0, env->FindClass("android/content/res/Configuration"), nullptr);
	}

	JNIEXPORT void JNICALL Java_android_content_res_AssetManager_nativeSetResourceResolutionLoggingEnabled(JNIEnv*, jclass, jlong, jboolean) {
	}

	JNIEXPORT jstring JNICALL Java_android_content_res_AssetManager_nativeGetLastResourceResolution(JNIEnv*, jclass, jlong) {
		return nullptr;
	}

	// Attribute resolution

	JNIEXPORT jintArray JNICALL Java_android_content_res_AssetManager_nativeAttributeResolutionStack(JNIEnv* env, jclass, jlong, jlong, jint, jint, jint) {
		return env->NewIntArray(0);
	}

	JNIEXPORT void JNICALL Java_android_content_res_AssetManager_nativeApplyStyle(JNIEnv* env, jclass, jlong, jlong themePtr, jint defStyleAttr,
	                                                                              jint defStyleRes, jlong xmlParserPtr, jintArray attrs, jlong outValuesAddress,
	                                                                              jlong outIndicesAddress) {
		warnXmlIgnored(xmlParserPtr, "applyStyle");
		std::vector<int32_t> values;
		std::vector<int32_t> indices;
		applyStyle(getTheme(themePtr), nullptr, static_cast<uint32_t>(defStyleAttr), static_cast<uint32_t>(defStyleRes), toVector(env, attrs), values, indices);
		writeInts(env, fromAddress(outValuesAddress), values);
		writeInts(env, fromAddress(outIndicesAddress), indices);
	}

	JNIEXPORT jboolean JNICALL Java_android_content_res_AssetManager_nativeResolveAttrs(JNIEnv* env, jclass, jlong, jlong themePtr, jint defStyleAttr,
	                                                                                    jint defStyleRes, jintArray inValues, jintArray attrs,
	                                                                                    jintArray outValues, jintArray outIndices) {
		std::vector<int32_t> values;
		std::vector<int32_t> indices;
		const bool result = resolveAttrs(getTheme(themePtr), static_cast<uint32_t>(defStyleAttr), static_cast<uint32_t>(defStyleRes), toVector(env, inValues),
		                                 toVector(env, attrs), values, indices);
		writeInts(env, outValues, values);
		writeInts(env, outIndices, indices);
		return result ? JNI_TRUE : JNI_FALSE;
	}

	JNIEXPORT jboolean JNICALL Java_android_content_res_AssetManager_nativeRetrieveAttributes(JNIEnv* env, jclass, jlong ptr, jlong xmlParserPtr,
	                                                                                          jintArray attrs, jintArray outValues, jintArray outIndices) {
		warnXmlIgnored(xmlParserPtr, "retrieveAttributes");
		std::vector<int32_t> values;
		std::vector<int32_t> indices;
		const bool result = retrieveAttributes(*getAssetManager(ptr), nullptr, toVector(env, attrs), values, indices);
		writeInts(env, outValues, values);
		writeInts(env, outIndices, indices);
		return result ? JNI_TRUE : JNI_FALSE;
	}

	// Themes

	JNIEXPORT jlong JNICALL Java_android_content_res_AssetManager_nativeThemeCreate(JNIEnv*, jclass, jlong ptr) {
		return ThemeRegistry::instance().add(std::make_shared<ThemeHandle>(getAssetManager(ptr)));
	}

	JNIEXPORT jlong JNICALL Java_android_content_res_AssetManager_nativeGetThemeFreeFunction(JNIEnv*, jclass) {
		return static_cast<jlong>(reinterpret_cast<uintptr_t>(&freeTheme));
	}

	JNIEXPORT void JNICALL Java_android_content_res_AssetManager_nativeThemeApplyStyle(JNIEnv*, jclass, jlong, jlong themePtr, jint resid, jboolean force) {
		if (!getTheme(themePtr).applyStyle(static_cast<uint32_t>(resid), force == JNI_TRUE)) {
			logger.fdebug("AssetManager.Theme.applyStyle: style {:#010x} not found", resid);
		}
	}

	JNIEXPORT void JNICALL Java_android_content_res_AssetManager_nativeThemeRebase(JNIEnv* env, jclass, jlong, jlong themePtr, jintArray styleIds,
	                                                                               jbooleanArray force, jint count) {
		auto& theme = getTheme(themePtr);
		theme.clear();
		const auto styles = toVector(env, styleIds);
		std::vector<jboolean> forces(count);
		if (count > 0) {
			env->GetBooleanArrayRegion(force, 0, count, forces.data());
		}
		for (jint i = 0; i < count && static_cast<size_t>(i) < styles.size(); ++i) {
			theme.applyStyle(styles[i], forces[i] == JNI_TRUE);
		}
	}

	JNIEXPORT void JNICALL Java_android_content_res_AssetManager_nativeThemeCopy(JNIEnv*, jclass, jlong, jlong dstThemePtr, jlong, jlong srcThemePtr) {
		getTheme(dstThemePtr).setTo(getTheme(srcThemePtr));
	}

	JNIEXPORT jint JNICALL Java_android_content_res_AssetManager_nativeThemeGetAttributeValue(JNIEnv* env, jclass, jlong ptr, jlong themePtr, jint resid,
	                                                                                          jobject typedValue, jboolean resolveReferences) {
		auto value = getTheme(themePtr).getAttribute(static_cast<uint32_t>(resid));
		if (!value || (resolveReferences && !getAssetManager(ptr)->resolveReference(*value))) {
			return -1;
		}
		return copyValue(env, *value, typedValue);
	}

	JNIEXPORT void JNICALL Java_android_content_res_AssetManager_nativeThemeDump(JNIEnv*, jclass, jlong, jlong, jint, jstring, jstring) {
	}

	JNIEXPORT jint JNICALL Java_android_content_res_AssetManager_nativeThemeGetChangingConfigurations(JNIEnv*, jclass, jlong themePtr) {
		return static_cast<jint>(getTheme(themePtr).getChangingConfigurations());
	}

	// Asset streams: no asset can be opened yet (see nativeOpenAsset)

	JNIEXPORT void JNICALL Java_android_content_res_AssetManager_nativeAssetDestroy(JNIEnv*, jclass, jlong) {
	}

	JNIEXPORT jint JNICALL Java_android_content_res_AssetManager_nativeAssetReadChar(JNIEnv*, jclass, jlong) {
		return -1;
	}

	JNIEXPORT jint JNICALL Java_android_content_res_AssetManager_nativeAssetRead(JNIEnv*, jclass, jlong, jbyteArray, jint, jint) {
		return -1;
	}

	JNIEXPORT jlong JNICALL Java_android_content_res_AssetManager_nativeAssetSeek(JNIEnv*, jclass, jlong, jlong, jint) {
		return -1;
	}

	JNIEXPORT jlong JNICALL Java_android_content_res_AssetManager_nativeAssetGetLength(JNIEnv*, jclass, jlong) {
		return 0;
	}

	JNIEXPORT jlong JNICALL Java_android_content_res_AssetManager_nativeAssetGetRemainingLength(JNIEnv*, jclass, jlong) {
		return 0;
	}

	// Overlays and debug

	JNIEXPORT jobject JNICALL Java_android_content_res_AssetManager_nativeGetOverlayableMap(JNIEnv*, jclass, jlong, jstring) {
		return nullptr;
	}

	JNIEXPORT jstring JNICALL Java_android_content_res_AssetManager_nativeGetOverlayablesToString(JNIEnv* env, jclass, jlong, jstring) {
		return newString(env, std::string());
	}

	JNIEXPORT jint JNICALL Java_android_content_res_AssetManager_getGlobalAssetCount(JNIEnv*, jclass) {
		return 0;
	}

	JNIEXPORT jstring JNICALL Java_android_content_res_AssetManager_getAssetAllocations(JNIEnv*, jclass) {
		return nullptr;
	}

	JNIEXPORT jint JNICALL Java_android_content_res_AssetManager_getGlobalAssetManagerCount(JNIEnv*, jclass) {
		return static_cast<jint>(AssetManagerRegistry::instance().size());
	}

}  // extern "C"
