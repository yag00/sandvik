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

package android.os;

/**
 * Sandvik replacement of the framework stub (the implementation ships with the StatsD mainline module, not available
 * in Sandvik): the module services are not registered, Context.getSystemService() returns null for them.
 */
public class StatsFrameworkInitializer {
	private StatsFrameworkInitializer() {
	}

	public static void setStatsServiceManager(StatsServiceManager statsServiceManager) {
	}

	public static void registerServiceWrappers() {
	}
}
