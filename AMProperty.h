/**
 * @file:   AMProperty.h
 * Smart properties itself.
 *
 * @author Zdeněk Skulínek  &lt;<a href="mailto:zdenek.skulinek@seznam.cz">me@zdenekskulinek.cz</a>&gt;
 */

#ifndef AMPROPERTY_H
#define AMPROPERTY_H

/**
 *  @ingroup SmartProperties
 *  @{
 */

#include "amexception/AMException.h"

#include <exception>

#include <string>
#include <sstream>

#ifdef DEBUG
#include <stdio.h>
#include <cstring>
#define AMProperty_log(...) printf(__VA_ARGS__)
#else
#define AMProperty_log(...)
#endif


#include "AMFunction.h"

namespace std{
    template<typename T> string to_string(const T&);
    template<typename T> string to_string(T&);
};


namespace AMSmartProperties {


class AMPropertyBase;
class AMPropertyRefBase;
class AMPropertyPool;

/**
 *  @ingroup SmartProperties
 *  @brief Generic functor type for specified  type of property.
 *  @param T Type of property
 */


/**
 *  @ingroup SmartProperties
 *  @class AMPropertyGeneralException
 *  @brief This exception is not thrown directly. It's a base class.
 */
struct AMPropertyGeneralException : AMCore::AMException {

	AMPropertyPool &m_pool;
public:
    AMPropertyGeneralException(AMPropertyPool &pool);
	/**
	 *  @brief propertyEntered
	 *  @returns pointer to property which was initiated and which causes and exception.
	     Its not the last property, which really causes an exception. Run trace on this property
		 and you may see list of properties, first is this property, last causes an expetion.
	 */
    AMPropertyBase* propertyEntered() const noexcept;
	/**
	 *  @brief propertyCaused
	 *  @returns pointer to property which causes and exception. This a last property in computation hiearchy.
	 */
    AMPropertyBase* propertyCaused() const noexcept;
	 /**
	  *  @brief Traces a property computation.
	  *  This function is available after any exception, othervise return "".
	  *  @returns std::string with property name (in case of use AMVariableNames) of adress on each line.
	  *  @throw This @c function will not throw an exception.
	  */
    std::string trace();
};

/**
 *  @ingroup SmartProperties
 *  @class AMPropertyDeletedException
 *  @brief This exception is thrown when compuded property depends on deleted property.
 */
struct AMPropertyDeletedException : AMPropertyGeneralException {


    AMPropertyDeletedException(AMPropertyPool &pool): AMPropertyGeneralException(pool) {
       msg_ = _("AMProperty: Detected deleted property. Member propertyXXX() show which property causes an exception. Run trace() to explain.");
    }
};


/**
 *  @ingroup SmartProperties
 *  @class AMPropertyLoopException
 *  @brief This exception is thrown when loop is detected.
 */
struct AMPropertyLoopException : AMPropertyGeneralException {


    AMPropertyLoopException(AMPropertyPool &pool): AMPropertyGeneralException(pool) {
        msg_ = _("AMProperty: Detected loop. Member propertyXXX() show which property causes an exception. Run trace() to explain.");
    }
};

/**
 *  @ingroup SmartProperties
 *  @class AMPropertyNestingException
 *  @brief This exception is thrown when too much properties was nested.
 */
struct AMPropertyNestingException : AMPropertyGeneralException {


