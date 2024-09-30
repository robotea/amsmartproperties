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



class AMView
{

    AMView* m_parent;
public:
    AMProperty<AMRect, AMRectSubproperties>  rect;
protected:
    AMPropertyContainerType<AMView*> m_children;

public:
    AMView(AMView* _parent, const char* _desc);
    AMView(AMPropertyPool& _pool, const char* _desc);
    void addSubview(AMView* view);
    virtual bool hitTest();
    AMPropertyContainerType<AMView*>& subviews();
    virtual AMPropertyPool& getPool();
};



#endif /* AMVIEW_H */
