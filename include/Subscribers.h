#pragma once
#include "EventBus.h"
#include <iostream>

class MaintenanceService : public EventSubscriber {
public:
    void onEvent(const std::string& type, const std::string& message) override {
        if(type == "HARDWARE_FAILURE") {
            std::cout << "[MaintenanceService] Dispatching technician for: " << message << "\n";
        }
    }
};

class SupplyChainSystem : public EventSubscriber {
public:
    void onEvent(const std::string& type, const std::string& message) override {
        if(type == "LOW_STOCK") {
            std::cout << "[SupplyChainSystem] Initiating restock protocol for: " << message << "\n";
        }
    }
};

class CityMonitoringCenter : public EventSubscriber {
public:
    void onEvent(const std::string& type, const std::string& message) override {
        if(type == "EMERGENCY_ACTIVATED" || type == "HARDWARE_FAILURE") {
            std::cout << "[CityMonitoringCenter] CRITICAL ALERT RECEIVED: " << message << "\n";
        }
    }
};
