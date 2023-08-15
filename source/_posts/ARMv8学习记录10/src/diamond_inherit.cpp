#include <cstdio>

class CFurniture
{
public:
    CFurniture(){
        m_nPrice = 0;
    }
    virtual ~CFurniture(){ // 家具类的虚析构函数
        printf("virtual ~CFurniture()\r\n");
    }
    virtual int GetPrice(){ // 获取家具价格
        return m_nPrice;
    };

protected:
    int m_nPrice; // 家具类的成员变量
};

class CSofa : virtual public CFurniture
{
public:
    CSofa(){
        m_nPrice = 1;
        m_nColor = 2;
    }
    virtual ~CSofa(){ // 沙发类虚析构函数
        printf("virtual ~CSofa()\r\n");
    }
    virtual int GetColor(){ // 获取沙发颜色
        return m_nColor;
    }
    virtual int SitDown(){ // 沙发可以坐下休息
        return printf("Sit down and rest your legs\r\n");
    }
protected:
    int m_nColor; // 沙发类成员变量
};

class CBed : virtual public CFurniture
{
public:
    CBed(){
        m_nPrice = 3;
        m_nLength = 4;
        m_nWidth = 5;
    }
    virtual ~CBed(){ // 床类的虚析构函数
        printf("virtual ~CBed()\r\n");
    }
    virtual int GetArea(){ // 获取床面积
        return m_nLength * m_nWidth;
    }
    virtual int Sleep(){ // 床可以用来睡觉
        return printf("go to sleep\r\n");
    }

protected:
    int m_nLength; // 床类成员变量
    int m_nWidth;
};

class CSofaBed : public CSofa, public CBed
{
public:
    CSofaBed(){
        m_nHeight = 6;
    }
    virtual ~CSofaBed(){ // 沙发床类的虚析构函数
        printf("virtual ~CSofaBed()\r\n");
    }
    virtual int SitDown(){ // 沙发可以坐下休息
        return printf("Sit down on the sofabed\r\n");
    }
    virtual int Sleep(){ // 床可以用来睡觉
        return printf("go to sleep on the sofabed\r\n");
    }
    virtual int GetHeight()
    {
return m_nHeight;
    }

protected:
    int m_nHeight; // 沙发类的成员变量
};
int main(int argc, char *argv[])
{
    CSofaBed SofaBed;
    return 0;
}