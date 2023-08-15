#include <cstdio>
#include <cstring>

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
};

void show(Point point){
    printf("cx=%lf, cy=%lf,\n", point.cx, point.cy);
}

class Person{
public:
    int age;
    int height;
    char name[32];    
};

void show(Person person){
    printf("age=%d, height=%d, name=%s\n", person.age, person.height, person.name); 
}

int main(int argc, char const *argv[])
{
    Point point(1.1, 2.2);
    show(point);

    Person person;
    person.age =18;
    person.height = 180;
    strcpy(person.name, "zhangsan");
    show(person);
    return 0;
}
