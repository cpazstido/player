#ifndef __VXCOMMON_H__
#define __VXCOMMON_H__

#include <set>
#include <map>
#include <list>
#include <deque>
#include <queue>
#include <stack>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cassert>

#ifdef _WIN32
#include <hash_map>
#else // _WIN32
#include <ext/hash_map>
#endif // _WIN32

#ifdef _WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

///////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#define SELECTANY   __declspec(selectany)
#else // _WIN32
#define SELECTANY   __attribute__((weak))
#endif // _WIN32

#ifdef _WIN32

///////////////////////////////////////////////////////////////////////////
// vxSRWLock

class vxSRWLock
{
	// constructor/destructor
public:
	vxSRWLock(void)
		: m_lreadcount(0)
	{
		InitializeCriticalSection(&m_cs_reader);
		InitializeCriticalSection(&m_cs_writer);

		m_hevt_enable_write = CreateEvent(NULL, TRUE, TRUE, NULL);
	}

	~vxSRWLock(void)
	{
#ifdef _DEBUG
		WaitForSingleObject(m_hevt_enable_write, INFINITE);
#endif // _DEBUG
		CloseHandle(m_hevt_enable_write);

		DeleteCriticalSection(&m_cs_reader);
		DeleteCriticalSection(&m_cs_writer);
	}

	// public interfaces
public:
	inline int RdLock(void)
	{
		EnterCriticalSection(&m_cs_writer);
		EnterCriticalSection(&m_cs_reader);
		if (1 == InterlockedIncrement(&m_lreadcount))
			ResetEvent(m_hevt_enable_write);
		LeaveCriticalSection(&m_cs_reader);
		LeaveCriticalSection(&m_cs_writer);

		return 0;
	}

	inline int TryRdLock(void)
	{
		if (!TryEnterCriticalSection(&m_cs_writer))
			return FALSE;
		EnterCriticalSection(&m_cs_reader);
		if (1 == InterlockedIncrement(&m_lreadcount))
			ResetEvent(m_hevt_enable_write);
		LeaveCriticalSection(&m_cs_reader);
		LeaveCriticalSection(&m_cs_writer);

		return TRUE;
	}

	inline int WrLock(void)
	{
		EnterCriticalSection(&m_cs_writer);
		WaitForSingleObject(m_hevt_enable_write, INFINITE);

		return 0;
	}

	inline int TryWrLock(void)
	{
		if (!TryEnterCriticalSection(&m_cs_writer))
			return FALSE;
		if (WAIT_TIMEOUT == WaitForSingleObject(m_hevt_enable_write, 0))
			return FALSE;
		return TRUE;
	}

	inline int UnLock(void)
	{
		EnterCriticalSection(&m_cs_reader);

		if (0 == InterlockedExchange(&m_lreadcount, m_lreadcount))
			LeaveCriticalSection(&m_cs_writer);
		else if (0 == InterlockedDecrement(&m_lreadcount))
			SetEvent(m_hevt_enable_write);

		LeaveCriticalSection(&m_cs_reader);

		return 0;
	}

	// class data
private:
	CRITICAL_SECTION   m_cs_reader;
	volatile LONG      m_lreadcount;

	CRITICAL_SECTION   m_cs_writer;
	HANDLE             m_hevt_enable_write;

};

///////////////////////////////////////////////////////////////////////////
// vxSimpleLock

class vxSimpleLock
{
	// constructor/destructor
public:
	vxSimpleLock(void)
	{
		InitializeCriticalSection(&m_cs_lock);
	}

	~vxSimpleLock(void)
	{
#ifdef _DEBUG
		EnterCriticalSection(&m_cs_lock);
		LeaveCriticalSection(&m_cs_lock);
#endif // _DEBUG

		DeleteCriticalSection(&m_cs_lock);
	}

	// public interfaces
public:
	inline void Lock(void)
	{
		EnterCriticalSection(&m_cs_lock);
	}

	inline void UnLock(void)
	{
		LeaveCriticalSection(&m_cs_lock);
	}

	// class data
protected:
	CRITICAL_SECTION   m_cs_lock;

};

///////////////////////////////////////////////////////////////////////////
// vxSemaphore

