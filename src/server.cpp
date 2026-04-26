#include "AuraKiosk.h"
#include "httplib.h"
#include <sstream>
#include <iostream>

// Initialize singleton
CentralRegistry* CentralRegistry::instance = nullptr;

// ── Global state (lives for the lifetime of the server) ───────────────────
Inventory g_inventory;

void initInventory() {
    g_inventory.addProduct(new Product("P001", "Water Bottle", 2.0, 20));
    g_inventory.addProduct(new Product("P002", "Sandwich",     5.0, 15));
    g_inventory.addProduct(new Product("P003", "Batteries",    3.0, 10));
}

// ── JSON helpers ──────────────────────────────────────────────────────────
std::string productJSON(Product* p) {
    std::ostringstream ss;
    ss << "{"
       << "\"id\":\""       << p->getId()       << "\","
       << "\"name\":\""     << p->getName()      << "\","
       << "\"basePrice\":"  << p->getBasePrice() << ","
       << "\"stock\":"      << p->getStock()
       << "}";
    return ss.str();
}

std::string inventoryJSON() {
    std::vector<std::string> ids = {"P001", "P002", "P003"};
    std::ostringstream ss;
    ss << "[";
    for (size_t i = 0; i < ids.size(); i++) {
        if (i > 0) ss << ",";
        ss << productJSON(g_inventory.getProduct(ids[i]));
    }
    ss << "]";
    return ss.str();
}

std::string okJSON(const std::string& msg) {
    return "{\"success\":true,\"message\":\"" + msg + "\"}";
}

std::string errJSON(const std::string& msg) {
    return "{\"success\":false,\"message\":\"" + msg + "\"}";
}

// Parse a simple key from JSON body e.g. "policy":"Emergency"
std::string parseField(const std::string& body, const std::string& key) {
    std::string search = "\"" + key + "\":\"";
    size_t pos = body.find(search);
    if (pos == std::string::npos) return "";
    pos += search.size();
    size_t end = body.find("\"", pos);
    return body.substr(pos, end - pos);
}

int parseIntField(const std::string& body, const std::string& key) {
    std::string search = "\"" + key + "\":";
    size_t pos = body.find(search);
    if (pos == std::string::npos) return 0;
    pos += search.size();
    return std::stoi(body.substr(pos));
}

