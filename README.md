# Aura Retail OS - FoodKiosk Implementation

**Team:** 404 Error Not Found  
**Members:** Shruti Makwana, Ayush Soni, Krisha Shah, Parin Makwana

## Project Overview
This is a partial implementation of the Aura Retail OS for **Subtask 2**, demonstrating a modular smart retail kiosk system with dynamic pricing capabilities.

## Implemented Design Patterns

### 1. **Singleton Pattern** - CentralRegistry
- **Purpose:** Single source of truth for system-wide configuration
- **Location:** `AuraKiosk.h` (lines 10-24)
- **Key Feature:** Only one instance exists throughout the application
- **Usage:** Stores pricing policy and emergency mode status

### 2. **Strategy Pattern** - Dynamic Pricing
- **Purpose:** Interchangeable pricing algorithms that can be switched at runtime
- **Location:** `AuraKiosk.h` (lines 27-56)
- **Concrete Strategies:**
  - `StandardPricing`: Regular prices (100%)
  - `DiscountedPricing`: Weekend sale (10% off)
  - `EmergencyPricing`: Disaster relief (50% off)
- **Key Feature:** Pricing behavior changes without modifying the purchase logic

### 3. **Command Pattern** - Purchase Operations
- **Purpose:** Encapsulate purchase requests as objects
- **Location:** `AuraKiosk.h` (lines 72-102)
- **Key Feature:** Each purchase is a trackable, executable command
- **Benefits:** Supports logging, undo operations (future), and transaction history

## System Features

### Core Components
1. **Product Class:** Encapsulates product data (ID, name, price, stock)
2. **Inventory System:** Manages all products in the kiosk
3. **CentralRegistry:** Global state management (Singleton)
4. **PricingStrategy:** Dynamic pricing engine (Strategy)
5. **PurchaseCommand:** Transaction handler (Command)

### Implemented Constraints
- ✅ **Derived Attribute:** Available stock calculated from current stock
- ✅ **Atomic Operations:** Purchase either completes fully or fails
- ✅ **Inventory Consistency:** Stock updated only on successful purchase
- ✅ **Dynamic Behavior:** Pricing changes at runtime based on system mode

## How to Compile and Run

### Using g++
```bash
cd src
g++ -std=c++11 main.cpp -o aura_kiosk
./aura_kiosk
```

### Using CMake (optional)
```bash
mkdir build && cd build
cmake ..
make
./aura_kiosk
```

## Simulation Scenario

The simulation demonstrates **Dynamic Pricing Change** through three scenarios:

### Scenario 1: Normal Operation
- Mode: Standard Pricing
- Action: Customer purchases 2 water bottles
- Price: $2.00 × 2 = $4.00

### Scenario 2: Weekend Sale
- Mode: Discounted Pricing (10% off)
- Action: System switches pricing strategy
- Customer purchases 3 sandwiches
- Price: $5.00 × 3 × 0.9 = $13.50

### Scenario 3: Emergency Mode
- Mode: Emergency Pricing (50% off)
- Action: Disaster relief mode activated
- Customer purchases 5 batteries and 3 water bottles
- Price: 50% discount applied to essential items
- Batteries: $3.00 × 5 × 0.5 = $7.50
- Water: $2.00 × 3 × 0.5 = $3.00

## Expected Output

```
==================================================
  AURA RETAIL OS - FoodKiosk Simulation
==================================================

📦 INVENTORY:
  Water Bottle - $2 (Stock: 20)
  Sandwich - $5 (Stock: 15)
  Batteries - $3 (Stock: 10)

==================================================
  SCENARIO 1: Normal Operation
==================================================
Mode: Standard Pricing

✅ Purchase successful!
   Item: Water Bottle x2
   Pricing: Standard
   Total: $4
   Remaining stock: 18

==================================================
  SCENARIO 2: Weekend Sale - Switching to Discounted
==================================================
🔄 Pricing policy changed to: Discounted

✅ Purchase successful!
   Item: Sandwich x3
   Pricing: Discounted (10% off)
   Total: $13.5
   Remaining stock: 12

==================================================
  SCENARIO 3: EMERGENCY MODE - Disaster Relief
==================================================
🚨 EMERGENCY MODE ACTIVATED!
🔄 Pricing policy changed to: Emergency

✅ Purchase successful!
   Item: Batteries x5
   Pricing: Emergency (50% off)
   Total: $7.5
   Remaining stock: 5
...
```

## Design Pattern Justification

### Why Singleton for CentralRegistry?
- System needs exactly ONE source of truth for global configuration
- Prevents conflicting states across different subsystems
- Easy access from anywhere in the codebase

### Why Strategy for Pricing?
- Pricing rules change based on time, events, or emergencies
- New pricing strategies can be added without modifying existing code
- Follows Open/Closed Principle: open for extension, closed for modification

### Why Command for Transactions?
- Each purchase is a discrete operation that needs tracking
- Supports future features like undo/redo and transaction history
- Decouples purchase request from execution logic

## Future Enhancements (Full Implementation)

For the final submission, we will add:
- **State Pattern:** Kiosk operational modes (Active, Maintenance, Emergency)
- **Observer Pattern:** Event system for subsystem communication
- **Chain of Responsibility:** Failure handling chain
- **Memento Pattern:** Transaction rollback capability
- **Abstract Factory:** Creating different kiosk types
- **CSV Persistence:** Save/load inventory and transactions
- **Hardware Abstraction Layer:** Support for different dispensers

## Project Structure

```
aura-retail-os/
├── src/
│   ├── AuraKiosk.h      # All pattern implementations
│   └── main.cpp         # Simulation driver
├── data/                # For CSV files (future)
└── README.md            # This file
```

## Testing

All three design patterns have been tested in the simulation:
- ✅ Singleton: Single CentralRegistry instance
- ✅ Strategy: Three different pricing strategies working
- ✅ Command: Purchase commands executing successfully

## Contributors & Responsibilities

- **Shruti Makwana:** CentralRegistry (Singleton), System integration
- **Krisha Shah:** Inventory management, Product class
- **Parin Makwana:** Command pattern, Transaction handling
- **Ayush Soni:** Pricing strategies (Strategy pattern), Simulation scenarios

---

**Course:** IT620 - Object Oriented Programming  
**Project:** Aura Retail OS  
**Submission:** Subtask 2 - Prototype Implementation
