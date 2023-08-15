#include <cstdio>
#include <cstring>

class Person{
public:
    int age;
    char* name;
public:
    Person(int age, const char* name){
        this->age = age;
        this->name = new char[strlen(name) + 1];
        strcpy(this->name, name);
    }
    ~Person(){
        this->age = 0;
        delete[] this->name;
        this->name = nullptr;
    }
};

void fun(){
    static Person p(38, "lisi");
    printf("age=%d, name=%s\n", p.age, p.name);
}

Person person1(18, "zhangsan");
static Person person2(28, "wangwu");


int main(int argc, char const *argv[])
{   
    printf("person1.age=%d, person1.name=%s\n", person1.age, person1.name);
    printf("person2.age=%d, person2.name=%s\n", person2.age, person2.name);
    fun();
    return 0;
}
