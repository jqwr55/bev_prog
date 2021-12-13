#include <std_lib_facilities.h>

int main() {

    while(true) {

        double n0;
        double n1;
        // read the numbers in
        std::cin >> n0;
        std::cin >> n1;

        if(std::cin.fail()) {
            std::cin.clear();

            char t;
            std::cin >> t;
            // reached terminator
            if(t == '|') {
                // exit loop;
                break;
            }
        }

        if(n0 < n1) {
            std::cout << "the smaller value is: " << n0 << std::endl;
            std::cout << "the larger value is: " << n1 << std::endl;
        }
            if( n1 - n0 < 1.0/100 ) {
                std::cout << "the numbers are almos equal" << std::endl;
            }
        if(n0 > n1) {
            std::cout << "the smaller value is: " << n1 << std::endl;
            std::cout << "the larger value is: " << n0 << std::endl;

            if( n0 - n1 < 1.0/100 ) {
                std::cout << "the numbers are almos equal" << std::endl;
            }
        }
        else {
            // implicit n0 == n1
            std::cout << "the numbers are equal" << std::endl;
        }

    }

    return 0;
}