    AMPropertyNestingException(AMPropertyPool &pool): AMPropertyGeneralException(pool) {
        msg_ = _("AMProperty: Detected too depth nesting. Member propertyXXX() show which property causes an exception. Run trace() to explain.");
    }
};


/**
 *  @ingroup SmartProperties
 *  @enum AMPropertyState
 *  @brief Smart property state
 *  This is combined state for each property. Bits 3-4 are stored in AMSmartProperties::AMPropertyPool,
 *  bits 0-2 are stored on each property (AMSmartProperties::AMProperty) itself.
 */
enum  AMPropertyState
{
    NORMAL = 0, /**< NORMAL = 0x00 | 0x0 | 0x0*/
    COMPUTING = 1, /**< COMPUTING = 0x00 | 0x0 | 0x1*/
    NOCOMPUTED = 2, /**< NOCOMPUTED = 0x00 | 0x0 | 0x2*/
    NOCOMPUTED_COMPUTING = 3, /**< NOCOMPUTED_COMPUTING = 0x00 | 0x0 | 0x3*/
    VALUEONLY = 4, /**< VALUEONLY = 0x00 | 0x0 | 0x4*/
    VALUEONLY_COMPUTING = 5, /**< VALUEONLY_COMPUTING = 0x00 | 0x0 | 0x5*/
    VALUEONLY_NOCOMPUTED = 6, /**< VALUEONLY_NOCOMPUTED = 0x00 | 0x0 | 0x6*/
    VALUEONLY_NOCOMPUTED_COMPUTING = 7, /**< VALUEONLY_NOCOMPUTED_COMPUTING = 0x00 | 0x0 | 0x7*/
    MODIFIED = 8, /**< MODIFIED = 0x00 | 0x8 | 0x0*/
    MODIFIED_COMPUTING = 9, /**< MODIFIED_COMPUTING = 0x00 | 0x8 | 0x1*/
    MODIFIED_NOCOMPUTED = 10, /**< MODIFIED_NOCOMPUTED = 0x00 | 0x8 | 0x2*/
    MODIFIED_NOCOMPUTED_COMPUTING = 11, /**< MODIFIED_NOCOMPUTED_COMPUTING = 0x00 | 0x8 | 0x3*/
    MODIFIED_VALUEONLY = 12, /**< MODIFIED_VALUEONLY = 0x00 | 0x8 | 0x4*/
    MODIFIED_VALUEONLY_COMPUTING = 13, /**< MODIFIED_VALUEONLY_COMPUTING = 0x00 | 0x8 | 0x5*/
    MODIFIED_VALUEONLY_NOCOMPUTED = 14, /**< MODIFIED_VALUEONLY_NOCOMPUTED = 0x00 | 0x8 | 0x6*/
    MODIFIED_VALUEONLY_NOCOMPUTED_COMPUTING = 15, /**< MODIFIED_VALUEONLY_NOCOMPUTED_COMPUTING = 0x00 | 0x8 | 0x7*/
    DESTROYED_NORMAL = 16, /**< DESTROYED_NORMAL = 0x10 | 0x0 | 0x0*/
    DESTROYED_COMPUTING = 17, /**< DESTROYED_COMPUTING = 0x10 | 0x0 | 0x1*/
    DESTROYED_NOCOMPUTED = 18, /**< DESTROYED_NOCOMPUTED = 0x10 | 0x0 | 0x2*/
    DESTROYED_NOCOMPUTED_COMPUTING = 19, /**< DESTROYED_NOCOMPUTED_COMPUTING = 0x10 | 0x0 | 0x3*/
    DESTROYED_VALUEONLY = 20, /**< DESTROYED_VALUEONLY = 0x10 | 0x0 | 0x4*/
    DESTROYED_VALUEONLY_COMPUTING = 21, /**< DESTROYED_VALUEONLY_COMPUTING = 0x10 | 0x0 | 0x5*/
    DESTROYED_VALUEONLY_NOCOMPUTED = 22, /**< DESTROYED_VALUEONLY_NOCOMPUTED = 0x10 | 0x0 | 0x6*/
    DESTROYED_VALUEONLY_NOCOMPUTED_COMPUTING = 23, /**< DESTROYED_VALUEONLY_NOCOMPUTED_COMPUTING = 0x10 | 0x0 | 0x7*/
    DESTROYED_MODIFIED = 24, /**< DESTROYED_MODIFIED = 0x10 | 0x8 | 0x0*/
    DESTROYED_MODIFIED_COMPUTING = 25, /**< DESTROYED_MODIFIED_COMPUTING = 0x10 | 0x8 | 0x1*/
    DESTROYED_MODIFIED_NOCOMPUTED = 26, /**< DESTROYED_MODIFIED_NOCOMPUTED = 0x10 | 0x8 | 0x2*/
    DESTROYED_MODIFIED_NOCOMPUTED_COMPUTING = 27, /**< DESTROYED_MODIFIED_NOCOMPUTED_COMPUTING = 0x10 | 0x8 | 0x3*/
    DESTROYED_MODIFIED_VALUEONLY = 28, /**< DESTROYED_MODIFIED_VALUEONLY = 0x10 | 0x8 | 0x4*/
    DESTROYED_MODIFIED_VALUEONLY_COMPUTING = 29, /**< DESTROYED_MODIFIED_VALUEONLY_COMPUTING = 0x10 | 0x8 | 0x5*/
    DESTROYED_MODIFIED_VALUEONLY_NOCOMPUTED = 30, /**< DESTROYED_MODIFIED_VALUEONLY_NOCOMPUTED = 0x10 | 0x8 | 0x6*/
    DESTROYED_MODIFIED_VALUEONLY_NOCOMPUTED_COMPUTING = 31, /**< DESTROYED_MODIFIED_VALUEONLY_NOCOMPUTED_COMPUTING = 0x10 | 0x8 | 0x7*/
};

/** @ingroup SmartProperties
 *  @brief Converts AMPropertyState to std::string.
 *  @param state
 *  @return string with state
 */
std::string to_string(const AMPropertyState& state);


/**
 *  @ingroup SmartProperties
 *  @brief AMSmartProperties::AMPropertyPool is a property pool for smart properties.
 *
 *  Class AMSmartProperties::AMPropertyPool is data stucture which hold set of smart properties. Typically, is Smart
 *  properties are used for GUI, One window own one property Pool.
 *
 *  AMSmartProperties::AMPropertyPool also helps tracing of properties, that are involved in property loop.
 *  It limits computation depth to TRACE_SIZE-1 (1023) levels. Tracing works only on case that loop is really detected.
 *
 */
class AMPropertyPool {
public:
    /**
	 *  @brief Max. computation depth of smart properties.
	 */
    static const int TRACE_SIZE = 1024;

protected:
    AMPropertyState m_state;
    AMPropertyBase* m_firstChild;

