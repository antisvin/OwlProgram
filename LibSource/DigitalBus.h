#ifndef __DigitalBus_h__
#define __DigitalBus_h__

#include "IntArray.h"
#include "OpenWareMidiControl.h"
#include "PatchProcessor.h"
#include "main.h"
#include <cstddef>

static constexpr uint8_t MAX_BUS_PARAMETERS = 40;
static constexpr uint8_t MAX_BUS_BUTTONS = 16;
static constexpr float param_default_lambda = 0.9f;
static constexpr float param_default_delta = 0.9f;

// We don't really need 4 bytes here, but that's what we get from service call
enum DigitalBusStatus : int32_t {
    BUS_STATUS_IDLE      = 0x00,
    BUS_STATUS_DISCOVER  = 0x01,
    BUS_STATUS_CONNECTED = 0x02,
    BUS_STATUS_ERROR     = 0xff,
    BUS_STATUS_UNKNOWN   = -1,
    // Unknown status is used when service call fails, which should not ever
    // happen
};

class DigitalBusParameter {
public:
    // Parameter with no smoothing
    DigitalBusParameter(PatchParameterId pid) : value(NULL), output_pid(pid) {
        parameter = new IntParameter(pid);
        updater = new LinearParameterUpdater<int, int>(
            0, 4095, getProgramVector()->parameters[parameter->getPid()]);
        updater->setParameter(parameter);
    }
    // Parameter with smoothing enabled
    DigitalBusParameter(PatchParameterId pid, float lambda, float delta) {
        output_pid = pid;
        parameter = new IntParameter(pid);
        value = new SmoothStiffValue<float>(
            lambda, delta, getProgramVector()->parameters[parameter->getPid()]);
        updater = new LinearParameterUpdater<int, SmoothStiffValue<float>>(0, 4095, *value);
        updater->setParameter(parameter);
    }
    ~DigitalBusParameter() {
        if (value != NULL)
            delete value;
        delete updater;
    }

    uint16_t update() {
        updater->update(getProgramVector()->parameters[parameter->getPid()]);
        return parameter->getValue();
    }

    const int getPid() {
        return parameter->getPid();
    }

    void setOutputId(PatchParameterId new_pid) {
        output_pid = new_pid;
    }

private:
    PatchParameterId output_pid;
    IntParameter *parameter;
    ParameterUpdater *updater;
    SmoothStiffValue<float> *value;

    friend class DigitalBus;
};

class DigitalBusButton {
public:
    DigitalBusButton(PatchButtonId bid) : bid(bid), output_bid(bid){};
    DigitalBusButton(PatchButtonId bid, PatchButtonId output_bid)
        : bid(bid), output_bid(output_bid){};
    void setOutputBid(PatchButtonId new_bid) {
        output_bid = new_bid;
    }
    PatchButtonId getBid() {
        return bid;
    }
    uint16_t getValue() {
        return value;
    }
    operator uint16_t() {
        return getValue();
    }
    void setValue(uint16_t value) {
        value = value;
    }

private:
    PatchButtonId bid, output_bid;
    uint16_t value;
};

class DigitalBus {
public:
    // Parameter/button registration
    void registerParameter(const PatchParameterId pid, bool smooth) {
        if (num_parameters < MAX_BUS_PARAMETERS - 1) {
            if (smooth)
                registerParameter(pid, param_default_lambda, param_default_delta);
            else
                registerParameter(pid);
        }
    }

    void registerParameter(const PatchParameterId pid) {
        if (num_parameters < MAX_BUS_PARAMETERS - 1) {
            registered_parameters[num_parameters++] = new DigitalBusParameter(pid);
        }
    }

    void registerParameter(const PatchParameterId pid, float lambda, float delta) {
        if (num_parameters < MAX_BUS_PARAMETERS - 1) {
            registered_parameters[num_parameters++] = new DigitalBusParameter(pid, lambda, delta);
        }
    }

    template <typename T>
    void registerParameter(const PatchParameter<T> &param, bool smooth) {
        registerParameter((PatchParameterId)param.getPid(), smooth);
    }

    template <typename T>
    void registerParameter(const PatchParameter<T> &param) {
        registerParameter((PatchParameterId)param.getPid());
    }

    template <typename T>
    void registerParameter(const PatchParameter<T> &param, float lambda, float delta) {
        registerParameter((PatchParameterId)param.getPid(), lambda, delta);
    }

    void registerButton(const PatchButtonId &bid) {
        if (num_buttons < MAX_BUS_BUTTONS - 1)
            registered_buttons[num_buttons++] = new DigitalBusButton(bid);
    }

    // Remapping parameters/buttons
    void remap(const PatchParameterId pid, PatchParameterId new_pid) {
        for (int i = 0; i < num_parameters; i++) {
            if (registered_parameters[i]->getPid() == pid) {
                registered_parameters[i]->setOutputId(new_pid);
                break;
            }
        }
    }

    template <typename T>
    void remap(const PatchParameter<T> &param,
                        PatchParameterId new_pid) {
        remap(param.getPid(), new_pid);
    }

    void remap(const PatchButtonId bid, PatchButtonId new_bid) {
        for (int i = 0; i < num_parameters; i++) {
            if (registered_buttons[i]->getBid() == bid) {
                registered_buttons[i]->setOutputBid(new_bid);
                break;
            }
        }
    }

    // Protocol object sending
    void sendCommand(uint8_t cmd_id, int16_t arg);
    void sendData(const ByteArray &data);
    void sendMessage(const char *msg);
    // Parameter/button sending.
    // Normally only incremental updates on parameter/button change are sent
    // but it's possible to force sending full state
    void sendParameters(bool force);
    void sendButtons(bool force);
    void setButton(PatchButtonId bid, uint16_t value) {
        for (int i = 0; i < num_buttons; i++) {
            if (registered_buttons[i]->getBid() == bid) {
                registered_buttons[i]->setValue(value);
            }
        }
    }
    // Various settings
    bool isEnabled();
    // This flag is set implicitly via callbacks, which means that it's much
    // faster compared to fetching state that uses service call to access bus
    // state.
    bool isConnected() {
        return connected;
    }
    bool isMidiForwardingEnabled();
    DigitalBusStatus getStatus();
    int32_t getPeers();
    // Next 2 functions will be added once we have parameter setting service calls:
    //void enable(bool state);
    //void midiForwardingEnable(bool state);

    void clear();    

private:    
    DigitalBusParameter *registered_parameters[MAX_BUS_PARAMETERS];
    int16_t sent_parameters[MAX_BUS_PARAMETERS];
    uint8_t num_parameters;
    DigitalBusButton *registered_buttons[MAX_BUS_BUTTONS];
    int16_t sent_buttons[MAX_BUS_BUTTONS];
    uint8_t num_buttons;

    friend void onBusDiscover();
    friend void onBusReset();
    bool connected;
};

extern DigitalBus bus;
#endif
