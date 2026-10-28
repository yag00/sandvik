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

package android.provider;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;

import java.util.HashMap;
import java.util.Map;

/**
 * In-process "settings" content provider (SettingsProvider of system_server), used by Settings.Global, Secure and
 * System. Settings are kept in memory: a setting never written is not defined, so the callers use their default.
 */
public final class SandvikSettingsProvider extends ContentProvider {
	public static final String AUTHORITY = "settings";

	private final Map<String, Map<String, String>> mTables = new HashMap<>();

	@Override
	public boolean onCreate() {
		return true;
	}

	/** Settings.NameValueCache uses call(): "GET_global", "PUT_secure"... with the setting name as argument */
	@Override
	public synchronized Bundle call(String method, String arg, Bundle extras) {
		final int separator = method.indexOf('_');
		if (separator < 0 || arg == null) {
			return null;
		}
		final String operation = method.substring(0, separator);
		final Map<String, String> table = mTables.computeIfAbsent(method.substring(separator + 1), k -> new HashMap<>());
		if ("GET".equals(operation)) {
			Bundle result = new Bundle();
			result.putString(Settings.NameValueTable.VALUE, table.get(arg));
			return result;
		}
		if ("PUT".equals(operation)) {
			table.put(arg, extras != null ? extras.getString(Settings.NameValueTable.VALUE) : null);
			return new Bundle();
		}
		return null;
	}

	@Override
	public Cursor query(Uri uri, String[] projection, String selection, String[] selectionArgs, String sortOrder) {
		return null;
	}

	@Override
	public String getType(Uri uri) {
		return null;
	}

	@Override
	public Uri insert(Uri uri, ContentValues values) {
		return null;
	}

	@Override
	public int delete(Uri uri, String selection, String[] selectionArgs) {
		return 0;
	}

	@Override
	public int update(Uri uri, ContentValues values, String selection, String[] selectionArgs) {
		return 0;
	}
}