    AMPropertyBase* m_trace_array[TRACE_SIZE];
    AMPropertyBase** m_trace_iterator;
    AMPropertyBase** m_trace_stop;

public:
    /**
     *  @brief Default construct creates an empty AMPropertyPool.
	 *  @throw This @c function will not throw an exception.
	 */
    AMPropertyPool();
	/**
     *  @brief Do not use it directly!
     */
    AMPropertyPool(AMPropertyState state);
    /**
	 *  @brief Returns state.
	 *  @return Pool state (Three lower bits-property bits are zero)
	 *  @throw This @c function will not throw an exception.
	 */
    AMPropertyState getPoolState() {return m_state;}
    /**
     *  @brief Set state to modified. This state is signalized from AMProperty and holds
	 *  this state untl next reset.
	 *  @throw This @c function will not throw an exception.
	 */
    void setModified() {m_state = (AMPropertyState)((unsigned int)m_state | 0x8);}
    /**
     *   @brief Resets pool.
     *   Mode info on @ref SmartProperties
	 *   @throw This @c function will not throw an exception.
	 */
    void reset();
	/**
	 *  @brief Test if Pool is in Destroyed state.
	 *  Trace is, when state signalised Destroyed flag.
	 *  @return bool is in Destroyed state.
	 *  @throw This @c function will not throw an exception.
	 */
        bool isDestroyed() {
            return ((static_cast<unsigned int>(m_state) & ~0xF ) == (static_cast<unsigned int>(AMPropertyState::DESTROYED_NORMAL)));
        }
	/**
	 *  @brief Test if Pool is in progress.
	 *  @return bool is in progress
	 *  @throw This @c function will not throw an exception.
	 */
	bool isInProgress() {
	    return (m_trace_iterator != m_trace_array);
        }
    /**
     *  @brief Only leaves from computing and signalises Loop.
	 *  @throw This @c function will not throw an exception.
     */
    void runLoop();
	/**
     *  @brief Only leaves from computing and signalises Deleted.
	 *  @throw This @c function will not throw an exception.
     */
    void runDeleted();
    /**
     *  @brief Traces a property computation.
	 *  This function is available after any exception, othervise return "".
	 *  @returns std::string with property name (in case of use AMVariableNames) of adress on each line.
	 *  @throw This @c function will not throw an exception.
     */
    std::string trace();
    /**
     *  @brief Destroys pool.
     *  State of pool moves for ever to state DESTROYED and Smart properties will do nothing.
	 *  @throw This @c function will not throw an exception.
     */
    void destroy();
    /**
     *  @brief Get default pool.
     *  Default pool is every time in state DESTROYED. Since it, every Smart properties removed from any pool is moved here.
     *  Smart property MUST have assigned any pool.
	 *  @return Default pool (\ref AMPropertyPool)
   	 *  @throw This @c function will not throw an exception.
     */
    static AMPropertyPool& defaultPool();
    /**
     *  @brief Dumps all properties as string description into specified buffer.
	 *  @param buffer
	 *  @param length buffer length
	 *  @throw This @c function will not throw an exception.
     */
    void dump(char* buffer = nullptr, int length = 0);
	/**
	 *  @brief Retrieve first property, that was actually computed (root). Valid only after exception.
	 *  @return pointer to that property.
	 */
	AMPropertyBase* getEnteringProperty();
	/**
	 *  @brief Retrieve last property, that was actually computed - property yhat causes and exception (leaf). Valid only after exception.
	 *  @return pointer to that property.
	 */
	AMPropertyBase* getLastCausedProperty();

    friend class AMPropertyBase;
    friend class AMPropertyRefBase;
protected:
    void plusDepthTraceItem(AMPropertyBase* item) {
        (*m_trace_iterator) = item;
        m_trace_iterator++;
        if (m_trace_iterator == m_trace_array+TRACE_SIZE) { throw AMPropertyNestingException(*this); }
    }
    void minusDepth() {if (m_trace_iterator == m_trace_array) {throw std::underflow_error("AMPropertyPool: Out of range");};m_trace_iterator--;}
    void addToPool(AMPropertyBase* property);
    void removeFromPool(AMPropertyBase* property);
};



/**
 *  @ingroup SmartProperties
 *  @class AMPropertyBase
 *  @brief Base interface and basic fuction for smart property.
 *  Most of these funcions can be used, whenever you have only reference to base class. But, in spite of this
 *  base class is not virtual, you can't instantiate it directly.
 */
class AMPropertyBase
{
	friend class AMPropertyPool;
protected:

    AMPropertyPool* m_pool;
    union {
        AMPropertyBase* m_next;
        AMPropertyState m_state;
        unsigned long   m_uint;
    };

    AMPropertyBase(AMPropertyPool& pool): m_pool(&pool), m_uint((unsigned long)NOCOMPUTED)  { m_pool->addToPool(this); }
    ~AMPropertyBase() {m_pool->removeFromPool(this);}
public:
    /**
     *  @brief Get the pool assigned to property
	 *  @return The pool reference (AMSmartProperties::AMPropertyPool).
	 *  @throw This @c function will not throw an exception.
     */
    AMPropertyPool& getPool() const {return *m_pool;}
    /**
     *  @brief Move property from one pool to another
	 *  @param newPool new AMSmartProperties::AMProperyPool in which should be this property placed.
	 *  @throw This @c function will not throw an exception.
     */
    void setPool(AMPropertyPool& newPool) { m_pool->removeFromPool(this);m_pool = (&newPool);m_pool->addToPool(this); }
    /**
     *  @brief Do not use it directly!
	 *  @return Next property in pool. You may iterate over all properties in AMSmartProperties::AMProperyPool.
	 *  @throw This @c function will not throw an exception.
	 */
    AMPropertyBase* getNext() const {return reinterpret_cast<AMPropertyBase*>(m_uint & ~0x3);}
    /**
     *  @brief Reset state od property.
	 *  This function is not intenden for use in common operation.
     */
    void resetState() {m_uint &= ~0x3;m_uint |= 0x2;}
	/**
	 *  @brief Information about property
	 *  @return true if property was not been computed since last reset.
	 *  @throw This @c function will not throw an exception.
	 */
	bool isNonComputed() const {return m_uint & 0x2;}
	/**
	 *  @brief Information about property
	 *  Set flag that property is not already computed.
	 *  @throw This @c function will not throw an exception.
	 */
	void setNoComputedFlag() {m_uint |= 0x2;}
	/**
	 *  @brief Information about property
	 *  @param noComputed Set/reset flag that property is not already computed.
	 *  @throw This @c function will not throw an exception.
	 */
	void setNoComputed(bool noComputed) {m_uint = noComputed ? m_uint | 0x2 : m_uint & ~0x2;}
	/**
	 *  @brief Information about property
	 *  @return true if property is computing. (Signals computing and calls another property)
	 *  @throw This @c function will not throw an exception.
	 */
	bool getComputingFlag() {return m_uint & 0x1;}
	/**
	 *  @brief Information about property
	 *  @return true if property computing is in progress. (was called by another property)
	 *  @throw This @c function will not throw an exception.
	 */
	bool isInProgress() {return getPool().isInProgress();}
	/**
	 *  @brief Sets operation mode to loop.
	 *  @throw This @c function will not throw an exception.
	 */
    void runLoop();

