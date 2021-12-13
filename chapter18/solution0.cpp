#include <iostream>
#include <vector>

int ga[10]{1, 2, 4, 8, 16, 32, 64, 128, 256, 512};
std::vector<int> gv{1, 2, 4, 8, 16, 32, 64, 128, 256, 512};


void array_cpy(int* dst, int* src, int n) {
    for(int i = 0; i < n; i++) {
        dst[i] = src[i];
    }
}
void print_array(std::ostream& os, int* arr, int n) {
    for(int i = 0; i < n; i++) {
        os << arr[i] << ' ';
    }
    os << std::endl;
}

void f(int* arr, int n) {

    int la[10];
    array_cpy(la, ga, 1);
    print_array(std::cout, la, 10);

    int* p = new int[n];
    array_cpy(p, arr, n);
    print_array(std::cout, la, n);
}


void print_vector(std::ostream& os, const std::vector<int>& a) {
    for(int i = 0; i < a.size(); i++) {
        os << a[i] << ' ';
    }
    os << std::endl;
}
void f(const std::vector<int>& vec) {

    std::vector<int> lv(10);
    lv = gv;
    print_vector(std::cout, lv);
    std::vector<int> lv2 = vec;
    print_vector(std::cout, lv2);
}

int factorial(int n) {
    if (n == 0) {
        return 1;
    }
    else {
        return(n * factorial(n-1));
    }
}

int main() {

    f(ga, 10);
    int aa[10];
    for(int i = 0; i < 10; i++) {
        aa[i] = factorial(i);
    }
    f(aa, 10);

    f(gv);
    std::vector<int> vv(10);
    for(int i = 0; i < 10; i++) {
        vv.push_back(factorial(i));
    }
    f(vv);

    return 0;
}