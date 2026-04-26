#pragma once

// PATTERN: Strategy
class PricingStrategy {
public:
    virtual ~PricingStrategy() = default;
    virtual double computePrice(double basePrice, int qty) = 0;
};

class StandardPricing : public PricingStrategy {
public:
    double computePrice(double basePrice, int qty) override {
        return basePrice * qty;
    }
};

class DiscountPricing : public PricingStrategy {
public:
    double computePrice(double basePrice, int qty) override {
        return (basePrice * qty) * 0.8; // 20% off
    }
};

class EmergencyPricing : public PricingStrategy {
public:
    double computePrice(double basePrice, int qty) override {
        return 0.0; // Free
    }
};
