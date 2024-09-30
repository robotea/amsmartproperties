#include "AMView.h"
#include "AMWindow.h"

AMView::AMView(AMView* _parent, const char* _desc):m_parent(_parent),rect(_parent ? _parent->getPool() : AMPropertyPool::defaultPool(), _desc)
{
    if (m_parent) {
        m_parent->addSubview(this);
    }
    AMProperty_log("AMView(%p)::AMView(AMView* _parent=%p, const char* desc=%s)\n",this,_parent, _desc);
};

AMView::AMView(AMPropertyPool& _pool, const char* _desc):m_parent(nullptr),rect(_pool,_desc)
{
    AMProperty_log("AMView(%p)::AMView(AMPropertyPool& _pool=%p, const char* _desc=%s)\n",this,&_pool,_desc);
};



void AMView::addSubview(AMView* view)
{
    AMProperty_log("AMView(%p)::addSubview(%p)\n",this,view);
    m_children.push_back(view);
};


bool AMView::hitTest()
{
    AMProperty_log("AMView(%p)::hitTest()\n",this);
    return false;
};


AMPropertyContainerType<AMView*>& AMView::subviews()
{
    return m_children;
};


AMPropertyPool& AMView::getPool() {
    if (m_parent) {
        return m_parent->getPool();
    }
    else {
        return AMPropertyPool::defaultPool();
    }
}
