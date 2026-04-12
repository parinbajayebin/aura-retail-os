#ifndef AURA_KIOSK_H
#define AURA_KIOSK_H

#include <string>
#include <iostream>
#include <map>

// ============= SINGLETON PATTERN: CentralRegistry =============
class CentralRegistry {
private:
    static CentralRegistry* instance;
    std::string pricingPolicy;
    bool emergencyMode;
    
    CentralRegistry() : pricingPolicy("Standard"), emergencyMode(false) {}
    
public:
    static CentralRegistry* getInstance() {
        if (!instance) instance = new CentralRegistry();
        return instance;
    }
    
    void setPricingPolicy(const std::string& policy) { pricingPolicy = policy; }
    std::string getPricingPolicy() const { return pricingPolicy; }
    void setEmergencyMode(bool mode) { emergencyMode = mode; }
    bool isEmergencyMode() const { return emergencyMode; }
};

// ============= STRATEGY PATTERN: Pricing Strategies =============
class PricingStrategy {
public:
    virtual double calculatePrice(double basePrice, int qty) = 0;
    virtual std::string getName() = 0;
    virtual ~PricingStrategy() = default;
};

class StandardPricing : public PricingStrategy {
public:
    double calculatePrice(double basePrice, int qty) override {
        return basePrice * qty;
    }
    std::string getName() override { return "Standard"; }
};

class DiscountedPricing : public PricingStrategy {
public:
    double calculatePrice(double basePrice, int qty) override {
        return basePrice * qty * 0.9; // 10% off
    }
    std::string getName() override { return "Discounted (10% off)"; }
};

class EmergencyPricing : public PricingStrategy {
public:
    double calculatePrice(double basePrice, int qty) override {
        return basePrice * qty * 0.5; // 50% off
    }
    std::string getName() override { return "Emergency (50% off)"; }
};

// ============= Product Class =============
class Product {
private:
    std::string id, name;
    double basePrice;
    int stock;
    
public:
    Product(std::string i, std::string n, double p, int s) 
        : id(i), name(n), basePrice(p), stock(s) {}
    
    std::string getId() const { return id; }
    std::string getName() const { return name; }
    double getBasePrice() const { return basePrice; }
    int getStock() const { return stock; }
    void setStock(int s) { stock = s; }
    bool canPurchase(int qty) const { return stock >= qty; }
};

// ============= COMMAND PATTERN: Purchase Command =============
class Command {
public:
    virtual bool execute() = 0;
    virtual std::string getLog() = 0;
    virtual ~Command() = default;
};

class PurchaseCommand : public Command {
private:
    Product* product;
    int quantity;
    PricingStrategy* strategy;
    double finalPrice;
    
public:
    PurchaseCommand(Product* p, int q, PricingStrategy* s) 
        : product(p), quantity(q), strategy(s), finalPrice(0) {}
    
    bool execute() override {
        if (!product->canPurchase(quantity)) {
            std::cout << "❌ Insufficient stock!\n";
            return false;
        }
        
        finalPrice = strategy->calculatePrice(product->getBasePrice(), quantity);
        product->setStock(product->getStock() - quantity);
        
        std::cout << "✅ Purchase successful!\n";
        std::cout << "   Item: " << product->getName() << " x" << quantity << "\n";
        std::cout << "   Pricing: " << strategy->getName() << "\n";
        std::cout << "   Total: $" << finalPrice << "\n";
        std::cout << "   Remaining stock: " << product->getStock() << "\n";
        return true;
    }
    
    std::string getLog() override {
        return "Purchased " + product->getName() + " x" + std::to_string(quantity);
    }
};

// ============= Simple Inventory =============
class Inventory {
private:
    std::map<std::string, Product*> products;
    
public:
    void addProduct(Product* p) { products[p->getId()] = p; }
    Product* getProduct(const std::string& id) { return products[id]; }
    
    void display() {
        std::cout << "\n📦 INVENTORY:\n";
        for (auto& p : products) {
            std::cout << "  " << p.second->getName() 
                     << " - $" << p.second->getBasePrice() 
                     << " (Stock: " << p.second->getStock() << ")\n";
        }
    }
    
    ~Inventory() {
        for (auto& p : products) delete p.second;
    }
};

#endif
