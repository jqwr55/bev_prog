#include <std_lib_facilities.h>

int main() {

    // task 1.
    std::cout << "Enter the name of the person you want to write to.\n";
    std::string first_name;
    std::cin >> first_name;
    std::cout << "\tDear " << first_name << ",.\n";

    // task 2.
    std::cout << "How are you? I am fine. I miss you\n";

    // task 3.
    std::cout << "Enter the name of another friend.\n";
    std::string friend_name;
    std::cin >> friend_name;
    std::cout << "Have you seen " << friend_name << " lately.\n";

    // task 4.
    char friend_gender{};
    std::cout << "Enter 'f' if your friend is female 'm' otherwise.\n";
    std::cin >> friend_gender;
    if(friend_gender == 'f') {
        std::cout << "If you see " << friend_name << " please ask her to call me.\n";
    }
    else if(friend_gender == 'm') {
        std::cout << "If you see " << friend_name << " please ask him to call me.\n";
    }

    // task 5.
    std::cout << "Enter the age of the recipeient.\n";
    int recipient_age;
    std::cin >> recipient_age;
    std::cout << "I hear you just had a birthday and you are " << recipient_age << " years old.\n";
    if(recipient_age <= 0 || recipient_age >= 100) {
        simple_error("you're kidding!\n");
    }

    // task 6.
    if(recipient_age < 12) {
        std::cout << "Next year you will be " << recipient_age + 1 << ".\n";
    }
    else if(recipient_age == 17) {
        std::cout << "Next year you will be able to vote.\n";
    }
    else if(recipient_age > 70) {
        std::cout << "I hope you are enjoying retirement.\n";
    }

    // task 7.    
    std::cout << "Yours sincerely, \n\n";

    return 0;
}