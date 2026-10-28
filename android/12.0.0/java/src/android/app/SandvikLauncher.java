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

package android.app;

import android.app.servertransaction.ClientTransaction;
import android.app.servertransaction.LaunchActivityItem;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.ApplicationInfo;
import android.content.pm.ProviderInfoList;
import android.content.pm.SandvikPackageManager;
import android.content.res.CompatibilityInfo;
import android.content.res.Configuration;
import android.os.Binder;
import android.os.Bundle;
import android.os.IBinder;
import android.os.LocaleList;
import android.os.Looper;
import android.os.Process;
import android.os.SandvikServiceManager;
import android.provider.SandvikSettingsProvider;

import com.android.internal.os.RuntimeInit;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.Locale;

/**
 * Starts an application activity without system_server.
 *
 * Runs the same startup as ActivityThread.main(), then plays the ActivityManagerService
 * role: it sends bindApplication() and a LaunchActivityItem transaction to the local
 * ApplicationThread, which ActivityThread handles once Looper.loop() runs.
 * System services come from an in-process SandvikServiceManager (none by default).
 *
 * Usage: SandvikLauncher <apk path> <package name> <activity class>
 */
public final class SandvikLauncher {
	private SandvikLauncher() {
	}

	public static void main(String[] args) throws Exception {
		if (args.length < 3) {
			throw new IllegalArgumentException("usage: SandvikLauncher <apk> <package> <activity>");
		}
		final String apkPath = args[0];
		final String packageName = args[1];
		final String activityName = args[2];

		// ActivityThread.main(), with attach(false) replayed here without its ActivityManagerService call
		SandvikServiceManager serviceManager = SandvikServiceManager.install();
		Looper.prepareMainLooper();
		ActivityThread thread = new ActivityThread();
		attach(thread);
		if (ActivityThread.sMainThreadHandler == null) {
			ActivityThread.sMainThreadHandler = thread.getHandler();
		}

		// ActivityManagerService.attachApplication()
		IApplicationThread app = thread.getApplicationThread();
		ApplicationInfo appInfo = makeApplicationInfo(apkPath, packageName);
		ActivityInfo activityInfo = makeActivityInfo(appInfo, activityName);
		serviceManager.addService("package", new SandvikPackageManager(appInfo, activityInfo), false, 0);
		SandvikActivityManager activityManager = new SandvikActivityManager();
		activityManager.publishProvider(SandvikSettingsProvider.AUTHORITY, "com.android.providers.settings", new SandvikSettingsProvider());
		serviceManager.addService(Context.ACTIVITY_SERVICE, activityManager, false, 0);
		serviceManager.addService(Context.GAME_SERVICE, new SandvikGameManagerService(), false, 0);
		// installd creates the data directories when the package is installed
		new java.io.File(appInfo.dataDir).mkdirs();
		new java.io.File(appInfo.deviceProtectedDataDir).mkdirs();
		Configuration config = makeConfiguration();
		CompatibilityInfo compatInfo = CompatibilityInfo.DEFAULT_COMPATIBILITY_INFO;

		app.bindApplication(packageName, appInfo, ProviderInfoList.fromList(new ArrayList<>()), null, null, null, null, null, 0,
		    false, false, false, false, config, compatInfo, null, new Bundle(), null, null, null, new long[0], null);

		// ActivityTaskSupervisor.realStartActivityLocked(): launch only, the resume needs a window manager
		Intent intent = new Intent(Intent.ACTION_MAIN);
		intent.addCategory(Intent.CATEGORY_LAUNCHER);
		intent.setComponent(new ComponentName(packageName, activityName));
		IBinder token = new Binder();
		ClientTransaction transaction = ClientTransaction.obtain(app, token);
		transaction.addCallback(LaunchActivityItem.obtain(intent, 0, activityInfo, config, new Configuration(), compatInfo, packageName,
		    null, 0, null, null, null, null, null, true, null, new Binder(), new SandvikActivityClientController(), null, new Binder(), false));
		app.scheduleTransaction(transaction);

		Looper.loop();
	}

	/** ActivityThread.attach(false) minus ActivityManagerService.attachApplication(), which this launcher plays, and the DDMS app name */
	private static void attach(ActivityThread thread) throws ReflectiveOperationException {
		setField(thread, "sCurrentActivityThread", thread);
		setField(thread, "mConfigurationController", new ConfigurationController(thread));
		thread.mSystemThread = false;
		RuntimeInit.setApplicationObject(((IApplicationThread) thread.getApplicationThread()).asBinder());
	}

	private static void setField(ActivityThread thread, String name, Object value) throws ReflectiveOperationException {
		Field field = ActivityThread.class.getDeclaredField(name);
		field.setAccessible(true);
		field.set(thread, value);
	}

	private static Configuration makeConfiguration() {
		// Phone in portrait, 1080x2340 @ 420dpi
		Configuration config = new Configuration();
		config.setToDefaults();
		config.setLocales(new LocaleList(Locale.getDefault()));
		config.densityDpi = 420;
		config.screenWidthDp = 411;
		config.screenHeightDp = 891;
		config.smallestScreenWidthDp = 411;
		config.orientation = Configuration.ORIENTATION_PORTRAIT;
		config.screenLayout = Configuration.SCREENLAYOUT_SIZE_NORMAL | Configuration.SCREENLAYOUT_LONG_YES;
		config.uiMode = Configuration.UI_MODE_TYPE_NORMAL | Configuration.UI_MODE_NIGHT_NO;
		config.touchscreen = Configuration.TOUCHSCREEN_FINGER;
		config.keyboard = Configuration.KEYBOARD_NOKEYS;
		config.navigation = Configuration.NAVIGATION_NONAV;
		return config;
	}

	private static ApplicationInfo makeApplicationInfo(String apkPath, String packageName) {
		ApplicationInfo info = new ApplicationInfo();
		info.packageName = packageName;
		info.processName = packageName;
		info.sourceDir = apkPath;
		info.publicSourceDir = apkPath;
		info.dataDir = "/data/user/0/" + packageName;
		info.credentialProtectedDataDir = info.dataDir;
		info.deviceProtectedDataDir = "/data/user_de/0/" + packageName;
		info.nativeLibraryDir = "/data/app/" + packageName + "/lib";
		info.uid = Process.myUid();
		info.minSdkVersion = 31;
		info.targetSdkVersion = 31;
		info.flags = ApplicationInfo.FLAG_HAS_CODE | ApplicationInfo.FLAG_INSTALLED;
		info.enabled = true;
		return info;
	}

	private static ActivityInfo makeActivityInfo(ApplicationInfo appInfo, String activityName) {
		ActivityInfo info = new ActivityInfo();
		info.name = activityName;
		info.packageName = appInfo.packageName;
		info.processName = appInfo.processName;
		info.applicationInfo = appInfo;
		info.exported = true;
		info.enabled = true;
		info.launchMode = ActivityInfo.LAUNCH_MULTIPLE;
		return info;
	}
}
