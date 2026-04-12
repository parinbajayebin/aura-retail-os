# Aura Retail OS — FoodKiosk Implementation

**Team:** 404 Error Not Found
---

## Project Overview

A modular smart retail kiosk system with dynamic pricing capabilities, implemented as a C++ backend server with a browser-based frontend. The system demonstrates three core design patterns — Singleton, Strategy, and Command — connected to a live web UI via a REST API.

---

## Project Structure

```
aura-retail-os/
├── src/
│   ├── AuraKiosk.h      # All pattern implementations (Singleton, Strategy, Command)
│   ├── main.cpp         # Terminal simulation driver (standalone testing)
│   ├── server.cpp       # C++ HTTP server — exposes AuraKiosk as REST API
│   └── index.html       # Browser frontend — calls the server via fetch
│   ├── httplib.h       # Single-header HTTP library (download once, see setup)
└── README.md
```

> `main.cpp` is used for standalone terminal testing. For the browser frontend, `server.cpp` acts as the entry point instead.

---

## Setup & Running

---

### Step 1 — Compile the C++ server

```bash
# Linux / Mac
g++ -std=c++17 -o server server.cpp -lpthread

# Windows (MinGW)
g++ -std=c++17 -o server.exe server.cpp -lws2_32 -D_WIN32_WINNT=0x0A00
```

No other installs needed — just `g++`, which everyone already has.

---

### Step 2 — Run the server

```bash
# Linux / Mac
./server

# Windows
.\server.exe
```

You should see:

```
╔══════════════════════════════════════╗
║   Aura Kiosk Server — localhost:8080  ║
╠══════════════════════════════════════╣
║  GET  /inventory                      ║
║  GET  /pricing                        ║
║  POST /pricing   {policy}             ║
║  POST /preview   {id, qty}            ║
║  POST /purchase  {id, qty}            ║
╚══════════════════════════════════════╝
```

---

### Step 3 — Open the frontend

Open `index.html` in your browser. No web server needed for the HTML file itself.

The green status bar at the top confirms it is connected to the C++ server. Every button in the browser makes a real HTTP call to your running C++ process:

| Browser action       | HTTP call        | C++ code called                       |
|----------------------|------------------|---------------------------------------|
| Page load            | GET /inventory   | `Inventory` → reads all products      |
| Switch pricing mode  | POST /pricing    | `CentralRegistry::setPricingPolicy()` |
| Select product / qty | POST /preview    | `PricingStrategy::calculatePrice()`   |
| Confirm purchase     | POST /purchase   | `PurchaseCommand::execute()`          |

Stock deductions are real — they happen inside your C++ `Inventory` object and persist for the lifetime of the server process.

---

### Standalone terminal testing (optional)

To test the C++ logic directly without the browser:

```bash
g++ -std=c++11 src/main.cpp -o aura_kiosk
./aura_kiosk
```

---

## Implemented Design Patterns

### 1. Singleton Pattern — CentralRegistry
- **Location:** `AuraKiosk.h` (lines 10–24)
- **Purpose:** Single source of truth for system-wide configuration. Only one instance exists throughout the application.
- **Stores:** Pricing policy and emergency mode status
- **Justification:** Prevents conflicting states across subsystems; easy access from anywhere in the codebase

### 2. Strategy Pattern — Dynamic Pricing
- **Location:** `AuraKiosk.h` (lines 27–56)
- **Purpose:** Interchangeable pricing algorithms switchable at runtime without modifying purchase logic
- **Concrete strategies:**
  - `StandardPricing` — Regular prices (100%)
  - `DiscountedPricing` — Weekend sale (10% off)
  - `EmergencyPricing` — Disaster relief (50% off)
- **Justification:** Follows the Open/Closed Principle — new strategies can be added without changing existing code

### 3. Command Pattern — Purchase Operations
- **Location:** `AuraKiosk.h` (lines 72–102)
- **Purpose:** Encapsulates each purchase as a trackable, executable object
- **Benefits:** Supports logging, transaction history, and future undo/redo capability
- **Justification:** Decouples purchase request from execution logic

---

## Simulation Scenarios

The terminal simulation (`main.cpp`) demonstrates dynamic pricing through three scenarios:

### Scenario 1 — Normal Operation
- Mode: Standard Pricing
- Action: Customer purchases 2 water bottles
- Price: $2.00 × 2 = **$4.00**

### Scenario 2 — Weekend Sale
- Mode: Discounted Pricing (10% off)
- Action: Customer purchases 3 sandwiches
- Price: $5.00 × 3 × 0.9 = **$13.50**

### Scenario 3 — Emergency Mode
- Mode: Emergency Pricing (50% off)
- Action: Disaster relief mode activated
- Batteries: $3.00 × 5 × 0.5 = **$7.50**
- Water: $2.00 × 3 × 0.5 = **$3.00**

---

## Implemented Constraints

- ✅ Derived Attribute: Available stock calculated from current stock
- ✅ Atomic Operations: Purchase either completes fully or fails
- ✅ Inventory Consistency: Stock updated only on successful purchase
- ✅ Dynamic Behavior: Pricing changes at runtime based on system mode

---

## Contributors & Responsibilities

| Member | Responsibility |
|---|---|
| Shruti Makwana | CentralRegistry (Singleton), system integration |
| Krisha Shah | Inventory management, Product class |
| Parin Makwana | Command pattern, transaction handling |
| Ayush Soni | Pricing strategies (Strategy pattern), simulation scenarios |

---

## Future Enhancements

- **State Pattern:** Kiosk operational modes (Active, Maintenance, Emergency)
- **Observer Pattern:** Event system for subsystem communication
- **Chain of Responsibility:** Failure handling chain
- **Memento Pattern:** Transaction rollback capability
- **Abstract Factory:** Creating different kiosk types
- **CSV Persistence:** Save/load inventory and transactions
- **Hardware Abstraction Layer:** Support for different dispensers

---

**Course:** IT620 - Object Oriented Programming  
**Project:** Aura Retail OS  
**Submission:** Subtask 2 - Prototype Implementation
