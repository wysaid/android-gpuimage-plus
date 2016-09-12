/*
* cgeThread.h
*
*  Created on: 2015-3-17
*      Author: Wang Yang
*        Mail: admin@wysaid.org
*/

#ifndef _CGE_THREAD_H_
#define _CGE_THREAD_H_

//使用 C++11 的线程库封装出一些简单易用的线程模型

#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <memory>

namespace CGE
{
    template<class Type>
    class CGESyncReadWriteQueue
    {
    public:
        
        void putData4Write(const Type& data)
        {
            m_writeMutex.lock();
            m_list4Write.push(data);
            m_writeMutex.unlock();
        }
        
        void putData4Read(const Type& data)
        {
            m_readMutex.lock();
            m_list4Read.push(data);
            m_readMutex.unlock();
        }
        
        bool hasData4Write()
        {
            m_list4Write.lock();
            bool canWrite = !m_list4Write.empty();
            m_list4Write.unlock();
            return canWrite;
        }
        
        bool hasData4Read()
        {
            m_list4Read.lock();
            bool canRead = !m_list4Read.empty();
            m_list4Read.unlock();
            return canRead;
        }
        
        Type getData4Write()
        {
            std::unique_lock<std::mutex> lock(m_writeMutex);
            if(m_list4Write.empty())
                return Type();
            
            Type data = m_list4Write.front();
            m_list4Write.pop();
            return data;
        }
        
        Type getData4Read()
        {
            std::unique_lock<std::mutex> lock(m_readMutex);
            if(m_list4Read.empty())
                return Type();
            
            Type data = m_list4Read.front();
            m_list4Read.pop();
            return data;
        }
        
        std::list<Type>& list4Read()
        {
            return m_list4Read;
        }
        
        std::list<Type>& list4Write()
        {
            return m_list4Write;
        }
        
    private:

        std::list<Type> m_list4Read;
        std::list<Type> m_list4Write;
        std::mutex m_readMutex;
        std::mutex m_writeMutex;
    };
    
	/*
	class:     CGEThreadPreemptive

	适用场景： 屏幕实时刷新、slider实时更新结果等
	
	任务简介： 任务具有持续性， 长时间执行类似任务
	           并且当新任务出现时， 可以马上 丢弃正在执行的任务 或 顶替排队中的任务， 等待中的任务永远不会超过一个。
			   一个 CGEThreadPreemptive 实体最多创建一个线程。

	注意： 最好不要在多个线程同时调用 同一个 CGEThreadPreemptive 的 API
	*/

	class CGEThreadPreemptive
	{
	public:
		CGEThreadPreemptive();
		~CGEThreadPreemptive();

		inline bool isActive() const { return m_runningStatus; }
		inline bool isRunning() const { return m_thread != nullptr; }

		void run();
		// void terminate(); //暂不可用
		void quit();

		void join(); // 本线程一旦启动， 除非调用 quit或者线程类被析构， 否则不会自动结束

	protected:

		virtual void runTask() = 0; // 请在子类实现

		void _run();

	public:

		std::mutex& getMutex() { return m_mutex; }
		std::thread* getThread() { return m_thread; }
		std::condition_variable& getCondition() { return m_condition; }

	protected:
		std::thread* m_thread;
		std::mutex m_mutex;
		std::condition_variable m_condition;
		bool m_taskRestart, m_threadOver;
		bool m_runningStatus;
	};

	//////////////////////////////////////////////////////////////////////////

	class CGEThreadPool
	{
	private:
		class Worker;

	public:
		class Work;

		CGEThreadPool(std::list<std::unique_ptr<Worker>>::size_type maxThreadNum = 1);
		~CGEThreadPool();

		bool isActive(); // 是否至少有一个线程处于活跃状态
		bool isBusy();   // 是否所有线程都处于活跃状态
        
        //当线程池还有线程正在工作时， 等待, ms=0 时表示等到非active状态为止
        void wait4Active(long ms = 0);
        //当线程池处于busy状态时等待， ms=0 表示等到到非busy为止
        void wait4Busy(long ms = 0);

		void terminate();
		void quit();  // 清除所有未被执行的工作， 并且所有线程完成当前正在执行的工作之后， 回收所有资源并返回。

		void join();  // 当所有工作执行完毕之后回收所有资源并返回

		std::list<Work>::size_type totalWorks() { return m_workList.size(); }
		std::list<std::unique_ptr<Worker>>::size_type totalWorkers() { return m_workerList.size(); }

		void run(const Work& work);

	private:

		std::list<Work> m_workList;
		std::list<std::unique_ptr<Worker>> m_workerList;

		std::condition_variable m_condition;
		std::mutex m_threadMutex, m_poolMutex;
		std::list<std::unique_ptr<Worker>>::size_type m_maxWorkerSize;
		bool m_threadOver, m_threadJoining;
	};

	class CGEThreadPool::Work
	{
	public:
		Work() : func(nullptr), arg(NULL) {}
		Work(const std::function<void (void*)>& _func, void* _arg = NULL) : func(_func), arg(_arg) {}

		void run() { if(func != nullptr) func(arg); }

	private:
		std::function<void (void*)> func;
		void* arg;
	};

	class CGEThreadPool::Worker
	{
	public:
		Worker(CGEThreadPool& pool);
		Worker(Worker& worker);
		Worker(Worker&& worker);
		~Worker();

		inline bool isActive() const { return m_runningStatus; }
		inline bool isRunning() const { return m_thread != nullptr; }

		void shouldLeave() { m_shouldLeave = true; }

		void run();
		void terminate();
		void waitForQuit();

		void join();

	protected:

		void _run();

	private:
		std::thread* m_thread;
		CGEThreadPool& m_pool;
		bool m_runningStatus, m_shouldLeave;
	};
}

#define CGE_THREAD_WORK CGE::CGEThreadPool::Work

#endif