// ── Main ──────────────────────────────────────────────────────────────────
int main() {
    initInventory();

    httplib::Server svr;

    // Allow CORS so the HTML file can call this server
    auto cors = [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin",  "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.set_header("Content-Type",                 "application/json");
    };

    // ── GET /inventory ─────────────────────────────────────────────────────
    // Returns all products with current stock
    svr.Get("/inventory", [&](const httplib::Request&, httplib::Response& res) {
        cors({}, res);
        res.set_content(inventoryJSON(), "application/json");
    });

    // ── GET /pricing ───────────────────────────────────────────────────────
    // Returns current pricing policy and emergency mode
    svr.Get("/pricing", [&](const httplib::Request&, httplib::Response& res) {
        cors({}, res);
        std::ostringstream ss;
        ss << "{"
           << "\"policy\":\""    << CentralRegistry::getInstance()->getPricingPolicy() << "\","
           << "\"emergency\":"   << (CentralRegistry::getInstance()->isEmergencyMode() ? "true" : "false")
           << "}";
        res.set_content(ss.str(), "application/json");
    });

    // ── POST /pricing ──────────────────────────────────────────────────────
    // Body: {"policy":"Discounted"}   or   {"policy":"Emergency"}
    svr.Post("/pricing", [&](const httplib::Request& req, httplib::Response& res) {
        cors({}, res);
        std::string policy = parseField(req.body, "policy");
        if (policy.empty()) {
            res.set_content(errJSON("Missing policy field"), "application/json");
            return;
        }
        CentralRegistry::getInstance()->setPricingPolicy(policy);
        CentralRegistry::getInstance()->setEmergencyMode(policy == "Emergency");
        std::cout << "[pricing] switched to: " << policy << "\n";
        res.set_content(okJSON("Pricing set to " + policy), "application/json");
    });

    // ── POST /preview ──────────────────────────────────────────────────────
    // Body: {"id":"P001","qty":3}
    // Returns calculated price without touching stock
    svr.Post("/preview", [&](const httplib::Request& req, httplib::Response& res) {
        cors({}, res);
        std::string id = parseField(req.body, "id");
        int qty        = parseIntField(req.body, "qty");
        Product* p     = g_inventory.getProduct(id);

        if (!p || qty <= 0) {
            res.set_content(errJSON("Invalid product or quantity"), "application/json");
            return;
        }

        std::string policy = CentralRegistry::getInstance()->getPricingPolicy();
        PricingStrategy* strategy = nullptr;
        if      (policy == "Discounted") strategy = new DiscountedPricing();
        else if (policy == "Emergency")  strategy = new EmergencyPricing();
        else                             strategy = new StandardPricing();

        double price = strategy->calculatePrice(p->getBasePrice(), qty);
        delete strategy;

        std::ostringstream ss;
        ss << "{\"price\":" << price << "}";
        res.set_content(ss.str(), "application/json");
    });

    // ── POST /purchase ─────────────────────────────────────────────────────
    // Body: {"id":"P001","qty":2}
    // Runs PurchaseCommand::execute() — deducts stock
    svr.Post("/purchase", [&](const httplib::Request& req, httplib::Response& res) {
        cors({}, res);
        std::string id = parseField(req.body, "id");
        int qty        = parseIntField(req.body, "qty");
        Product* p     = g_inventory.getProduct(id);

        if (!p) {
            res.set_content(errJSON("Product not found"), "application/json");
            return;
        }

        std::string policy = CentralRegistry::getInstance()->getPricingPolicy();
        PricingStrategy* strategy = nullptr;
        if      (policy == "Discounted") strategy = new DiscountedPricing();
        else if (policy == "Emergency")  strategy = new EmergencyPricing();
        else                             strategy = new StandardPricing();

        // ← This is the real PurchaseCommand from your AuraKiosk.h
        PurchaseCommand cmd(p, qty, strategy);
        bool ok = cmd.execute();

        double finalPrice = ok ? strategy->calculatePrice(p->getBasePrice(), qty) : 0;
        delete strategy;

        std::ostringstream ss;
        ss << "{"
           << "\"success\":"     << (ok ? "true" : "false")                      << ","
           << "\"message\":\""   << (ok ? "Purchase successful" : "Insufficient stock") << "\","
           << "\"finalPrice\":"  << finalPrice                                    << ","
           << "\"remaining\":"   << p->getStock()
           << "}";

        std::cout << "[purchase] " << id << " x" << qty
                  << (ok ? " OK $" : " FAILED — stock: ") << (ok ? finalPrice : p->getStock()) << "\n";

        res.set_content(ss.str(), "application/json");
    });

    // Handle preflight CORS
    svr.Options(".*", [&](const httplib::Request&, httplib::Response& res) {
        cors({}, res);
        res.status = 204;
    });

    std::cout << "╔══════════════════════════════════════╗\n";
    std::cout << "║   Aura Kiosk Server — localhost:8080  ║\n";
    std::cout << "╠══════════════════════════════════════╣\n";
    std::cout << "║  GET  /inventory                      ║\n";
    std::cout << "║  GET  /pricing                        ║\n";
    std::cout << "║  POST /pricing   {policy}             ║\n";
    std::cout << "║  POST /preview   {id, qty}            ║\n";
    std::cout << "║  POST /purchase  {id, qty}            ║\n";
    std::cout << "╚══════════════════════════════════════╝\n\n";

    svr.listen("0.0.0.0", 8080);
    return 0;
}
