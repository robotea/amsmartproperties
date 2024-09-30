/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   AMCoords.h
 * Author: zdenek
 *
 * Created on 3. září 2019, 17:39
 */

#ifndef AMCOORDS_H
#define AMCOORDS_H


#include "../../AMProperty.h"
#include "AMUICommon.h"
#include <string>


using namespace AMSmartProperties;
struct AMPoint
{
    double x;
    double y;

    bool operator == (const AMPoint& other) const {return (x == other.x) && (y == other.y);}
    bool operator == (const AMPoint&& other) const {return (x == other.x) && (y == other.y);}
    AMPoint& operator +(const AMPoint& other) { x += other.x; y += other.y; return *this;}
};

class AMPointSubproperties
{
public:
    AMProperty<double, double, AMPoint, &AMPoint::x> x;
    AMProperty<double, double, AMPoint, &AMPoint::y> y;


    AMPointSubproperties(AMPropertyPool& pool, AMPoint* tPtr): x(pool, &tPtr->x), y(pool, &tPtr->y)
        {
            AMProperty_log("AMPointSubproperties(%p)::AMPointSubproperties(AMPoint* tPtr=%p)\n",this,tPtr);
        };
    ~AMPointSubproperties()
        {
            AMProperty_log("AMPointSubproperties(%p)::~AMPointSubproperties\n",this);
        };
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


struct AMSize
{
    double width;
    double height;

    bool operator == (const AMSize& other) const {return (width == other.width) && (height == other.height);}
    bool operator == (const AMSize&& other) const {return (width == other.width) && (height == other.height);}
};

class AMSizeSubproperties
{
public:
    AMProperty<double, double, AMSize, &AMSize::width> width;
    AMProperty<double, double, AMSize, &AMSize::height> height;

    AMSizeSubproperties(AMPropertyPool& pool, AMSize* tPtr): width(pool, &tPtr->width)/*, width(width)*/, height(pool, &tPtr->height)/*, height(height)*/
        {
            AMProperty_log("AMSizeSubproperties(%p)::AMSizeSubproperties(AMSize* tPtr=%p)\n",this,tPtr);
        };
    ~AMSizeSubproperties()
        {
            AMProperty_log("AMSizeSubproperties(%p)::~AMSizeSubproperties\n",this);
        };
    void compute() {
        width.compute();
        height.compute();
    };
	void setPool(AMPropertyPool& pool) {
		width.setPool(pool);
		height.setPool(pool);
	}
    void setDescription(const char * desc);
    std::string to_string() const;
};


struct AMRect
{
    AMPoint origin;
    AMSize  sz;

    bool operator == (const AMRect& other) const {return (origin == other.origin) && (sz == other.sz);}
    bool operator == (const AMRect&& other) const {return (origin == other.origin) && (sz == other.sz);}
};

class AMRectSubproperties
{
public:
    AMProperty<AMPoint, AMPointSubproperties, AMRect, &AMRect::origin> origin;
    AMProperty<AMSize, AMSizeSubproperties, AMRect, &AMRect::sz> sz;

    AMRectSubproperties(AMPropertyPool& pool, AMRect* tPtr): origin(pool, &tPtr->origin)/*, origin(origin)*/, sz(pool, &tPtr->sz)/*, sz(sz)*/
        {
            AMProperty_log("AMRectSubproperties(%p)::AMRectSubproperties(AMRect* tPtr=%p)\n",this,tPtr);
        };
    ~AMRectSubproperties()
        {
            AMProperty_log("AMRectSubproperties(%p)::~AMRectSubproperties\n",this);
        };
    void compute() {
        origin.compute();
        sz.compute();
    };
	void setPool(AMPropertyPool& pool) {
		origin.setPool(pool);
		sz.setPool(pool);
	}
    void setDescription(const char * desc);
    std::string to_string() const;
};

namespace std {

    template<> string to_string(::AMPoint&);
    template<> string to_string(::AMSize&);
    template<> string to_string(::AMRect&);
    template<> string to_string(const ::AMPoint&);
    template<> string to_string(const ::AMSize&);
    template<> string to_string(const ::AMRect&);
    template<> string to_string(::AMProperty<AMPoint>&);
    template<> string to_string(::AMProperty<AMSize>&);
    template<> string to_string(::AMProperty<AMRect>&);
}


#endif /* AMCOORDS_H */