	/**
	 *  @brief Sets operation mode to deleted.
	 *  It works, if current mode is normal.
	 *  @throw This @c function will not throw an exception.
	 */
	void runDeleted();

	/**
	 *  @brief Get state (AMSmartProperties::AMPropertyState) of property.
	 *  Bits 0,1 comes from this property. Bit 2 is given from inherited class. Oher bits, 3,4 comes from AMSmartProperties::AMProperyPool.
	 *  Warning! If parameter valueOnly has set any other bits than (0, AMPropertyState::VALUEONLY), result is undefined.
	 *  @param valueOnly this one bit information comes from inherited class.
	 *  @return state (AMSmartProperties::AMPropertyState)
	 *  @throw This @c function will not throw an exception.
	 */
	AMPropertyState getState(AMPropertyState valueOnly) const {return  static_cast<AMPropertyState>(static_cast<unsigned int>(getPool().getPoolState()) | (m_uint & 0x3) | static_cast<unsigned int>(valueOnly));}
	/**
	 *  @brief Debug helper. Simple dumps property into std::string.
	 *  @return stringstream
	 *  @throw This @c function will not throw an exception.
	 */
	std::string to_string() const;

protected:
    void setComputingFlag() {m_uint |= 0x1;}
    void clearComputingFlag() {m_uint &= ~0x1;}
    void clearComputingAndNocomputedFlag() {m_uint &= ~0x3;}
    void setModified() {setNoComputedFlag();getPool().setModified();}
	void plusDepthTraceItem(AMPropertyBase* item) {getPool().plusDepthTraceItem(item);}
    void minusDepth() {getPool().minusDepth();}
	void setNext(AMPropertyBase* newNext) { m_uint = reinterpret_cast<unsigned long>(newNext) | (m_uint & 0x3); }
	void setVariableName(const void* ptr, const char * prefix, const char * delimiter, const char * name) const;
	bool isEnabledVariableNames() const;
	std::string getVariableName(const void* ptr) const;
	const char* getVariableNameCStr(const void* ptr) const;
};

template<typename T, typename TSubProperties, typename TParent, T TParent::*pointer_member>
class AMPropertyKernel
{
    T*const m_memberPtr;
public:
    constexpr AMPropertyKernel(AMPropertyPool& pool, T*const valuePtr): m_memberPtr(valuePtr){};
    TParent* getParentPointer() {
        union {
            T TParent::*my_pointer_member = pointer_member;
            int offset;
        };
        return (TParent*)(((char*)m_memberPtr) - offset);
        }
    TParent& getParent() {return *getParentPointer();}

    T& getT() {return *(m_memberPtr);};
    const T& getTconst() const {return *m_memberPtr;};
    T*const getTPointerConst() const {return m_memberPtr;}
	T* getTPointer() {return m_memberPtr;}

    constexpr TSubProperties* getSubpropertiesPtr() {return nullptr;}
    constexpr T TParent::* getPointerToMember() {return pointer_member;}

    void compute() {};
    void setDescription(const char * desc) {}
	void setPool(AMPropertyPool& pool) {}

    void to_string_valptr(std::ostringstream& os) const {os<<"val="<<((reinterpret_cast<const unsigned long>(m_memberPtr) <= sizeof(TSubProperties)) ? std::string("nullptr") : std::to_string(*m_memberPtr))<<", ptr="<<std::hex<<m_memberPtr;}
    void to_string_subprops(std::ostringstream& os) const {};
};

template<typename T, typename TSubProperties, typename TParent, T TParent::*pointer_member>
class AMPropertyKernelWithSub
{
    T*const         m_memberPtr;
    TSubProperties  m_subMembers;
public:
    constexpr AMPropertyKernelWithSub(AMPropertyPool& pool, T*const valuePtr): m_memberPtr(valuePtr), m_subMembers(pool, m_memberPtr){};
    TParent* getParentPointer() {
        union {
            T TParent::*my_pointer_member = pointer_member;
            int offset;
        };
        return (TParent*)(((char*)m_memberPtr) - offset);
        }
    TParent& getParent() {return *getParentPointer();}

    T& getT() {return *(m_memberPtr);};
    const T& getTconst() const {return *m_memberPtr;};
	T*const getTPointerConst() const {return m_memberPtr;}
	T* getTPointer() {return m_memberPtr;}


    constexpr TSubProperties* getSubpropertiesPtr() {return &m_subMembers;}
    constexpr T TParent::* getPointerToMember() {return pointer_member;}

    void compute() {m_subMembers.compute();}
    void setDescription(const char * desc) {m_subMembers.setDescription(desc);}
	void setPool(AMPropertyPool& pool) {m_subMembers.setPool(pool);}

