#include "EventHandler.h"

//better use boost::ptr_map instead of std::map
EventHandler::~EventHandler()
{
	Handlers::iterator it = mHandlers.begin();
	while(it != mHandlers.end())
	{
		delete it->second;
		++it;
	}
	mHandlers.clear();
}

void EventHandler::handleEvent(const Event* event)
{
	Handlers::iterator it = mHandlers.find(TypeInfo(typeid(*event)));
	if(it != mHandlers.end())
	{
		it->second->exec(event);
	}
}