class vxSemaphore
{
	// constructor/destructor
public:
	vxSemaphore(LONG lInitialCount, LONG lMaximumCount)
	{
		m_hSemaphore = CreateSemaphore(NULL, lInitialCount, lMaximumCount, NULL);
		assert(NULL != m_hSemaphore);
	}

	~vxSemaphore(void)
	{
		if (NULL != m_hSemaphore)
		{
			CloseHandle(m_hSemaphore);
			m_hSemaphore = NULL;
		}
	}

	// public interfaces
public:
	void Wait(void)
	{
		if (NULL == m_hSemaphore)
		{
			return;
		}

		DWORD dwWait = WaitForSingleObject(m_hSemaphore, INFINITE);
	}

	void Release(void)
	{
		if (NULL == m_hSemaphore)
		{
			return;
		}

		LONG lPreviousCount = 0;
		bool bRelease = ReleaseSemaphore(m_hSemaphore, 1, &lPreviousCount);
	}

	// class data
protected:
	HANDLE          m_hSemaphore;

};

///////////////////////////////////////////////////////////////////////////

#else // _WIN32

///////////////////////////////////////////////////////////////////////////
// vxSRWLock

class vxSRWLock
{
	// constructor/destructor
public:
	vxSRWLock(void)
	{
		int iRet = pthread_rwlock_init(&m_rwlock, NULL);
		assert(0 == iRet);
	}

	~vxSRWLock(void)
	{
		int iRet = pthread_rwlock_destroy(&m_rwlock);
		assert(0 == iRet);
	}

	// operator methods
public:
	inline int RdLock(void)    { return pthread_rwlock_rdlock(&m_rwlock); }
	inline int TryRdLock(void) { return pthread_rwlock_tryrdlock(&m_rwlock); }

	inline int WrLock(void)    { return pthread_rwlock_wrlock(&m_rwlock); }
	inline int TryWrLock(void) { return pthread_rwlock_trywrlock(&m_rwlock); }

	inline int UnLock(void)    { return pthread_rwlock_unlock(&m_rwlock); }

	// class data
protected:
	pthread_rwlock_t  m_rwlock;

};

///////////////////////////////////////////////////////////////////////////
// vxSimpleLock

class vxSimpleLock
{
	// constructor/destructor
public:
	vxSimpleLock(void)
	{
		int iRet = pthread_rwlock_init(&m_rwlock, NULL);
		assert(0 == iRet);
	}

	~vxSimpleLock(void)
	{
		int iRet = pthread_rwlock_destroy(&m_rwlock);
		assert(0 == iRet);
	}

	// public interfaces
public:
	inline void Lock(void)
	{
		pthread_rwlock_wrlock(&m_rwlock);
	}

	inline void UnLock(void)
	{
		pthread_rwlock_unlock(&m_rwlock);
	}

	// class data
protected:
	pthread_rwlock_t  m_rwlock;

};


#endif // _WIN32

///////////////////////////////////////////////////////////////////////////
// SyncAutoLock

template< class _Lock = vxSRWLock, bool bWrite = true, bool bLock = true >
class SyncAutoLockT
{
	// constructor/destructor
public:
	SyncAutoLockT(_Lock *ptSynObj)
		: m_ptSynObj(ptSynObj)
	{
		assert(NULL != m_ptSynObj);
		if (bLock)
		{
			if (bWrite)
				m_ptSynObj->WrLock();
			else
				m_ptSynObj->RdLock();
		}
	}

	~SyncAutoLockT(void)
	{
		if (bLock)
		{
			m_ptSynObj->UnLock();
		}
	}

	// class data
protected:
	_Lock      * m_ptSynObj;
};

class vxAutoLock
{
	// constructor/destructor
public:
	vxAutoLock(vxSimpleLock * ptlock)
	{
		m_ptlock = ptlock;
		m_ptlock->Lock();
	}

	~vxAutoLock(void)
	{
		m_ptlock->UnLock();
	}

	// class data
protected:
	vxSimpleLock * m_ptlock;
};

///////////////////////////////////////////////////////////////////////////

typedef SyncAutoLockT< vxSRWLock, false >  vxAutoReadLock;
typedef SyncAutoLockT< vxSRWLock, true >   vxAutoWriteLock;
typedef SyncAutoLockT< vxSRWLock, true >   vxAutoLockEx;

