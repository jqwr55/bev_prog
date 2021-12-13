#include <iostream>
#include <vector>

void print_array10(std::ostream& os, int* a) {
    for(int i = 0; i < 10; i++) {
        os << a[i] << ' ';
    }
    os << std::endl;
}
void print_array(std::ostream& os, int* a, int n) {
    for(int i = 0; i < n; i++) {
        os << a[i] << ' ';
    }
    os << std::endl;
}
void set_vector(std::vector<int>& vec, int offset, int n) {
    vec.reserve(n);
    for(int i = 0; i < n; i++) {
        vec.push_back(offset + n);
    }
}
void print_vector(std::ostream& os, const std::vector<int>& a) {
    for(int i = 0; i < a.size(); i++) {
        os << a[i] << ' ';
    }
    os << std::endl;
}

int main() {

    {
        int* arr = new int[10];
        for(int i = 0; i < 10; i++) {
            std::cout << arr[i] << ' ';
        }
        std::cout << std::endl;

        delete[] arr;
    }
    {
        int* arr = new int[10]{100,101,102,103,104,105,106,107,108,109};
        print_array(std::cout, arr, 10);
        delete[] arr;
    }
    {
        int* arr = new int[11]{100,101,102,103,104,105,106,107,108,109,110};
        print_array(std::cout, arr, 11);
        delete[] arr;
    }
    {
        int* arr = new int[20];
        for(int i = 0; i < 20; i++) {
            arr[i] = 100 + i;
        }
        print_array(std::cout, arr, 20);
        
        delete[] arr;
    }
    {
        std::vector<int> v;
        set_vector(v, 100, 10);
        print_vector(std::cout, v);
    }
    {
        std::vector<int> v;
        set_vector(v, 100, 11);
        print_vector(std::cout, v);
    }
    {
        std::vector<int> v;
        set_vector(v, 100, 20);
        print_vector(std::cout, v);
    }
    {
        int* p1 = new int(7);
        std::cout << p1 << ' ' << *p1 << std::endl;

        int* p2 = new int[7]{1,2,4,8,16,32,64};
        print_array(std::cout, p2, 7);

        int* p3 = p2;
        p2 = p1;
        p2 = p3;

        std::cout << p1 << ' ' << *p1 << ' ' << p2 << ' ' << *p2 << std::endl;

        delete[] p1;
        delete[] p2;
    }
    {
        int* p1 = new int[10]{1,2,4,8,16,32,64,128,256,512};
        int* p2 = new int[10];
        for(int i = 0; i < 10; i++) {
            p2[i] = p1[i];
        }
        delete[] p1;
        delete[] p2;
    }
    {
        std::vector<int> p1{1,2,4,8,16,32,64,128,256,512};
        std::vector<int> p2;
        p2 = p1;
    }

    return 0;
}