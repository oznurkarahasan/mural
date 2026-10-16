#include "extendtohomephase.h"
void ExtendToHomePhase::extendToHome(AsyncWebServerRequest *request) {
    auto moveTime = movement->extendToHome() + 1; // extra second of waiting for good measure
    request->send(200, "text/plain", String(moveTime));
}

// Skips homing: assumes Mural is already hanging at the home position (e.g. it returned there after the previous drawing).
void ExtendToHomePhase::doneWithPhase(AsyncWebServerRequest *request) {
    if (movement->hasStartedHoming() || movement->isMoving()) {
        request->send(400, "text/plain", "Already extending to home");
        return;
    }

    movement->resumeTopDistance(movement->getTopDistance());
    manager->setPhase(PhaseManager::PenCalibration);
    manager->respondWithState(request);
}

ExtendToHomePhase::ExtendToHomePhase(PhaseManager* manager, Movement* movement) {
    this->manager = manager;
    this->movement = movement;
}

const char* ExtendToHomePhase::getName() {
    return "ExtendToHome";
}

void ExtendToHomePhase::loopPhase() {
    if (movement->hasStartedHoming() && !movement->isMoving()) {
        manager->setPhase(PhaseManager::PenCalibration);
    }
}