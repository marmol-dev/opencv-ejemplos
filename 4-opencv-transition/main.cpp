#include <iostream>
#include <string>

using namespace std;

class Point {
private:
    string algo;
public:
    Point(string alguno) : algo(alguno) {};
    string getAlgo(){
        return algo;
    }
    void setAlgo(string algo){
        this -> algo = algo;
    }
    string toString(){
        return algo;
    }
};

int main() {
    cout << "Hello, World!" << endl;
    Point miPunto = Point("hola que tal");
    cout << miPunto.toString() << endl;
    return 0;
}