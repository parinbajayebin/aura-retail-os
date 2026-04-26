#pragma once
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <memory>

// PATTERN: Observer
class EventSubscriber {
public:
    virtual ~EventSubscriber() = default;
    virtual void onEvent(const std::string& type, const std::string& message) = 0;
};

class EventBus {
public:
    static EventBus& getInstance();
    
    void subscribe(const std::string& type, std::shared_ptr<EventSubscriber> subscriber);
    void publish(const std::string& type, const std::string& message, const std::string& severity = "INFO");

private:
    EventBus() = default;
    std::map<std::string, std::vector<std::shared_ptr<EventSubscriber>>> subscribers;
    std::mutex mtx;
};
