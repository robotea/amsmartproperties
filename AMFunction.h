/**  @file AMFunction.h
*    This file is function wrapper like std::function but optimized for Smart properties.
*    @author Zdeněk Skulínek  &lt;<a href="mailto:zdenek.skulinek@seznam.cz">me@zdenekskulinek.cz</a>&gt;
*/

/**
 *  @ingroup SmartProperties
 * @{
 */

#ifndef AMFUNCTION_H
#define AMFUNCTION_H

#include <functional>

namespace AMSmartProperties {

	#ifdef AMFUNCTION_DEFAULT_DATA_SIZE
    static const int _AMFUNCTION_DEFAULT_DATA_SIZE = AMFUNCTION_DEFAULT_DATA_SIZE;
	#else
	static const int _AMFUNCTION_DEFAULT_DATA_SIZE = sizeof(void*);
	#endif
    /*
     * Derives from @c unary_function or @c binary_function, or perhaps
     * nothing, depending on the number of arguments provided. The
     * primary template is the basis case, which derives nothing.
     */
    template<typename _Res, typename ... _ArgTypes>
    struct _T_AMFN_maybe_unary_or_binary_function {
    };

    // Derives from @c unary_function, as appropriate.
    template<typename _Res, typename _T1>
    struct _T_AMFN_maybe_unary_or_binary_function<_Res, _T1>
            : std::unary_function<_T1, _Res> {
    };

    // Derives from @c binary_function, as appropriate.
    template<typename _Res, typename _T1, typename _T2>
    struct _T_AMFN_maybe_unary_or_binary_function<_Res, _T1, _T2>
            : std::binary_function<_T1, _T2, _Res> {
    };


    enum _E_AMFN_manager_operation {
        _get_type_info,
        _clone_functor,
        _clone_functor_move,
        _destroy_functor
    };

	struct _T_AMFN_size_base {
        static unsigned long _M_max;
    };

#ifdef COLLECTING_DATA

    template<typename _Functor>
    struct _T_AMFN_size : _T_AMFN_size_base {

        unsigned long _M_get_size() {return _M_size;}

        static  int _M_register_size() {
            if (_M_max < sizeof(_Functor)) {
                _M_max = sizeof(_Functor);
            }
            return sizeof(_Functor);
        }
        static unsigned long _M_size;
    };
    template<typename _Functor>
    unsigned long _T_AMFN_size<_Functor>::_M_size = _T_AMFN_size<_Functor>::_M_register_size();
#endif

    /*
     *  Trait identifying "location-invariant" types, meaning that the
     *  address of the object (or any of its members) will not escape.
     *  Trivially copyable types are location-invariant and users can
     *  specialize this trait for other types.
     */
    template<typename _Tp>
    struct _T_AMFN_is_location_invariant : std::is_trivially_copyable<_Tp>::type {
    };

    template<typename _From, typename _To>
    using _T_AMFN_check_func_return_type = std::__or_<std::is_void<_To>, std::is_same<_From, _To>/*, std::is_convertible<_From, _To>*/ >;

    union _T_AMFN_nocopy_types {
        void *_M_object;
        const void *_M_const_object;
        char _M_data[_AMFUNCTION_DEFAULT_DATA_SIZE];
    };

	template<typename _T, typename _U> constexpr size_t _T_AMFN_offset_of(_U _T::*member)
	{
    	return (char*)&((_T*)nullptr->*member) - (char*)nullptr;
	}

    class _T_AMFN_any_data {
    public:
        _T_AMFN_any_data() {}

        void *_M_access() { return &_M_data._M_data[0]; }

        const void *_M_access() const { return &_M_data._M_data[0]; }

        void *_M_access_ptr() { return _M_data._M_object; }

        const void *_M_access_ptr() const { return _M_data._M_object; }

        void _M_set_access_ptr(void *ptr) { _M_data._M_object = ptr; }

        template<typename _Tp>
        _Tp &_M_get_functor_ptr() { return *static_cast<_Tp *>(_M_data._M_object); }

