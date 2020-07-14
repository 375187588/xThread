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
int main(int argc, char** argv)
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