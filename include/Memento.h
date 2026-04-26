#pragma once
#include <vector>
#include "CSVManager.h"

// PATTERN: Memento
class TransactionMemento {
public:
    TransactionMemento(const std::vector<ProductRecord>& invState) : inventoryState(invState) {}
    std::vector<ProductRecord> getInventoryState() const { return inventoryState; }
private:
    std::vector<ProductRecord> inventoryState;
};

class TransactionCaretaker {
public:
    void saveState(const TransactionMemento& memento) {
        history.push_back(memento);
    }
    TransactionMemento restoreState() {
        if (!history.empty()) {
            TransactionMemento m = history.back();
            history.pop_back();
            return m;
        }
        return TransactionMemento({});
    }
    bool hasState() const { return !history.empty(); }
private:
    std::vector<TransactionMemento> history;
};
