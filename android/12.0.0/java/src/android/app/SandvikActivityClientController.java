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

import android.content.pm.ActivityInfo;
import android.os.IBinder;
import android.view.Display;

/**
 * ActivityTaskManagerService side of the activities (IActivityClientController), given to the app with the
 * LaunchActivityItem: one task on the default display, with the launched activity on top. Lifecycle reports
 * (activityResumed, activityPaused...) are accepted and ignored.
 */
final class SandvikActivityClientController extends ActivityClientControllerBase {
	private static final int TASK_ID = 1;

	@Override
	public int getDisplayId(IBinder token) {
		return Display.DEFAULT_DISPLAY;
	}

	@Override
	public int getTaskForActivity(IBinder token, boolean onlyRoot) {
		return TASK_ID;
	}

	@Override
	public boolean isTopOfTask(IBinder token) {
		return true;
	}

	@Override
	public int getRequestedOrientation(IBinder token) {
		return ActivityInfo.SCREEN_ORIENTATION_UNSPECIFIED;
	}

	@Override
	public int getLaunchedFromUid(IBinder token) {
		return android.os.Process.myUid();
	}
}
