/*
 * @file   AMProperty_2.cpp
 * @author: zdenek
 *
 * Created on 3. září 2019, 16:59
 */
 /**
 *  @example AMProperty_2.cpp
 *
 *   Bad example of using Smart properties.
 * @{
 */
using namespace std;

#include <iostream>
#include "../AMProperty.h"

using namespace AMSmartProperties;
using namespace std;

int main(int argc, char **argv) {

	float k = 1.11f;
	float l = 2.22f;
	float m = 3.33f;

	AMPropertyPool pool;
	AMProperty<float> sum(pool, [&]{return k+l+m;});

	cout << "k+l+m=" << sum() << endl;//6.66

	l = 4.44f;

	cout << "k+l+m=" << sum() << endl;//6.66!

}
/**@}*/
