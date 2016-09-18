#pragma once

#include <map>
#include "TypeInfo.h"

// Event Handler
// Based on this article: http://www.gamedev.net/page/resources/_/technical/game-programming/effective-event-handling-in-c-r2459
//

class Event
{
protected:
	virtual ~Event() {};
};

class HandlerFunctionBase
{
public:
	virtual ~HandlerFunctionBase() {};
	void exec(const Event* event) {call(event);}

private:
	virtual void call(const Event*) = 0;
};


template <class T, class EventT>
class MemberFunctionHandler : public HandlerFunctionBase
{
public:
	typedef void (T::*MemberFunc)(EventT*);
	MemberFunctionHandler(T* instance, MemberFunc memFn) : mInstance(instance), mFunction(memFn) {};

    
	void call(const Event* event)
	{
		(mInstance->*mFunction)(static_cast<EventT*>(event));
    }

private:
	T* mInstance;
	MemberFunc mFunction;
};


class EventHandler
{
public:
	~EventHandler();
	void handleEvent(const Event*);

	template <class T, class EventT>
	void registerEventFunc(T*, void (T::*memFn)(EventT*));

private:
	typedef std::map<TypeInfo, HandlerFunctionBase*> Handlers;
	Handlers mHandlers;
};


template <class T, class EventT>
void EventHandler::registerEventFunc(T* obj, void (T::*memFn)(EventT*))
{
	mHandlers[TypeInfo(typeid(EventT))]= new MemberFunctionHandler<T, EventT>(obj, memFn);
}
