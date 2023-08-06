

class Base
{
protected:
    int n;

public:
    int foo(int p)
    {
        return n + p;
    }

    virtual int bar(int p){
        return n + p;
    }
};

struct Point
{
    double cx, cy;
};

class Derived : public Base
{
public:
    int foo(int p)
    {
        return n + b + p;
    }

protected:
    int a, b;
    Point a_point;
    char c;
};

int main(int argc, char **argv)
{
    return sizeof(Derived);
}