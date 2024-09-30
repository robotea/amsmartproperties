/**
 * @defgroup SmartProperties Smart properties
 * @brief Easy way to make reposnsivity of properties.
 *
 * Lets imagine, that ou have nad coord of some graphical item. For this example, it represents
 * coords of button on the screen. Lets assign lambda function to this coord rather than float number.
 * Now, you are able to signe simple formulat such as "parent width / 2 + 300px". Your view with button
 * is now responsive.
 *
 * <h3>Grouping</h3>
 * Smart properties are grouped into separate groups-pools represented by AMSmartProperties::AMPropertyPool class.
 * It reduces CPU compution cost, because you typically need recompute only one group. CPU cost may
 * grow quadratically in some case of usage. You may imagine properties as tree of SmartProperties properties with
 * root window rect.
 * Typically, this group is owned by one window.
 *
 * <h3>Multi thread</h3>
 * Since Smart properties itself are not thread safe, Using pools allow to use in multithreaded environment.
 * If every thread has its own separated polls, it may coexist simlutaneously and safe.
 *
 * <h3>Nesting</h3>
 * Smart properties are designed also for nesting. For example, You shlould have smart property of type
 * rect, which consists from smart properties origin and size, and origin consists of x,y and size
 * consists from smart properties width, height. Subproperty has only reference to real type T,
 * because generating any data strucures on type T is unwanted. So, there is two classes in this case.
 * One is very simple type T, frees from any data structures related to Smart properties, and isolated
 * class with definition of Smart subpropertes.
 *
 * <H3>Smart properties lifecycle</h3>
 *  When smart properties are set up. are a reset state, i.e. the first reading of the Smart property
 * causes them to be executed. After the execution, the calculated value is placed into the variable.
 * Each subsequent read then only reads this variable. There's one exception. If you save a value
 * directly to the properties, it is copied to the variable and no calculation is should be carried out.
 * Now the stored value is always read. And that's until you change the smart property. Smart property
 * signals for the entire pool(AMSmartProperties::AMProperyPool) status modified. Smart properties on which Smart
 * property depends can no longer be relied on, so the smart property must be recalculated and saved
 * when read. Recomputing all variables on which the property may depend, it does not seem to make sense,
 * so the calculation is calculated after each and for any reading from smart properties within the pool.
 * It is now possible to perform a reset operation on the pool that sets up all smart properties in the
 * pool stored. Life cycle returns to the beginning. It is advisable to perform the reset operation once
 * per a certain time, preferably in the main thread loop.
 *
 * <h3>Debugging smart properties</h3>
 * Also, smart properties has debugging capabilities. You can trace computing path after exception,
 * give a text names to pointers (addresses of properties) and every property has function to_string.
 * More at @ref Debug.
 *
 * <h3>Tutorial</h3>
 * Look at tutorial @ref SmartPropertiesTutorial.
 */
