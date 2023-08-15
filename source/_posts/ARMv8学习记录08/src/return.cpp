#include <cstdio>
#include <cstring>

class Point{
public:
    static int count;
    double cx;
    double cy;
};

Point createPoint(double x, double y){
    Point point;
    point.cx = x;
    point.cy = y;
    return point;
}

class Person{
public:
    int age;
    int height;
    char name[32];    
};

Person createPerson(int age, int height, const char *name){
    Person person;
    person.age =age;
    person.height = height;
    strcpy(person.name, name);
    return person;
}

int main(int argc, char const *argv[])
{
    Point point = createPoint(1.1, 2.2);
    printf("cx = %f, cy = %f\n", point.cx, point.cy);
    Person person = createPerson(18, 180, "zhangsan");
    printf("age=%d, height=%d, name=%s\n", person.age, person.height, person.name);
    
    return 0;
}
