#ifndef QDYNAMICEVENTSDATA_H
#define QDYNAMICEVENTSDATA_H

#include <QCoreApplication>
#include <QSharedData>
#include <QObject>
#include <QList>
#include <QThread>
#include <QMutex>
#include <QMap>
#include <QRegularExpression>
#include <functional>

#define QDYNAMICEVENTSPROXY_EVENT_TYPE (QEvent::Type)(QEvent::User + 666)

class QDynamicEventsProxyObject : public QObject
{
	Q_OBJECT
public:
	explicit QDynamicEventsProxyObject();

	bool event(QEvent* ev);

};

class QDynamicEventsProxyEvent : public QEvent
{
public:
	explicit QDynamicEventsProxyEvent();

	std::function<void()> m_eventFunc;

};

// have all template classes derive from common base class which to contains the static members
class QDynamicEventsDataBase {

protected:
	static QDynamicEventsProxyObject * getObjectForThread(QThread * p_currThd);
    static QHash< QThread *, QDynamicEventsProxyObject * > s_threadMap;
	static QMutex    s_mutex;
	static qlonglong s_funcId;
};

// forward declaration to be able to make friend
template<class ...Types>
class QDynamicEventsData;
// create a handle class to be able to remove event subscription
class QDynamicEventsHandle
{
public:
	QDynamicEventsHandle(QString strEventName = QString(), QThread * p_handleThread = nullptr, qlonglong funcId = -1);
private:
	// make friend, so it can access internal methods
	template<class ...Types>
	friend class QDynamicEventsData;

	QString   m_strEventName ;
	QThread * mp_handleThread;
	qlonglong m_funcId;
};

// forward declaration to be able to pass as arg
template<class ...Types>
class QDynamicEvents;

// the actual dynamic events object implementation, Types are the callback arguments
template<class ...Types>
class QDynamicEventsData : public QSharedData, public QDynamicEventsDataBase
{
public:
	QDynamicEventsData();
	QDynamicEventsData(const QDynamicEventsData &other);
	~QDynamicEventsData();

	// consumer API

	// on method	
	QDynamicEventsHandle on(QString strEventName, std::function<void(Types(&...args))> callback, std::function<bool(Types(&...args))> filter = nullptr, Qt::ConnectionType connection = Qt::AutoConnection);
	// once method	
	QDynamicEventsHandle once(QString strEventName, std::function<void(Types(&...args))> callback, std::function<bool(Types(&...args))> filter = nullptr, Qt::ConnectionType connection = Qt::AutoConnection);
	// off method (all callbacks registered to an specific event name)
	void off(QString strEventName);
	// off method (specific callback based on handle)
	void off(QDynamicEventsHandle evtHandle);
	// off method (all callbacks)
	void off();

	// provider API

	void trigger(QDynamicEvents<Types...> ref, QString strEventName, Types(&...args));

private:
	// thread safety first
    QRecursiveMutex m_mutex;
	// list of connections to avoid memory leaks
	QList<QMetaObject::Connection> m_connectionList;
	// struct to store callback data
	struct CallbackData
	{
		std::function<void(Types(&...args))> callback;
		std::function<bool(Types(&...args))> filter;
		Qt::ConnectionType                   connection;
	};
	// map of maps of maps, multiple callbacks by:
	QMap< QString,      // by event name
		QMap< QThread *,  // by thread
			QMap< qlonglong, // an identifier for the function
				  CallbackData
				> 
			> 
		> m_callbacksMap;
	// map of maps of maps, multiple callbacks by:
	QMap< QString,      // by event name
		QMap< QThread *,  // by thread
			QMap< qlonglong, // an identifier for the function
		          CallbackData
				>
			>
		> m_callbacksMapOnce;
	// create proxy object for unknown thread
	void createProxyObj(QString &strEventName);
	// internal on
	void onInternal(QString &strEventName, qlonglong &funcId, std::function<void(Types(&...args))> callback, std::function<bool(Types(&...args))> filter = nullptr, Qt::ConnectionType connection = Qt::AutoConnection);
	// internal once
	void onceInternal(QString &strEventName, qlonglong &funcId, std::function<void(Types(&...args))> callback, std::function<bool(Types(&...args))> filter = nullptr, Qt::ConnectionType connection = Qt::AutoConnection);
	// off method (all callbacks registered to an specific event name)
	void offInternal(QString &strEventName);
	// off method (specific callback based on handle)
	void offInternal(QString &strEventName, QThread *pThread, qlonglong &funcId);
	// internal trigger
	void triggerInternal(QDynamicEvents<Types...> ref, QString &strEventName, Types(&...args));
};

