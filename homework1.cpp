#include <iostream>
int num1=3;
int num2=10;
int num3=5;
void block();
void documentspass() {
    num2--;
    if(num2>0) {
        checkpass();
    }
    else block();
}
void checkpass() {
    num3--;
    if(num3>0) {

    }
    else block();
}
void passenger() {
    num1--;
    if(num1>0) {
        documentspass();
    }
    else block();
}

int main() {
    passenger();
}