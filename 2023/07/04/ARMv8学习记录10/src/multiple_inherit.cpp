#include <cstdio>

class CSofa{
public:
    CSofa(){
        m_nColor = 2;
    }
    virtual ~CSofa(){ // 沙发类虚析构函数
        printf("virtua ~CSofa()\n");
    }
    virtual int GetColor(){ // 获取沙发颜色
        return m_nColor;
    }
    virtual int SitDown(){ // 沙发可以坐下休息
        return printf("Sit down and rest yourlegs\n");
    }

protected:
    int m_nColor; // 沙发类成员变量
};

// 定义床类
class CBed{
public:
    CBed(){
        m_nLength = 4;
        m_nWidth = 5;
    }
    virtual ~CBed(){ // 床类虚析构函数
        printf("virtual~CBed()\r\n");
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
// 子类沙发床定义，派生自CSofa类和CBed类
class CSofaBed : public CSofa, public CBed
{
public:
    CSofaBed(){
        m_nHeight = 6;
    }
    virtual ~CSofaBed(){ // 沙发床类的虚析构函数
        printf("virtual~CSofaBed()\r\n");
    }
    virtual int SitDown(){ // 沙发可以坐下休息
        return printf("Sit down on the sofabed\r\n");
    }
    virtual int Sleep(){ // 床可以用来睡觉
        return printf("go to sleep on the sofabed\r\n");
    }
    virtual int GetHeight(){
        return m_nHeight;
    }

protected:
    int m_nHeight; // 沙发类的成员变量
};

int main(int argc, char const *argv[])
{
    CSofaBed SofaBed;
    CSofa* pSofa=&SofaBed;
    pSofa->SitDown();
    CBed* pBed=&SofaBed;
    pBed->Sleep();

    pSofa = new CSofaBed;
    pSofa->SitDown();
    pBed = dynamic_cast<CBed*>(pSofa);
    pBed->Sleep();

    delete pSofa;

    return 0;
}
