/*
 * @file   AMProperty_3.cpp
 * @author: zdenek
 *
 * Created on 3. září 2019, 16:59
 */
 /**
 *  @example AMProperty_3.cpp
 *
 *   Smart properties and data structures.
 * @{
 */
using namespace std;

#include <iostream>
#include <list>
#include "../AMProperty.h"

using namespace AMSmartProperties;
using namespace std;

int main(int argc, char **argv) {

	AMPropertyPool pool;
    std::list<AMProperty<int> > myList;

    for(int i=0; i < 10; i++) {
        AMProperty<int> value(pool, i+1);
        myList.push_back(value);
    }

    AMProperty<int> sum(pool, [&]	{
        int sum = 0;
        for(std::list<AMProperty<int> >::iterator it = myList.begin(); it != myList.end(); it++) {
            sum += (int)*it;
        }
        return sum;
    });

    cout << "sum1 =" << sum() << endl; //55

    std::list<AMProperty<int> >::iterator it = myList.begin();
    std::advance(it, 4);
    *it = 25; //previous value 5

    cout << "sum2 =" << sum() << endl; //75

}
/**@}*/
