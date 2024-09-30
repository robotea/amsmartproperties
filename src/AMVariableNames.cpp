#include "../AMVariableNames.h"
#include <sstream>

namespace AMSmartProperties {

AMVariableNames* AMVariableNames::m_instance = nullptr;
bool AMVariableNames::m_enabled = false;


AMVariableNames::AMVariableNames() : m_map() {}

AMVariableNames& AMVariableNames::instance()
{
    if (!m_instance) {
        m_instance = new AMVariableNames();
    }
    return *m_instance;
}

void AMVariableNames::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

void AMVariableNames::setVariableName(const void* ptr, const char * prefix, const char * delimiter, const char * name)
{
    if (m_enabled) {
        std::string str = prefix ? prefix : "";
        str += delimiter ? delimiter : "";
        str += name ? name : "";
        if (str == "") {
            return;
        }
        AMVariableNames& instance = AMVariableNames::instance();
        instance.m_map.insert({ptr, str});
    }
}


void AMVariableNames::setVariableName(const void* ptr, std::string name, bool enabled)
{
    if (enabled) {
        AMVariableNames& instance = AMVariableNames::instance();
        instance.m_map.insert({ptr, name});
    }
}

std::string AMVariableNames::getVariableName(const void* ptr)
{
    std::stringstream strm;

    if (m_enabled) {
        AMVariableNames& instance = AMVariableNames::instance();
        std::map<const void*, std::string>::iterator it;
        it = instance.m_map.find(ptr);
        if (it == instance.m_map.end()) {
            strm << ptr;
        }
        else {
            strm<<it->second<<"("<<ptr<<")";
        }
    }
    else {
        strm << ptr;
    }
    return strm.str();
}

const char* AMVariableNames::getVariableNameCStr(const void* ptr)
{
    std::stringstream strm;

    if (m_enabled) {
        AMVariableNames& instance = AMVariableNames::instance();
        std::map<const void*, std::string>::iterator it;
        it = instance.m_map.find(ptr);
        if (it == instance.m_map.end()) {
            return nullptr;
        }
        else {
            return it->second.c_str();
        }
    }
    return nullptr;
}

} //namespace