template<class ...Types>
QDynamicEventsData<Types...>::QDynamicEventsData()
    : m_mutex()
{
	// nothing to do here
}

template<class ...Types>
QDynamicEventsData<Types...>::QDynamicEventsData(const QDynamicEventsData &other) : QSharedData(other),
m_mutex(other.m_mutex),
m_connectionList(other.m_connectionList),
m_callbacksMap(other.m_callbacksMap)
{
	// nothing to do here
}

template<class ...Types>
QDynamicEventsData<Types...>::~QDynamicEventsData()
{
	//QMutexLocker locker(&m_mutex);
	// remove connections
	for (int i = 0; i < m_connectionList.count(); i++)
	{
		QObject::disconnect(m_connectionList[i]);
	}
	m_callbacksMap.clear();
	m_callbacksMapOnce.clear();
}

template<class ...Types>
void QDynamicEventsData<Types...>::off(QString strEventName)
{
	QMutexLocker locker(&m_mutex);
	// split by spaces
    static QRegularExpression regExp = QRegularExpression("\\s+");
    QStringList listEventNames = strEventName.split(regExp, Qt::SkipEmptyParts);
	// for each event name
	for (int i = 0; i < listEventNames.count(); i++)
	{
		offInternal(listEventNames[i]);
	}
}

template<class ...Types>
void QDynamicEventsData<Types...>::offInternal(QString &strEventName)
{
	// remove for all threads and all callbacks
	m_callbacksMap.remove(strEventName);
	m_callbacksMapOnce.remove(strEventName);
}

template<class ...Types>
void QDynamicEventsData<Types...>::off(QDynamicEventsHandle evtHandle)
{
	QMutexLocker locker(&m_mutex);
	// split by spaces
    static QRegularExpression regExp = QRegularExpression("\\s+");
    QStringList listEventNames = evtHandle.m_strEventName.split(regExp, Qt::SkipEmptyParts);
	// for each event name
	for (int i = 0; i < listEventNames.count(); i++)
	{
		offInternal(listEventNames[i], evtHandle.mp_handleThread, evtHandle.m_funcId);
	}
}

template<class ...Types>
void QDynamicEventsData<Types...>::offInternal(QString &strEventName, QThread *pThread, qlonglong &funcId)
{
	// remove very specific callback
	m_callbacksMap[strEventName][pThread].remove(funcId);
	m_callbacksMapOnce[strEventName][pThread].remove(funcId);
}

template<class ...Types>
void QDynamicEventsData<Types...>::off()
{
	QMutexLocker locker(&m_mutex);
	// remove all callbacks
	m_callbacksMap.clear();
	m_callbacksMapOnce.clear();
}

template<class ...Types>
void QDynamicEventsData<Types...>::createProxyObj(QString &strEventName)
{
	QMutexLocker locker(&m_mutex);
	// get current thread
	QThread * p_currThd = QThread::currentThread();
	// create obj for thread if not existing, else return existing
	QDynamicEventsProxyObject * p_obj = QDynamicEventsDataBase::getObjectForThread(p_currThd);
	// split by spaces
    static QRegularExpression regExp = QRegularExpression("\\s+");
    QStringList listEventNames = strEventName.split(regExp, Qt::SkipEmptyParts);
	// for each event name
	for (int i = 0; i < listEventNames.count(); i++)
	{
		QString strCurrEvtName = listEventNames[i];
		// wait until object destroyed to remove callbacks struct
		// NOTE : need to disconnect these connections to avoid memory leaks due to lambda memory allocations
		m_connectionList.append(QObject::connect(p_obj, &QObject::destroyed, [this, strCurrEvtName, p_currThd]() {
			// delete callbacks when thread gets deleted
			this->m_callbacksMap[strCurrEvtName].remove(p_currThd);
			this->m_callbacksMapOnce[strCurrEvtName].remove(p_currThd);
		}));
	}
}

