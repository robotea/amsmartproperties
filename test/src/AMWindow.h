#include "AMView.h"

class AMWindow: public AMView
{
    AMPropertyPool m_pool;
public:
    AMWindow();
    AMPropertyPool& getPool();
};


