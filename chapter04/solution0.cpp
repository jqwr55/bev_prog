#include <std_lib_facilities.h>
#undef vector

int main() {


    try {
        std::cout << "Success!\n";
        std::cout << "Success!\n";
        std::cout << "Success" << "!\n";
        std::cout << "success" << '\n';
        
        {
            int res = 7;
            std::vector<int> v(10);
            v[5] = res;
            std::cout << "Success!\n";
        }

        {
            vector<int> v(10);
            v[5] = 7;
            if (v[5] != 7)
                cout << "Success!\n";
        }

        if (true) 
            cout << "Success!\n";
        else
            std::cout << "Fail!\n";

        {
            bool c = false;
            if (c)
                cout << "Success!\n";
            else
                cout << "Fail!\n";
        }        

        {
            std::string s = "ape";
            bool c = "fool" > s;
            if (c)
                std::cout << "Success!\n";
        }

        {
            std::string s = "ape";
            if (s != "fool")
                std::cout << "Success!\n";
        }

        {
            std::string s = "ape";
            if (s != "fool")
                std::cout << "Success!\n";
        }

        {
            std::string s = "ape";
            if (s != "fool")
                std::cout << "Success!\n";
        }

        {
            vector<char> v(5);
            for (int i = 0; 0 < v.size(); ++i)
                std::cout << "Success!\n";
        }

        {
            vector<char> v(5);
            for(int i = 0; i <= v.size(); ++i) std::cout << "Success!\n";
        }

        {
            std::string s = "Success!\n";
            for (int i = 0; i < s.size(); ++i) std::cout << s[i];
        }

        if (true) 
            std::cout << "Success!\n";
        else 
            std::cout << "Fail!\n";
        {
            int x = 2000;
            int c = x;
            if (c == 2000)
                std::cout << "Success!\n";
        }

        {
            std::string s = "Success!\n";
            for (int i = 0; i < s.size(); ++i)
                std::cout << s[i];
        }

        {
            std::vector<char> v(5);
            for (int i = 0; i <= v.size(); ++i) cout << "Success!\n";
        }
        
        {
            int i = 0;
            int j = 9;
            while (i < 10) ++j;
            if (j < i)
                std::cout << "Success!\n";
        }

        {
            int x = 2;
            double d = 5 / (x - 2);
            if (d == 2 * x + 0.5)
                std::cout << "Success!\n";
        }

        {
            std::string s = "Success!\n";
            for (int i = 0; i <= 10; ++i) std::cout << s[i];
        }

        {
            int i = 0;
            while (i < 10) ++i;
            if (i<i) std::cout << "Success!\n";
        }

        {
            int x = 4;
            double d = 5/(x-2);
            if (d == 2 * x + 0.5) std::cout << "Success!\n";
        }

        std::cout << "Success!\n";

        keep_window_open();
    }
    catch(exception& e) {
        std::cerr << "error: " << e.what() << '\n';
        keep_window_open();
        return 1;
    }
    catch(...) {
        std::cerr << "Oops: unkown exception!\n";
        keep_window_open();
        return 2;
    }

    return 0;
}