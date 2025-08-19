#include <iostream>
using namespace std;

int main()
{
    int a = 0b1100111;
    cout << (a) << endl;
    cout << int(a & (1 << 0)) << endl;
    cout << int(a & (1 << 1)) << endl;
    cout << int(a & (1 << 2)) << endl;
    cout << int(a & (1 << 3)) << endl;
    cout << int(a & (1 << 4)) << endl;
    cout << int(a & (1 << 5)) << endl;
    cout << int(a & (1 << 6)) << endl;
    return 0;
}