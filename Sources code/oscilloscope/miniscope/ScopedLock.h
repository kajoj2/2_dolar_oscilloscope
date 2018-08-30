/** \file
    \brief Scoped lock
*/

#ifndef ScopedLockH
#define ScopedLockH

/** \brief Scoped lock template
    \note Usage: ScopedLock<Mutex> lock(mutex);
*/
template <class M> class ScopedLock
{
public:
	ScopedLock(M& mutex): m_mutex(mutex) {
		m_mutex.lock();
	}
	~ScopedLock(void) {
		m_mutex.unlock();
	}
private:
	M& m_mutex;
	ScopedLock();
	ScopedLock(const ScopedLock&);
	ScopedLock& operator = (const ScopedLock&);
};

#endif
