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
			std::cout << "from thread t:" << " msg:" << msg << std::endl;
		}
		msg = "from Fctor thread msg";
	}
};

int main(int argc, char** argv)
{
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
	getchar();
	return 0;
}