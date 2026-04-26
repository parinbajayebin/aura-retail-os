#include "Command.h"
#include "KioskInterface.h"
#include "CSVManager.h"
#include "EventBus.h"
#include <iostream>
#include <ctime>

PurchaseItemCommand::PurchaseItemCommand(KioskInterface* kiosk, const std::string& userId, const std::string& productId, int qty)
    : kiosk(kiosk), userId(userId), productId(productId), qty(qty), success(false) {}

bool PurchaseItemCommand::execute() {
    auto inv = kiosk->getInventory()->getAllProducts();
    kiosk->getCaretaker()->saveState(TransactionMemento(inv));

    if (!kiosk->verifier->verify(userId)) {
        EventBus::getInstance().publish("TRANSACTION_FAILED", "Verification failed for user " + userId, "WARNING");
        undo();
        return false;
    }

    auto product = kiosk->getInventory()->getProduct(productId);
    if(product.product_id.empty()) {
        undo();
        return false;
    }

    if (!kiosk->getInventory()->reserveItem(productId, qty)) {
        EventBus::getInstance().publish("TRANSACTION_FAILED", "Could not reserve item " + productId, "WARNING");
        undo();
        return false;
    }

    double finalPrice = kiosk->getPricingStrategy()->computePrice(product.price, qty);
    
    if (!kiosk->dispenser->dispense(productId)) {
        EventBus::getInstance().publish("HARDWARE_FAILURE", "Dispenser failed for " + productId, "CRITICAL");
        undo();
        return false;
    }
    
    TransactionRecord txn;
    txn.txn_id = "TXN-" + std::to_string(std::time(nullptr));
    txn.product_id = productId;
    txn.user_id = userId;
    txn.qty = qty;
    txn.final_price = finalPrice;
    txn.status = "SUCCESS";
    txn.timestamp = std::to_string(std::time(nullptr));

    CSVManager::getInstance().writeTransaction(txn);
    kiosk->getInventory()->updateStock(productId, -qty, -qty);

    success = true;
    log();
    return true;
}

void PurchaseItemCommand::undo() {
    if(kiosk->getCaretaker()->hasState()) {
        TransactionMemento m = kiosk->getCaretaker()->restoreState();
        CSVManager::getInstance().writeInventory(m.getInventoryState());
        std::cout << "[Memento] Transaction state rolled back.\n";
    }
}

void PurchaseItemCommand::log() {
    if(success) {
        CSVManager::getInstance().logEvent("PURCHASE", "INFO", "User " + userId + " purchased " + productId);
    }
}

RestockCommand::RestockCommand(KioskInterface* kiosk, const std::string& productId, int qty)
    : kiosk(kiosk), productId(productId), qty(qty), success(false) {}

bool RestockCommand::execute() {
    kiosk->getInventory()->updateStock(productId, qty, 0);
    success = true;
    log();
    return true;
}

void RestockCommand::undo() {
    if(success) {
        kiosk->getInventory()->updateStock(productId, -qty, 0);
    }
}

void RestockCommand::log() {
    if(success) {
        CSVManager::getInstance().logEvent("RESTOCK", "INFO", "Restocked " + std::to_string(qty) + " of " + productId);
    }
}

void CommandInvoker::executeCommand(std::shared_ptr<Command> command) {
    if(command->execute()) {
        history.push_back(command);
    }
}

void CommandInvoker::undoLastCommand() {
    if(!history.empty()) {
        history.back()->undo();
        history.pop_back();
    }
}
