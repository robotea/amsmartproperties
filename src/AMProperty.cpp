/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../AMProperty.h"
#include "../AMVariableNames.h"

namespace AMSmartProperties {
    std::string to_string(const AMPropertyState& state)
    {
        switch(state) {
        case AMPropertyState::NORMAL: return std::string("NORMAL");
        case AMPropertyState::COMPUTING: return std::string("COMPUTING");
        case AMPropertyState::NOCOMPUTED: return std::string("NOCOMPUTED");
        case AMPropertyState::NOCOMPUTED_COMPUTING: return std::string("NOCOMPUTED_COMPUTING");
        case AMPropertyState::VALUEONLY: return std::string("VALUEONLY");
        case AMPropertyState::VALUEONLY_COMPUTING: return std::string("VALUEONLY_COMPUTING");
        case AMPropertyState::VALUEONLY_NOCOMPUTED: return std::string("VALUEONLY_NOCOMPUTED");
        case AMPropertyState::VALUEONLY_NOCOMPUTED_COMPUTING: return std::string("VALUEONLY_NOCOMPUTED_COMPUTING");
        case AMPropertyState::MODIFIED: return std::string("MODIFIED");
        case AMPropertyState::MODIFIED_COMPUTING: return std::string("MODIFIED_COMPUTING");
        case AMPropertyState::MODIFIED_NOCOMPUTED: return std::string("MODIFIED_NOCOMPUTED");
        case AMPropertyState::MODIFIED_NOCOMPUTED_COMPUTING: return std::string("MODIFIED_NOCOMPUTED_COMPUTING");
        case AMPropertyState::MODIFIED_VALUEONLY: return std::string("MODIFIED_VALUEONLY");
        case AMPropertyState::MODIFIED_VALUEONLY_COMPUTING: return std::string("MODIFIED_VALUEONLY_COMPUTING");
        case AMPropertyState::MODIFIED_VALUEONLY_NOCOMPUTED: return std::string("MODIFIED_VALUEONLY_NOCOMPUTED");
        case AMPropertyState::MODIFIED_VALUEONLY_NOCOMPUTED_COMPUTING: return std::string("MODIFIED_VALUEONLY_NOCOMPUTED_COMPUTING");
        case AMPropertyState::DESTROYED_NORMAL: return std::string("DESTROYED_NORMAL");
        case AMPropertyState::DESTROYED_COMPUTING: return std::string("DESTROYED_COMPUTING");
        case AMPropertyState::DESTROYED_NOCOMPUTED: return std::string("DESTROYED_NOCOMPUTED");
        case AMPropertyState::DESTROYED_NOCOMPUTED_COMPUTING: return std::string("DESTROYED_NOCOMPUTED_COMPUTING");
        case AMPropertyState::DESTROYED_VALUEONLY: return std::string("DESTROYED_VALUEONLY");
        case AMPropertyState::DESTROYED_VALUEONLY_COMPUTING: return std::string("DESTROYED_VALUEONLY_COMPUTING");
        case AMPropertyState::DESTROYED_VALUEONLY_NOCOMPUTED: return std::string("DESTROYED_VALUEONLY_NOCOMPUTED");
        case AMPropertyState::DESTROYED_VALUEONLY_NOCOMPUTED_COMPUTING: return std::string("DESTROYED_VALUEONLY_NOCOMPUTED_COMPUTING");
        case AMPropertyState::DESTROYED_MODIFIED: return std::string("DESTROYED_MODIFIED");
        case AMPropertyState::DESTROYED_MODIFIED_COMPUTING: return std::string("DESTROYED_MODIFIED_COMPUTING");
        case AMPropertyState::DESTROYED_MODIFIED_NOCOMPUTED: return std::string("DESTROYED_MODIFIED_NOCOMPUTED");
        case AMPropertyState::DESTROYED_MODIFIED_NOCOMPUTED_COMPUTING: return std::string("DESTROYED_MODIFIED_NOCOMPUTED_COMPUTING");
        case AMPropertyState::DESTROYED_MODIFIED_VALUEONLY: return std::string("DESTROYED_MODIFIED_VALUEONLY");
        case AMPropertyState::DESTROYED_MODIFIED_VALUEONLY_COMPUTING: return std::string("DESTROYED_MODIFIED_VALUEONLY_COMPUTING");
        case AMPropertyState::DESTROYED_MODIFIED_VALUEONLY_NOCOMPUTED: return std::string("DESTROYED_MODIFIED_VALUEONLY_NOCOMPUTED");
        case AMPropertyState::DESTROYED_MODIFIED_VALUEONLY_NOCOMPUTED_COMPUTING: return std::string("DESTROYED_MODIFIED_VALUEONLY_NOCOMPUTED_COMPUTING");
        default: return std::string("Unknown");
        }
    }


AMPropertyPool g_defaultPool = AMPropertyPool(AMPropertyState::DESTROYED_NORMAL);

AMPropertyPool& AMPropertyPool::defaultPool() {
    return g_defaultPool;
}

AMPropertyPool::AMPropertyPool():
    m_state(AMPropertyState::NORMAL), m_firstChild(nullptr), m_trace_array(), m_trace_iterator(m_trace_array), m_trace_stop(m_trace_array)
{
}

AMPropertyPool::AMPropertyPool(AMPropertyState state):
    m_state(state), m_firstChild(nullptr), m_trace_array(), m_trace_iterator(m_trace_array), m_trace_stop(m_trace_array)
{
}

void AMPropertyPool::dump(char* buffer, int length)
{
    int i=0;
    AMPropertyBase* it = m_firstChild;
    if (buffer) {
        char* pbuf = buffer;
        int rlen = length;
        pbuf[0] = '\0';
        while (it) {
            int ln = snprintf(pbuf, rlen, "D  %i -> %s\n", i, it->to_string().c_str());
            pbuf += ln;
            rlen -= ln;
            i++;
            it = it->getNext();
        }

    }
    else {
        while (it) {
            printf("D  %i -> %s\n", i, it->to_string().c_str());
            i++;
            it = it->getNext();
        }
    }

}

void AMPropertyPool::reset()
{
    AMPropertyBase* it = m_firstChild;
    while (it) {
        it->resetState();
        it = it->getNext();
    }
    m_state = AMPropertyState::NORMAL;
    m_trace_iterator = m_trace_stop = m_trace_array;
}

void AMPropertyPool::addToPool(AMPropertyBase* _property)
{
    // This means not able to add to default poll. But there is no need, have linked list of abandonned properties.
    if ((AMPropertyState)(((unsigned int)m_state) & 0xF) == AMPropertyState::DESTROYED_NORMAL) {
        return;
    }
    _property->setNext(m_firstChild);
    m_firstChild = _property;
}

void AMPropertyPool::removeFromPool(AMPropertyBase* _property)
{
    if ((AMPropertyState)(((unsigned int)m_state) & 0xF) == AMPropertyState::DESTROYED_NORMAL) {
        _property->setNext(nullptr);
        return;
    }
    AMPropertyBase* it = m_firstChild;
    AMPropertyBase* lastit = nullptr;
    while (it) {
        if (it == _property) {
            if (!lastit) {
                m_firstChild = it->getNext();
            }
            else {
                lastit->setNext(it->getNext());
            }
            it->setNext(nullptr);
            break;
        }
        lastit = it;
        it = it->getNext();
    }
}

void AMPropertyPool::destroy()
{
    if ((AMPropertyState)(((unsigned int)m_state) & 0xF) == AMPropertyState::DESTROYED_NORMAL) {
        return;
    }
    m_state = AMPropertyState::DESTROYED_NORMAL;
    /*
    AMPropertyBase* it = m_firstChild;
    while (it) {
        it->setPool(AMPropertyPool::defaultPool());
        it = it->getNext();
    }*/
    m_firstChild = nullptr;
}

void AMPropertyPool::runLoop()
{
    m_trace_stop = m_trace_iterator;
    m_trace_iterator = m_trace_array;
    for(AMPropertyBase** it = m_trace_array;it != m_trace_stop; it++) {
        (*it)->clearComputingFlag();
    }
}

void AMPropertyPool::runDeleted()
{
    m_trace_stop = m_trace_iterator;
    m_trace_iterator = m_trace_array;

    for(AMPropertyBase** it = m_trace_array;it != m_trace_stop; it++) {
        (*it)->clearComputingFlag();
    }
}

std::string AMPropertyPool::trace() {

    const static int SIZE = 16384;
    char buffer[SIZE];
    std::stringstream ss(std::ios_base::trunc | std::ios_base::out);
    ss.rdbuf()->pubsetbuf(buffer, SIZE);

    AMPropertyBase** it = m_trace_array;

    for(;it != m_trace_stop;it++) {
        ss << AMVariableNames::getVariableName(*it) << std::endl;
    }
    ss.flush();

    int retLength = ss.tellp();
    if (retLength < 0) {
        retLength = SIZE-1;
    }
    buffer[retLength] = 0;

    return std::string(buffer);
}

AMPropertyBase* AMPropertyPool::getEnteringProperty()
{
    return m_trace_array[0];
}

AMPropertyBase* AMPropertyPool::getLastCausedProperty()
{
    if (m_trace_iterator == m_trace_array && m_trace_stop > m_trace_iterator && m_trace_stop < m_trace_array+TRACE_SIZE) {
        auto beforeTraceStop = m_trace_stop;
        beforeTraceStop--;
        return *beforeTraceStop;
    }
    return nullptr;
}

AMPropertyGeneralException::AMPropertyGeneralException(AMPropertyPool& pool) :
    AMCore::AMException(), m_pool(pool) {}

AMPropertyBase* AMPropertyGeneralException::propertyEntered() const noexcept
{
    return m_pool.getEnteringProperty();
}

AMPropertyBase* AMPropertyGeneralException::propertyCaused() const noexcept
{
    return m_pool.getLastCausedProperty();
}

std::string AMPropertyGeneralException::trace()
{
    return m_pool.trace();
}

void AMPropertyBase::runDeleted()
{
    AMPropertyPool& pool = getPool();
    pool.plusDepthTraceItem(this);
    pool.runDeleted();
    throw AMPropertyDeletedException(pool);
}

void AMPropertyBase::runLoop()
{
    AMPropertyPool& pool = getPool();
    pool.plusDepthTraceItem(this);
    pool.runLoop();
    clearComputingFlag();
    throw AMPropertyLoopException(pool);
}
/*
void AMPropertyBase::setComputeFnForTrace(void (*traceComputeFunction)(void *vptr))
{
    AMPropertyPool& pool = getPool();
    pool.setComputeFnForTrace(traceComputeFunction);
}*/

/*
std::string AMPropertyBase::trace() {
    return getPool().trace(this);
}*/

std::string AMPropertyBase::to_string() const
{
    std::ostringstream os;
    os<<"["<<AMVariableNames::getVariableName(this);
    os<<", st=";
    os<<AMSmartProperties::to_string(static_cast<AMPropertyState>(m_uint & 0x3) );
    os<<"]";
    return os.str();
}

void AMPropertyBase::setVariableName(const void* ptr, const char * prefix, const char * delimiter, const char * name) const
{
    AMVariableNames::setVariableName(ptr, prefix, delimiter, name);
}

bool AMPropertyBase::isEnabledVariableNames() const
{
    return AMVariableNames::isEnabled();
}

std::string AMPropertyBase::getVariableName(const void* ptr) const
{
    return AMVariableNames::getVariableName(ptr);
}

const char* AMPropertyBase::getVariableNameCStr(const void* ptr) const
{
    return AMVariableNames::getVariableNameCStr(ptr);
}

}
