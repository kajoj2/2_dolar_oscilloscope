/** \file
    \brief Mutex (Win32)
*/

#ifndef MutexH
#define MutexH

/** \brief Mutex primitive (Win32)
*/
class Mutex
{
public:
	Mutex () {
	    InitializeCriticalSection(&cs);
    }
	~Mutex () {
	    DeleteCriticalSection(&cs);
    }
	/** \brief Wait for ownership of the critical section object
	*/
	inline void lock () {
        EnterCriticalSection(&cs);
    }
    /** \brief Release ownershop of the critical section object
    */
	inline void unlock () {
        LeaveCriticalSection(&cs);
	}
private:
	CRITICAL_SECTION cs;
	Mutex(const Mutex&);
	Mutex& operator = (const Mutex&);
};

#endif
