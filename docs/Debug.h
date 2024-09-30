/**
 * @defgroup Debug Smart properties debug
 * @brief Debug possibilities for smart properties.
 *
 * In smart properties, when some error occured, is quite hard to assign address of excpetion or
 * smart property object and it's real variable name in debugger.
 *
 * <h3>Variable naming</h3>
 * Smart help is naming properies. It works very simple, everywhere, when out constructs a smart property,
 * add as parameter any name. On opposite side, then you want retrieve a name you my call AMVariableNames::getVariableName().
 * Of cource, this functionality can be switched off in release. AMVariableNames will store and looking into any string
 * at runtime. Default state is disabled. State is typically switched on on application init procedure.
 * Let's say, that this works for all variables, not only for smart properties.
 *
 * <h3>Method to_string</h3>
 * Every propery has a function to_string. AS expected, this function return std::string with dump of property.
 * However, since base class AMPropertyBase is not virtual, calling to_string via this base pointer reutuns very
 * brief informations.
 *
 * <h3>Tracing</h3>
 * There are small amout of types of errors, that may ocuured during computation of smart properties.
 * Computation can throws exceptions AMPropertyNestingException in case of computing depth is too big,
 * AMPropertyLoopException, when copunting loop is detected or AMPropertyDeletedException, when you try
 * to use deleted smart property (smart property assigned to AMProperyPool::defaultPool()).
 * There is a chain of properties involve in excpetion. Entering smart property, which was computation
 * initiated, last property in chain, which exception caused, and whole chain are available by calling
 * simple methods of every exception.
 */
