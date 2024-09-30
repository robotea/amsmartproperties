/*
 * @file   AMproperty_1.cpp
 * @author: zdenek
 *
 * Created on 3. září 2019, 16:59
 */
 /**
 *  @example AMProperty_1.cpp
 *
 *   Basic example of using Smart properties.
 * @{
 */
using namespace std;

#include <iostream>
#include "../AMProperty.h"

using namespace AMSmartProperties;
using namespace std;

int main(int argc, char **argv) {

	AMPropertyPool pool;
    AMProperty<float> propk(pool, 1.11f);
	AMProperty<float> propl(pool, 2.22f);
	AMProperty<float> propm(pool, 3.33f);


	AMProperty<float> sum(pool, [&]{return propk+propl+propm;});

	cout << "k+l+m=" << (float)sum << endl;

	propl = 4.44f;

	cout << "k+l+m=" << sum() << endl;

}
/**@}*/
