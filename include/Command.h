#pragma once
#include <string>
#include <memory>
#include <vector>

class KioskInterface;

// PATTERN: Command
class Command {
public:
    virtual ~Command() = default;
    virtual bool execute() = 0;
    virtual void undo() = 0;
    virtual void log() = 0;
};

class PurchaseItemCommand : public Command {
public:
    PurchaseItemCommand(KioskInterface* kiosk, const std::string& userId, const std::string& productId, int qty);
    bool execute() override;
    void undo() override;
    void log() override;
private:
    KioskInterface* kiosk;
    std::string userId;
    std::string productId;
    int qty;
    bool success;
};

class RestockCommand : public Command {
public:
    RestockCommand(KioskInterface* kiosk, const std::string& productId, int qty);
    bool execute() override;
    void undo() override;
    void log() override;
private:
    KioskInterface* kiosk;
    std::string productId;
    int qty;
    bool success;
};

class CommandInvoker {
public:
    void executeCommand(std::shared_ptr<Command> command);
    void undoLastCommand();
private:
    std::vector<std::shared_ptr<Command>> history;
};
