#include "InventoryProxy.h"
#include "CentralRegistry.h"
#include "EventBus.h"
#include <iostream>

// RealInventory methods
int RealInventory::getStock(const std::string& productId) {
    std::lock_guard<std::mutex> lock(mtx);
    auto inv = CSVManager::getInstance().readInventory();
    for(const auto& p : inv) {
        if(p.product_id == productId) return p.stock;
    }
    return 0;
}

int RealInventory::getAvailableStock(const std::string& productId) {
    std::lock_guard<std::mutex> lock(mtx);
    auto inv = CSVManager::getInstance().readInventory();
    for(const auto& p : inv) {
        if(p.product_id == productId) {
            // Hardware dependency constraint
            if(!CentralRegistry::getInstance().hasHardwareModule(p.hardware_required)) {
                return 0; // Marked unavailable
            }
            return p.stock - p.reserved;
        }
    }
    return 0;
}

bool RealInventory::reserveItem(const std::string& productId, int qty) {
    std::lock_guard<std::mutex> lock(mtx);
    auto inv = CSVManager::getInstance().readInventory();
    for(auto& p : inv) {
        if(p.product_id == productId) {
            if(!CentralRegistry::getInstance().hasHardwareModule(p.hardware_required)) return false;
            if(p.stock - p.reserved >= qty) {
                p.reserved += qty;
                CSVManager::getInstance().writeInventory(inv);
                if(p.stock - p.reserved < 5) {
                    EventBus::getInstance().publish("LOW_STOCK", "Product " + p.name + " is running low.", "WARNING");
                }
                return true;
            }
            return false;
        }
    }
    return false;
}

void RealInventory::updateStock(const std::string& productId, int qtyChange, int reservedChange) {
    std::lock_guard<std::mutex> lock(mtx);
    auto inv = CSVManager::getInstance().readInventory();
    for(auto& p : inv) {
        if(p.product_id == productId) {
            p.stock += qtyChange;
            p.reserved += reservedChange;
            CSVManager::getInstance().writeInventory(inv);
            break;
        }
    }
}

std::vector<ProductRecord> RealInventory::getAllProducts() {
    std::lock_guard<std::mutex> lock(mtx);
    return CSVManager::getInstance().readInventory();
}

ProductRecord RealInventory::getProduct(const std::string& productId) {
    std::lock_guard<std::mutex> lock(mtx);
    auto inv = CSVManager::getInstance().readInventory();
    for(const auto& p : inv) {
        if(p.product_id == productId) return p;
    }
    return ProductRecord{};
}

// SecureInventoryProxy methods
SecureInventoryProxy::SecureInventoryProxy() {}

int SecureInventoryProxy::getStock(const std::string& productId) {
    return realInventory.getStock(productId);
}

int SecureInventoryProxy::getAvailableStock(const std::string& productId) {
    return realInventory.getAvailableStock(productId);
}

bool SecureInventoryProxy::reserveItem(const std::string& productId, int qty) {
    std::string mode = CentralRegistry::getInstance().getMode();
    if(mode == "MAINTENANCE") {
        std::cout << "[Security] Reservation blocked in MAINTENANCE mode.\n";
        return false;
    }
    bool success = realInventory.reserveItem(productId, qty);
    CSVManager::getInstance().logEvent("INVENTORY_ACCESS", "INFO", "Reserved " + std::to_string(qty) + " of " + productId);
    return success;
}

void SecureInventoryProxy::updateStock(const std::string& productId, int qtyChange, int reservedChange) {
    realInventory.updateStock(productId, qtyChange, reservedChange);
    CSVManager::getInstance().logEvent("INVENTORY_ACCESS", "INFO", "Updated stock for " + productId);
}

std::vector<ProductRecord> SecureInventoryProxy::getAllProducts() {
    return realInventory.getAllProducts();
}

ProductRecord SecureInventoryProxy::getProduct(const std::string& productId) {
    return realInventory.getProduct(productId);
}
