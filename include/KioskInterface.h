#pragma once
#include <string>
#include <memory>
#include "KioskState.h"
#include "PricingStrategy.h"
#include "InventoryProxy.h"
#include "KioskFactory.h"
#include "Memento.h"

struct PurchaseResult {
    bool success;
    std::string message;
};

// PATTERN: Facade
class KioskInterface {
public:
    KioskInterface();
    
    PurchaseResult purchaseItem(const std::string& userId, const std::string& productId, int qty);
    bool refundTransaction(const std::string& txnId);
    bool runDiagnostics();
    bool restockInventory(const std::string& productId, int qty);
    void switchMode(const std::string& mode);

    std::shared_ptr<KioskState> getState() const { return currentState; }
    void setState(std::shared_ptr<KioskState> newState) { currentState = newState; }
    
    std::shared_ptr<PricingStrategy> getPricingStrategy() const { return currentPricing; }
    void setPricingStrategy(std::shared_ptr<PricingStrategy> newStrategy) { currentPricing = newStrategy; }

    SecureInventoryProxy* getInventory() { return &inventory; }
    TransactionCaretaker* getCaretaker() { return &caretaker; }
    
    std::string getStatus();

    std::unique_ptr<Dispenser> dispenser;
    std::unique_ptr<Verifier> verifier;

private:
    std::shared_ptr<KioskState> currentState;
    std::shared_ptr<PricingStrategy> currentPricing;
    SecureInventoryProxy inventory;
    TransactionCaretaker caretaker;
};
