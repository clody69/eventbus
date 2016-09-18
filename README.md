# EventBus

This project implements a simple event bus to allow the inter-class communication between c++ modules. It allows a publish-subscribe communication without the components knowing about each other and this allows a higher level of decoupling between the modules. It's based on the exchange of synchronous events and event handlers. Custom events can be created as well. 

The ```EventBus``` doesn't specify how to use it. It can be used as a singleton or there can be seprate instances for different topics. 

The resolution of the event types happens via ```static_cast``` instead of a  ```dynamic_cast```. This allows to leverage the C++ type checking and it's producing safer code (although more verbose). More information in this [article](http://www.gamedev.net/page/resources/_/technical/game-programming/effective-event-handling-in-c-r2459).


# Usage

## Creating a custom event

Creating a custom event only requires to derive from the base ```Event``` class and add the custom fields. 
```
class LocationEvent : public Event {
public:
    LocationEvent(double inLatitude, double inLongitude) : mLatitude(inLatitude), mLongitude(inLongitude) {}

    double getLatitutde() const {return mLatitude; }
    double getLongitude() const {return mLongitude; }

private:
    double mLatitude;
    double mLongitude;
};
```

## Creating an event handler

An object that is interested to listen to a particular event must define an event handler. This is achieved by inheriting from the class ```EventHandler```, adding the functions for handling the event and registering the handlers in the constructor. An object can listen to any number of events by just adding more handlers.

```
class Track : public EventHandler {
public:
    Track() 
    {
        registerEventFunc(this, &Track::onLocationEvent);
        ...
    }
    void onLocationEvent(const LocationEvent* e)
    {
    ...
    }
};
```

## Creating an event bus

An event bus can be created by instantating the class ```EventBus```. Several buses can cohexist at the same time. For instance you can have a bus for a specifc topic or content (e.g. tweets, actions, gps events). 

```
    EventBus gps;
    EventBus actions;
```

## Subscribing and unsubscribing from the bus
To receive events an object must subscribe to the bus. To stop receiving events it can unsubscribe.

```
    Track track;
    
    gps.subscribe(&track);
```

## Firing an event
To post a new event to the bus you have to create the event and post it to the bus. All subscribers will receive the event.

```
    gps.post( new LocationEvent(20,20));
    gps.post( new LocationEvent(30,30));
```



