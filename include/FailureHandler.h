#pragma once
#include <string>
#include <memory>

struct FailureContext {
    std::string productId;
    std::string errorType;
    bool resolved = false;
};

// PATTERN: Chain of Responsibility
class FailureHandler {
public:
    virtual ~FailureHandler() = default;
    void setNext(std::shared_ptr<FailureHandler> nextHandler) {
        next = nextHandler;
    }
    virtual void handle(FailureContext& context) {
        if (next) {
            next->handle(context);
        }
    }
protected:
    std::shared_ptr<FailureHandler> next;
};

class AutoRetryHandler : public FailureHandler {
public:
    void handle(FailureContext& context) override;
};

class RecalibrationHandler : public FailureHandler {
public:
    void handle(FailureContext& context) override;
};

class TechnicianAlertHandler : public FailureHandler {
public:
    void handle(FailureContext& context) override;
};
