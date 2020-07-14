#include <string>
#include <iostream>
#include <thread>
using namespace std;

void thread_fun()
{
	std::cout << "run thread" << std::endl;
	for (int i = 0; i < 100; i++)
	{
		std::cout << "from child thread:" << i << std::endl;
	}
}
class Fctor
{
public:
	void operator()(std::string msg)
	{
		for(int i=0;i<100;i++)
		{

			std::cout << "from thread t:" << "  msg:" << msg << std::endl;

		}
		msg = "from Fctor thread msg";
	}
};
/////////////////////////////////////////////////////////////////////////////////////
int main1(int argc, char** argv)
{
	std::cout << "1----------------------------线程管理-----------------------------------" << std::endl;
	std::cout << "Main thread id:" << std::this_thread::get_id() << std::endl;
	//std::thread t(thread_fun);
	//Fctor f;
	std::string msg = "thread parame";
	//1 std::thread t1((Fctor()),std::ref(msg));
	//将msg移动到子线程
	std::thread t1((Fctor()), std::move(msg));
	//t1 为空不存在
	std::thread t2 = std::move(t1);
	std::cout << "child thread t2 id:" << t2.get_id() << std::endl;
	t2.join();
	//启动多少个线程进行并发编程
	std::cout << "开启线程并发数:" << std::thread::hardware_concurrency() << std::endl;	
	std::cout << "2----------------------------数据竞争与互斥对象-----------------------------------" << std::endl;

	getchar();
	return 0;
}

#include <mutex>
std::mutex mu;
void shared_print(std::string msg, int id)
{
	std::lock_guard<std::mutex> guard(mu);
	std::cout << msg << ":" << id << std::endl;
}
void function_2()
{
	int i = 0;
	while (i < 100)
	{
		shared_print("Form t1:", i);
		i++;
	}
}
//////////////////////////////////
#include <fstream>
//这样可以保证线程绝对安全
class LogFile
{
public:
	LogFile()
	{
		//f.open("log.txt");
	}
	//产生死锁
	void shared_print1(std::string id, int value)
	{
		//第一种产生死锁和解决办法
		/*
		std::lock_guard<std::mutex> locker1(m_mutext1);//产生死锁 1,2 2,1 解决办法就是互斥顺序一至 1，2 1，2
		std::lock_guard<std::mutex> locker2(m_mutext2);//产生死锁
		std::cout << "From :" << id << ":" << value << std::endl;
		*/

		//解决死锁 第二种办法
		std::lock(m_mutext1, m_mutext2);
		std::lock_guard<std::mutex> locker1(m_mutext1, std::adopt_lock);//产生死锁 1,2 2,1 解决办法就是互斥顺序一至 1，2 1，2
		std::lock_guard<std::mutex> locker2(m_mutext2, std::adopt_lock);//产生死锁
		std::cout << "From :" << id << ":" << value << std::endl;
	}
	//死锁现象
	void shared_print2(std::string id, int value)
	{
		/*
		//std::lock_guard<std::mutex> locker2(m_mutext2);//产生死锁
		//std::lock_guard<std::mutex> locker1(m_mutext1);//产生死锁

		//解决死锁 第一种办法
		//std::lock_guard<std::mutex> locker2(m_mutext1);//解决死锁
		//std::lock_guard<std::mutex> locker1(m_mutext2);//解决死锁
		//std::cout << "From :" << id << ":" << value << std::endl;
		*/

		//解决死锁 第二种办法
		std::lock(m_mutext1, m_mutext2);
		//还原产生死锁
		std::lock_guard<std::mutex> locker2(m_mutext2, std::adopt_lock);//产生死锁
		std::lock_guard<std::mutex> locker1(m_mutext1, std::adopt_lock);//产生死锁	

	}
	void shared_print(std::string id, int value)
	{
		/*
		{
			//可以安全，但是可以用std::once_flag来换
			std::unique_lock<std::mutex> locker(m_mopen,std::defer_lock);
			if (!f.is_open())
			{
				//线程不安全
				//std::lock_guard<std::mutex> locker(m_mopen);
				f.open("log.txt");
			}
		}
		*/
		std::call_once(m_flag, [&]() {f.open("log.txt"); });
		std::lock_guard<std::mutex> locker(m_mutext1);
		f << "Form :" << id << ":" << value << std::endl;
		//unique_lock
		/*
		std::unique_lock<std::mutex> locker(m_mutext1, std::defer_lock);
		locker.lock();
		f << "Form :" << id << ":" << value << std::endl;
		locker.unlock();
		//...
		locker.lock();
		
		//C++11的移动语义
		std::unique_lock<std::mutex> locker2 = std::move(locker);
		*/
	}
	//不能将这个f传出，传出后就不在受保护
	//std::ofstream& GetStream() { return f; }
	//也不能这样实现让外为所欲为
	//void processf(void fun(ofstream&)){fun(f);}
private:
	std::mutex m_mutext1;
	std::mutex m_mutext2;//死锁
	std::ofstream f;
	std::mutex m_mopen;
	std::once_flag m_flag;
};
void function_3(LogFile& log)
{
	int i = 0;
	while (i < 100)
	{
		log.shared_print1("Form t1:", i);
		i++;
	}
}
int main2(int argc, char** argv)
{
	//std::thread t1(function_2);
	LogFile log;
	std::thread t1(function_3, std::ref(log));
	int i = 0;
	while (i < 100)
	{
		log.shared_print2(std::string("From main:"), i);
		i++;
	}
	t1.join();
	getchar();
	return 0;
}

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
int main3(int argc, char** argv)
{
	std::thread t1(fun1);
	std::thread t2(fun2);	
	t1.join();
	t2.join();
	getchar();
	return 0;
}



