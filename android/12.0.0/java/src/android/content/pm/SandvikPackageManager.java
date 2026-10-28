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

package android.content.pm;

import android.content.ComponentName;

/**
 * In-process "package" service (PackageManagerService): only the launched application is installed.
 * Every permission is granted. The methods not overridden here return nothing (see PackageManagerServiceBase).
 */
public final class SandvikPackageManager extends PackageManagerServiceBase {
	private final ApplicationInfo mApplicationInfo;
	private final ActivityInfo mActivityInfo;

	public SandvikPackageManager(ApplicationInfo applicationInfo, ActivityInfo activityInfo) {
		mApplicationInfo = applicationInfo;
		mActivityInfo = activityInfo;
	}

	private boolean isInstalled(String packageName) {
		return mApplicationInfo.packageName.equals(packageName);
	}

	@Override
	public PackageInfo getPackageInfo(String packageName, int flags, int userId) {
		if (!isInstalled(packageName)) {
			return null;
		}
		PackageInfo info = new PackageInfo();
		info.packageName = packageName;
		info.applicationInfo = new ApplicationInfo(mApplicationInfo);
		info.versionCode = mApplicationInfo.versionCode;
		if ((flags & PackageManager.GET_ACTIVITIES) != 0) {
			info.activities = new ActivityInfo[] {new ActivityInfo(mActivityInfo)};
		}
		return info;
	}

	@Override
	public int getPackageUid(String packageName, int flags, int userId) {
		return isInstalled(packageName) ? mApplicationInfo.uid : -1;
	}

	@Override
	public ApplicationInfo getApplicationInfo(String packageName, int flags, int userId) {
		return isInstalled(packageName) ? new ApplicationInfo(mApplicationInfo) : null;
	}

	@Override
	public ActivityInfo getActivityInfo(ComponentName component, int flags, int userId) {
		if (component == null || !isInstalled(component.getPackageName()) || !mActivityInfo.name.equals(component.getClassName())) {
			return null;
		}
		return new ActivityInfo(mActivityInfo);
	}

	@Override
	public String[] getPackagesForUid(int uid) {
		return uid == mApplicationInfo.uid ? new String[] {mApplicationInfo.packageName} : null;
	}

	@Override
	public String getNameForUid(int uid) {
		return uid == mApplicationInfo.uid ? mApplicationInfo.packageName : null;
	}

	@Override
	public int getComponentEnabledSetting(ComponentName component, int userId) {
		return PackageManager.COMPONENT_ENABLED_STATE_DEFAULT;
	}

	@Override
	public int getApplicationEnabledSetting(String packageName, int userId) {
		return PackageManager.COMPONENT_ENABLED_STATE_DEFAULT;
	}

	@Override
	public int checkPermission(String permName, String packageName, int userId) {
		return PackageManager.PERMISSION_GRANTED;
	}

	@Override
	public int checkUidPermission(String permName, int uid) {
		return PackageManager.PERMISSION_GRANTED;
	}
}