template<class ...Types>
QDynamicEventsHandle QDynamicEventsData<Types...>::on(QString strEventName, std::function<void(Types(&...args))> callback, std::function<bool(Types(&...args))> filter/* = nullptr*/, Qt::ConnectionType connection/* = Qt::AutoConnection*/)
{
	// split by spaces
    static QRegularExpression regExp = QRegularExpression("\\s+");
    QStringList listEventNames = strEventName.split(regExp, Qt::SkipEmptyParts);
	// create proxy object if necessary
	this->createProxyObj(strEventName);
	// get callback uuid
	qlonglong funcId = QDynamicEventsDataBase::s_funcId++;
	// lock after
	QMutexLocker locker(&m_mutex);
	// for each event name
	for (int i = 0; i < listEventNames.count(); i++)
	{
		onInternal(listEventNames[i], funcId, callback, filter, connection);
	}
	// return hash
	return QDynamicEventsHandle(strEventName, QThread::currentThread(), funcId);
}

template<class ...Types>
void QDynamicEventsData<Types...>::onInternal(QString &strEventName, qlonglong &funcId, std::function<void(Types(&...args))> callback, std::function<bool(Types(&...args))> filter/* = nullptr*/, Qt::ConnectionType connection/* = Qt::AutoConnection*/)
{
	// [NOTE] No lock in internal methods
	m_callbacksMap[strEventName][QThread::currentThread()][funcId].callback   = callback  ;
	m_callbacksMap[strEventName][QThread::currentThread()][funcId].filter     = filter    ;
	m_callbacksMap[strEventName][QThread::currentThread()][funcId].connection = connection;
}

template<class ...Types>
QDynamicEventsHandle QDynamicEventsData<Types...>::once(QString strEventName, std::function<void(Types(&...args))> callback, std::function<bool(Types(&...args))> filter/* = nullptr*/, Qt::ConnectionType connection/* = Qt::AutoConnection*/)
{
	// split by spaces
    static QRegularExpression regExp = QRegularExpression("\\s+");
    QStringList listEventNames = strEventName.split(regExp, Qt::SkipEmptyParts);
	// create proxy object if necessary
	this->createProxyObj(strEventName);
	// get callback uuid
	qlonglong funcId = QDynamicEventsDataBase::s_funcId++;
	// lock after
	QMutexLocker locker(&m_mutex);
	// for each event name
	for (int i = 0; i < listEventNames.count(); i++)
	{
		onceInternal(listEventNames[i], funcId, callback, filter, connection);
	}
	// return hash
	return QDynamicEventsHandle(strEventName, QThread::currentThread(), funcId);
}

template<class ...Types>
void QDynamicEventsData<Types...>::onceInternal(QString &strEventName, qlonglong &funcId, std::function<void(Types(&...args))> callback, std::function<bool(Types(&...args))> filter/* = nullptr*/, Qt::ConnectionType connection/* = Qt::AutoConnection*/)
{
	// [NOTE] No lock in internal methods
	m_callbacksMapOnce[strEventName][QThread::currentThread()][funcId].callback   = callback  ;
	m_callbacksMapOnce[strEventName][QThread::currentThread()][funcId].filter     = filter    ;
	m_callbacksMapOnce[strEventName][QThread::currentThread()][funcId].connection = connection;
}

template<class ...Types>
void QDynamicEventsData<Types...>::trigger(QDynamicEvents<Types...> ref, QString strEventName, Types(&...args))
{
	QMutexLocker locker(&m_mutex);	
	// split by spaces
    static QRegularExpression regExp = QRegularExpression("\\s+");
    QStringList listEventNames = strEventName.split(regExp, Qt::SkipEmptyParts);
	// for each event name
	for (int i = 0; i < listEventNames.count(); i++)
	{
		triggerInternal(ref, listEventNames[i], args...);
	}
}