#include <functional>
#include <iostream>
#include <fstream>
#include <mutex>
#include <thread>
#include <string>
#include <deque>
#include <chrono>
#include <future>
#include <condition_variable>
using namespace std;

std::mutex mtxx;
std::condition_variable condd;

void factorial14(int N, int& x)
{
	int res = 1;
	for (int i = N; i > 1; i--)
	{
		res *= i;
	}
	std::cout << "Result is:" << res << std::endl;
	x = res;
}

int factorial13(int N)
{
	int res = 1;
	for (int i = N; i > 1; i--)
	{
		res *= i;
	}
	std::cout << "Result is:" << res << std::endl;
	return res;
}

//第二种多std::future<int> 
int factorial12(std::future<int>& f)
{
	int res = 1;
	int N = f.get();//会有一个异常
	for (int i = N; i > 1; i--)
	{
		res *= i;
	}
	std::cout << "Result is:" << res << std::endl;
	return res;
}

int factorial(std::shared_future<int> f)
{
	int res = 1;
	int N = f.get();//会有一个异常
	for (int i = N; i > 1; i--)
	{
		res *= i;
	}
	std::cout << "Result is:" << res << std::endl;
	return res;
}

int main4(int argc, char** argv)
{
	int x;
	//子线程获取主线程变量
	std::promise<int> p;
	//std::promise<int> p2 = std::move(p);
	std::future<int> fu = p.get_future();

	//std::thread t1(factorial,4,std::ref(x));
	//t1.join();
	//std::future<int> f = std::async(std::launch::async, factorial,4);
	//只能move不能copy
	
	/*
	//第二种多std::future<int>
	std::future<int> f = std::async(std::launch::async, factorial, std::ref(fu));
	std::future<int> f1 = std::async(std::launch::async, factorial, std::ref(fu));
	std::future<int> f2 = std::async(std::launch::async, factorial, std::ref(fu));
	std::future<int> f3 = std::async(std::launch::async, factorial, std::ref(fu));
	*/
	std::shared_future<int> sf = fu.share();
	std::future<int> f = std::async(std::launch::async, factorial, sf);
	std::future<int> f1 = std::async(std::launch::async, factorial, sf);
	std::future<int> f2 = std::async(std::launch::async, factorial, sf);
	std::future<int> f3 = std::async(std::launch::async, factorial, sf);

	p.set_value(4);
	//主线程获取子线程变量
	x = f.get();
	std::cout << "f from child value:" << x << std::endl;

	x = f1.get();
	std::cout << "f1 from child value:" << x << std::endl;
	getchar();
	return 0;
}


#include <mutex>
#include <fstream>
#include <future>

using namespace std;

class A
{
public:
	void f(int x, char c) {}
	int operator()(int N) { return 0; }
};
void foo(int x) {}

int main5(int argc, char** argv)
{
	A a;
	std::thread t1(a, 6);//传递a的拷贝给子线程
	std::thread t2(std::ref(a), 6);//传递a的引用给子线程
	std::thread t3(std::move(a), 6);//a在主线程中将不在有效
	std::thread t4(A(), 6);//传递临时创建的a对象给子线程
	std::thread t5(foo, 6);
	std::thread t6([](int x) {return x * x; },6);
	std::thread t7(&A::f, a, 8, 'w');//传递a的拷贝成员函数给子线程
	std::thread t8(&A::f, &a, 8, 'w');//传递a的地址的成员函数给子线程
	std::async(std::launch::async, a, 6); 
	getchar();
	return 0;
}

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
int main6(int argc, char** argv)
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