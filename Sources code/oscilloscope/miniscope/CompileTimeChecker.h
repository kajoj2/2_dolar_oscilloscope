/** \file
	\brief Macro to check for compile time error
	("Modern C++ Design" by A. Alexandrescu, slightly modified to comply with TC2006)

	\note Usage: STATIC_CHECK(expression, Error_Description);
	i.e. STATIC_CHECK(sizeof(From) <= sizeof(To), Destination_Type_Too_Narrow);
*/

#ifndef CompileTimeCheckerH
#define CompileTimeCheckerH

template<bool> struct CompileTimeChecker 
{ 
   CompileTimeChecker(...); 
}; 
template<> struct CompileTimeChecker<false> { }; 
#define STATIC_CHECK(expr, msg) {\
	class ERROR_##msg {};\
	(void)sizeof(CompileTimeChecker<(expr) != 0>(&ERROR_##msg()));\
	}

#endif