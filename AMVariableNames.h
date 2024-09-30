/**
 * @file:   AMVariableNames.h
 * Smart properties debug.
 *
 * @author Zdeněk Skulínek  &lt;<a href="mailto:zdenek.skulinek@seznam.cz">me@zdenekskulinek.cz</a>&gt;
 */

#ifndef AMVARIABLENAMES_H
#define AMVARIABLENAMES_H

/**
 *  @ingroup Debug
 *  @{
 */


#include <map>
#include <string>

namespace AMSmartProperties {

/**
 *  @ingroup Debug
 *  @class AMVariableNames
 *  @brief Simple sigleton, that assign address in memory to human readable name.
 *  Function is implicitly disabled, so impact to computing power is minimal.
 */
class AMVariableNames
{
    static bool m_enabled;
    std::map<const void*, std::string> m_map;
    static AMVariableNames* m_instance;

    AMVariableNames();
    static AMVariableNames& instance();
public:
	/**
	 *  @brief enable or disable this function.
	 *  Typically called on on initialisation. Default state is false - disabled.
	 *  @param new value
	 */
    static void setEnabled(bool);
	/**
	 *  @brief assign variable name with address.
	 *  Stored string value is prefix+delimiter+name. reasin for split it into threee paramaters
	 *  is avoid cocatenate string in case that varible name are disabled.
	 *  @param ptr pointer assigned to variable name
	 *  @param prefix first part of variable name
	 *  @param delimiter second part of variable name
	 *  @param name last part of variable name
	 */
    static void setVariableName(const void* ptr, const char * prefix, const char * delimiter, const char * name);
	/**
	 *  @brief assign variable name with adsress and with force enabled parameter.
	 *  This function does as same as previous (static void setVariableName(const void* ptr, const char * prefix, const char * delimiter, const char * name)),
	 *  but enabled depend by parameter. So you may force add it. Usage is for examle when you constructs static object, and this class cannot by enabled by
	 *  calling enable().
	 *  @param ptr pointer assigned to variable name
	 *  @param name of variable name
	 *  @param enabled force enabled state
	 */
    static void setVariableName(const void* ptr, std::string name, bool enabled);
	/**
	 *  @brief string with variable name and address.
	 *  In case, that variable names is allowed and name was previously added to AMVariableNames, returns something as "NAME(0xADDRESS)".
	 *  In other cases, such as AMVariableNames is disabled or was disabled when was called setVariableName or setVariableName was not callet with this address,
	 *  returns something as "(0xADDRESS)".
	 *  @returns varible name
	 */
    static std::string getVariableName(const void* ptr);
	/**
	 *  @brief string with variable name and address.
	 *  In case, that variable names is allowed and name was previously added to AMVariableNames, returns something as "NAME".
	 *  In other cases, such as AMVariableNames is disabled or was disabled when was called setVariableName or setVariableName was not callet with this address,
	 *  returns nullptr.
	 *  @returns varible name or null
	 */
    static const char* getVariableNameCStr(const void* ptr);
	/**
	 *  @brief get info that function is enabled.
	 *  @returns enabled
	 */
    static bool isEnabled() {
		return m_enabled;
	}
};

} //namespace

/** @} */
#endif
