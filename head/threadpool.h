/*
 * OOPS - Particle Swarm Optimization for the Arc Orienteering Problem
 * Copyright (C) 2018	Manuel Weitzman
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __threadpool_h__
#define __threadpool_h__

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
private:
	std::vector<std::thread> _W; // workers
	std::queue< std::packaged_task<void()> > _T; // tasks

	std::mutex _m; // mutex
	std::condition_variable _c; // condition
	bool _r; // ready
public:
	ThreadPool(unsigned int = 0);
	ThreadPool(ThreadPool const &) = delete;
	ThreadPool &operator=(ThreadPool const &) = delete;
	~ThreadPool(void);

	template<typename F, typename ...ArgTypes>
	auto enqueue(F &&f, ArgTypes &&...a)
		-> std::future<typename std::result_of<F(ArgTypes...)>::type>;
};

ThreadPool::ThreadPool(unsigned int threads)
	: _W()
	, _T()
	, _m()
	, _c()
	, _r(false)
{
	if (!threads)
		threads = std::thread::hardware_concurrency() + 1;

	// Create workers, each try to get a job and do it while there are jobs
	// to do
	for (unsigned int i = 0; i < threads; i++) _W.emplace_back([this] {
		for ( ;; ) {
			std::packaged_task<void()> t;
			{
				std::unique_lock<std::mutex> guard(_m);
				_c.wait(guard, [this] {
					return _r || !_T.empty();
				});

				if (_r && _T.empty())
					return;

				t = std::move(_T.front()); _T.pop();
			}
			t();
		}
	});
}

ThreadPool::~ThreadPool(void)
{
	// Before destruction, allow al jobs to finish
	{
		std::unique_lock<std::mutex> guard(_m);
		_r = true;
	}
	_c.notify_all();

	// Wait for all jobs
	for (auto &w : _W)
		w.join();
}

template <typename F, typename... AT>
auto ThreadPool::enqueue(F &&f, AT &&...a)
	-> std::future<typename std::result_of<F(AT...)>::type>
{
	// Create a job, bind parameters to a function and enqueue it
	// deduce all types atomatically (this is a template after all)
	using ret_type = typename std::result_of<F(AT...)>::type;

	auto task = std::packaged_task<ret_type()>(std::bind(std::forward<F>(f),
						   std::forward<AT>(a)...));

	std::future<ret_type> promise = task.get_future();
	{
		std::unique_lock<std::mutex> guard(_m);
		_T.emplace(std::move(task));
	}
	_c.notify_one();
	return promise;
}

#endif
