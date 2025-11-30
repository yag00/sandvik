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

#include <string.h>
#include <gtest/gtest.h>

#include <chrono>
#include <atomic>
#include <thread>
#include <system/thread.hpp>

using namespace sandvik;

class DummyThread : public Thread {
	public:
		DummyThread(const std::string& name) : Thread(name), _counter(0), _done(false) {}

		void onStart() override {
			_done = false;
		}

		int getCounter() const {
			return _counter.load();
		}

		void stopThread() {
			_done = true;
			stop();
		}

	protected:
		void loop() override {
			// Increment the counter in the loop
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			_counter++;
		}

		bool done() override {
			return _done;
		}

	private:
		std::atomic<int> _counter;
		std::atomic<bool> _done;
};


TEST(Thread, Basic) {
	DummyThread thread("JoinTestThread");
	EXPECT_EQ(Thread::ThreadState::NotStarted, thread.getState());
	EXPECT_EQ(thread.getCounter(), 0);
	// Start the thread
	thread.run();
	std::this_thread::sleep_for(std::chrono::milliseconds(300));  // Let the thread run for a while
	EXPECT_GT(thread.getCounter(), 1);
	EXPECT_EQ(true, thread.isRunning());
	EXPECT_EQ(Thread::ThreadState::Running, thread.getState());
	// Stop the thread
	thread.stopThread();
	// Join the thread
	thread.join();
	// Verify that the thread is no longer running
	EXPECT_FALSE(thread.isRunning());
}

TEST(Thread, StateTransitions) {
	DummyThread thread("TestThread");
	// Initial state
	EXPECT_EQ(Thread::ThreadState::NotStarted, thread.getState());
	// Start the thread
	thread.run(false);
	EXPECT_EQ(Thread::ThreadState::Running, thread.getState());
	// Suspend the thread
	thread.suspend();
	EXPECT_EQ(Thread::ThreadState::Suspended, thread.getState());
	// Resume the thread
	thread.resume();
	EXPECT_EQ(Thread::ThreadState::Running, thread.getState());
	// Stop the thread
	thread.stopThread();
	EXPECT_EQ(Thread::ThreadState::Stopped, thread.getState());
	// Join the thread
	thread.join();
	EXPECT_EQ(Thread::ThreadState::Stopped, thread.getState());
	EXPECT_FALSE(thread.isRunning());
}

TEST(Thread, SuspendAndResume) {
	DummyThread thread("TestThread");
	// Start the thread
	thread.run(false);
	std::this_thread::sleep_for(std::chrono::milliseconds(300));  // Let the thread run for a while
	// Suspend the thread
	thread.suspend();
	EXPECT_EQ(Thread::ThreadState::Suspended, thread.getState());
	int counterAfterSuspend = thread.getCounter();
	std::this_thread::sleep_for(std::chrono::milliseconds(300));  // Wait while the thread is suspended
	// Verify that the counter has not increased
	EXPECT_EQ(counterAfterSuspend, thread.getCounter());
	// Resume the thread
	thread.resume();
	std::this_thread::sleep_for(std::chrono::milliseconds(300));  // Let the thread run again
	// Verify that the counter has increased
	EXPECT_GT(thread.getCounter(), counterAfterSuspend);
	// Stop the thread
	thread.stopThread();
	thread.join();
	// Verify that the thread is no longer running
	EXPECT_FALSE(thread.isRunning());
}

TEST(Thread, StopWhileSuspended) {
	DummyThread thread("TestThread");

	// Start the thread
	thread.run(false);
	std::this_thread::sleep_for(std::chrono::milliseconds(300));  // Let the thread run for a while

	// Suspend the thread
	thread.suspend();
	EXPECT_EQ(Thread::ThreadState::Suspended, thread.getState());

	// Stop the thread while suspended
	thread.stopThread();
	EXPECT_EQ(Thread::ThreadState::Stopped, thread.getState());

	// Join the thread
	thread.join();
	EXPECT_FALSE(thread.isRunning());
}

TEST(Thread, DoubleStop) {
	DummyThread thread("TestThread");

	// Start the thread
	thread.run(false);
	std::this_thread::sleep_for(std::chrono::milliseconds(300));  // Let the thread run for a while

	// Stop the thread
	thread.stopThread();
	EXPECT_EQ(Thread::ThreadState::Stopped, thread.getState());

	// Call stop again
	thread.stopThread();
	EXPECT_EQ(Thread::ThreadState::Stopped, thread.getState());

	// Join the thread
	thread.join();
	EXPECT_FALSE(thread.isRunning());
}

TEST(Thread, JoinWithoutStart) {
	DummyThread thread("TestThread");
	// Join the thread without starting it
	thread.join();
	// Verify that the thread is not running
	EXPECT_FALSE(thread.isRunning());
	EXPECT_EQ(Thread::ThreadState::NotStarted, thread.getState());
	// Start the thread
	thread.run(false);
	std::this_thread::sleep_for(std::chrono::milliseconds(300));  // Let the thread run for a while
	// Stop the thread
	thread.stopThread();
	EXPECT_EQ(Thread::ThreadState::Stopped, thread.getState());
	// Join the thread
	thread.join();
	EXPECT_FALSE(thread.isRunning());
}

TEST(Thread, ImmediateStopAfterStart) {
	DummyThread thread("TestThread");
	// Start the thread
	thread.run(false);
	// Immediately stop the thread
	thread.stopThread();
	EXPECT_EQ(Thread::ThreadState::Stopped, thread.getState());
	// Join the thread
	thread.join();
	EXPECT_FALSE(thread.isRunning());
}

TEST(Thread, restartAfterStop) {
	DummyThread thread("TestThread");
	// Start the thread
	thread.run(false);
	// Immediately stop the thread
	thread.stopThread();
	EXPECT_EQ(Thread::ThreadState::Stopped, thread.getState());
	// Restart the thread
	thread.run(false);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Let the thread time to be in running state
	EXPECT_EQ(Thread::ThreadState::Running, thread.getState());
	// Stop the thread
	thread.stopThread();
	EXPECT_EQ(Thread::ThreadState::Stopped, thread.getState());
	// Join the thread
	thread.join();
	EXPECT_FALSE(thread.isRunning());
}
