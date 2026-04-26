#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <thread>
#include <chrono>

class Dispenser {
public:
    virtual ~Dispenser() = default;
    virtual bool dispense(const std::string& productId) = 0;
};

class Verifier {
public:
    virtual ~Verifier() = default;
    virtual bool verify(const std::string& userId) = 0;
};

class SpiralDispenser : public Dispenser {
public:
    bool dispense(const std::string& productId) override {
        std::cout << "Spiral Dispensing " << productId << "... (taking time)\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return true;
    }
};

class RoboticArmDispenser : public Dispenser {
public:
    bool dispense(const std::string& productId) override {
        std::cout << "Robotic Arm Dispensing " << productId << "... (taking time)\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return true;
    }
};

class PrescriptionVerifier : public Verifier {
public:
    bool verify(const std::string& userId) override { return true; }
};

class IdentityVerifier : public Verifier {
public:
    bool verify(const std::string& userId) override { return true; }
};

// PATTERN: Abstract Factory
class KioskFactory {
public:
    virtual ~KioskFactory() = default;
    virtual std::unique_ptr<Dispenser> createDispenser() = 0;
    virtual std::unique_ptr<Verifier> createVerifier() = 0;
};

class PharmacyKioskFactory : public KioskFactory {
public:
    std::unique_ptr<Dispenser> createDispenser() override {
        return std::make_unique<RoboticArmDispenser>();
    }
    std::unique_ptr<Verifier> createVerifier() override {
        return std::make_unique<PrescriptionVerifier>();
    }
};

class FoodKioskFactory : public KioskFactory {
public:
    std::unique_ptr<Dispenser> createDispenser() override {
        return std::make_unique<SpiralDispenser>();
    }
    std::unique_ptr<Verifier> createVerifier() override {
        return std::make_unique<IdentityVerifier>();
    }
};

class EmergencyKioskFactory : public KioskFactory {
public:
    std::unique_ptr<Dispenser> createDispenser() override {
        return std::make_unique<RoboticArmDispenser>();
    }
    std::unique_ptr<Verifier> createVerifier() override {
        return std::make_unique<IdentityVerifier>();
    }
};
