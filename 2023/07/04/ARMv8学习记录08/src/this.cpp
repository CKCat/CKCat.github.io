#include <cstdio>

class Point{
public:
    double cx;
    double cy;
public:
    Point(double cx, double cy){
        this->cx = cx;
        this->cy = cy;
    }

    void setx(double cx){
        this->cx = cx;
    }

    void sety(double cy){
        this->cy = cy;
    }
};

int main(int argc, char const *argv[])
{
    Point point(1.1, 2.2);
    printf("cx = %f, cy = %f\n", point.cx, point.cy);
    point.setx(3.14);
    point.sety(6.18);
    printf("cx = %f, cy = %f\n", point.cx, point.cy);
    return 0;
}
