/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   main.cpp
 * Author: zdenek
 *
 * Created on 3. září 2019, 16:59
 */

#include "gtest/gtest.h"
using namespace std;
namespace AMTEST3{
#define COLLECTING_DATA
#define AMFUNCTION_DEFAULT_DATA_SIZE (16*sizeof(void*))
#include "../../AMFunction.h"
	namespace AMSmartProperties
	{
		unsigned long _T_AMFN_size_base::_M_max = 0;
	}
}
using namespace AMTEST3::AMSmartProperties;


TEST(AMFunction, Maxsize_default_size) {
    auto fn1 = []{return 5;};
    EXPECT_EQ(sizeof(&fn1), sizeof(void*));
    EXPECT_EQ(sizeof(fn1), 1UL);
    EXPECT_EQ(sizeof(AMFunction<void()>), 18*sizeof(void*));
    AMFunction<int()> amfn1([]{return 15;});
    int res = amfn1();
    EXPECT_EQ(res, 15);

    //store on heap - loo large
    double q=1.0,w=2.0,e=3.0,k=4.0;
    auto fn2 = [&]{return q+w+e+k;};
    AMFunction<double()> amfn2([&]{return q+w+e+k;});
    double resd = amfn2();
    EXPECT_EQ(resd, 10.0);
    EXPECT_EQ(sizeof(fn2), 4*sizeof(double));

    AMFunction<double()> amfn3([&]{return q+w+e+k;});
    resd = amfn3();
    EXPECT_EQ(resd, 10.0);
    EXPECT_EQ(sizeof(fn2), 4*sizeof(double));
    EXPECT_EQ(sizeof(amfn3), 18*sizeof(void*));

    EXPECT_EQ((AMFunction<void()>::getMaximumFunctorSize()), 32UL);
}
