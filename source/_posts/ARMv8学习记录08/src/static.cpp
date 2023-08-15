#include <cstdio>

class Point{
public:
    static int count;
    double cx;
    double cy;
public:
    Point(double cx, double cy){
        this->cx = cx;
        this->cy = cy;
    }
    void show(){
        printf("cx=%lf, cy=%lf, count=%d\n", this->cx, this->cy, count);
    }
};

int Point::count = 1;

int main(int argc, char const *argv[])
{
    Point point(1.1, 2.2);
    point.show();
    return 0;
}
