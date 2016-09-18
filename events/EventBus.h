#pragma once
#include "CNotifier.h"

class EventBus : public App::SafeNotifier<EventHandler> {
public:
    EventBus() { }
    virtual ~EventBus() { }
    
    void post(const Event* e) { notify(&EventHandler::handleEvent, e); }
    void subscribe(EventHandler* handler) { addListener(handler); }
    void unsubscribe(EventHandler* handler) { removeListener(handler); }
};
