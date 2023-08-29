#include<cstdio>

void may_throw(){ // 可能抛出异常
    printf("may_throw\n");
    throw 123;
}
auto non_block_throw = []{
    printf("non_block_throw\n");
    may_throw();
};
void no_throw() noexcept{ // 不可能抛出异常
    printf("no_throw\n");
    return;
}
auto block_throw = []()noexcept{
    printf("block_throw\n");
    no_throw();
};
int main(int argc, char const *argv[]){
    
    try {
        may_throw();
        printf("no execute\n");
    } catch (...) {
        printf("捕获异常, 来自 may_throw()\n");
    }
    try {
        non_block_throw();
    } catch (...) {
        printf("捕获异常, 来自 non_block_throw()");
    }
    try {
        block_throw();
    } catch (...) {
        printf("捕获异常, 来自 block_throw()");
    }
    return 0;
}