///////////////////////////////////////////////////////////////////////////
// vxVector

template< class _Ty, class _Lock = vxSRWLock, class _Ax = std::allocator< _Ty > >
class vxVector : public std::vector< _Ty, _Ax >, public _Lock
{
	// constructor/destructor
public:
	vxVector(void)  {  }
	~vxVector(void) {  }
};

///////////////////////////////////////////////////////////////////////////
// vxDeque

template< class _Ty, class _Lock = vxSRWLock, class _Ax = std::allocator< _Ty > >
class vxDeque : public std::deque< _Ty, _Ax >, public _Lock
{
	// constructor/destructor
public:
	vxDeque(void)  {  }
	~vxDeque(void) {  }
};

///////////////////////////////////////////////////////////////////////////
// vxList

template< class _Ty, class _Lock = vxSRWLock, class _Ax = std::allocator< _Ty > >
class vxList : public std::list< _Ty, _Ax >, public _Lock
{
	// constructor/destructor
public:
	vxList(void)  {  }
	~vxList(void) {  }
};

///////////////////////////////////////////////////////////////////////////
// vxQueue

template< class _Ty, class _Lock = vxSRWLock, class _Container = std::deque< _Ty > >
class vxQueue : public std::queue< _Ty, _Container >, public _Lock
{
	// constructor/destructor
public:
	vxQueue(void)  {  }
	~vxQueue(void) {  }
};

///////////////////////////////////////////////////////////////////////////
// vxStack

template< class _Ty, class _Lock = vxSRWLock, class _Container = std::deque< _Ty > >
class vxStack : public std::stack< _Ty, _Container >, public _Lock
{
	// constructor/destructor
public:
	vxStack(void)  {  }
	~vxStack(void) {  }
};

///////////////////////////////////////////////////////////////////////////
// vxSet

template< class _Kty, class _Lock = vxSRWLock, class _Pr = std::less< _Kty >, class _Alloc = std::allocator< _Kty > >
class vxSet : public std::set< _Kty, _Pr, _Alloc >, public _Lock
{
	// constructor/destructor
public:
	vxSet(void)  {  }
	~vxSet(void) {  }
};

///////////////////////////////////////////////////////////////////////////
// vxMap

template< class _Kty, class _Ty, class _Lock = vxSRWLock, class _Pr = std::less< _Kty >, class _Alloc = std::allocator< std::pair< const _Kty, _Ty > > >
class vxMap : public std::map< _Kty, _Ty, _Pr, _Alloc >, public _Lock
{
	// constructor/destructor
public:
	vxMap(void)  {  }
	~vxMap(void) {  }
};

///////////////////////////////////////////////////////////////////////////
// vxMultimap

template< class _Kty, class _Ty, class _Lock = vxSRWLock, class _Pr = std::less< _Kty >, class _Alloc = std::allocator< std::pair< const _Kty, _Ty > > >
class vxMultimap : public std::multimap< _Kty, _Ty, _Pr, _Alloc >, public _Lock
{
	// constructor/destructor
public:
	vxMultimap(void)  {  }
	~vxMultimap(void) {  }
};

///////////////////////////////////////////////////////////////////////////
// vxHash_map

template<class _Kty, class _Ty, class _Lock = vxSRWLock, class _Tr = hash_compare< _Kty, std::less< _Kty > >, class _Alloc = std::allocator< std::pair< const _Kty, _Ty > > >
class vxHash_map : public stdext::hash_map< _Kty, _Ty, _Tr, _Alloc >, public _Lock
{
	// constructor/destructor
public:
	vxHash_map(void)  {  }
	~vxHash_map(void) {  }
};

///////////////////////////////////////////////////////////////////////////
// vxCircularQueue

template< class _Ty >
class vxCircularQueue
{
	// const value define
public:
	typedef enum ConstValueID
	{
		DEF_CIRQUEUE_SIZE = 10,
	} ConstValueID;

	// constructor/destructor
public:
	vxCircularQueue(size_t st_size = DEF_CIRQUEUE_SIZE)
		: m_st_front_pos(0)
		, m_st_size(0)
	{
		assert(st_size > 0);
		m_vec_items.resize(st_size);
	}

