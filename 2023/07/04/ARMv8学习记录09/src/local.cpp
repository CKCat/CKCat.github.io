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
    Point point(123, 456);
    printf("cx = %d, cy = %d\n", point.cx, point.cy);
    return 0;
}
