#pragma once

#include <list>
#include <algorithm> // std::find
#include <utility>   // std::forward
#include <assert.h>
#include <mutex>
/*
 * Notifier is an implementation of Observer patter.
 * It has add/remove listeners method and notify method.
 
 Usage example:
 1. define listener interface
 
 class MyListener
 {
 public:
 void onMyEvent(int x, std::string y) = 0;
 };
 
 2. define "Subject" class and inherit it from CNotifier or aggregate it
 
 class MySubjectInherit : public CNotifier<MyListener>
 {
 };
 
 struct MySubject
 {
 SafeNotifier<MyListener> myCNotifier;
 };
 
 3. subscribe on events
 
 MySubject subject;
 MyListenerImpl object;
 
 subject.myCNotifier.addListener(&object);
 
 4. notify (Notitifcation use method pointer to deliver event)
 
 subject.myCNotifier.notify(&MyListener::onMyEvent, 5, "hello");
 
 WARNING: there are 2 implemetations:
 - CNotifier - implementation for single-thread usage
 - SafeCNotifier - for multithread usage
 */



namespace App {
    namespace _impl
    {
        struct DummyMutex
        {
            inline void lock() {}
            inline void unlock() {}
        };
        
        template <class Listener, class Mutex>
        class CNotifier
        {
        public:
            void addListener(Listener* listener);
            void removeListener(Listener* listener);
            void removeAllListeners();
            
            template <typename ... Argv, typename ...Arg2s>
            void notify(void (Listener::*method)(Argv...), Arg2s&&...argv);
            
        private:
            using ListenersList = std::list<Listener*>;
            enum class DelayedAction
            {
                none,
                removeCurrent,
                removeAll
            };
            
        private:
            ListenersList mListeners;
            typename ListenersList::iterator mCurrent = mListeners.end();
            DelayedAction mAction = DelayedAction::none;
            Mutex mMutex;
        };
        
        template <class Listener, class Mutex>
        void CNotifier<Listener, Mutex>::addListener(Listener* listener)
        {
            assert(listener);
            if (!listener)
                return;
            
            std::lock_guard<Mutex> lock(mMutex);
            assert(std::find(mListeners.begin(), mListeners.end(), listener) == mListeners.end());
            
            mListeners.push_back(listener);
        }
        
        template <class Listener, class Mutex>
        void CNotifier<Listener, Mutex>::removeListener(Listener* listener)
        {
            assert(listener);
            
            std::lock_guard<Mutex> lock(mMutex);
            
            auto it = std::find(mListeners.begin(), mListeners.end(), listener);
            if (it == mListeners.end())
                return;
            
            if (mCurrent != it)
            {
                mListeners.erase(it);
            }
            else
            {
                mAction = DelayedAction::removeCurrent;
            }
        }
        
        template <class Listener, class Mutex>
        void CNotifier<Listener, Mutex>::removeAllListeners()
        {
            std::lock_guard<Mutex> lock(mMutex);
            
            if (mCurrent != mListeners.end())
            {
                mAction = DelayedAction::removeAll;
            }
            else
            {
                mListeners.clear();
                mAction = DelayedAction::none;
            }
        }
        
        template <class Listener, class Mutex>
        template <typename ... Argv, typename ...Arg2s>
        void CNotifier<Listener, Mutex>::notify(void (Listener::*method)(Argv...), Arg2s&&...args)
        {
            std::lock_guard<Mutex> lock(mMutex);
            assert(mCurrent == mListeners.end());
            
            if (mCurrent != mListeners.end())
                return;
            
            for (mCurrent = mListeners.begin(); mCurrent != mListeners.end();)
            {
                auto listener = *mCurrent;
                (listener->*method)(std::forward<Arg2s>(args)...);
                
                switch (mAction)
                {
                    case DelayedAction::none:
                        ++mCurrent;
                        break;
                        
                    case DelayedAction::removeCurrent:
                        mCurrent = mListeners.erase(mCurrent);
                        mAction = DelayedAction::none;
                        break;
                        
                    case DelayedAction::removeAll:
                        mListeners.clear();
                        mAction = DelayedAction::none;
                        break;
                }
            }
        }
    }
    
    template<class Listener>
    using Notifier = _impl::CNotifier<Listener, _impl::DummyMutex>;
    
    template<class Listener>
    using SafeNotifier = _impl::CNotifier<Listener, std::recursive_mutex>;
}
