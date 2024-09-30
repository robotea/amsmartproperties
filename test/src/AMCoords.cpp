/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "AMCoords.h"

namespace std {

template<> string to_string(::AMPoint& _x) {
    return string("AMPoint[")+to_string(_x.x)+string(",")+to_string(_x.y)+string("]");
}

template<> string to_string(::AMSize& _x) {
    return string("AMSize[")+to_string(_x.width)+string(",")+to_string(_x.height)+string("]");
}

template<> string to_string(::AMRect& _x) {
    AMPoint org = _x.origin;
    AMSize  sz = _x.sz;
    return string("AMRect[")+to_string(org)+string(",")+to_string(sz)+string("]");
}
/*
template string to_string(::AMPoint& _x);
template string to_string(::AMSize& _x);
template string to_string(::AMRect& _x);
*/

template<> string to_string(const ::AMPoint& _x) {
    return string("AMPoint{")+to_string(_x.x)+string(",")+to_string(_x.y)+string("}");
}

template<> string to_string(const ::AMSize& _x) {
    return string("AMSize{")+to_string(_x.width)+string(",")+to_string(_x.height)+string("}");
}

template<> string to_string(const ::AMRect& _x) {
    const AMPoint org = _x.origin;
    const AMSize  sz = _x.sz;
    return string("AMRect{")+to_string(org)+string(",")+to_string(sz)+string("}");
}
/*
template string to_string(const ::AMPoint& _x);
template string to_string(const ::AMSize& _x);
template string to_string(const ::AMRect& _x);
*/

template<> string to_string(::AMProperty<AMPoint>& _x) {
    return _x.to_string();
}
template<> string to_string(::AMProperty<AMSize>& _x) {
    return _x.to_string();
}
template<> string to_string(::AMProperty<AMRect>& _x) {
    return _x.to_string();
}
/*
template string to_string(::AMProperty<AMPoint>& _x);
template string to_string(::AMProperty<AMSize>& _x);
template string to_string(::AMProperty<AMRect>& _x);
*/
}

std::string AMPointSubproperties:: to_string() const
{
    std::ostringstream os;
            os<<"Props[x=";
            os<<x.to_string();
            os<<", y=";
            os<<y.to_string();
            os<<"]";
    return os.str();
};

void AMPointSubproperties::setDescription(const char * desc)
{
    x.setDescription(desc , ".x");
    y.setDescription(desc , ".y");
}

std::string AMSizeSubproperties::to_string() const
{
    std::ostringstream os;
            os<<"Props[width=";
            os<<width.to_string();
            os<<", height=";
            os<<height.to_string();
            os<<"]";
    return os.str();
}

void AMSizeSubproperties::setDescription(const char *  desc)
{
    width.setDescription(desc , ".width");
    height.setDescription(desc , ".height");
}


std::string AMRectSubproperties::to_string() const
{
    std::ostringstream os;
            os<<"Props[origin=";
            os<<origin.to_string();
            os<<", sz=";
            os<<sz.to_string();
            os<<"]";
    return os.str();
}

void AMRectSubproperties::setDescription(const char * desc)
{
    origin.setDescription(desc , ".origin");
    sz.setDescription(desc , ".sz");
}
