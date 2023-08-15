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
    // 拷贝构造函
    Person(const Person& person){
        this->age = person.age;
        this->name = new char[strlen(person.name) + 1];
        strcpy(this->name, person.name);
    }
    Person& operator= (const Person& person){
        this->age = person.age;
        this->name = new char[strlen(person.name) + 1];
        strcpy(this->name, person.name);
        return *this;
    }

    ~Person(){
        this->age = 0;
        delete[] this->name;
        this->name = nullptr;
    }
};

void show(const Person& person){
    printf("age=%d, name=%s\n", person.age, person.name);
}

Person create(int age, const char* name){
    return Person(age, name);
}

int main(int argc, char const *argv[])
{
    Person person = create(18, "zhangsan");
    printf("age=%d, name=%s\n", person.age, person.name);
    show(person);
    return 0;
}