        template<typename _Tp>
        const _Tp &_M_get_functor_ptr() const { return *static_cast<const _Tp *>(_M_data._M_object); }

        template<typename _Tp>
        _Tp &_M_get_functor() { return *static_cast<_Tp *>(static_cast<void *>(&_M_data._M_data[0])); }

        template<typename _Tp>
        const _Tp &
        _M_get_functor() const { return *static_cast<const _Tp *>(static_cast<const void *>(&_M_data._M_data[0])); }

        _T_AMFN_nocopy_types _M_data;
    };

    template<typename _Signature>
    class _T_AMFN_kernel_base;

	template<typename _Signature>
	class _T_AMFN_generic_functor;

	template<typename _Res, typename ... _ArgTypes>
    class _T_AMFN_generic_functor<_Res(_ArgTypes...)> {
		_Res operator ()(_ArgTypes ...);
	};

    template<typename _Res, typename ... _ArgTypes>
    class _T_AMFN_kernel_base<_Res(_ArgTypes...)> {
    protected:
        _T_AMFN_any_data _M_functor;

		typedef _Res (_T_AMFN_generic_functor<_Res(_ArgTypes...)>::*_T_AMFN_invoker_member_type)(_ArgTypes ...) const;

		void* _M_invoke_ptr;

        typedef const std::type_info *(*_T_AMFN_kernel_op_ptr)(_T_AMFN_kernel_base *_kernel_ptr, const _T_AMFN_any_data &_src,
                                                          _E_AMFN_manager_operation _op);

        _T_AMFN_kernel_op_ptr _M_kernel_op_ptr;

    public:
        _T_AMFN_kernel_base(_T_AMFN_kernel_op_ptr _kernel_op_ptr) noexcept:
                _M_functor(),
                _M_invoke_ptr(nullptr),
                _M_kernel_op_ptr(_kernel_op_ptr) {};

        _T_AMFN_kernel_base() noexcept:
                _M_functor(),
                _M_invoke_ptr(nullptr),
                _M_kernel_op_ptr(nullptr) {};

        _T_AMFN_kernel_base(const _T_AMFN_kernel_base &_other) :
                _M_functor(),
                _M_invoke_ptr(_other._M_invoke_ptr),
                _M_kernel_op_ptr(_other._M_kernel_op_ptr) {
                    if (_M_kernel_op_ptr) {
                        _M_kernel_op_ptr(this, _other._M_functor, _clone_functor);
                    }
        }

        _T_AMFN_kernel_base(_T_AMFN_kernel_base &&_other) :
                _M_functor(),
                _M_invoke_ptr(_other._M_invoke_ptr),
                _M_kernel_op_ptr(_other._M_kernel_op_ptr) {
                    if (_M_kernel_op_ptr) {
                        _M_kernel_op_ptr(this, _other._M_functor, _clone_functor_move);
                    }
                    _other._M_destroy();
        }

        ~_T_AMFN_kernel_base() {
            if (_M_kernel_op_ptr) {
                _M_kernel_op_ptr(this, _M_functor, _destroy_functor);
            }
        }

		_T_AMFN_invoker_member_type _M_get_invoker() const
		{
			union {
				_T_AMFN_invoker_member_type _M_invoker_member;
				void* _M_void_ptr_array[2];
			} u;
			u._M_void_ptr_array[0] = _M_invoke_ptr;
			u._M_void_ptr_array[1] = nullptr;
			return u._M_invoker_member;
		}

        _T_AMFN_kernel_base &operator=(const _T_AMFN_kernel_base &_other) {
            if (_M_kernel_op_ptr) {
                _M_kernel_op_ptr(this, _M_functor, _destroy_functor);
            }
            _M_invoke_ptr = _other._M_invoke_ptr;
            _M_kernel_op_ptr = _other._M_kernel_op_ptr;
            if (_M_kernel_op_ptr) {
                _M_kernel_op_ptr(this, _other._M_functor, _clone_functor);
            }
            return *this;
        }

        const _T_AMFN_any_data &_M_get_functor() const { return _M_functor; }


