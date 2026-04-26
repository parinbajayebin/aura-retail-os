#include "KioskState.h"
#include "KioskInterface.h"
#include "Command.h"
#include <iostream>

bool ActiveState::handlePurchase(KioskInterface* kiosk, const std::string& userId, const std::string& productId, int qty) {
    PurchaseItemCommand cmd(kiosk, userId, productId, qty);
    return cmd.execute();
}

bool ActiveState::handleRestock(KioskInterface* kiosk, const std::string& productId, int qty) {
    RestockCommand cmd(kiosk, productId, qty);
    return cmd.execute();
}

bool MaintenanceState::handlePurchase(KioskInterface* kiosk, const std::string& userId, const std::string& productId, int qty) {
    std::cout << "Cannot purchase in MAINTENANCE state.\n";
    return false;
}

bool MaintenanceState::handleRestock(KioskInterface* kiosk, const std::string& productId, int qty) {
    RestockCommand cmd(kiosk, productId, qty);
    return cmd.execute();
}

bool PowerSavingState::handlePurchase(KioskInterface* kiosk, const std::string& userId, const std::string& productId, int qty) {
    std::cout << "Waking up from POWER_SAVING mode...\n";
    kiosk->switchMode("ACTIVE");
    return kiosk->purchaseItem(userId, productId, qty).success;

}

bool PowerSavingState::handleRestock(KioskInterface* kiosk, const std::string& productId, int qty) {
    std::cout << "Waking up from POWER_SAVING mode...\n";
    kiosk->switchMode("ACTIVE");
    return kiosk->restockInventory(productId, qty);
}

bool EmergencyLockdownState::handlePurchase(KioskInterface* kiosk, const std::string& userId, const std::string& productId, int qty) {
    if (qty > 1 || userPurchases[userId] + qty > 1) {
        std::cout << "Emergency constraint: Max 1 item per user.\n";
        return false;
    }
    PurchaseItemCommand cmd(kiosk, userId, productId, qty);
    if (cmd.execute()) {
        userPurchases[userId] += qty;
        return true;
    }
    return false;
}

bool EmergencyLockdownState::handleRestock(KioskInterface* kiosk, const std::string& productId, int qty) {
    RestockCommand cmd(kiosk, productId, qty);
    return cmd.execute();
}
