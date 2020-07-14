#include <mutex>
#include <fstream>
#include <future>
#include <deque>
#include <condition_variable>
#include <thread>
#include <chrono>
using namespace std;
int factorial6(int N)
{
	int nRet = 1;
	for (int i = N; i > 1; i--)
		nRet *= i;
	std::cout << "Result is:" << nRet << std::endl;
	return nRet;
}
std::deque<std::packaged_task<int()>> task_q6;
std::condition_variable cond6;
std::timed_mutex test_mutex;
void f()
{
	auto now = std::chrono::steady_clock::now();
	test_mutex.try_lock_until(now + std::chrono::seconds(10));
	std::cout << "hello world\n";
}
int main(int argc, char** argv)
{
	std::lock_guard<std::timed_mutex> l(test_mutex);
	std::thread t(f);
	t.join();	
	std::thread t1(factorial6, 6);
	//让线程休眠3毫秒
	std::this_thread::sleep_for(chrono::milliseconds(3));
	//也可以是时间点来创建
	chrono::steady_clock::time_point tp = chrono::steady_clock::now() + chrono::milliseconds(4);
	std::this_thread::sleep_until(tp);
	//让锁休眠3毫秒 时间限制函数
	std::mutex mtx66;
	std::unique_lock<std::mutex> locker(mtx66);
	//条件变量时间限制
	std::condition_variable cond666;
	cond666.wait_for(locker, chrono::milliseconds(3));
	cond666.wait_until(locker, tp);
	//可以将异步操作队列化,按照期望的顺序执行
	std::promise<int> p;
	std::future<int> f = p.get_future();
	f.wait_for(chrono::milliseconds(3));
	f.wait_until(tp);
	getchar();
	return 0;
}