        void _M_destroy() {
            if (_M_kernel_op_ptr) {
                _M_kernel_op_ptr(this, _M_functor, _destroy_functor);
            }
            _M_invoke_ptr = nullptr;
            _M_kernel_op_ptr = nullptr;
        }

        bool _M_empty() const noexcept {
            return _M_invoke_ptr == nullptr;
        }

#if __cpp_rtti

        const std::type_info &_M_get_type_info() const noexcept {
            if (_M_empty()) {
                return typeid(void);
            }
            _T_AMFN_kernel_base *nonconstthis = const_cast<_T_AMFN_kernel_base *>(this);
            return *_M_kernel_op_ptr(nonconstthis, _M_functor, _get_type_info);
        }

#endif
    };

    template<typename _Signature, typename _Functor>
    class _T_AMFN_kernel;

    template<typename _Signature, typename _Functor>
    class _AMFunction_handler;

    template<typename _Res, typename _Functor, typename ... _ArgTypes>
    class _T_AMFN_kernel<_Res(_ArgTypes ...), _Functor>
            : public _T_AMFN_kernel_base<_Res(_ArgTypes...)> {

        static const std::size_t _M_max_size = sizeof(_T_AMFN_nocopy_types);
        static const std::size_t _M_max_align = __alignof__(_T_AMFN_nocopy_types);
            public:
        static const bool _C_stored_locally =
               (_T_AMFN_is_location_invariant<_Functor>::value
                 && (sizeof(_Functor) <= _M_max_size)
                 && (__alignof__(_Functor) <= _M_max_align)
                 && (_M_max_align % __alignof__(_Functor) == 0))
                 && (_AMFUNCTION_DEFAULT_DATA_SIZE > 0);
        typedef std::integral_constant<bool, _C_stored_locally> _Local_storage;
        typedef _T_AMFN_kernel_base<_Res(_ArgTypes...)> _T_Base;

        void _M_clone(const _T_AMFN_any_data &_functor, std::true_type) {
            _Functor *vp = &_T_Base::_M_functor.template _M_get_functor<_Functor>();
            ::new(vp) _Functor(_functor.template _M_get_functor<_Functor>());
        }

        void _M_clone(const _T_AMFN_any_data &_functor, std::false_type) {
            void *rv;
            rv = static_cast<void *>(new _Functor(_functor.template _M_get_functor_ptr<_Functor>()));
            _T_Base::_M_functor._M_set_access_ptr(rv);
        }

        void _M_move(const _T_AMFN_any_data &_functor, std::true_type) {
            _Functor *vp = &_T_Base::_M_functor.template _M_get_functor<_Functor>();
            ::new(vp) _Functor(std::move(_functor.template _M_get_functor<_Functor>()));
        }

        void _M_move(const _T_AMFN_any_data &_functor, std::false_type) {
            void *rv;
            rv = static_cast<void *>(new _Functor(std::move(_functor.template _M_get_functor_ptr<_Functor>())));
            _T_Base::_M_functor._M_set_access_ptr(rv);
        }

        void _M_init(const _Functor &_source, std::true_type) {
            _Functor *vp = &_T_Base::_M_functor.template _M_get_functor<_Functor>();
            ::new(vp) _Functor(_source);
        }

        void _M_init(const _Functor &_source, std::false_type) {
            void *rv;
            rv = static_cast<void *>(new _Functor(_source));
            _T_Base::_M_functor._M_set_access_ptr(rv);
        }

        void _M_destroy(std::true_type) {
            _T_Base::_M_functor.template _M_get_functor<_Functor>().~_Functor();
        }

        void _M_destroy(std::false_type) {
            delete &_T_Base::_M_functor.template _M_get_functor_ptr<_Functor>();
        }

        static const std::type_info *
        _M_manage_op(_T_AMFN_kernel_base<_Res(_ArgTypes ...)> *_me, const _T_AMFN_any_data &_functor,
                     _E_AMFN_manager_operation _op) {
            switch (_op) {
                case _destroy_functor:
                    static_cast<_T_AMFN_kernel *>(_me)->_M_destroy(_Local_storage());
                    break;
                case _clone_functor:
                    static_cast<_T_AMFN_kernel *>(_me)->_M_clone(_functor, _Local_storage());
                    break;
                case _clone_functor_move:
                    static_cast<_T_AMFN_kernel *>(_me)->_M_move(_functor, _Local_storage());
                    break;
#if __cpp_rtti
                case _get_type_info:
                    return &typeid(_Functor);
                    break;
#endif
            }
            return nullptr;
        }

        //typedef _Res (*_Invoker_type)(const void*, _ArgTypes ...);
		typedef _Res (_AMFunction_handler<_Res(_ArgTypes...), _Functor>::*_T_AMFN_invoker_member_type)(_ArgTypes ...);
		typedef _Res (_Functor::*_Invoker_functor_type)(_ArgTypes...) const;

		void _M_set_invoker(_Invoker_functor_type _invoker)
		{
			union {
				_Invoker_functor_type _M_invoker_member;
				void* _M_void_ptr_array[2];
			} u;
			u._M_invoker_member = _invoker;
			if (u._M_void_ptr_array[1] != nullptr) {
				throw std::bad_function_call();
			}
			_T_Base::_M_invoke_ptr = u._M_void_ptr_array[0];
		}

		void _M_set_invoker_member(_T_AMFN_invoker_member_type _invoker)
		{
			union {
				_T_AMFN_invoker_member_type _M_invoker_member;
				void* _M_void_ptr_array[2];
			} u;
			u._M_invoker_member = _invoker;
			if (u._M_void_ptr_array[1] != nullptr) {
				throw std::bad_function_call();
			}
			_T_Base::_M_invoke_ptr = u._M_void_ptr_array[0];
		}

    public:

        _T_AMFN_kernel(const _Functor &_functor, _T_AMFN_invoker_member_type invoke_ptr) :
                _T_AMFN_kernel_base<_Res(_ArgTypes...)>(
                        &_M_manage_op
                ) {
					if (_C_stored_locally) {
						_M_set_invoker(&_Functor::operator ());
					}
					else {
						_M_set_invoker_member(invoke_ptr);
					}
            _M_init(_functor, _Local_storage());
        };
    };


