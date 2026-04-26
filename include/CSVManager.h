#pragma once
#include <string>
#include <vector>
#include <map>
#include <mutex>

struct ProductRecord {
    std::string product_id;
    std::string name;
    std::string category;
    double price;
    int stock;
    int reserved;
    std::string hardware_required;
};

struct TransactionRecord {
    std::string txn_id;
    std::string product_id;
    std::string user_id;
    int qty;
    double final_price;
    std::string status;
    std::string timestamp;
};

struct KioskConfigRecord {
    std::string kiosk_id;
    std::string kiosk_type;
    std::string mode;
    std::vector<std::string> hardware_modules;
};

class CSVManager {
public:
    static CSVManager& getInstance();
    
    std::vector<ProductRecord> readInventory();
    void writeInventory(const std::vector<ProductRecord>& inventory);
    
    std::vector<TransactionRecord> readTransactions();
    void writeTransaction(const TransactionRecord& txn);
    void writeTransactions(const std::vector<TransactionRecord>& txns);
    
    KioskConfigRecord readConfig();
    void writeConfig(const KioskConfigRecord& config);
    
    void logEvent(const std::string& type, const std::string& severity, const std::string& message);
    std::vector<std::map<std::string, std::string>> readEvents();

private:
    CSVManager() = default;
    std::mutex mtx;
};
