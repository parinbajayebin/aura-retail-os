#include "KioskInterface.h"
#include "CentralRegistry.h"
#include <iostream>

KioskInterface::KioskInterface() {
    CentralRegistry::getInstance().loadConfig();
    std::string type = CentralRegistry::getInstance().getKioskType();
    std::unique_ptr<KioskFactory> factory;

    if (type == "PharmacyKiosk") factory = std::make_unique<PharmacyKioskFactory>();
    else if (type == "EmergencyKiosk") factory = std::make_unique<EmergencyKioskFactory>();
    else factory = std::make_unique<FoodKioskFactory>();

    dispenser = factory->createDispenser();
    verifier = factory->createVerifier();

    switchMode(CentralRegistry::getInstance().getMode());
}

PurchaseResult KioskInterface::purchaseItem(const std::string& userId, const std::string& productId, int qty) {
    if (qty <= 0) {
        return {false, "Invalid quantity. Please enter a number greater than 0."};
    }
    
    if (getStatus() == "EMERGENCY" && qty > 1) {

        return {false, "Emergency Constraint: Maximum 1 item allowed."};
    }
    
    int avail = inventory.getAvailableStock(productId);
    if (avail < qty) {
        if (avail == 0) {
            return {false, "Out of stock! There is no quantity left to order."};
        } else {
            return {false, "Insufficient stock! Only " + std::to_string(avail) + " items left."};
        }
    }
    
    bool success = currentState->handlePurchase(this, userId, productId, qty);
    if (success) {
        return {true, "Purchase successful!"};
    } else {
        return {false, "Transaction failed due to a hardware or system error."};
    }
}


bool KioskInterface::refundTransaction(const std::string& txnId) {
    std::cout << "Refunding transaction " << txnId << "\n";
    return true;
}

bool KioskInterface::runDiagnostics() {
    std::cout << "Running diagnostics...\n";
    return true;
}

bool KioskInterface::restockInventory(const std::string& productId, int qty) {
    return currentState->handleRestock(this, productId, qty);
}

void KioskInterface::switchMode(const std::string& mode) {
    CentralRegistry::getInstance().setMode(mode);
    if (mode == "ACTIVE") {
        setState(std::make_shared<ActiveState>());
        setPricingStrategy(std::make_shared<StandardPricing>());
    } else if (mode == "MAINTENANCE") {
        setState(std::make_shared<MaintenanceState>());
        setPricingStrategy(std::make_shared<StandardPricing>());
    } else if (mode == "POWER_SAVING") {
        setState(std::make_shared<PowerSavingState>());
        setPricingStrategy(std::make_shared<StandardPricing>());
    } else if (mode == "EMERGENCY") {
        setState(std::make_shared<EmergencyLockdownState>());
        setPricingStrategy(std::make_shared<EmergencyPricing>());
    } else if (mode == "DISCOUNT") {
        setState(std::make_shared<ActiveState>());
        setPricingStrategy(std::make_shared<DiscountPricing>());
    } else {
        setState(std::make_shared<ActiveState>());
        setPricingStrategy(std::make_shared<StandardPricing>());
    }
}

std::string KioskInterface::getStatus() {
    return currentState->getName();
}
