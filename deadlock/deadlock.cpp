//////////////////////////////////
#include <fstream>
#include <mutex>
#include <string>
#include <iostream>
#include <thread>
using namespace std;
//这样可以保证线程绝对安全
class LogFile
{
public:
	LogFile()
	{
		f.open("log.txt");
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
		std::lock_guard<std::mutex> locker2(m_mutext2,std::adopt_lock);//产生死锁
		std::lock_guard<std::mutex> locker1(m_mutext1,std::adopt_lock);//产生死锁	
	}
	//不能将这个f传出，传出后就不在受保护
	//std::ofstream& GetStream() { return f; }
	//也不能这样实现让外为所欲为
	//void processf(void fun(ofstream&)){fun(f);}
private:
	std::mutex m_mutext1;
	std::mutex m_mutext2;//死锁
	std::ofstream f;
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
int main(int argc, char** argv)
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