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

#include "../../AMFunction.h"
#include "gtest/gtest.h"

using namespace AMSmartProperties;

TEST(AMFunction, ConstructDestruct) {

     AMFunction<int(double)>* amfnp = new AMFunction<int(double)>();
     delete amfnp;
     amfnp = new AMFunction<int(double)>(nullptr);
     delete amfnp;

     AMFunction<int(double)> amfn1;
     int res = 0;
     try {
          res = amfn1( 2.9 );
          FAIL() << "empty AMFunction should throw exception on call";
     }
     catch(std::bad_function_call& e) {
     }
     catch(...) {
          FAIL() << "empty AMFunction throws undefined exception";
     }
     EXPECT_EQ(res, 0);

     AMFunction<int(int,int)> amfn2([](int x, int y){
		 return x+y;
	 });
     res = amfn2(6, 15);
     EXPECT_EQ(res, 21);

     AMFunction<int(int,int)> amfn4(std::move(amfn2));
     res = amfn4( 7, 57);
     EXPECT_EQ(res, 64);
     try {
          res = amfn2(3, 9);
          FAIL() << "empty AMFunction should throw exception on call";
     }
     catch(std::bad_function_call& e) {
     }
     catch(...) {
          FAIL() << "empty AMFunction throws undefined exception";
     }

}


TEST(AMFunction, Operators) {

    int res;

    AMFunction<int(int,int)> amfn1([](int x, int y){return x+y;});
    res = amfn1(8, 57);
    EXPECT_EQ(res, 65);

    AMFunction<int(int,int)> amfn2;
    EXPECT_FALSE(amfn2);
    EXPECT_FALSE(amfn2 != nullptr);
    EXPECT_FALSE(nullptr != amfn2);
    EXPECT_TRUE(amfn2 == nullptr);
    EXPECT_TRUE(nullptr == amfn2);
    amfn2 = amfn1;
    res = amfn2(88, -5);
    EXPECT_EQ(res, 83);
    EXPECT_TRUE(amfn2);
    EXPECT_TRUE(amfn2 != nullptr);
    EXPECT_TRUE(nullptr != amfn2);
    EXPECT_FALSE(amfn2 == nullptr);
    EXPECT_FALSE(nullptr == amfn2);

    AMFunction<int(int,int)> amfn3 = std::move(amfn2);
    res = amfn3(81, 9);
    EXPECT_EQ(res, 90);
    EXPECT_TRUE(amfn3);
    amfn3 = nullptr;
    EXPECT_FALSE(amfn3);

    AMFunction<int(int,int, int)> amfn5;
    amfn5 = [](int a, int b, int c){return a+b+c;};
    res = amfn5(81, 9, 12);
    EXPECT_EQ(res, 102);
    amfn5 = [&](int, int, int){return res + 61;};
    res = amfn5(1,2,3);
    EXPECT_EQ(res, 163);

    int externint = 24;
    AMFunction<int(int,int,int)> amfn6([&](int x, int y, int z){
        return x+y+z+externint;
        });
    AMFunction<int(int,int,int)> amfn7([&](int x, int y, int z){
        return x+y+z-externint;
        });
    res = amfn6( 5, 15, 25);
    EXPECT_EQ(res, 69);
    res = amfn7( 5, 15, 25);
    EXPECT_EQ(res, 21);
    amfn6 = amfn7;
    res = amfn6( 5, 15, 25);
    EXPECT_EQ(res, 21);
    res = amfn7( 5, 15, 25);
    EXPECT_EQ(res, 21);



}

TEST(AMFunction, Rtti) {

    int externint = 24;
    AMFunction<int(int,int,int)> amfn6([&](int x, int y, int z){return x+y+z+externint;});
    AMFunction<int(int,int,int)> amfn7([&](int x, int y, int z){return x+y+z-externint;});
    EXPECT_NE(amfn6.target_type(), amfn7.target_type());
    EXPECT_STRNE(amfn6.target_type().name(), amfn7.target_type().name());
}


TEST(AMFunction, Sizes) {

    //store in object
    auto fn1 = []{return 5;};
    EXPECT_EQ(sizeof(&fn1), sizeof(void*));
    EXPECT_EQ(sizeof(fn1), 1UL);
    EXPECT_EQ(sizeof(AMFunction<void()>), 3*sizeof(void*));
    //EXPECT_EQ(sizeof(AMFunction<void()>), 8*sizeof(void*));
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
    EXPECT_EQ(sizeof(amfn3), 3*sizeof(void*));
}


TEST(AMFunction, EX1) {
	int res;

    AMFunction<int(int,int)> amfn1([](int x, int y){return x+y;});
    res = amfn1(8, 57);
	EXPECT_EQ(res, 65);
    //cout << "res (amfn1) = " << res << endl; // 65

    AMFunction<int(double)> amfn2;
    try {
         res = amfn2( 2.9 );
         //this is not called
         FAIL() << "empty AMFunction should throw exception on call";
    }
    catch(std::bad_function_call& e) {
         //cout << "amfn2 std::bad_function_call was thrown" << endl;
    }
    catch(...) {
         //its also not called
         FAIL() << "empty AMFunction throws undefined exception";
    }

	EXPECT_TRUE(amfn1);
    //cout << "amfn1 is empty: " << (!amfn1 ? "YES" : "NO") << endl;
	EXPECT_FALSE(amfn2);
    //cout << "amfn2 is empty: " << (!amfn2 ? "YES" : "NO") << endl;

    AMFunction<int(int,int, int)> amfn3;
    amfn3 = [](int a, int b, int c){return a+b+c;};
    res = amfn3(81, 9, 12);
	EXPECT_EQ(res, 102);
    //cout << "amfn3()-> " << res << endl; //102
    amfn3 = [&](int, int, int){return res + 61;};
    res = amfn3(1,2,3);
	EXPECT_EQ(res, 163);
    //cout << "amfn3()-> " << res << endl; //163

    AMFunction<int(int,int, int)> amfn4;
    amfn4 = amfn3;
    res = amfn4(1,2,3);
	EXPECT_EQ(res, 224);
    //cout << "amfn4()-> " << res << endl; //224
}

int main(int argc, char **argv) {

     ::testing::InitGoogleTest(&argc, argv);
     return RUN_ALL_TESTS();
}