template<class ...Types>
void QDynamicEventsData<Types...>::triggerInternal(QDynamicEvents<Types...> ref, QString &strEventName, Types(&...args))
{
	// [NOTE] No lock in internal methods

	// on method callbacks *********************************************************
	// for each thread where there are callbacks to be called
	auto listThreads = m_callbacksMap[strEventName].keys();
	for (int i = 0; i < listThreads.count(); i++)
	{
		auto p_currThread      = listThreads.at(i);
		auto p_currObject      = QDynamicEventsDataBase::getObjectForThread(p_currThread);
		auto &mapOnlyCallbacks = this->m_callbacksMap[strEventName][p_currThread];
		// loop all callbacks for current thread
		auto listHandles = mapOnlyCallbacks.keys();
		for (int j = 0; j < listHandles.count(); j++)
		{
			auto currHandle        = listHandles.at(j);
			auto &currCallbackData = mapOnlyCallbacks[currHandle];
			// skip filtered
			if (currCallbackData.filter && !currCallbackData.filter(args...))
			{
				continue;
			}
			// invoke according to connection type
			if (currCallbackData.connection == Qt::DirectConnection || (currCallbackData.connection == Qt::AutoConnection && p_currThread == QThread::currentThread()))
			{
				// call directly
				currCallbackData.callback(args...);
			} 
			else if (currCallbackData.connection == Qt::QueuedConnection || (currCallbackData.connection == Qt::AutoConnection && p_currThread != QThread::currentThread()))
			{
				// get copy of callback to be executed in thread
				auto &currCallback = currCallbackData.callback;
				// create object in heap and assign function (event loop takes ownership and deletes it later)
				QDynamicEventsProxyEvent * p_Evt = new QDynamicEventsProxyEvent;
				// NOTE need to pass currCallback as copy because if an off() gets execd before event loop resumes, callbacks will not be called
				p_Evt->m_eventFunc = [ref, currCallback, args...]() mutable {
					currCallback(args...);
					// unused, but we need it to keep at least one reference until all callbacks are executed
					Q_UNUSED(ref)
				};
				// post event for object with correct thread affinity
				QCoreApplication::postEvent(p_currObject, p_Evt);
			}
			else
			{
				Q_ASSERT_X(false, "QDynamicEventsData<Types...>::triggerInternal", "Unsupported connection type.");
			}
		} // for j
	} // for i

	// once method callbacks *********************************************************
	// for each thread where there are callbacks to be called
	auto listThreadsOnce = m_callbacksMapOnce[strEventName].keys();
	for (int i = 0; i < listThreadsOnce.count(); i++)
	{
		auto p_currThread         = listThreadsOnce.at(i);
		auto p_currObject         = QDynamicEventsDataBase::getObjectForThread(p_currThread);
		auto mapOnlyCallbacksOnce = this->m_callbacksMapOnce[strEventName].take(p_currThread);
		// filter callbacks
		auto listHandles = mapOnlyCallbacksOnce.keys();
		for (int j = 0; j < listHandles.count(); j++)
		{
			auto currHandle            = listHandles.at(j);
			auto &currCallbackDataOnce = mapOnlyCallbacksOnce[currHandle];
			// skip filtered
			if (currCallbackDataOnce.filter && !currCallbackDataOnce.filter(args...))
			{
				continue;
			}
			// invoke according to connection type
			if (currCallbackDataOnce.connection == Qt::DirectConnection || (currCallbackDataOnce.connection == Qt::AutoConnection && p_currThread == QThread::currentThread()))
			{
				// call directly
				currCallbackDataOnce.callback(args...);
			}
			else if (currCallbackDataOnce.connection == Qt::QueuedConnection || (currCallbackDataOnce.connection == Qt::AutoConnection && p_currThread != QThread::currentThread()))
			{
				// get copy of callback to be executed in thread
				auto &currCallbackOnce = currCallbackDataOnce.callback;
				// create object in heap and assign function (event loop takes ownership and deletes it later)
				QDynamicEventsProxyEvent * p_Evt = new QDynamicEventsProxyEvent;
				// NOTE below the difference is the 'take' method which ensures callbacks are only execd once
				p_Evt->m_eventFunc = [ref, currCallbackOnce, args...]() mutable {
					currCallbackOnce(args...);
					// unused, but we need it to keep at least one reference until all callbacks are executed
					Q_UNUSED(ref)
				};
				// post event for object with correct thread affinity
				QCoreApplication::postEvent(p_currObject, p_Evt);
			}
			else
			{
				Q_ASSERT_X(false, "QDynamicEventsData<Types...>::triggerInternal", "Unsupported connection type.");
			}
		} // for j
	} // for i
}

#endif // QDYNAMICEVENTSDATA_H
