#include "EventBus.h"
#include "CSVManager.h"
#include <iostream>

EventBus& EventBus::getInstance() {
    static EventBus instance;
    return instance;
}

void EventBus::subscribe(const std::string& type, std::shared_ptr<EventSubscriber> subscriber) {
    std::lock_guard<std::mutex> lock(mtx);
    subscribers[type].push_back(subscriber);
}

void EventBus::publish(const std::string& type, const std::string& message, const std::string& severity) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        CSVManager::getInstance().logEvent(type, severity, message);
        std::cout << "[EVENT] " << type << ": " << message << "\n";
        if(subscribers.find(type) != subscribers.end()) {
            for(auto& sub : subscribers[type]) {
                sub->onEvent(type, message);
            }
        }
    }
}
