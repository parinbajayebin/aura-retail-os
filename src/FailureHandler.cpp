#include "FailureHandler.h"
#include "EventBus.h"
#include "CSVManager.h"
#include <iostream>

void AutoRetryHandler::handle(FailureContext& context) {
    std::cout << "[AutoRetryHandler] Attempting retry for " << context.productId << "...\n";
    context.resolved = false;
    if(!context.resolved && next) {
        next->handle(context);
    }
}

void RecalibrationHandler::handle(FailureContext& context) {
    std::cout << "[RecalibrationHandler] Recalibrating hardware...\n";
    context.resolved = false;
    if(!context.resolved && next) {
        next->handle(context);
    }
}

void TechnicianAlertHandler::handle(FailureContext& context) {
    std::cout << "[TechnicianAlertHandler] Alerting technician!\n";
    EventBus::getInstance().publish("HARDWARE_FAILURE", "Unresolved issue with " + context.productId + ". Technician alerted.", "CRITICAL");
    context.resolved = false;
}
