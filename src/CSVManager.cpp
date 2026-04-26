#include "CSVManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>

std::vector<std::string> split_csv(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

CSVManager& CSVManager::getInstance() {
    static CSVManager instance;
    return instance;
}

std::vector<ProductRecord> CSVManager::readInventory() {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<ProductRecord> inventory;
    std::ifstream file("data/inventory.csv");
    if (!file.is_open()) return inventory;

    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        if(line.empty()) continue;
        auto tokens = split_csv(line, ',');
        if(tokens.size() >= 7) {
            ProductRecord pr;
            pr.product_id = tokens[0];
            pr.name = tokens[1];
            pr.category = tokens[2];
            pr.price = std::stod(tokens[3]);
            pr.stock = std::stoi(tokens[4]);
            pr.reserved = std::stoi(tokens[5]);
            pr.hardware_required = tokens[6];
            inventory.push_back(pr);
        }
    }
    return inventory;
}

void CSVManager::writeInventory(const std::vector<ProductRecord>& inventory) {
    std::lock_guard<std::mutex> lock(mtx);
    std::ofstream file("data/inventory.csv");
    file << "product_id,name,category,price,stock,reserved,hardware_required\n";
    for (const auto& pr : inventory) {
        file << pr.product_id << "," << pr.name << "," << pr.category << ","
             << pr.price << "," << pr.stock << "," << pr.reserved << "," << pr.hardware_required << "\n";
    }
}

std::vector<TransactionRecord> CSVManager::readTransactions() {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<TransactionRecord> txns;
    std::ifstream file("data/transactions.csv");
    if (!file.is_open()) return txns;

    std::string line;
    std::getline(file, line);
    while (std::getline(file, line)) {
        if(line.empty()) continue;
        auto tokens = split_csv(line, ',');
        if(tokens.size() >= 7) {
            TransactionRecord tr;
            tr.txn_id = tokens[0];
            tr.product_id = tokens[1];
            tr.user_id = tokens[2];
            tr.qty = std::stoi(tokens[3]);
            tr.final_price = std::stod(tokens[4]);
            tr.status = tokens[5];
            tr.timestamp = tokens[6];
            txns.push_back(tr);
        }
    }
    return txns;
}

void CSVManager::writeTransaction(const TransactionRecord& txn) {
    std::lock_guard<std::mutex> lock(mtx);
    std::ofstream file("data/transactions.csv", std::ios::app);
    file << txn.txn_id << "," << txn.product_id << "," << txn.user_id << ","
         << txn.qty << "," << txn.final_price << "," << txn.status << "," << txn.timestamp << "\n";
}

void CSVManager::writeTransactions(const std::vector<TransactionRecord>& txns) {
    std::lock_guard<std::mutex> lock(mtx);
    std::ofstream file("data/transactions.csv");
    file << "txn_id,product_id,user_id,qty,final_price,status,timestamp\n";
    for (const auto& txn : txns) {
        file << txn.txn_id << "," << txn.product_id << "," << txn.user_id << ","
             << txn.qty << "," << txn.final_price << "," << txn.status << "," << txn.timestamp << "\n";
    }
}

KioskConfigRecord CSVManager::readConfig() {
    std::lock_guard<std::mutex> lock(mtx);
    KioskConfigRecord config;
    std::ifstream file("data/kiosk_config.csv");
    if (!file.is_open()) return config;

    std::string line;
    std::getline(file, line);
    if (std::getline(file, line)) {
        auto tokens = split_csv(line, ',');
        if(tokens.size() >= 4) {
            config.kiosk_id = tokens[0];
            config.kiosk_type = tokens[1];
            config.mode = tokens[2];
            config.hardware_modules = split_csv(tokens[3], '|');
        }
    }
    return config;
}

void CSVManager::writeConfig(const KioskConfigRecord& config) {
    std::lock_guard<std::mutex> lock(mtx);
    std::ofstream file("data/kiosk_config.csv");
    file << "kiosk_id,kiosk_type,mode,hardware_modules\n";
    file << config.kiosk_id << "," << config.kiosk_type << "," << config.mode << ",";
    for(size_t i=0; i<config.hardware_modules.size(); ++i) {
        file << config.hardware_modules[i];
        if(i < config.hardware_modules.size()-1) file << "|";
    }
    file << "\n";
}

void CSVManager::logEvent(const std::string& type, const std::string& severity, const std::string& message) {
    std::lock_guard<std::mutex> lock(mtx);
    std::ofstream file("data/event_log.csv", std::ios::app);
    std::string timestamp = std::to_string(std::time(nullptr));
    static int id_counter = 1;
    file << "EVT-" << id_counter++ << "," << type << "," << severity << "," << message << "," << timestamp << "\n";
}

std::vector<std::map<std::string, std::string>> CSVManager::readEvents() {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::map<std::string, std::string>> events;
    std::ifstream file("data/event_log.csv");
    if(!file.is_open()) return events;
    std::string line;
    std::getline(file, line);
    while(std::getline(file, line)) {
        if(line.empty()) continue;
        auto tokens = split_csv(line, ',');
        if(tokens.size() >= 5) {
            std::map<std::string, std::string> ev;
            ev["event_id"] = tokens[0];
            ev["type"] = tokens[1];
            ev["severity"] = tokens[2];
            ev["message"] = tokens[3];
            ev["timestamp"] = tokens[4];
            events.push_back(ev);
        }
    }
    return events;
}
