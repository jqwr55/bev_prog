#include <iostream>
#include <fstream>
#include <vector>

struct Point {
    int x;
    int y;
};
std::ostream& operator<<(std::ostream& os, Point p) {
    return os << '(' << p.x << ", " << p.y << ')';
}
bool operator!=(Point p0, Point p1) {
    return p0.x != p1.x || p0.y != p1.y;
}

int main() {

    std::cout << "Please enter 7 points x,y each" << std::endl;
    std::vector<Point> original_points;
    for(int i = 0; i < 7; i++) {
        Point p;
        std::cin >> p.x;
        std::cin >> p.y;
        original_points.push_back(p);
    }
    for(int i = 0; i < original_points.size(); i++) {
        std::cout << original_points[i] << std::endl;
    }

    std::ofstream file("mydata.txt");
    for(int i = 0; i < original_points.size(); i++) {
        file << original_points[i] << std::endl;
    }
    file.close();

    std::ifstream readFile("mydata.txt");
    std::vector<Point> processed_points;

    while(readFile) {
        Point p;
        readFile >> p.x;
        readFile >> p.y;
        processed_points.push_back(p);
    }
    readFile.close();

    if(processed_points.size() != original_points.size()) {
        std::cout << "Something went wrong" << std::endl;
    }
    else {
        for(int i = 0; i < processed_points.size(); i++) {
            if(processed_points[i] != original_points[i]) {
                std::cout << "Something went wrong" << std::endl;
                break;
            }
        }
    }

    return 0;
}