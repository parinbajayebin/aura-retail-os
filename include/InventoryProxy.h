#pragma once
#include <string>
#include <vector>
#include "CSVManager.h"

class InventoryAccess {
public:
    virtual ~InventoryAccess() = default;
    virtual int getStock(const std::string& productId) = 0;
    virtual int getAvailableStock(const std::string& productId) = 0;
    virtual bool reserveItem(const std::string& productId, int qty) = 0;
    virtual void updateStock(const std::string& productId, int qtyChange, int reservedChange) = 0;
    virtual std::vector<ProductRecord> getAllProducts() = 0;
    virtual ProductRecord getProduct(const std::string& productId) = 0;
};

class RealInventory : public InventoryAccess {
public:
    int getStock(const std::string& productId) override;
    int getAvailableStock(const std::string& productId) override;
    bool reserveItem(const std::string& productId, int qty) override;
    void updateStock(const std::string& productId, int qtyChange, int reservedChange) override;
    std::vector<ProductRecord> getAllProducts() override;
    ProductRecord getProduct(const std::string& productId) override;
private:
    std::mutex mtx;
};

// PATTERN: Proxy
class SecureInventoryProxy : public InventoryAccess {
public:
    SecureInventoryProxy();
    
    int getStock(const std::string& productId) override;
    int getAvailableStock(const std::string& productId) override;
    bool reserveItem(const std::string& productId, int qty) override;
    void updateStock(const std::string& productId, int qtyChange, int reservedChange) override;
    std::vector<ProductRecord> getAllProducts() override;
    ProductRecord getProduct(const std::string& productId) override;

private:
    RealInventory realInventory;
};
