/*
* cgeThread.cpp
*
*  Created on: 2015-3-17
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#include "cgeThread.h"
#include <cassert>
#include <cstdio>

namespace CGE
{
	// CGEThreadPreemptive

	CGEThreadPreemptive::CGEThreadPreemptive() : m_thread(nullptr), m_taskRestart(false), m_threadOver(false), m_runningStatus(false)
	{

	}

	CGEThreadPreemptive::~CGEThreadPreemptive()
	{
		quit();
	}

	void CGEThreadPreemptive::run()
	{
		m_runningStatus = true;
		m_taskRestart = true;

		if(m_thread != nullptr)
		{
			m_mutex.lock();
			m_condition.notify_one();			
			m_mutex.unlock();
		}
		else
		{
			m_thread = new std::thread(std::bind(&CGEThreadPreemptive::_run, this));
		}
		
	}

	void CGEThreadPreemptive::_run()
	{
		for(;;)
		{
			{
				std::unique_lock<std::mutex> lock(m_mutex);

				if(m_threadOver)
					break;

				if(!m_taskRestart)
				{
					m_runningStatus = false;
					m_condition.wait(lock);

					if(m_threadOver)
						break;

					m_runningStatus = true;
				}

				m_taskRestart = false;
			}

			runTask();
		}
	}

	// void CGEThreadPreemptive::terminate()
	// {
	// 	//std::terminate();
	// }

	void CGEThreadPreemptive::quit()
	{
		m_mutex.lock();
		m_threadOver = true;
		m_mutex.unlock();

		m_condition.notify_all();

		if(m_thread != nullptr)
		{
			m_thread->join();
			delete m_thread;
			m_thread = nullptr;
		}
	}

	void CGEThreadPreemptive::join()
	{
		if(m_thread != nullptr && m_thread->joinable())
			m_thread->join();
	}

	//////////////////////////////////////////////////////////////////////////

    
    CGEThreadPool* CGEThreadPool::globalThreadPool = nullptr;
    
    void CGEThreadPool::setGlobalThreadNum(size_t maxThreadNum)
    {
        if(globalThreadPool != nullptr)
            delete globalThreadPool;
        globalThreadPool = new CGEThreadPool(maxThreadNum);
    }
    
    void CGEThreadPool::runOnGlobalPool(const CGE::CGEThreadPool::Work &work)
    {
        if(globalThreadPool == nullptr)
            globalThreadPool = new CGEThreadPool();
        globalThreadPool->run(work);
    }
    
    void CGEThreadPool::clearGlobalPool()
    {
        delete globalThreadPool;
    }
    
	CGEThreadPool::CGEThreadPool(std::list<std::unique_ptr<Worker>>::size_type maxWorkerNum) : m_threadOver(false), m_maxWorkerSize(maxWorkerNum), m_threadJoining(false)
	{
		assert(maxWorkerNum >= 1);
	}

	CGEThreadPool::~CGEThreadPool()
	{
		quit();
	}

	bool CGEThreadPool::isActive()
	{
		std::unique_lock<std::mutex> lock(m_threadMutex);

		if(!m_workList.empty())
			return true;

		for(auto& t : m_workerList)
		{
			if(t->isActive())
				return true;
		}
		return false;
	}

	bool CGEThreadPool::isBusy()
	{
		for(auto& t : m_workerList)
		{
			if(!t->isActive())
				return false;
		}
		return true;
	}
    
    void CGEThreadPool::wait4Active(long ms)
    {
        if(ms <= 0)
        {
            while(isActive())
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        else
        {
            while(isActive() && ms > 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                --ms;
            }
        }
    }
    
    void CGEThreadPool::wait4Busy(long ms)
    {
        if(ms <= 0)
        {
            while(isBusy())
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        else
        {
            while(isBusy() && ms > 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                --ms;
            }
        }
    }

	void CGEThreadPool::terminate()
	{

	}

	void CGEThreadPool::quit()
	{
		if(m_threadOver && m_workList.empty() && m_workerList.empty())
		{
			return;
		}

		m_threadMutex.lock();
		m_workList.clear();
		m_threadOver = true;
		m_threadMutex.unlock();

		m_poolMutex.lock();

		m_condition.notify_all();

		for(auto& t : m_workerList)
		{
			t->waitForQuit();
		}
		m_workerList.clear();
		m_poolMutex.unlock();
	}

	void CGEThreadPool::join()
	{
		m_poolMutex.lock();
		m_threadJoining = true;
		m_condition.notify_all();
		for(auto& t : m_workerList)
		{
			t->join();
		}
		m_workerList.clear();
		m_threadJoining = false;
		m_poolMutex.unlock();
	}

	void CGEThreadPool::run(const Work& work)
	{
		m_threadMutex.lock();
		m_workList.push_back(work);
		m_threadMutex.unlock();

		m_poolMutex.lock();

		if(m_workerList.size() < m_maxWorkerSize && isBusy())
		{
			m_workerList.push_back(std::unique_ptr<Worker>(new Worker(*this)));
			m_workerList.back()->run();
		}
		else if(!isBusy())
		{
			m_condition.notify_one();
		}
		m_poolMutex.unlock();
	}

	//////////////////////////////////////////////////////////////////////////

	CGEThreadPool::Worker::Worker(CGEThreadPool& pool) : m_thread(nullptr), m_pool(pool), m_runningStatus(false), m_shouldLeave(false)
	{

	}

	CGEThreadPool::Worker::Worker(Worker& worker) : m_pool(worker.m_pool)
	{

	}

	CGEThreadPool::Worker::Worker(Worker&& worker) : m_pool(worker.m_pool)
	{
		m_thread = worker.m_thread;
		m_runningStatus = worker.m_runningStatus;
		m_shouldLeave = worker.m_shouldLeave;
		worker.m_thread = nullptr;
	}

	CGEThreadPool::Worker::~Worker()
	{
		waitForQuit();
	}

	void CGEThreadPool::Worker::run()
	{
		if(m_thread == nullptr)
		{
			m_runningStatus = true;

			m_thread = new std::thread(std::bind(&CGEThreadPool::Worker::_run, this));
		}
	}

	void CGEThreadPool::Worker::_run()
	{
		for(;;)
		{
			CGEThreadPool::Work work;

			{
				std::unique_lock<std::mutex> lock(m_pool.m_threadMutex);

				if(m_pool.m_threadOver || m_shouldLeave)
					break;

				if(m_pool.m_workList.empty())
				{
					m_runningStatus = false;

					if(m_pool.m_threadJoining)
						break;

					m_pool.m_condition.wait(lock);

					if(m_pool.m_threadOver || m_shouldLeave)
						return;
				}

				if(m_pool.m_workList.empty())
					continue;

				work = std::move(m_pool.m_workList.front());
				m_pool.m_workList.pop_front();
				m_runningStatus = true; 
			}

			work.run();
		}
	}

	void CGEThreadPool::Worker::terminate()
	{

	}

	void CGEThreadPool::Worker::waitForQuit()
	{
		join();
		delete m_thread;
		m_thread = nullptr;
		m_runningStatus = false;
	}

	void CGEThreadPool::Worker::join()
	{
		if(m_thread != nullptr && m_thread->joinable())
			m_thread->join();
	}

}
