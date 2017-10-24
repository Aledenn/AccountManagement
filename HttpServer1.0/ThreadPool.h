#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <iostream>
#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <future>
#include <semaphore.h>
using namespace std;

class CThreadPool
{
	using Task = std::function<void()>;
public:
	CThreadPool(unsigned int n)
	{
		n = n < 1 ? 1 : n;
		for (unsigned int i = 0; i < n; ++i)
		{
			m_tPool.emplace_back(&CThreadPool::RunTask, this);
			//m_tPool.push_back(thread(&CThreadPool::RunTask, this));
		}
	}
	~CThreadPool() 
	{
		StopAddTask();
		sem_destroy(&m_sSem);
		for (std::thread& thread : m_tPool) 
		{
			thread.join();
		}
	}

	template<typename Func, class... Args>
	auto AddTask(Func&& f, Args&&... args)->std::future<decltype(std::bind(std::forward<Func>(f), std::forward<Args>(args)...)())>
	{
		if (stop.load())
			throw std::runtime_error("�ѹر������ύ���");
		auto func = std::bind(std::forward<Func>(f), std::forward<Args>(args)...);
		auto task = std::make_shared<std::packaged_task<decltype(func())()>>(func);
		std::lock_guard<std::mutex> Lock(this->m_mLock);
		this->m_qTasks.emplace([task]() { (*task)(); });
		sem_post(&m_sSem);
		return task->get_future();
	}
	void RunTask()
	{
		while (true)
		{
		//	cout<<"�߳� "<< std::this_thread::get_id()<<endl;
			if (0 != sem_wait(&m_sSem))
				break;
			//printf("�߳� %d ����ִ��\n", std::this_thread::get_id());
			std::unique_lock<std::mutex> Lock(m_mLock);
			//printf("�߳� %d ִ����\n", std::this_thread::get_id());
			if (!m_qTasks.empty())
			{
				Task task{ std::move(m_qTasks.front()) };
				m_qTasks.pop();
				Lock.unlock();
				task();
			}
		}
	}
	void StopAddTask()
	{
		stop = true;
	}
	void StartAddTask()
	{
		stop = false;
	}
private:
	vector<thread> m_tPool;	//�̳߳�
	queue<Task> m_qTasks;	//�������
	mutex m_mLock;			//������
	sem_t m_sSem;			//�ź���
	atomic<bool> stop;		//�ر��ύ
};

#endif //_THREADPOOL_H_
