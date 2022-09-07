#include "task.h"
#include "uv.h"
#include "main_loop.h"
#include <iostream>
#include <string>
using namespace std;
class C
{
public:
    void dosome(int a)
    {
        int i = a;
        i += 4;

        cout << i << endl;
    }
    void dosome2(string s)
    {

        cout << s << endl;
    }
    char* dosome3(char* c) {

        return (char*)("ss");
    }

    string dosome4(string c) {
        return c;
    }
};

class D
{
public:
    int dosome(int a)
    {
        int i = a;
        i += 4;

        cout << i << endl;
        return 5;
    }
    int dosome2(string s)
    {
        cout << s << endl;
    }
    char* dosome3(char* s)
    {
        cout << s << endl;
        return (char*)"ss";
    }
    string dosome4(string c) {
        return c;
    }
};
int main() {
	D d;
	C c;
    //base::OnceCallback<string(string)> callback3 = base::BindOnce(&D::dosome4, &d);
    //delete &callback3; 

	//base::OnceCallback<string(string)> callback = base::BindOnce(&D::dosome4, &d);
	base::OnceCallback<string(string)> call = base::BindOnce(&C::dosome4, &c, string("sentcall"));

    base::OnceCallback<char*(char*)> callback = base::BindOnce(&D::dosome3, &d);
    //base::OnceCallback<char*(char*)> call = base::BindOnce(&C::dosome3, &c, (char*)("sentcall"));
    //callback.Run("sbb!!");
    //auto res = call.operate();
    //auto res = callback.Run("sb");
    //void* s = call.operate();
    //std::string ss1 = *static_cast<std::string*>(s);
	//task::RunAsync(callback, call);
    loop::MainLoop * loop = loop::MainLoop::Get();
    loop->PostTask(std::move(callback), std::move(call));
    loop->Run();
}