    template<typename _Res, typename _Functor, typename... _ArgTypes>
    class _AMFunction_handler<_Res(_ArgTypes...), _Functor>
            : public _T_AMFN_kernel<_Res(_ArgTypes...), _Functor> {
        typedef _T_AMFN_kernel<_Res(_ArgTypes...), _Functor> _T_Base;

    public:
        _AMFunction_handler(_Functor _functor) : _T_Base(_functor, &_AMFunction_handler<_Res(_ArgTypes...), _Functor>::_M_invoke) {}

        _Res _M_invoke(_ArgTypes ... __args) {
			const void* newvfunctor = _T_Base::_M_get_functor()._M_access_ptr();
			const _Functor& newfunctor = *static_cast<const _Functor*>(newvfunctor);
            return newfunctor(std::forward<_ArgTypes>(__args)...);

        }
    };

    template<typename _Functor, typename... _ArgTypes>
    class _AMFunction_handler<void(_ArgTypes...), _Functor>
            : public _T_AMFN_kernel<void(_ArgTypes...), _Functor> {
        typedef _T_AMFN_kernel<void(_ArgTypes...), _Functor> _T_Base;

    public:
        _AMFunction_handler(_Functor _functor) : _T_Base(_functor, &_AMFunction_handler<void(_ArgTypes...), _Functor>::_M_invoke) {}

        void _M_invoke(_ArgTypes ... __args) {
			const void* newvfunctor = _T_Base::_M_get_functor()._M_access_ptr();
			const _Functor& newfunctor = *static_cast<const _Functor*>(newvfunctor);
            newfunctor(std::forward<_ArgTypes>(__args)...);
        }
    };

