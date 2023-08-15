#include <stdio.h>

class Point
{
public:
    int cx;
    int cy;
public:
    Point(int x):cx(x), cy(0){};
    ~Point(){
        cx = 0;
        cy = 0;
    };

    virtual void setx(int cx){
        this->cx = cx;
    }
    virtual void sety(int cy){
        this->cy = cy;
    }
};

int main(int argc, char const *argv[])
{
    Point point(12);
    point.sety(34);
    printf("cx = %d, cy = %d\n", point.cx, point.cy);
    return 0;
}
