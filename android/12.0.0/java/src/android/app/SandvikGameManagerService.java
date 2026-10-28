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

/**
 * GameManagerService (IGameManagerService): the GameManager constructor requires the service. No game mode is
 * supported, as for a package that is not a game.
 */
final class SandvikGameManagerService extends IGameManagerService.Stub {
	@Override
	public int getGameMode(String packageName, int userId) {
		return GameManager.GAME_MODE_UNSUPPORTED;
	}

	@Override
	public void setGameMode(String packageName, int gameMode, int userId) {
	}

	@Override
	public int[] getAvailableGameModes(String packageName) {
		return new int[0];
	}

	@Override
	public boolean getAngleEnabled(String packageName, int userId) {
		return false;
	}
}
