#include "AuraKiosk.h"
#include <iostream>

// Initialize static singleton instance
CentralRegistry* CentralRegistry::instance = nullptr;

void printHeader(const std::string& title) {
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "  " << title << "\n";
    std::cout << std::string(50, '=') << "\n";
}

int main() {
    printHeader("AURA RETAIL OS - FoodKiosk Simulation");
    
    // Get singleton instance
    CentralRegistry* registry = CentralRegistry::getInstance();
    
    // Initialize inventory
    Inventory inventory;
    inventory.addProduct(new Product("P001", "Water Bottle", 2.0, 20));
    inventory.addProduct(new Product("P002", "Sandwich", 5.0, 15));
    inventory.addProduct(new Product("P003", "Batteries", 3.0, 10));
    
    inventory.display();
    
    // SIMULATION: Dynamic Pricing Change
    
    // ========== SCENARIO 1: Standard Pricing ==========
    printHeader("SCENARIO 1: Normal Operation");
    std::cout << "Mode: " << registry->getPricingPolicy() << " Pricing\n\n";
    
    PricingStrategy* standardPricing = new StandardPricing();
    Command* cmd1 = new PurchaseCommand(inventory.getProduct("P001"), 2, standardPricing);
    cmd1->execute();
    
    // ========== SCENARIO 2: Switch to Discounted Pricing ==========
    printHeader("SCENARIO 2: Weekend Sale - Switching to Discounted");
    registry->setPricingPolicy("Discounted");
    std::cout << "🔄 Pricing policy changed to: " << registry->getPricingPolicy() << "\n\n";
    
    PricingStrategy* discountedPricing = new DiscountedPricing();
    Command* cmd2 = new PurchaseCommand(inventory.getProduct("P002"), 3, discountedPricing);
    cmd2->execute();
    
    // ========== SCENARIO 3: Emergency Mode Activated ==========
    printHeader("SCENARIO 3: EMERGENCY MODE - Disaster Relief");
    registry->setEmergencyMode(true);
    registry->setPricingPolicy("Emergency");
    std::cout << "🚨 EMERGENCY MODE ACTIVATED!\n";
    std::cout << "🔄 Pricing policy changed to: " << registry->getPricingPolicy() << "\n\n";
    
    PricingStrategy* emergencyPricing = new EmergencyPricing();
    Command* cmd3 = new PurchaseCommand(inventory.getProduct("P003"), 5, emergencyPricing);
    cmd3->execute();
    
    Command* cmd4 = new PurchaseCommand(inventory.getProduct("P001"), 3, emergencyPricing);
    cmd4->execute();
    
    // Final inventory status
    printHeader("FINAL INVENTORY STATUS");
    inventory.display();
    
    std::cout << "\n✅ Simulation completed successfully!\n\n";
    
    // Cleanup
    delete standardPricing;
    delete discountedPricing;
    delete emergencyPricing;
    delete cmd1;
    delete cmd2;
    delete cmd3;
    delete cmd4;
    delete registry;
    
    return 0;
}
