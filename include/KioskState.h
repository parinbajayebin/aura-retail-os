#pragma once
#include <string>
#include <memory>
#include <map>

class KioskInterface; // forward declaration

// PATTERN: State
class KioskState {
public:
    virtual ~KioskState() = default;
    virtual bool handlePurchase(KioskInterface* kiosk, const std::string& userId, const std::string& productId, int qty) = 0;
    virtual bool handleRestock(KioskInterface* kiosk, const std::string& productId, int qty) = 0;
    virtual std::string getName() const = 0;
};

class ActiveState : public KioskState {
public:
    bool handlePurchase(KioskInterface* kiosk, const std::string& userId, const std::string& productId, int qty) override;
    bool handleRestock(KioskInterface* kiosk, const std::string& productId, int qty) override;
    std::string getName() const override { return "ACTIVE"; }
};

class MaintenanceState : public KioskState {
public:
    bool handlePurchase(KioskInterface* kiosk, const std::string& userId, const std::string& productId, int qty) override;
    bool handleRestock(KioskInterface* kiosk, const std::string& productId, int qty) override;
    std::string getName() const override { return "MAINTENANCE"; }
};

class PowerSavingState : public KioskState {
public:
    bool handlePurchase(KioskInterface* kiosk, const std::string& userId, const std::string& productId, int qty) override;
    bool handleRestock(KioskInterface* kiosk, const std::string& productId, int qty) override;
    std::string getName() const override { return "POWER_SAVING"; }
};

class EmergencyLockdownState : public KioskState {
private:
    std::map<std::string, int> userPurchases;
public:
    bool handlePurchase(KioskInterface* kiosk, const std::string& userId, const std::string& productId, int qty) override;
    bool handleRestock(KioskInterface* kiosk, const std::string& productId, int qty) override;
    std::string getName() const override { return "EMERGENCY"; }
};

