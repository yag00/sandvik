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

import java.util.HashMap;
import java.util.Map;

/**
 * In-process servicemanager: there is no binder driver nor system_server in Sandvik, so only the services added
 * here exist. An unknown service is absent (null), as on a device where it is not running, and framework code
 * takes its "service not available" path.
 */
public final class SandvikServiceManager extends IServiceManager.Stub {
	private final Map<String, IBinder> mServices = new HashMap<>();

	/** Installs this service manager as the one returned by ServiceManager.getIServiceManager() */
	public static SandvikServiceManager install() throws ReflectiveOperationException {
		SandvikServiceManager serviceManager = new SandvikServiceManager();
		java.lang.reflect.Field field = ServiceManager.class.getDeclaredField("sServiceManager");
		field.setAccessible(true);
		field.set(null, serviceManager);
		return serviceManager;
	}

	@Override
	public synchronized IBinder getService(String name) {
		return mServices.get(name);
	}

	@Override
	public synchronized IBinder checkService(String name) {
		return mServices.get(name);
	}

	@Override
	public synchronized void addService(String name, IBinder service, boolean allowIsolated, int dumpPriority) {
		mServices.put(name, service);
	}

	@Override
	public synchronized String[] listServices(int dumpPriority) {
		return mServices.keySet().toArray(new String[0]);
	}

	@Override
	public void registerForNotifications(String name, IServiceCallback callback) {
	}

	@Override
	public void unregisterForNotifications(String name, IServiceCallback callback) {
	}

	@Override
	public synchronized boolean isDeclared(String name) {
		return mServices.containsKey(name);
	}

	@Override
	public String[] getDeclaredInstances(String iface) {
		return new String[0];
	}

	@Override
	public String updatableViaApex(String name) {
		return null;
	}

	@Override
	public void registerClientCallback(String name, IBinder service, IClientCallback callback) {
	}

	@Override
	public synchronized void tryUnregisterService(String name, IBinder service) {
		mServices.remove(name);
	}

	@Override
	public ServiceDebugInfo[] getServiceDebugInfo() {
		return new ServiceDebugInfo[0];
	}
}
