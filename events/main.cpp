#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

#include "EventHandler.h"
#include "EventBus.h"

class JSONEvent : public Event {
public:
    JSONEvent() { }
    
    virtual std::string ToJson(void) const { return "JSON String"; }
    virtual void FromJson(const std::string& inJson) { }
};

class GPSEvent : public JSONEvent {
public:
    GPSEvent(double inLatitude, double inLongitude) : mLatitude(inLatitude), mLongitude(inLongitude) {}

    std::string ToJson(void) const override {return "GPS string"; }
    void FromJson(const std::string& inJson) override {}

    double getLatitutde() const {return mLatitude; }
    double getLongitude() const {return mLongitude; }

private:
    double mLatitude;
    double mLongitude;
};

class Command : public Event {
public:
    int id;
    int source;
}

class TrackCommand : public Event {
public:
    enum Action {
        eStart = 0,
        eStop
    };

    TrackCommand(Action a): mAction(a) {}

    Action mAction;
};

class Track : public EventHandler {
public:
    Track() : mTracking(false)
    {
        registerEventFunc(this, &Track::onGPSEvent);
        registerEventFunc(this, &Track::onTrackCommand);
    }
    void onGPSEvent(const GPSEvent* e)
    {
        if(mTracking)
            std::cout << "Track point at " << e->getLatitutde() << "," << e->getLongitude() << std::endl;
    }
    void onTrackCommand(const TrackCommand* a )
    {
        mTracking = (a->mAction == TrackCommand::eStart);
    }
    
private:
    
    bool mTracking;

};

class RouteCommand : public Event {
public:
    enum Action {
        eStartNavigation = 0,
        eStopNavigation
    };
    
    RouteCommand(Action a): mAction(a) {}
    
    Action mAction;
};


class Route : public EventHandler {
public:
    Route() : mNavigating(false)
    {
        registerEventFunc(this, &Route::onGPSEvent);
        registerEventFunc(this, &Route::onRouteCommand);
    }
    void onGPSEvent(const GPSEvent* e)
    {
        if(mNavigating)
            std::cout << "Navigating at " << e->getLatitutde() << "," << e->getLongitude() << std::endl;
    }
    void onRouteCommand(const RouteCommand* a )
    {
        mNavigating = (a->mAction == RouteCommand::eStartNavigation);
    }
    
private:
    
    bool mNavigating;
    
};

//Global bus for gps and actions
EventBus gps;
EventBus actions;


//Threads for sending events
void send_gps_events(){
    while(true) {
        gps.post(new GPSEvent(10,10));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void send_track_action(){
    while(true) {
        actions.post( new TrackCommand(TrackCommand::eStart));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        actions.post( new TrackCommand(TrackCommand::eStop));
    }
}

void send_route_action(){
    while(true) {
        actions.post( new RouteCommand(RouteCommand::eStartNavigation));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        actions.post( new RouteCommand(RouteCommand::eStopNavigation));
    }
}



int main()
{


    std::cout << "\n=== Event Bus ===" << std::endl;
    
    std::thread t1(send_gps_events);
    std::thread t2(send_gps_events);
    std::thread t3(send_gps_events);

    std::thread t4(send_track_action);
    std::thread t5(send_track_action);
    std::thread t6(send_route_action);

    Track track1;
    Track track2;
    Route route;
    
    gps.subscribe(&track1);
    gps.subscribe(&track2);
    gps.subscribe(&route);
    actions.subscribe(&track1);
    actions.subscribe(&track2);
    actions.subscribe(&route);

    gps.post( new GPSEvent(10,10));
    gps.post( new GPSEvent(15,15));
    gps.post( new GPSEvent(20,20));

    actions.post( new TrackCommand(TrackCommand::eStart));

    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    gps.post( new GPSEvent(30,30));
    gps.post( new GPSEvent(45,45));
    gps.post( new GPSEvent(50,50));

    actions.post( new TrackCommand(TrackCommand::eStop));
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    gps.post( new GPSEvent(60,60));
    actions.post( new RouteCommand(RouteCommand::eStartNavigation));
    gps.post( new GPSEvent(70,70));
    gps.post( new GPSEvent(85,85));
    actions.post( new RouteCommand(RouteCommand::eStopNavigation));
    gps.post( new GPSEvent(90,90));

    std::terminate();
}