	~vxCircularQueue(void)
	{
		m_vec_items.clear();
	}

	// public interfaces
public:
	inline size_t capacity_size(void) const
	{
		return m_vec_items.size();
	}

	inline size_t size(void) const
	{
		return m_st_size;
	}

	inline bool empty(void) const
	{
		return (0 == m_st_size);
	}

	inline bool full(void) const
	{
		return (size() == capacity_size());
	}

	bool front(_Ty& _Val)
	{
		if (empty())
		{
			return false;
		}
		_Val = m_vec_items[m_st_front_pos];
		return true;
	}

	bool pop_front(void)
	{
		if (empty())
		{
			return false;
		}
		m_st_front_pos = (m_st_front_pos + 1) % capacity_size();
		--m_st_size;
		return true;
	}

	bool push_back(const _Ty& _Val)
	{
		if (full())
		{
			return false;
		}
		m_vec_items[(m_st_front_pos + m_st_size)  % capacity_size()] = _Val;
		m_st_size = (m_st_size + 1) % (capacity_size() + 1);
		return true;
	}

	void resize(size_t st_new_size)
	{
		assert(st_new_size > 0);
		m_st_front_pos = m_st_size = 0;
		m_vec_items.resize(st_new_size);
	}

	void clear(void)
	{
		m_st_front_pos = 0;
		m_st_size      = 0;

		size_t st_vec_size = m_vec_items.size();
		m_vec_items.clear();
		m_vec_items.resize(st_vec_size);
	}

	const _Ty& operator [] (size_t st_index) const
	{
		assert((st_index >= 0) && (st_index < size()));
		size_t st_pos = (m_st_front_pos + st_index) % capacity_size();
		return m_vec_items[st_pos];
	}

	_Ty& operator [] (size_t st_index)
	{
		assert((st_index >= 0) && (st_index < size()));
		size_t st_pos = (m_st_front_pos + st_index) % capacity_size();
		return m_vec_items[st_pos];
	}

	_Ty& operator = (const typename vxCircularQueue< _Ty > & obj)
	{
		if (this != obj)
		{
			m_vec_items    = obj.m_vec_items;
			m_st_front_pos = obj.m_st_front_pos;
			m_st_size      = obj.m_st_size;
		}
		return this;
	}

	// class data
protected:
	size_t                       m_st_front_pos;
	size_t                       m_st_size;
	typename std::vector< _Ty >  m_vec_items;

};

///////////////////////////////////////////////////////////////////////////
// vxPCQueue : producer - consumer queue

template< class _Ty, const size_t _DefMaxSemaphoreCount = 100 >
class vxPCQueue
{
	// constructor/destructor
public:
	vxPCQueue(size_t st_max_semaphore = _DefMaxSemaphoreCount)
		: m_semaphore_in(st_max_semaphore, st_max_semaphore)
		, m_semaphore_out(0, st_max_semaphore)
	{

	}

	~vxPCQueue(void)
	{
		clear();
	}

	// public interfaces
public:
	void push(const _Ty &_Val)
	{
		m_semaphore_in.Wait();

		m_queue.Lock();
		m_queue.push_back(_Val);
		m_queue.UnLock();

		m_semaphore_out.Release();
	}

	bool pop(_Ty &_Val)
	{
		bool bVal = false;

		m_semaphore_out.Wait();

		m_queue.Lock();
		if (m_queue.size() > 0)
		{
			_Val = m_queue.front();
			m_queue.pop_front();
			bVal = true;
		}
		m_queue.UnLock();

		m_semaphore_in.Release();

		return bVal;
	}

	size_t size(void)
	{
		m_queue.Lock();
		size_t st_size = m_queue.size();
		m_queue.UnLock();

		return st_size;
	}

	void clear(void)
	{
		m_queue.Lock();
		m_queue.clear();
		m_queue.UnLock();
	}

	// class data
protected:
	vxSemaphore                           m_semaphore_in;
	vxSemaphore                           m_semaphore_out;
	typename vxDeque< _Ty, vxSimpleLock > m_queue;

};

///////////////////////////////////////////////////////////////////////////
// vxObjectPool

template < class _Ty >
class ObjectAlloctor
{
public:
	static _Ty * alloc(void * pv_param)
	{
		return new _Ty();
	}