    template<typename _Signature>
    class AMFunction;

/**
 *  @ingroup SmartProperties
 *  @brief AMSmartProperties::AMFunction is a function wrapper for smart properties.
 *
 *  Class template AMSmartProperties::AMfunction is function wrapper, basically intended for smart properties.
 *  Instances of AMSmartProperties::AMFunction can store, copy, and invoke Functor type callable target -- lambda expressions.
 *  Rather than dynamically alocation, stores functors itself. Its optimalisation for real-time operations.
 *  Class compute the most bigger memory footprint of all stored Functors. It completelly removes need of
 *  dynamic memory allocation.
 *
 *  <h3>Terminology taken from std:function</h3>
 *  The stored callable object is called the target of AMSmartProperties::AMFunction. If a AMSmartProperties::AMFunction
 *  contains no target, it is called empty. Invoking the target of an empty AMSmartProperties::AMFunction
 *  results in std::bad_function_call exception being thrown.
 *
 *  <h3>Comparison between AMSmartProperties::AMFunction and std::function</h3>
 *  <table>
 *  <tr><th>Feature</th><th>AMSmartProperties::AMFunction</th><th>std::function</th></tr>
 *  <tr><td>Object size</td><td>3&times;sizeof(void*) + local storage</td><td>4&times;sizeof(void*)</td></tr>
 *  <tr><td>Minimum local storage size</td><td>sizeof(void*)</td><td>2&times;sizeof(void*)</td></tr>
 *  <tr><td>Maximum local storage size</td><td>_data_size parameter</td><td>2&times;sizeof(void*)</td></tr>
 *  <tr><td>Invocation speed</td><td>one call</td><td>two calls</td></tr>
 *  <tr><td>Invocation memory constiption</td><td>one call stack space</td><td>two calls stack space</td></tr>
 *  <tr><td>Supports functors</td><td>YES</td><td>types</td></tr>
 *  <tr><td>Supports pointer to functions</td><td>NO</td><td>YES</td></tr>
 *  <tr><td>Supports pointers to member functions</td><td>NO</td><td>YES</td></tr>
 *  <tr><td>Supports realtime operations</td><td>YES</td><td>YES, but with restrictions</td></tr>
 *  </table>
 */
    template<typename _Res, typename ... _ArgTypes>
    class AMFunction<_Res(_ArgTypes ...)>
            : public _T_AMFN_maybe_unary_or_binary_function<_Res, _ArgTypes ...> {

        _T_AMFN_kernel_base<_Res(_ArgTypes...)> _M_kernel;

        template<typename _Func,
                typename _Res2 = typename std::result_of<_Func &(_ArgTypes ...)>::type>
        struct _Callable : _T_AMFN_check_func_return_type<_Res2, _Res> {
        };

        template<typename _Tp>
        struct _Callable<AMFunction, _Tp> : std::false_type {
        };

        template<typename _Cond, typename _Tp>
        using _Requires = typename std::enable_if<_Cond::value, _Tp>::type;


    public:

        /**
         *  @brief Default construct creates an empty AMFunction.
         *  @throw This @c function will not throw an exception.
         */
        AMFunction() noexcept: _M_kernel() {}

        /**
         *  @brief Creates an empty AMFunction.
         *  @throw This @c function will not throw an exception.
         */
        AMFunction(std::nullptr_t) noexcept: _M_kernel() {}

        /**
         *  @brief %AMFunction copy constructor.
         *  @param _other A %AMFunction object with identical call signature.
         *
         *  The newly-created %AMFunction contains a copy of the target of @a
         *  _other (if it has one).
         *
         *  @throw Constructor of target can throw an exception.
         */
        AMFunction(const AMFunction<_Res(_ArgTypes...)> &_other) : _M_kernel(_other._M_kernel) {}


        /**
         *  @brief %AMFunction move constructor.
         *  @param _other A %AMFunction object rvalue with identical call signature.
         *
         *  The newly-created %AMFunction contains the target of @a _other
         *  (if it has one).
         *
         *  @throw Constructor of target can throw an exception.
         */
        AMFunction(AMFunction<_Res(_ArgTypes...)> &&_other) : _M_kernel(std::move(_other._M_kernel)) {}

        /**
         *  @brief Builds a %AMFunction that targets to a copy of Functor object.
         *  @param _f A %AMFunction object that is callable with parameters of
         *  type @c T1, @c T2, ..., @c TN and returns a value convertible
         *  to @c Res.
         *
         *  @throw Constructor of target can throw an exception.
         */
        template<typename _Functor,
                typename = _Requires<std::__not_<std::is_same<_Functor, AMFunction> >, void>,
                typename = _Requires<_Callable<_Functor>, void> >
        AMFunction(_Functor _f);


        /**
         *  @brief %AMFunction assignment operator.
         *  @param _other A %AMFunction with identical call signature.
         *  @returns @c *this
         *
         *  The target of @a _other is copied to @c *this. If @a _other has no
         *  target, then @c *this will be empty.
         *
         *  @throw Constructor of new target of destructor of old target can throw an exception.
         */
        AMFunction &operator=(const AMFunction &_other) {
            if (!_M_kernel._M_empty()) {
                _M_kernel._M_destroy();
            }
            _M_kernel = _other._M_kernel;
            return *this;
        }

        /**
         *  @brief %AMFunction move-assignment operator.
         *  @param _other A %AMFunction rvalue with identical call signature.
         *  @returns @c *this
         *
         *  The target of @a _other is moved to @c *this. If @a _other has no
         *  target, then @c *this will be empty.
         *
         *  @throw Constructor of new target of destructor of old target can throw an exception.
         */
        AMFunction &operator=(AMFunction &&_other) {
            if (!_M_kernel._M_empty()) {
                _M_kernel._M_destroy();
            }
            _M_kernel = std::move(_other._M_kernel);
            return *this;
        }

        /**
         *  @brief %AMFunction assignment to zero.
         *  @returns @c *this
         *
         *  The target of @c *this is deallocated, leaving it empty.
         *
         *  @throw Destructor of old target can throw an exception.
         */
        AMFunction &operator=(std::nullptr_t) {
            if (!_M_kernel._M_empty()) {
                _M_kernel._M_destroy();
            }
            return *this;
        }

        /**
         *  @brief %AMFunction assignment to a new target.
         *  @param _f A %AMFunction object that is callable with parameters of
         *  type @c T1, @c T2, ..., @c TN and returns a value convertible
         *  to @c Res.
         *  @return @c *this
         *
         *  @throw Constructor of new target of destructor of old target can throw an exception.
         */

        template<typename _Functor>
        _Requires<_Callable<typename std::decay<_Functor>::type>, AMFunction &>
        operator=(_Functor &_f) {
#ifdef COLLECTING_DATA
            static _T_AMFN_size<_Functor> _functor_size;
            _functor_size._M_get_size();
#endif
            if (!_M_kernel._M_empty()) {
                _M_kernel._M_destroy();
            }
            typedef _AMFunction_handler<_Res(_ArgTypes...), _Functor> _My_handler;
            ::new(&_M_kernel) _My_handler(_f);
            return *this;
        }

        /**
         *  @brief Determine if the %AMFunction has a target.
         *
         *  @return @c true when this %AMFunction object contains a target,
         *  or @c false when it is empty.
         *
         *  @throw This @c function will not throw an exception.
         */
        explicit operator bool() const noexcept {
            return !_M_kernel._M_empty();
        }

        /**
         *  @brief Invokes the AMFunction targeted by @c *this.
         *  @returns the result of the target.
         *
         *  The AMFunction call operator invokes the target AMFunction object
         *  stored by @c this.
         *  @throws bad_function_call when @c !(bool)*this
         */
        _Res operator()(_ArgTypes ... __args) const {
            if (_M_kernel._M_empty()) {
                throw std::bad_function_call();
            }
			const _T_AMFN_generic_functor<_Res(_ArgTypes...)>& obj = _M_kernel._M_get_functor().template _M_get_functor<_T_AMFN_generic_functor<_Res(_ArgTypes...)> >();
			typedef _Res (_T_AMFN_generic_functor<_Res(_ArgTypes...)>::*_T_AMFN_invoker_member_type)(_ArgTypes ...) const;
			_T_AMFN_invoker_member_type memberfn = _M_kernel._M_get_invoker();
			_Res rv = ((obj).*(memberfn))(std::forward<_ArgTypes>(__args)...);
            return rv;
        }

#if __cpp_rtti

        /**
         *  @brief Determine the type of the target of this AMFunction object
         *  @returns the type identifier of the target AMFunction object, or
         *  @c typeid(void) if @c !(bool)*this.
         *
         *  @throw This @c function will not throw an exception.
         */
        const std::type_info &target_type() const noexcept {
            const std::type_info &_typeinfo_result = _M_kernel._M_get_type_info();
            return _typeinfo_result;
        }

#endif

        /**
         *  @brief Determines maximum functor size used in program.
         *  @returns maximum functor size
         *
         *  Determines maximum functor size used in program,
         *  respectively, only funcotors tha was passed to constructor of
         *  AMSmartProperties::AMFunction, or AMSmartProperties::AMFunction::operator=.
         *  Value is intended to next compile for real-time systems.
         *  You must pass COLLECTING_DATA macto to compiling. In case of
         *  macro not passed, returns simply -1.
         *  This @c function will not throw an %exception.
         *
         *  @throw This @c function will not throw an exception.
         */
        static unsigned long getMaximumFunctorSize() noexcept {

            unsigned long _return_value = -1;
#ifdef COLLECTING_DATA
            _return_value = _T_AMFN_size_base::_M_max;
#endif
            return _return_value;
        }
    };

