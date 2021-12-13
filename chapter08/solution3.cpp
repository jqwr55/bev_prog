#include <iostream>

enum Month {
    jan=1, feb, mar, apr, may, jun, jul, aug, sep, oct, nov, dec
};

namespace v1 {

    struct Date {
        int y;
        int m;
        int d;
    };
    void init_day(Date& dd, int y, int m, int d) {

        if(y <= 0 || m < 0 || m > 12 || d < 0 || d > 31) {
            throw std::runtime_error("bad date");
        }

        dd.y = y;
        dd.m = m;
        dd.d = d;
    }
    void add_day(Date& dd, int n) {
        dd.d += n % 30;
        int m = dd.m + (n / 30);
        dd.m = m % 12;
        dd.y += m / 12;
    }
    std::ostream& operator<<(std::ostream& os, const Date& d) {
        return os << '(' << d.y << ',' << d.m << ',' << d.d << ')';
    }
}

namespace v2 {

    class Date {
            int y, m, d;
    public:
            Date(int y_, int m_, int d_) : y(y_), m(m_), d(d_) {
                if(y <= 0 || m < 0 || m > 12 || d < 0 || d > 31) {
                    throw std::runtime_error("bad date");
                }
            }
            void add_day(int n) {
                d += n % 30;
                int mm = m + (n / 30);
                m = mm % 12;
                y += mm / 12;
            }
            int month() { return m; }
            int day() { return d; }
            int year() { return y; }
    };
    std::ostream& operator<<(std::ostream& os, Date& d) {
        return os << '(' << d.year() << ',' << d.month() << ',' << d.day() << ')';
    }
}
namespace v3 {

    class Year {
        static const int min = 1800;
        static const int max = 2200;
    public:
        class Invalid { };
        Year(int x) : y{x} { if (x<min || max<=x) throw Invalid{}; }
        int year() { return y; }
    private:
        int y;
    };

    class Date {
    public:
        Date(Year y_, Month m_, int d_) : y(y_), m(m_), d(d_) {
            if(y.year() <= 0 || (int)m < 0 || (int)m > 12 || d < 0 || d > 31) {
                throw std::runtime_error("bad date");
            }
        }
        void add_day(int n) {
            d += n % 30;
            int mm = m + (n / 30);
            m = Month(mm % 12);
            y = Year(y.year() + mm / 12);
        }
        Month month() { return m; }
        int day() { return d; }
        Year year() { return y; }
    private:
        Year y;
        Month m;
        int d;
    };
    std::ostream& operator<<(std::ostream& os, Date& d) {
        return os << '(' << d.year().year() << ',' << d.month() << ',' << d.day() << ')';
    }
};
namespace v4 {

    class Year {
        static const int min = 1800;
        static const int max = 2200;
    public:
        class Invalid { };
        Year(int x) : y{x} { if (x<min || max<=x) throw Invalid{}; }
        int year() { return y; }
    private:
        int y;
    };

    class Date {
    public:
        Date(Year y, Month m, int d);
        void add_day(int n) {
            d += n % 30;
            int mm = m + (n / 30);
            m = Month(mm % 12);
            y = Year(y.year() + mm / 12);
        }
        void add_month(int n) {
            int mm = m + n;
            m = Month(mm % 12);
            y = Year(y.year() + mm / 12);
        }
        void add_year(int n) {
            y = Year(y.year() + n);
        }
        Month month() const { return m; }
        int day()     const { return d; }
        Year year()   const { return y; }
    private:
        Year y;
        Month m;
        int d;
    };
    std::ostream& operator<<(std::ostream& os, Date& d) {
        return os << '(' << d.year().year() << ',' << d.month() << ',' << d.day() << ')';
    }
};

int main() {

    {
        v1::Date today;
        v1::init_day(today, 1978, jun, 25);
        v1::Date tomorow = today;
        v1::add_day(tomorow, 1);
        std::cout << today << ' ' << tomorow << std::endl;
    }
    {
        v2::Date today(1978, jun, 25);
        v2::Date tomorow = today;
        tomorow.add_day(1);
        std::cout << today << ' ' << tomorow << std::endl;
    }
    {
        v3::Date today(v3::Year(1978), jun, 25);
        v3::Date tomorow = today;
        tomorow.add_day(1);
        std::cout << today << ' ' << tomorow << std::endl;
    }

    return 0;
}