	static void initial(_Ty * _obj_ptr, void * pv_param)
	{

	}

	static void recyc(_Ty *_obj_ptr)
	{

	}

	static void destroy(_Ty *& _obj_ptr)
	{
		delete _obj_ptr; _obj_ptr = NULL;
	}
};

template< class _Ty, class _ObjAllocator = ObjectAlloctor< _Ty > >
class vxObjectPool
{
	// constructor/destructor
public:
	vxObjectPool(void)
	{

	}

	~vxObjectPool(void)
	{
		clear();
	}

	// class property
public:

	// public interfaces
public:
	_Ty * alloc(void * pv_param)
	{
		vxAutoLockEx autoLock(&m_sync_lock);

		_Ty * _obj_ptr = NULL;

		if (m_lst_recyc.empty())
		{
			_obj_ptr = _ObjAllocator::alloc(pv_param);
			if (NULL != _obj_ptr)
			{
				m_lst_alloc.push_back(_obj_ptr);
			}
		}
		else
		{
			_obj_ptr = m_lst_recyc.front();
			m_lst_recyc.pop_front();

			_ObjAllocator::initial(_obj_ptr, pv_param);
		}

		return _obj_ptr;
	}

	void recyc(_Ty *_obj_ptr)
	{
		if (NULL != _obj_ptr)
		{
			vxAutoLockEx autoLock(&m_sync_lock);
			m_lst_recyc.push_back(_obj_ptr);

			_ObjAllocator::recyc(_obj_ptr);
		}
	}

	size_t count(void)
	{
		vxAutoLockEx autoLock(&m_sync_lock);
		return (m_lst_alloc.size() - m_lst_recyc.size());
	}

	size_t size(void)
	{
		vxAutoLockEx autoLock(&m_sync_lock);
		return m_lst_alloc.size();
	}

	void clear(void)
	{
		vxAutoLockEx autoLock(&m_sync_lock);

		std::list< _Ty * >::iterator itlst = m_lst_alloc.begin();
		for (; itlst != m_lst_alloc.end(); ++itlst)
		{
			_ObjAllocator::destroy(*itlst);
		}
		m_lst_alloc.clear();
		m_lst_recyc.clear();
	}

	// class data
protected:
	vxSRWLock                    m_sync_lock;
	typename std::list< _Ty * >  m_lst_alloc;
	typename std::list< _Ty * >  m_lst_recyc;

};

//////////////////////////////////////////////////////////////////////////
// vxSington class

typedef void (*SIDestroy)(void);

class SingtonMangr
{
	// static methods
public:
	static void RegInstanceDestroy(SIDestroy sid)
	{
		s_setSID.insert(sid);
	}

	static void ReleaseSingtonInstance(void)
	{
		vxSet< SIDestroy >::iterator itset = s_setSID.begin();
		for (; itset != s_setSID.end(); ++itset)
		{
			(*itset)();
		}
	}

protected:
	static vxSet< SIDestroy > s_setSID;
};

SELECTANY vxSet< SIDestroy > SingtonMangr::s_setSID;


template< class _Ty, class _Lock = vxSimpleLock >
class vxSington
{
	// constructor/destructor
protected:
	vxSington()          {  }
	virtual ~vxSington() {  }

public:
	static _Ty& Instance(void)
	{
		if (NULL == s_instance)
		{
			s_lock.Lock();
			if (NULL == s_instance)
			{
				s_instance = new _Ty();
				SingtonMangr::RegInstanceDestroy(vxSington< _Ty >::Destroy);
			}
			s_lock.UnLock();
		}
		return *s_instance;
	}

private:
	static void Destroy(void)
	{
		if (s_instance != NULL)
		{
			delete s_instance;
			s_instance = NULL;
		}
	}

	static _Lock          s_lock;
	static _Ty * volatile s_instance;
};

template< class _Ty, class _Lock > \
SELECTANY _Lock vxSington< _Ty, _Lock >::s_lock;

template< class _Ty, class _Lock > \
SELECTANY _Ty * volatile vxSington< _Ty, _Lock >::s_instance = NULL;


///////////////////////////////////////////////////////////////////////////

#endif //__VXCOMMON_H__
