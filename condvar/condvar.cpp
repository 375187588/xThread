#include <functional>
#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
#include <string>
#include <deque>
#include <chrono>
#include <condition_variable>
using namespace std;
std::deque<int> q;
std::mutex mtx;
std::condition_variable cond;
void fun1()
{
	int count = 10;
	while (count > 0)
	{
		std::unique_lock<std::mutex> locker(mtx);
		q.push_back(count);
		locker.unlock();
		//通知一次，让消费者去消费
		//cond.notify_one();
		cond.notify_all();//通知所有消费者消息
		std::this_thread::sleep_for(chrono::seconds(1));
		count--;
	}
}
void fun2()
{
	int data = 0;
	//一直循环是非常低效的，因此用条件变量来解决这个问题
	while (data != 1)
	{
		std::unique_lock<std::mutex> locker(mtx);
		//if (!q.empty())
		//{
			//没有通知就等
			//如果q为空才等待
			cond.wait(locker, []() {return !q.empty(); });
			data = q.back();
			q.pop_back();
			locker.unlock();
			std::cout << "t2 got a value from t1:" << data << std::endl;
		//}
		/*else
		{
			locker.unlock();
		}*/
	}
}

int main(int argc, char** argv)
{
	std::thread t1(fun1);
	std::thread t2(fun2);	
	t1.join();
	t2.join();
	getchar();
	return 0;
}