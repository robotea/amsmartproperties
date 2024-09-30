/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   AMView.h
 * Author: zdenek
 *
 * Created on 3. září 2019, 17:00
 */

#ifndef AMVIEW_H
#define AMVIEW_H

#include "AMUICommon.h"
#include "AMCoords.h"
#include <list>
#include <memory>


class AMView
{
    std::list<std::shared_ptr<AMView>> m_children;
public:
    AMPropertyImpl<AMRect>  rect;

    AMView():rect() {
         AMProperty_log("AMView(%p)::AMView()\n",this);
    };

    AMView(double x, double y, double width, double height): rect(AMRect(x, y, width, height)) {
         AMProperty_log("AMView(%p)::AMView(x,y,width,height)\n",this);
         rect->origin->x.setValueFn([=](){return x;});
         rect->origin->y.setValueFn([=](){return y;});
         rect->size->width.setValueFn([=](){return width;});
         rect->size->height.setValueFn([=](){return height;});*/
    };

    AMView(std::function<double()> x, std::function<double()> y, std::function<double()> width, std::function<double()>  height):
               rect(AMRect(AMPoint(x, y), AMSize(width, height))) {
          AMProperty_log("AMView(%p)::AMView(fn x, fn y, fn width, fn height)\n",this);
          rect->origin->x.setValueFn(x);
          rect->origin->y.setValueFn(y);
          rect->size->width.setValueFn(width);
          rect->size->height.setValueFn(height);
    };

    ~AMView() {
         AMProperty_log("AMView(%p)::~AMView()\n",this);
    };

    bool hitTest() {
         AMProperty_log("AMView(%p)::hitTest()\n",this);
         return false;
    };

    void addSubview(std::shared_ptr<AMView> view) {
         AMProperty_log("AMView(%p)::addSubview(%p)\n",this,view.get());
         m_children.push_back(view);
    }

    AMRect& rect() {
         return m_rect.getValue();
    }

    std::list<std::shared_ptr<AMView>>& subviews() {
         return m_children;
    };

};
#endif /* AMVIEW_H */
