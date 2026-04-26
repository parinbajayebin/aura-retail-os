#include "CentralRegistry.h"
#include "EventBus.h"

CentralRegistry& CentralRegistry::getInstance() {
    static CentralRegistry instance;
    return instance;
}

void CentralRegistry::loadConfig() {
    std::lock_guard<std::mutex> lock(registryMtx);
    config = CSVManager::getInstance().readConfig();
}

std::string CentralRegistry::getKioskId() {
    std::lock_guard<std::mutex> lock(registryMtx);
    return config.kiosk_id;
}

std::string CentralRegistry::getKioskType() {
    std::lock_guard<std::mutex> lock(registryMtx);
    return config.kiosk_type;
}

std::string CentralRegistry::getMode() {
    std::lock_guard<std::mutex> lock(registryMtx);
    return config.mode;
}

void CentralRegistry::setMode(const std::string& mode) {
    std::lock_guard<std::mutex> lock(registryMtx);
    config.mode = mode;
    CSVManager::getInstance().writeConfig(config);
}

bool CentralRegistry::hasHardwareModule(const std::string& module) {
    std::lock_guard<std::mutex> lock(registryMtx);
    if(module == "none" || module == "") return true;
    for(const auto& m : config.hardware_modules) {
        if(m == module) return true;
    }
    return false;
}

std::vector<std::string> CentralRegistry::getHardwareModules() {
    std::lock_guard<std::mutex> lock(registryMtx);
    return config.hardware_modules;
}
