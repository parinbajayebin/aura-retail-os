#pragma once
#include <string>
#include <vector>
#include <mutex>
#include "CSVManager.h"

// PATTERN: Singleton
class CentralRegistry {
public:
    static CentralRegistry& getInstance();
    
    void loadConfig();
    
    std::string getKioskId();
    std::string getKioskType();
    std::string getMode();
    void setMode(const std::string& mode);
    
    bool hasHardwareModule(const std::string& module);
    std::vector<std::string> getHardwareModules();

private:
    CentralRegistry() = default;
    
    KioskConfigRecord config;
    std::mutex registryMtx;
};
