#include "device.h"
#include "DigitalBus.h"
#include "ServiceCall.h"
//#include "ProgramVector.h"

void DigitalBus::sendCommand(uint8_t cmd_id, uint16_t data){
    doBusCommandSend(cmd_id, data);
}

void DigitalBus::sendData(const ByteArray& data){
    doBusDataSend(data.getData(), data.getSize());
}

void DigitalBus::sendMessage(const char* msg){
    doBusMessageSend(msg);
}

void DigitalBus::sendParameters(bool force){
    if (num_parameters > 0) {
        auto pv = getProgramVector();
        for (int i = 0; i < num_parameters; i++){
            auto value = registered_parameters[i]->update();
            //auto value = pv->parameters[pid];
            //registered_parameters[i].update(value);
            //value = registered_parameters[i]->get
            if (force || sent_parameters[i] != (uint16_t)value){
                doBusParameterSend(registered_parameters[i]->getPid(), value);
                sent_parameters[i] = value;
            }
        }
    }
}

void DigitalBus::sendButtons(bool force){
    if (num_buttons > 0) {
        auto pv = getProgramVector();
        for (int i = 0; i < num_buttons; i++){
            auto value = bus.button_values[i];
            if (force || sent_buttons[i] != value){
                doBusButtonSend(registered_buttons[i], value);
                sent_buttons[i] = value;
            }
        }
    }
}

bool DigitalBus::isEnabled() {

}
    
void DigitalBus::enable(bool state) {

}

DigitalBusStatus DigitalBus::getStatus() {
    DigitalBusStatus status;
    void* args[] = {
        (void*)(SYSEX_CONFIGURATION_DIGITAL_BUS_STATUS), (void*)&status
    };
    int ret = getProgramVector()->serviceCall(OWL_SERVICE_GET_PARAMETERS, args, 2);
    if(ret == OWL_SERVICE_OK){
        return status;
    }
    else {
        return BUS_STATUS_UNKNOWN;
    }
}

int32_t DigitalBus::getPeers() {
    int32_t peers;
    void* args[] = {
        (void*)(SYSEX_CONFIGURATION_DIGITAL_BUS_PEERS), (void*)&peers
    };
    int ret = getProgramVector()->serviceCall(OWL_SERVICE_GET_PARAMETERS, args, 2);
    if(ret == OWL_SERVICE_OK){
        return peers;
    }
    else {
        return -1;
    }
}

void DigitalBus::clear() {
    for (int i = 0; i < num_parameters; i++){
        delete registered_parameters[i];
    }
    num_parameters = 0;
    num_buttons = 0;
};

DigitalBus bus;
