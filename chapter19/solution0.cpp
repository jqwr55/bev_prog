#include <iostream>
#include <vector>
#include <string>


template<typename T>
struct S {
        S(T v) : val(v) {}
        const T& ConstGet() const;
        T& Get();
        void Set(const T& other);

        void operator = (const T& other);
    private:
        T val;
};
template<typename T>
const T& S<T>::ConstGet() const {
    return val;
}
template<typename T>
T& S<T>::Get() {
    return val;
}
template<typename T>
void S<T>::Set(const T& other) {
    val = other;
}
template<typename T>
void S<T>::operator = (const T& other) {
    val = other;
}
std::ostream& operator << (std::ostream& os, const std::vector<int>& v) {
    for(int i = 0; i <  v.size(); i++) {
        os << v[i] << ' ';
    }
    os << std::endl;

    return os;
}
template<typename T>
std::istream& operator >> (std::istream& is, S<T>& other) {
    T o;
    is >> o;
    other.Set(o);
    return is;
}

template<typename T> 
void read_val(T& v) {
    std::cin >> v;
}

int main() {

    S<int> s0(0);
    S<char> s1(0);
    S<double> s2(0);
    S<std::string> s3("Hello");
    S<std::vector<int>> s4(std::vector<int>{1,2,3});
    
    std::cout << s0.Get() << std::endl;
    std::cout << s1.Get() << std::endl;
    std::cout << s2.Get() << std::endl;
    std::cout << s3.Get() << std::endl;
    std::cout << s4.Get() << std::endl;

    read_val(s0);
    read_val(s1);
    read_val(s2);
    read_val(s3);
}