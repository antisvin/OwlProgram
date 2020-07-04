#ifndef __DigitalBus_h__
#define __DigitalBus_h__

#include <cstddef>
#include "main.h"
#include "IntArray.h"
#include "OpenWareMidiControl.h"
#include "PatchProcessor.h"

static constexpr uint8_t MAX_BUS_PARAMETERS = 16;
static constexpr uint8_t MAX_BUS_BUTTONS = 16;
static constexpr uint8_t MAX_BUS_COMMANDS = 16;
static constexpr float param_lambda = 0.95f;
static constexpr int16_t param_histeresis = 5U;

// We don't really need 4 bytes here, but that's what we get from service call
enum DigitalBusStatus : int32_t {
    BUS_STATUS_IDLE      = 0x00,
    BUS_STATUS_DISCOVER  = 0x01,
    BUS_STATUS_CONNECTED = 0x02,
    BUS_STATUS_ERROR     = 0xff,
    BUS_STATUS_UNKNOWN   = -1    // This is used when service call fails, which should not ever happen
};


class DigitalBusParameter {
public:
    DigitalBusParameter(PatchParameterId pid, bool smooth){
        parameter = new IntParameter(pid);
        if (smooth){
            value = new SmoothStiffValue<float>(0.9, 0.9, 0);
            updater = new LinearParameterUpdater<int, SmoothStiffValue<float>>(
                0, 4095, *value);
        }
        else {
            updater =  new LinearParameterUpdater<int, int>(
                0, 4095, 0);
        }
        updater->setParameter(parameter);
    };
    ~DigitalBusParameter(){
        if (value != NULL)
            delete value;
        delete updater;
    }

    uint16_t update(){
        updater->update(getProgramVector()->parameters[parameter->getPid()]);
        return parameter->getValue();
    }

    int getPid(){
        return parameter->getPid();
    }

private:
    IntParameter* parameter;
    ParameterUpdater* updater;    
    SmoothStiffValue<float>* value;
};

class DigitalBus {
public:
//    DigitalBus() = default;
    // Parameter/button registration
    void registerParameter(const PatchParameterId pid, bool smooth) {
        if (num_parameters < MAX_BUS_PARAMETERS - 1){
            registered_parameters[num_parameters++] = new DigitalBusParameter(pid, smooth);
        }
    }
    template<typename T>
    void registerParameter(const PatchParameter<T>& param, bool smooth) {
        registerParameter((PatchParameterId)param.getPid(), smooth);
    }
    void registerButton(const PatchButtonId& bid) {
        if (num_buttons < MAX_BUS_BUTTONS - 1) 
            registered_buttons[num_buttons++] = bid;
    }
    // Protocol object sending
    void sendCommand(uint8_t cmd_id, uint16_t arg);
    void sendData(const ByteArray& data);
    void sendMessage(const char* msg);
    // Normally only incremental updates on parameter/button change are sent
    // but it's possible to force sending full state
    void sendParameters(bool force);
    void sendButtons(bool force);
    void setButton(PatchButtonId bid, uint16_t value, uint16_t samples){
        button_values[bid] = value;
    }
    // Various settings
    bool isEnabled();
    void enable(bool state);
    DigitalBusStatus getStatus();
    int32_t getPeers();
private:
    void clear();
    DigitalBusParameter* registered_parameters[MAX_BUS_PARAMETERS];
    int16_t sent_parameters[MAX_BUS_PARAMETERS];
    uint8_t num_parameters;
    PatchButtonId registered_buttons[MAX_BUS_BUTTONS];
    int16_t button_values[MAX_BUS_BUTTONS];
    int16_t sent_buttons[MAX_BUS_BUTTONS];
    uint8_t num_buttons;
};

extern DigitalBus bus;
#endif
