# Aura Retail OS - FoodKiosk Implementation

**Team:** 404 Error Not Found  
**Members:** Shruti Makwana, Ayush Soni, Krisha Shah, Parin Makwana

## Project structure

```
aura-retail-os/
├── server.cpp       ← C++ HTTP server (exposes AuraKiosk as REST API)
├── index.html       ← browser frontend (calls the server via fetch)
├── httplib.h        ← single-header HTTP library (download once, see below)
```

## Step 1 — Compile the C++ server

```bash
# Linux / Mac
g++ -std=c++17 -o server server.cpp -lpthread

# Windows (MinGW)
g++ -std=c++17 -D_WIN32_WINNT=0x0A00 -o server server.cpp -lws2_32
```

No other installs needed. Just `g++` which everyone already has.

---

## Step 2 — Run the server

```bash
./server.exe
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

## Step 3 — Open the frontend

Just open `index.html` in your browser. No web server needed for the HTML file itself.

The green status bar at the top confirms it is connected to the C++ server.

---

## How it works

Every button in the browser makes a real HTTP call to your C++ server:

| Browser action          | HTTP call           | C++ code called                          |
|-------------------------|---------------------|------------------------------------------|
| Page load               | GET /inventory      | `Inventory` → reads all products         |
| Switch pricing mode     | POST /pricing       | `CentralRegistry::setPricingPolicy()`    |
| Select product / qty    | POST /preview       | `PricingStrategy::calculatePrice()`      |
| Confirm purchase        | POST /purchase      | `PurchaseCommand::execute()`             |

Stock deductions are real — they happen inside your C++ `Inventory` object and persist for the lifetime of the server process.