#include <iostream>

struct Data {
    const char* first_Name;
    const char* last_Name;
    const char* telephone;
    const char* email;
};

int main() {

    int birth_year = 2001;
    int current_year = 2021;
    std::cout << "dec\t" << std::dec << birth_year << std::endl;
    std::cout << "hex\t" << std::hex << birth_year << std::endl;
    std::cout << "oct\t" << std::oct << birth_year << std::endl;

    // std::base
    std::cout << std::dec << std::showbase << birth_year << std::endl;
    std::cout << std::hex << std::showbase << birth_year << std::endl;
    std::cout << std::oct << std::showbase << birth_year << std::endl;
    
    std::cout << "Age\t" << std::dec << current_year - birth_year << std::endl;

    int a,b,c,d;
    std::cin >> a >> std::oct >> b >> std::hex >> c >> d;
    std::cout << a << '\t'<< b << '\t'<< c << '\t'<< d << '\n' ;

    float f = 1234567.89;
    std::cout << std::defaultfloat  << f << std::endl;
    std::cout << std::fixed         << f << std::endl;
    std::cout << std::scientific    << f << std::endl;


    Data friends[1] = {
        {"Prim", "Krisztian", "01234567", "p@email.com"},
    };

    for(auto& f : friends) {
        std::cout << f.first_Name << '\t' << f.last_Name << '\t' << f.telephone << '\t'  << f.email << std::endl;
    }

    return 0;
}