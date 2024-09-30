/**
 * @page SmartPropertiesTutorial Smart Properties Tutorial
 * @brief Easy way to learn Smart properties.
 *

<h3>Getting started</h3>

One little piece of code will probably say more than a thousand words.
Let's say we have three variables and we want to have their sum in the variable
sum and always up to date.

@code{.cpp}

 AMPropertyPool pool;

 AMProperty<float> propk(pool, 1.11f);
 AMProperty<float> propl(pool, 2.22f);
 AMProperty<float> propm(pool, 3.33f);

 AMProperty<float> sum(pool, [&]{return propk+propl+propm;});

 cout << "k+l+m=" << (float)sum << endl;  //print sum, 6.66

 propl = 4.44f; //we add by 2.22

 cout << "k+l+m=" << sum() << endl;  //print new sum, 8.88

 @endcode

Note the parameter capture for lambda function references. Call this lambda function a functor. Vice versa
if you are going to want to pass smart property through some pointer, the compiler saves it as the
pointer. In this case, present it with ([=]).

As you can see, just change the variable propl, the sum will be calculated again the next time you try to read it.

Each Smart property must be tied to some pool (AMSmartProperties::AMPropertyPool). Pool bundles Smart properties together
and ensures their joint functionality. Typically, one window has one pool.

It remains to be seen what a function call is for property (operator()). One of the basic principles
Smart properties is that the lambda function does not take any parameter and returns type T, in our case float. Reading from
Smart property is therefore in most cases associated with typecasting from AMSmartProperties::AMProperty <T>na T. In Smart properties it is possible to
three ways, all of which are equivalent for functionality. The first is the implicit cast we see in the
seventh row. Although we add up Smart properties, we eventually return the float. The second option can be seen on line 9, where explicitly
we get the stored value as a float. Casting to float in this way can be annoying, so there is one more option
-- operator(). So, as can be seen on line 11.

The previous code can be found @ref AMProperty_1.cpp.

<h3>How not to do it completely</h3>

@code{.cpp}

float k = 1.11f;
float l = 2.22f;
float m = 3.33f;

AMPropertyPool pool;
AMProperty<float> prop1(pool, [&]{return k+l+m;});

cout << "k+l+m=" << sum() << endl;//6.66

l = 4.44;

cout << "k+l+m=" << sum() << endl;//6.66!

 @endcode

 If you try this, you'll find that after changing variable l, you don't count the sum. The results are cached.
This means that variables only recalculate if you change something, exactly if you change any Smart property
in the same pool (AMSmartProperties::AMPropertyPool) functor or value.

Although this method is not the solution to the introduction problem, it may be useful -- for you to break the loop.

The previous code can be found @ref AMProperty_2.cpp.


<h3>Smart properties and data structures</h3>

But let's go back to the first example. Let's just say we don't want to add up three variables,
but we want to go through some kind of data structure in which
Smart properties are stored. It's possible, of course. Here's an example.

@code{.cpp}

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

@endcode

The previous code can be found @ref AMProperty_3.cpp.


<h3>Nesting Smart properties</h3>

Let's say we want to have a Point class that is smart property itself and at the same time contains,
Smart properties x and y.

@code{.cpp}
struct Point
{
    double x;
    double y;

};
@endcode

And where are the Smart properties inside? Well, they would increase the structure of Point and limit the possibilities of its use.
Plus, we'd like to be able to get the back type out of Smart Property, like, a float from AMProperty<float>.

Therefore, we define one more structure.

@code{.cpp}
class PointSubproperties
{
public:
    AMProperty<double, double, Point, &Point::x> x;
    AMProperty<double, double, Point, &Point::y> y;

    AMPointSubproperties(AMPropertyPool& pool, Point* tPtr): x(pool, &tPtr->x), y(pool, &tPtr->y) {};
    ~AMPointSubproperties() {};

    void compute() {
        x.compute();
        y.compute();
    };
	void setPool(AMPropertyPool& pool) {
		x.setPool(pool);
		y.setPool(pool);
	}
    void setDescription(const char * desc);
    std::string to_string() const;
};

namespace std {

    template<> string to_string(::Point&);
    template<> string to_string(const ::Point&);
    template<> string to_string(::AMProperty<Point>&);;
}

@endcode

... and with it this code...

@code{.cpp}
#include "AMCoords.h"

namespace std {

	template<> string to_string(::Point& _x) {
	    return string("Point[")+to_string(_x.x)+string(",")+to_string(_x.y)+string("]");
	}

	template string to_string(::Point& _x);

	template<> string to_string(const ::Point& _x) {
	    return string("Point{")+to_string(_x.x)+string(",")+to_string(_x.y)+string("}");
	}

	template string to_string(const ::Point& _x);

	template<> string to_string(::AMProperty<Point>& _x) {
	    return _x.to_string();
	}

	template string to_string(::AMProperty<Point>& _x);

}

std::string PointSubproperties:: to_string() const
{
    std::ostringstream os;
            os<<"Props[x=";
            os<<x.to_string();
            os<<", y=";
            os<<y.to_string();
            os<<"]";
    return os.str();
};

void PointSubproperties::setDescription(const char * desc)
{
    x.setDescription(desc , ".x");
    y.setDescription(desc , ".y");
}
@endcode


-# Smart properties defines in a slightly modified way. We will tell them that their
the parent structure is Point and their address within that structure (as pointer to member).

-# Parent Smart property, AMProperty<Point>, will call our constructor in this format,
where pool is the first parameter. which we just copy into subproperties and pointer to the structure
Point. We'll use it to get the real address of the subproperties and pass it on as a pointer.
Subproperty therefore has no value in it and instead just points to it by reference.

-# Then it is necessary to create several trivial methods, where we will just do some work for all
required subproperties. Namely compute(), setPool(pool), setDescription(description).

-# For debug, but i don't think it can be translated without them, it's a good idea to implement to_string.
I simply write the text and involve the call into to_string to the subproperties. I will do as for Point,
for const Point, for AMProperty<Point> and also for PointSubproperties.

-# I must not forget these four methods to instance when they are cpp file.

In the end, I'm going to use my creation...

@code{.cpp}
AMProperty<Point, PointSubproperties> origin;
@endcode

<h3>And what if the whole Point is subproperty?</h3>
We can combine both approaches, for example, when I want to create a Smart property Rect.

@code{.cpp}
struct Rect
{
    AMPoint origin;
    ...

};

class RectSubproperties
{
	AMProperty<AMPoint, AMPointSubproperties, Rect, &Rect::origin> origin;
	...
}
@endcode
 */
