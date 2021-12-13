#include <iostream>

void swap_v(int a, int b) {
    int tmp = a;
    a = b;
    b = tmp;
}
void swap_r(int& a, int& b) {
    int tmp = a;
    a = b;
    b = tmp;
}
/*
void swap_cr(const int& a, const int& b) {
    int tmp = a;
    a = b;
    b = tmp;
}
*/

int main() {

    int x = 7;
    int y = 9;
    swap_r(x,y);
    swap_v(7,9);
    /*
    const int cx = 7;
    const int cy = 9;
    swap_cr(cx,cy);
    swap_v(7.7,9.9);
    double dx = 7.7;
    double dy = 9.9;
    swap_r(dx,dy);
    */

    return 0;
}