    template<typename _Res, typename ... _ArgTypes>
    template<typename _Functor, typename, typename>
    AMFunction<_Res(_ArgTypes...)>::AMFunction(_Functor __f) {
#ifdef COLLECTING_DATA
        static _T_AMFN_size<_Functor> _functor_size;
        _functor_size._M_get_size();
#endif
        typedef _AMFunction_handler<_Res(_ArgTypes...), _Functor> _My_handler;
        ::new(&_M_kernel) _My_handler(__f);
    };

	/**
	 *  @ingroup SmartProperties
	 *  @brief Compares an AMFunction target with nullptr
	 *
	 *  @returns @c true if the wrapper has no target, @c false otherwise
	 *
	 *  @throw This @c function will not throw an exception.
	 */
    template<typename _Res, typename ... _Args>
    inline bool
    operator==(const AMFunction<_Res(_Args ...)> &__f, std::nullptr_t) noexcept {
        return !static_cast<bool>(__f);
    }

	/**
	 *  @ingroup SmartProperties
	 *  @brief Compares an AMFunction target with nullptr
	 *
	 *  @returns @c true if the wrapper has no target, @c false otherwise
	 *
	 *  @throw This @c function will not throw an exception.
	 */
    template<typename _Res, typename ... _Args>
    inline bool
    operator==(std::nullptr_t, const AMFunction<_Res(_Args ...)> &__f) noexcept {
        return !static_cast<bool>(__f);
    }

	/**
	 *  @ingroup SmartProperties
	 *  @brief Compares an AMFunction target with nullptr
	 *  @returns @c false if the wrapper has no target, @c true otherwise
	 *
	 *  @throw This @c function will not throw an exception.
	 */
    template<typename _Res, typename ... _Args>
    inline bool
    operator!=(const AMFunction<_Res(_Args ...)> &__f, std::nullptr_t) noexcept {
        return static_cast<bool>(__f);
    }

	/**
	 *  @ingroup SmartProperties
	 *  @brief Compares an AMFunction target with nullptr
	 *  @returns @c false if the wrapper has no target, @c true otherwise
	 *
	 *  @throw This @c function will not throw an exception.
	 */
    template<typename _Res, typename ... _Args>
    inline bool
    operator!=(std::nullptr_t, const AMFunction<_Res(_Args ...)> &__f) noexcept {
        return static_cast<bool>(__f);
    }

}//namespace AMSmartProperties
/** @} */
#endif //AMFUNCTION_H
