/*
 * @file   AMFunction_1.cpp
 * @author: zdenek
 *
 * Created on 3. září 2019, 16:59
 */
 /**
 *  @example AMFunction_1.cpp
 *
 *   More complex example of usage AMSmartProperties::AMFunction.
 * @{
 */
using namespace std;

#include <iostream>
#include "../AMFunction.h"

using namespace AMSmartProperties;
using namespace std;

int main(int argc, char **argv) {

    int res;

    AMFunction<int(int,int)> amfn1([](int x, int y){return x+y;});
    res = amfn1(8, 57);
    cout << "res (amfn1) = " << res << endl; // 65

    AMFunction<int(double)> amfn2;
    try {
         res = amfn2( 2.9 );
         //this is not called
         cout << "empty AMFunction should throw exception on call" << endl;
    }
    catch(std::bad_function_call& e) {
         cout << "amfn2 std::bad_function_call was thrown" << endl;
    }
    catch(...) {
         //its also not called
         cout << "empty AMFunction throws undefined exception" << endl;
    }

    cout << "amfn1 is empty: " << (!amfn1 ? "YES" : "NO") << endl;
    cout << "amfn2 is empty: " << (!amfn2 ? "YES" : "NO") << endl;

    AMFunction<int(int,int, int)> amfn3;
    amfn3 = [](int a, int b, int c){return a+b+c;};
    res = amfn3(81, 9, 12);
    cout << "amfn3()-> " << res << endl; //102
    amfn3 = [&](int, int, int){return res + 61;};
    res = amfn3(1,2,3);
    cout << "amfn3()-> " << res << endl; //163

    AMFunction<int(int,int, int)> amfn4;
    amfn4 = amfn3;
    res = amfn4(1,2,3);
    cout << "amfn4()-> " << res << endl; //224

}
/**@}*/
