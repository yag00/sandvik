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

import android.content.ContentProvider;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.pm.ProviderInfo;
import android.os.IBinder;

import java.util.HashMap;
import java.util.Map;

/**
 * In-process "activity" service (ActivityManagerService): content providers published by Sandvik (the
 * system ones, like settings), every permission granted. The methods not overridden here return nothing
 * (see ActivityManagerServiceBase).
 */
final class SandvikActivityManager extends ActivityManagerServiceBase {
	private final Map<String, ContentProviderHolder> mProviders = new HashMap<>();

	/** Publishes an in-process provider, as if it was running in system_server */
	void publishProvider(String authority, String packageName, ContentProvider provider) {
		ProviderInfo info = new ProviderInfo();
		info.authority = authority;
		info.packageName = packageName;
		info.name = provider.getClass().getName();
		info.exported = true;
		info.enabled = true;
		info.applicationInfo = new ApplicationInfo();
		info.applicationInfo.packageName = packageName;
		provider.attachInfo(null, info);
		ContentProviderHolder holder = new ContentProviderHolder(info);
		holder.provider = provider.getIContentProvider();
		holder.noReleaseNeeded = true;
		mProviders.put(authority, holder);
	}

	@Override
	public ContentProviderHolder getContentProvider(IApplicationThread caller, String callingPackage, String name, int userId, boolean stable) {
		return mProviders.get(name);
	}

	@Override
	public boolean refContentProvider(IBinder connection, int stableDelta, int unstableDelta) {
		return true;
	}

	@Override
	public int checkPermission(String permission, int pid, int uid) {
		return PackageManager.PERMISSION_GRANTED;
	}
}
