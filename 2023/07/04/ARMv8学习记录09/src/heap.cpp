#include <cstdio>

class Point{
public:
    int cx;
    int cy;
public:
    Point(int cx, int cy){
        this->cx = cx;
        this->cy = cy;
    }
    ~Point(){
        this->cx = 0;
        this->cy = 0;
    }
};

int main(int argc, char const *argv[])
{
    Point *p = new Point(123, 456);
    printf("cx = %d, cy = %d\n", p->cx, p->cy);
    // 需要手动释放
    delete p;
    return 0;
}