    void to_string_valptr(std::ostringstream& os) const {os<<"val="<<((reinterpret_cast<const unsigned long>(m_memberPtr) <= sizeof(TSubProperties)) ? std::string("nullptr") : std::to_string(*m_memberPtr))<<", ptr="<<std::hex<<m_memberPtr;}
    void to_string_subprops(std::ostringstream& os) const {os<<m_subMembers.to_string();};
};

template<typename T, typename TSubProperties, typename TParent, T TParent::*pointer_member>
class AMPropertyKernelWithT
{
    T               m_value;
public:
    constexpr AMPropertyKernelWithT(AMPropertyPool& pool, T*const valuePtr): m_value(valuePtr == nullptr? T() : *valuePtr){};
    TParent* getParentPointer(){return nullptr;}

    T& getT() {return m_value;};
    const T& getTconst() const {return m_value;};
	T*const getTPointerConst() const {T& ref=const_cast<T&>(m_value);return &ref;}
	T* getTPointer() {return &m_value;}


    constexpr TSubProperties* getSubpropertiesPtr() {return nullptr;}
    constexpr T TParent::* getPointerToMember() {return pointer_member;}

    void compute() {}
    void setDescription(const char * desc) {}
	void setPool(AMPropertyPool& pool) {}

    void to_string_valptr(std::ostringstream& os) const {os<<"val="<<std::to_string(m_value);}
    void to_string_subprops(std::ostringstream& os) const {};
};

template<typename T, typename TSubProperties, typename TParent, T TParent::*pointer_member>
class AMPropertyKernelWithTWithSub
{
    T               m_value;
    TSubProperties  m_subMembers;
public:
    constexpr AMPropertyKernelWithTWithSub(AMPropertyPool& pool, T*const valuePtr): m_value(valuePtr == nullptr? T() : *valuePtr), m_subMembers(pool, &m_value){};
    TParent* getParentPointer(){return nullptr;}

    T& getT() {return m_value;};
    const T& getTconst() const {return m_value;};
	T*const getTPointerConst() const {T& ref=const_cast<T&>(m_value);return &ref;}
	T* getTPointer() {return &m_value;}

    constexpr TSubProperties* getSubpropertiesPtr() {return &m_subMembers;}
    constexpr T TParent::* getPointerToMember() {return pointer_member;}

    void compute() {m_subMembers.compute();}
    void setDescription(const char * desc) {m_subMembers.setDescription(desc);}
	void setPool(AMPropertyPool& pool) {m_subMembers.setPool(pool);}

    void to_string_valptr(std::ostringstream& os) const {os<<"val="<<std::to_string(m_value);}
    void to_string_subprops(std::ostringstream& os) const {os<<m_subMembers.to_string();};
};


/**
 *  @ingroup SmartProperties
 *  @class AMPropertyNonBase
 *  @brief "NULL" type class.
 *  Used by AMSmartProperties::AMProperty, when parent class is not defined.
 */
class AMPropertyNonBase
{
};


template<typename T, typename TSubProperties, typename TParent, T TParent::*pointer_member>
class AMProperty;


/**
 *  @ingroup SmartProperties
 *  @class AMPropertyRefBase
 *  @brief Smart property reference base class.
 */
class AMPropertyRefBase
{
	friend class AMPropertyPool;

protected:
	AMPropertyBase*    m_target;
	AMPropertyRefBase(AMPropertyBase* target) : m_target(target) {}
	~AMPropertyRefBase() {}
	AMPropertyBase* getTarget() {return m_target;}
	void setTarget(AMPropertyBase* target) {m_target = target;}
};


/**
 *  @ingroup SmartProperties
 *  @class AMProperty
 *  @brief Smart property class.
 *  @param T - type of property itself
 *  @param TSubProperties - if AMSmartProperties::AMProperty<T> has nested subproperties, this associated subproperties class. Othervise T class.
 *  @param TParent - if you intend have this property as subproperty os some parent and you want to acces it as parent.property, class of parent.
 *  @param T TParent::*pointer_member - if you intend have this property as subproperty os some parent and you want to acces it as parent.property, poiner to member TParent::property.
 */
template<typename T, typename TSubProperties = T, typename TParent = AMPropertyNonBase, T TParent::*pointer_member = nullptr>
class AMProperty: public AMPropertyBase
{

public:
	/**
	 *  @brief Functor type for this property.
	 */
    typedef AMFunction<T()> type_fn;
protected:
    type_fn m_f;

