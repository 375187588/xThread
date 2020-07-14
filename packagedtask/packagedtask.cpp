#include <mutex>
#include <fstream>
#include <future>
#include <deque>
#include <condition_variable>
#include <thread>
using namespace std;
int ffactorial(int N)
{
	int res = 1;
	for (int i = N; i > 1; i--)
	{
		res *= i;
	}
	std::cout << "Result is:" << res << std::endl;
	return res;
}
std::deque<std::packaged_task<int()>> task_q;
std::mutex mtx1;
std::condition_variable cond2;
void thread_1()
{
	std::packaged_task<int()> t;
	{
		//解决数据竟争
		//std::lock_guard<std::mutex> locker(mtx1);
		//使用了条件变量必须用 unique_lock
		std::unique_lock<std::mutex> locker(mtx1);
		cond2.wait(locker, [] {return !task_q.empty(); });
		t = std::move(task_q.front());
	}	
	t();
}
int main(int argc, char** argv)
{
	//std::thread t1(ffactorial, 6);
	//连接一个可调用的对象来关联一个future来异步获取结果
	//std::packaged_task<int(int)> t(ffactorial);	
	//std::packaged_task<int()> t(std::bind(ffactorial,6));
	//获得与packaged_task 共享状态相关联的 future对象
	//std::future<int> ret = t.get_future();
	//等待任务完成并获取结果
	//int value = ret.get();
	std::thread t1(thread_1);
	std::packaged_task<int()> t(std::bind(ffactorial, 6));
	//获得与packaged_task 共享状态相关联的 future对象
	std::future<int> ret = t.get_future();
	{
		//解决数据竟争
		std::lock_guard<std::mutex> locker(mtx1);
		task_q.push_back(std::move(t));
	}	
	cond2.notify_one();
	int value = ret.get();
	std::cout << "异步获得的结果为：" << value << std::endl;
	t1.join();
	getchar();
	return 0;

}