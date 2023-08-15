#include <cstdio>

class CVirtualBase{
public:
    virtual void Show() = 0; // 定义纯虚函数
};
class CVirtualChild : public CVirtualBase { // 定义继承虚基类的子类
public:
    virtual void Show(){ // 实现纯虚函数
        printf("虚基类分析\r\n");
    }
};
int main(int argc, char *argv[])
{
    CVirtualChild VirtualChild;
    VirtualChild.Show();
    return 0;
}