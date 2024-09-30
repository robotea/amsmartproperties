#include "AMWindow.h"

AMWindow::AMWindow(): AMView(m_pool,"Window"), m_pool()
{
    AMProperty_log("AMWindow(%p)::AMWindow()\n",this);
};


AMPropertyPool& AMWindow::getPool()
{
    return m_pool;
};
