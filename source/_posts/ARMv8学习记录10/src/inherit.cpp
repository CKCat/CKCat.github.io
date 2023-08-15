#include <cstdio>

class CAnimal
{
public:
   int weight;
   int age;
public:
    CAnimal(){
        weight = 0;
        age = 0;
    }
    virtual ~CAnimal(){
        weight = 0;
        age = 0;
    }
    virtual void eat(){
        printf("Animal Eat food.\n");
    }
    virtual void sleep(){
        printf("Animal Sleep.\n");
    }
    virtual void setweight(int weight){
        this->weight = weight;
    }
    virtual void setage(int age){
        this->age = age;
    }
    void show(){
        printf("weight=%d, age=%d\n", weight, age);
    }
};

class CDog: public CAnimal
{
public:
    int nType;
public:
    CDog(){
        nType = 0;
    }
    ~CDog(){
        nType = 0;
    }
    void setType(int nType){
        this->nType = nType;
    }
    virtual void eat(){
        printf("Dog Eat food.\n");
    }
};

class CCat: public CAnimal
{
public:
    int nType;
public:
    CCat(){
        nType = 1;
    }
    ~CCat(){
        nType = 1;
    }
    void setType(int nType){
        this->nType = nType;
    }   
    virtual void sleep(){
        printf("Cat Sleep.\n");
    }
};

int main(int argc, char* argv[])
{
    CDog dog;
    CAnimal* pdog = &dog;
    pdog->eat();
	CAnimal* pcat = new CCat;
    pcat->sleep();
    delete pcat;
	return 0;
}