    typename std::conditional< pointer_member != nullptr,
        typename std::conditional< std::is_same<T,TSubProperties>::value, AMPropertyKernel<T, TSubProperties, TParent, pointer_member>, AMPropertyKernelWithSub<T, TSubProperties, TParent, pointer_member> >::type  ,
        typename std::conditional< std::is_same<T,TSubProperties>::value, AMPropertyKernelWithT<T, TSubProperties, TParent, pointer_member>, AMPropertyKernelWithTWithSub<T, TSubProperties, TParent, pointer_member> >::type
    >::type m_kernel;


public:
    /**
	 *  @brief Constructor for subproperty.
	 *  Do not use othervise. Suproperty has only reference to real type T. This should be only called from T subproperies class.
	 *  @param pool AMSmartProperties::AMPropertyPool.
	 *  @param ptr - pointer to real address of T within parent object.
	 */
    AMProperty(AMPropertyPool& pool, T*const ptr): AMPropertyBase(pool), m_f(nullptr), m_kernel(pool, ptr)
    {
        static_assert(pointer_member != nullptr, "This constructor is intended only for subproperties, do not use it.");
        AMProperty_log("AMProperty(%p)::AMProperty(T* ptr=%p) %s\n",this, ptr,to_string().c_str());
    }
	/**
	 *  @brief "Default" constructor.
	 *  @param pool AMSmartProperties::AMPropertyPool.
	 *  @param name property name. Incase of AMSmartProperties::AMVariableNames is not enabled, it does nothing.
	 */
    AMProperty(AMPropertyPool& pool, const char* name = nullptr): AMPropertyBase(pool), m_f(nullptr), m_kernel(pool, nullptr)
    {
        static_assert(pointer_member == nullptr, "This constructor is not intended for subproperties, do not use it.");
		AMProperty_log("AMProperty(%p)::AMProperty() %s\n",this,to_string().c_str());
        setVariableName(this, "", "", name);
        m_kernel.setDescription(name);

    };
	/**
	 *  @brief "Value" constructor.
	 *  Every reading from property, returns this value.
	 *  @param pool AMSmartProperties::AMPropertyPool.
	 *  @param value initial value of type T.
	 *  @param name property name. Incase of AMSmartProperties::AMVariableNames is not enabled, it does nothing.
	 */
    AMProperty(AMPropertyPool& pool, T value, const char* name = nullptr): AMPropertyBase(pool), m_f(nullptr), m_kernel(pool, &value)
    {
        static_assert(pointer_member == nullptr, "This constructor is not intended for subproperties, do not use it.");
		AMProperty_log("AMProperty(%p)::AMProperty(T value=%s) %s\n",this, std::to_string(value).c_str(),to_string().c_str());
        setVariableName(this, "", "", name);
        m_kernel.setDescription(name);

    };
	/**
	 *  @brief "Functor" constructor.
	 *  Every reading from property, returns result of this function. (i may be cached, ref: @ref SmartProperties)
	 *  @param pool AMSmartProperties::AMPropertyPool.
	 *  @param f initial functor of type AMSmartProperties::AMProperty::type_fn.
	 *  @param name property name. Incase of AMSmartProperties::AMVariableNames is not enabled, it does nothing.
	 */
    constexpr AMProperty(AMPropertyPool& pool, type_fn f, const char* name = nullptr): AMPropertyBase(pool), m_f(f), m_kernel(pool, nullptr)
    {
        static_assert(pointer_member == nullptr, "This constructor is not intended for subproperties, do not use it.");
		AMProperty_log("AMProperty(%p)::AMProperty(type_fn _f=%p) %s\n",&f,this,to_string().c_str());
        setVariableName(this, "", "", name);
        m_kernel.setDescription(name);

    };
	/**
	 *  @brief "Copy" constructor.
	 *  @param other property copy from.
	 */
    AMProperty(const AMProperty& other) : AMPropertyBase(other.getPool()), m_f(other.m_f), m_kernel(other.getPool(), other.m_kernel.getTPointerConst()){
		static_assert(pointer_member == nullptr, "This constructor is not intended for subproperties, do not use it.");
		AMProperty_log("AMProperty(%p)::AMProperty(AMProperty& other)) %s\n",this,other.to_string().c_str());
		const char* name = getVariableNameCStr(&other);
		setVariableName(this, "", "", name);
        m_kernel.setDescription(name);
    }
	/**
	 *  @brief "Move" constructor.
	 *  Smart roperty has not move constructor, due to composition. (All member are storaged inside smart property)
	 *  @param other property move from.
	 */
    constexpr AMProperty(AMProperty&& other) : AMPropertyBase(other.getPool()), m_f(std::move(other.m_f)), m_kernel(other.getPool(), other.m_kernel.getTPointer()){
		static_assert(pointer_member == nullptr, "This constructor is not intended for subproperties, do not use it.");
		AMProperty_log("AMProperty(%p)::AMProperty(AMProperty& other)) %s\n",this,other.to_string().c_str());
		const char* name = getVariableNameCStr(&other);
		setVariableName(this, "", "", name);
        m_kernel.setDescription(name);
		other.setPool(AMPropertyPool::defaultPool());
    }
	/**
	 *  @brief Destructor.
	 */
    ~AMProperty()
    {
        AMProperty_log("AMProperty(%p)::AMProperty(%x)",this);
		setPool(AMPropertyPool::defaultPool());
    };
	/**
	 *  @brief Copy functor into property.
	 *  Do not use it directly..
	 */
    void setValueFn(const type_fn& _f)
    {
        AMProperty_log("AMProperty(%p)::setValueFn(%p) %s\n",this, &_f,to_string().c_str());
        setModified();
        m_f=_f;
    };
	/**
	 *  @brief Move functor into property.
	 *  Do not use it directly..
	 */
    void setValueFn(const type_fn&& _f)
    {
        AMProperty_log("AMProperty(%p)::setValueFn(%p) %s\n",this, &_f,to_string().c_str());
        setModified();
        m_f=std::move(_f);
    };
	/**
	 *  @brief Get functor from property.
	 */
    type_fn& getValueFn()
    {
        AMProperty_log("AMProperty(%p)::getValueFn() -> %p ---%s\n",this, &m_f,to_string().c_str());
        return m_f;
    };
	/**
	 *  @brief Set Pool (AMSmartProperties::AMPropertyPool)
	 *  You may reassign property to another pool(window), or inactives it(assign to AMSmartProperties::AMPropertyPool::defaultPool())
	 */
	void setPool(AMPropertyPool& pool)
	{
		AMPropertyBase::setPool(pool);
		m_kernel.setPool(pool);
	}
	/**
	 *  @brief get last result.
	 *  This function does not compute property. Consider use it directly.
	 */
    const T& getResult() const
	{
		return m_kernel.getTconst();
	}
	/**
	 *  @brief set last result.
	 *  This function does not compute property. Consider use it directly. Stored value will be there until next recompution.
	 */
    void setResult(T value)
	{
		AMProperty_log("AMProperty(%p)::setResult(%s) \n",this, std::to_string(m_kernel.getTconst()).c_str());
		m_kernel.getT() = value;
	}
	/**
	 *  @brief get subproperties.
	 *  Consider use it directly.
	 */
    constexpr TSubProperties* getSubProperties()
	{
		/*AMProperty_log("AMProperty(%p)::T* operator->() %s\n",this,to_string().c_str());*/
		return m_kernel.getSubpropertiesPtr();
	}
	/**
	 *  @brief test, if property has a constant value (functor is empty)
	 */
    bool isValueOnly() const
	{
		return m_f == nullptr;
	}
	/**
	 *  @brief test if proparty has subproperties.
	 */
    constexpr bool hasSubproperties() const
	{
		return pointer_member != nullptr;
	}
	/**
	 *  @brief returns a computed value.
	 *  Freshly computed, or cached, in case of nothig is modified.
	 *  @returns T
	 *	@throws may thwor and exception. (AMPropertyLoopException, AMPropertyNestingException, AMPropertyDeletedException)
	 */
    T operator()()
    {
        AMProperty_log("AMProperty(%p)::const T& operator() const ->%s --- %s\n",(const void *)this, std::to_string(m_kernel.getTconst()).c_str(), to_string().c_str());
		compute();
		return getResult();
    }
	/**
	 *  @brief computes result.
	 *  Consider use it directly.
	 */
    void compute()
    {
		AMProperty_log("AMProperty(%p)::compute --- %s\n",(const void *)this, to_string().c_str());
        AMPropertyState state = getState(m_f == nullptr ? AMPropertyState::VALUEONLY : AMPropertyState::NORMAL);
        switch(state) {
        case AMPropertyState::NORMAL:
                                    break;
        case AMPropertyState::COMPUTING:
                                    runLoop();
                                    break;
        case AMPropertyState::NOCOMPUTED:
                                    setComputingFlag();
                                    plusDepthTraceItem(this);
									setResult(m_f());
                                    minusDepth();
                                    clearComputingAndNocomputedFlag();
                                    break;
        case AMPropertyState::NOCOMPUTED_COMPUTING:
                                    runLoop();
                                    break;
        case AMPropertyState::VALUEONLY:
                                    break;
        case AMPropertyState::VALUEONLY_COMPUTING:
                                    runLoop();
                                    break;
        case AMPropertyState::VALUEONLY_NOCOMPUTED:
                                    plusDepthTraceItem(this);
                                    m_kernel.compute();
                                    minusDepth();
                                    break;
        case AMPropertyState::VALUEONLY_NOCOMPUTED_COMPUTING:
                                    runLoop();
                                    break;
        case AMPropertyState::MODIFIED:
									setComputingFlag();
									plusDepthTraceItem(this);
									setResult(m_f());
									minusDepth();
									clearComputingAndNocomputedFlag();
									break;
        case AMPropertyState::MODIFIED_COMPUTING:
                                    runLoop();
                                    break;
        case AMPropertyState::MODIFIED_NOCOMPUTED:
									setComputingFlag();
									plusDepthTraceItem(this);
									setResult(m_f());
									minusDepth();
									clearComputingAndNocomputedFlag();
									break;
        case AMPropertyState::MODIFIED_NOCOMPUTED_COMPUTING:
                                    runLoop();
                                    break;
        case AMPropertyState::MODIFIED_VALUEONLY:
                                    plusDepthTraceItem(this);
                                    m_kernel.compute();
                                    minusDepth();
                                    break;
        case AMPropertyState::MODIFIED_VALUEONLY_COMPUTING:
                                    runLoop();
                                    break;
        case AMPropertyState::MODIFIED_VALUEONLY_NOCOMPUTED:
                                    plusDepthTraceItem(this);
                                    m_kernel.compute();
									minusDepth();
                                    break;
        case AMPropertyState::MODIFIED_VALUEONLY_NOCOMPUTED_COMPUTING:
                                    runLoop();
                                    break;
        case AMPropertyState::DESTROYED_NORMAL:
        case AMPropertyState::DESTROYED_COMPUTING:
        case AMPropertyState::DESTROYED_NOCOMPUTED:
        case AMPropertyState::DESTROYED_NOCOMPUTED_COMPUTING:
        case AMPropertyState::DESTROYED_VALUEONLY:
        case AMPropertyState::DESTROYED_VALUEONLY_COMPUTING:
        case AMPropertyState::DESTROYED_VALUEONLY_NOCOMPUTED:
        case AMPropertyState::DESTROYED_VALUEONLY_NOCOMPUTED_COMPUTING:
        case AMPropertyState::DESTROYED_MODIFIED:
        case AMPropertyState::DESTROYED_MODIFIED_COMPUTING:
        case AMPropertyState::DESTROYED_MODIFIED_NOCOMPUTED:
        case AMPropertyState::DESTROYED_MODIFIED_NOCOMPUTED_COMPUTING:
        case AMPropertyState::DESTROYED_MODIFIED_VALUEONLY:
        case AMPropertyState::DESTROYED_MODIFIED_VALUEONLY_COMPUTING:
        case AMPropertyState::DESTROYED_MODIFIED_VALUEONLY_NOCOMPUTED:
        case AMPropertyState::DESTROYED_MODIFIED_VALUEONLY_NOCOMPUTED_COMPUTING:
									runDeleted();
                                    break;
		default: break;
        }
    }
	/**
	 *  @brief returns a computed value.
	 *  Freshly computed, or cached, in case of nothig is modified.
	 *  @returns T
	 *	@throws may thwor and exception. (AMPropertyLoopException, AMPropertyNestingException, AMPropertyDeletedException)
	 */
    operator const T& ()
    {
        AMProperty_log("AMProperty(%p)::operator const T& () const ->%s --- %s\n",(const void *)this, std::to_string(m_kernel.getTconst()).c_str(), to_string().c_str());
		compute();
		return getResult();
    }
	/**
	 *  @brief Copy functor into property.
	 *  @param new functor
	 *  @returns new functor
	 */
    const type_fn& operator = (const type_fn& right) {
        setValueFn(right);
        return getValueFn();
    }
	/**
	 *  @brief Move functor into property.
	 *  @param new functor
	 *  @returns new functor
	 */
    const type_fn& operator = (const type_fn&& right) {
        setValueFn(std::move(right));
        return getValueFn();
    }
	/**
	 *  @brief set last result.
	 *  This function does not compute property. Stored value will be there until next recompution.
	 *  @param const T right side
	 *  @returns newly stored value
	 */
    const T& operator = (const T& right) {
        setValueFn(nullptr);
        setResult(right);
        setModified();
        return right;
    }
	/**
	 *  @brief get subproperties.
	 */
    constexpr TSubProperties* operator->()
	{
		AMProperty_log("AMProperty(%p)::T* operator->() %s\n",this,to_string().c_str());
		return m_kernel.getSubpropertiesPtr();
	}
	/**
	 *  @brief operator +
	 *  computes a value and add it to right side parameter.
	 *  @param right side
	 *  @return result
	 */
    T operator+(const T& other) {
        AMProperty_log("AMProperty(%p)::operator+(T&) %s --- %s\n",(const void *)this, to_string().c_str(), std::to_string(other));
		compute();
		T result =  getResult();
        result = result + other;
        return result;
    }
	/**
	 *  @brief operator -
	 *  computes a value and substract by right side parameter.
	 *  @param right side
	 *  @return result
	 */
    T operator-(const T& other) {
        AMProperty_log("AMProperty(%p)::operator-(T&) %s --- %s\n",(const void *)this, to_string().c_str(), std::to_string(other));
		compute();
		T result =  getResult();
        result = result - other;
        return result;
    }
	/**
	 *  @brief operator *
	 *  computes a value and multiply by right side parameter.
	 *  @param right side
	 *  @return result
	 */
    T operator*(const T& other) {
        AMProperty_log("AMProperty(%p)::operator*(T&) %s --- %s\n",(const void *)this, to_string().c_str(), std::to_string(other));
		compute();
		T result =  getResult();
        result = result * other;
        return result;
    }
	/**
	 *  @brief operator /
	 *  computes a value and add divide by right side parameter.
	 *  @param right side
	 *  @return result
	 */
    T operator/(const T& other) {
        AMProperty_log("AMProperty(%p)::operator/(T&) %s --- %s\n",(const void *)this, to_string().c_str(), std::to_string(other));
		compute();
		T result =  getResult();
        result = result / other;
        return result;
    }
	/**
	 *  @brief operator %
	 *  computes a value and add module by right side parameter.
	 *  @param right side
	 *  @return result
	 */
    T operator%(const T& other) {
        AMProperty_log("AMProperty(%p)::operator%(T&) %s --- %s\n",(const void *)this, to_string().c_str(), std::to_string(other));
		compute();
		T result =  getResult();
        result = result % other;
        return result;
    }
	/**
	 *  @brief operator ++
	 *  computes a value and add increment it. Value does not store back!
	 *  @param right side
	 *  @return result
	 */
    T operator++() {
        AMProperty_log("AMProperty(%p)::operator++() %s\n",(const void *)this, to_string().c_str());
		compute();
		T result =  getResult();
        result = result++;
        return result;
    }
	/**
	 *  @brief operator --
	 *  computes a value and decrement it.Value does not store back!
	 *  @param right side
	 *  @return result
	 */
    T operator--() {
        AMProperty_log("AMProperty(%p)::operator--() %s\n",(const void *)this, to_string().c_str());
		compute();
		T result =  getResult();
        result = result--;
        return result;
    }
	/**
	 *  @brief set description (variable names).
	 *  Name consists from two equvalent parts. Reason for split is avoiding joining strings in case of varible names not active.
	 *  @param desc first part of name
	 *  @param suffix second part of name
	 */
    void setDescription(const char * desc, const char * suffix) {
		AMProperty_log("AMProperty(%p)::setDescritption(%s, %s) %s\n",this,desc,suffix,to_string().c_str());
        if (isEnabledVariableNames()) {
            std::string str = desc;
            str += suffix;
            setVariableName(this, "", "",str.c_str());
            m_kernel.setDescription(str.c_str());
        }
    }
	/**
	 *  @brief dumps a property info a std::string.
	 */
    std::string to_string() const{
        std::ostringstream os;
            os << getVariableName(this);
            os << ", ";
            m_kernel.to_string_valptr(os);
            os << ", st=";
			os << AMSmartProperties::to_string(getState(m_f == nullptr ? AMPropertyState::VALUEONLY : AMPropertyState::NORMAL));
            os << ", fn=";
            os << &m_f;
            os << "{";
            m_kernel.to_string_subprops(os);
            os << "}]";
        return os.str();
    }

};

}//namespace

/** @} */
#endif /* AMPROPERTY_H */
