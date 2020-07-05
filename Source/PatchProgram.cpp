#include <string> /* include this here to avoid errors with std::min/std::max later */
#include "ProgramVector.h"
#include "ServiceCall.h"
#include "SampleBuffer.hpp"
#include "PatchProcessor.h"
#include "message.h"
#include "Patch.h"
#include "main.h"
#include "heap.h"
#include "system_tables.h"
#ifdef USE_MIDI_CALLBACK
#include "MidiMessage.h"
#endif
#ifdef USE_DIGITALBUS
#include "DigitalBus.h"
#endif
#include "registerpatch.h"

static PatchProcessor processor;
PatchProcessor* getInitialisingPatchProcessor(){
  return &processor;
}

void doSetPatchParameter(uint8_t id, int16_t value){
  ProgramVector* vec = getProgramVector();
  if(vec->checksum >= PROGRAM_VECTOR_CHECKSUM_V12 &&
     vec->setPatchParameter != NULL && vec->parameters[id] != value)
    vec->setPatchParameter(id, value);
}

void doSetButton(uint8_t id, uint16_t value, uint16_t samples){
  ProgramVector* vec = getProgramVector();
  if(vec->checksum >= PROGRAM_VECTOR_CHECKSUM_V12 &&
     // // if it is not a MIDI note, check that value has changed
     // (id > 15 || (bool)(vec->buttons&(1<<id)) != (bool)value) &&
     vec->setButton != NULL){
    vec->setButton((PatchButtonId)id, value, samples);
    if(id < 16){
      if(value)
	vec->buttons |= (1<<id);
      else
	vec->buttons &= ~(1<<id);
    }
  }
#if USE_DIGITALBUS
  bus.setButton((PatchButtonId)id, value, samples);
#endif
}

void onButtonChanged(uint8_t id, uint16_t value, uint16_t samples){
  if(processor.patch != NULL)
    processor.patch->buttonChanged((PatchButtonId)id, value, samples);
}

#ifdef USE_SCREEN
void onDrawCallback(uint8_t* pixels, uint16_t width, uint16_t height){
  if(processor.patch != NULL){
    ScreenBuffer screen(width, height);
    screen.setBuffer(pixels);
    processor.patch->processScreen(screen);
  }
}
#endif /* USE_SCREEN */

#ifdef USE_DIGITALBUS
void onBusDiscover(){
  bus.connected = true;
  if (processor.patch != NULL){
    processor.patch->processBusDiscover();
  }
}

void onBusReset(){
  bus.connected = false;
  if (processor.patch != NULL){
    processor.patch->processBusReset();
  }
}

void onBusCommand(uint8_t cmd, int16_t data){
  if (processor.patch != NULL){
      processor.patch->processBusCommand(cmd, data);
  }
}

void onBusMessage(const char* msg){
  if(processor.patch != NULL){
    processor.patch->processBusMessage(msg);
  }
}

void onBusData(const uint8_t* data, uint16_t size){
  if(processor.patch != NULL){
    // We copy bus data to a temporary byte array for passing to patch.
    // This would avoid theoretical situation when a flood of serial data overwrites
    // values in buffer. If that can't happen, we could use data as is, but make object const
    //const ByteArray bus_data(const_cast<uint8_t*>(data), size);
    //ByteArray bus_data = ByteArray::create(size);
    //bus_data.copyFrom(const_cast<uint8_t*>(data), size);
    ByteArray bus_data(const_cast<uint8_t*>(data), size);
    processor.patch->processBusData(bus_data);
    //ByteArray::destroy(bus_data);
  }
}

static void (*bus_parameter_send_callback)(uint8_t, int16_t) = NULL;
void doBusParameterSend(uint8_t pid, int16_t data){
  if(bus_parameter_send_callback != NULL)
    bus_parameter_send_callback(pid, data);
}

static void (*bus_button_send_callback)(uint8_t, int16_t) = NULL;
void doBusButtonSend(uint8_t bid, int16_t data){
  if(bus_button_send_callback != NULL)
    bus_button_send_callback(bid, data);
}

static void (*bus_command_send_callback)(uint8_t, int16_t) = NULL;
void doBusCommandSend(uint8_t cmd_id, int16_t data){
  if(bus_command_send_callback != NULL)
    bus_command_send_callback(cmd_id, data);
}

static void (*bus_message_send_callback)(const char* msg) = NULL;
void doBusMessageSend(const char* msg) {
  if(bus_message_send_callback != NULL)
    bus_message_send_callback(msg);
}

static void (*bus_data_send_callback)(const uint8_t *data, uint16_t size) = NULL;
void doBusDataSend(const uint8_t *data, uint16_t size){
  if(bus_data_send_callback != NULL)
    bus_data_send_callback(data, size);
}
#endif /* USE_DIGITALBUS */

#ifdef USE_MIDI_CALLBACK
void onMidiCallback(uint8_t port, uint8_t status, uint8_t d1, uint8_t d2){
  static MidiMessage msg;
  if(processor.patch != NULL){
    msg.data[0] = port;
    msg.data[1] = status;
    msg.data[2] = d1;
    msg.data[3] = d2;
    processor.patch->processMidi(msg);
  }
}

static void (*midi_send_callback)(uint8_t, uint8_t, uint8_t, uint8_t) = NULL;
void doMidiSend(uint8_t port, uint8_t d0, uint8_t d1, uint8_t d2){
  if(midi_send_callback != NULL)
    midi_send_callback(port, d0, d1, d2);
}
#endif /* USE_MIDI_CALLBACK */

#define REGISTER_PATCH(T, STR, IN, OUT) registerPatch(STR, IN, OUT, new T)

void registerPatch(const char* name, uint8_t inputs, uint8_t outputs, Patch* patch){
  if(patch == NULL)
    error(OUT_OF_MEMORY_ERROR_STATUS, "Out of memory");
  if(getProgramVector()->registerPatch != NULL)
    getProgramVector()->registerPatch(name, inputs, outputs);
  processor.setPatch(patch, name);
}

static SampleBuffer* samples;
void setup(ProgramVector* pv){
  setSystemTables(pv);
#ifdef USE_SCREEN
  void* drawArgs[] = {(void*)SYSTEM_FUNCTION_DRAW, (void*)&onDrawCallback};
  pv->serviceCall(OWL_SERVICE_REGISTER_CALLBACK, drawArgs, 2);
#endif /* USE_SCREEN */
#ifdef USE_MIDI_CALLBACK
  void* midiRxArgs[] = {(void*)SYSTEM_FUNCTION_MIDI, (void*)&onMidiCallback};
  pv->serviceCall(OWL_SERVICE_REGISTER_CALLBACK, midiRxArgs, 2);

  midi_send_callback = NULL;
  void* midiTxArgs[] = {(void*)SYSTEM_FUNCTION_MIDI, &midi_send_callback};
  pv->serviceCall(OWL_SERVICE_REQUEST_CALLBACK, midiTxArgs, 2);

#endif /* USE_MIDI_CALLBACK */
#if USE_DIGITALBUS
  // Bus state callbacks
  void* busDiscoverArgs[] = {(void*)SYSTEM_FUNCTION_BUS_DISCOVER, (void*)&onBusDiscover};
  pv->serviceCall(OWL_SERVICE_REGISTER_CALLBACK, busDiscoverArgs, 2);

  void* busResetArgs[] = {(void*)SYSTEM_FUNCTION_BUS_RESET, (void*)&onBusReset};
  pv->serviceCall(OWL_SERVICE_REGISTER_CALLBACK, busResetArgs, 2);

  // Bus receive callbacks
  void* busCommandRxArgs[] = {(void*)SYSTEM_FUNCTION_BUS_COMMAND, (void*)&onBusCommand};
  pv->serviceCall(OWL_SERVICE_REGISTER_CALLBACK, busCommandRxArgs, 2);

  void* busDataRxArgs[] = {(void*)SYSTEM_FUNCTION_BUS_DATA, (void*)&onBusData};
  pv->serviceCall(OWL_SERVICE_REGISTER_CALLBACK, busDataRxArgs, 2);

  void* busMessageRxArgs[] = {(void*)SYSTEM_FUNCTION_BUS_MESSAGE, (void*)&onBusMessage};
  pv->serviceCall(OWL_SERVICE_REGISTER_CALLBACK, busMessageRxArgs, 2);

  // Bus transmit callbacks
  bus_parameter_send_callback = NULL;
  void* busParameterTxArgs[] = {(void*)SYSTEM_FUNCTION_BUS_PARAMETER, &bus_parameter_send_callback};
  pv->serviceCall(OWL_SERVICE_REQUEST_CALLBACK, busParameterTxArgs, 2);

  bus_button_send_callback = NULL;
  void* busButtonTxArgs[] = {(void*)SYSTEM_FUNCTION_BUS_BUTTON, &bus_button_send_callback};
  pv->serviceCall(OWL_SERVICE_REQUEST_CALLBACK, busButtonTxArgs, 2);

  bus_command_send_callback = NULL;
  void* busCommandTxArgs[] = {(void*)SYSTEM_FUNCTION_BUS_COMMAND, &bus_command_send_callback};
  pv->serviceCall(OWL_SERVICE_REQUEST_CALLBACK, busCommandTxArgs, 2);

  bus_data_send_callback = NULL;
  void* busDataTxArgs[] = {(void*)SYSTEM_FUNCTION_BUS_DATA, &bus_data_send_callback};
  pv->serviceCall(OWL_SERVICE_REQUEST_CALLBACK, busDataTxArgs, 2);

  bus_message_send_callback = NULL;
  void* busMessageTxArgs[] = {(void*)SYSTEM_FUNCTION_BUS_MESSAGE, &bus_message_send_callback};
  pv->serviceCall(OWL_SERVICE_REQUEST_CALLBACK, busMessageTxArgs, 2);
#endif /* USE_DIGITAL_BUS */

  samples = new SampleBuffer(pv->audio_blocksize);
#include "registerpatch.cpp"
}

void run(ProgramVector* pv){
#if 0
  for(;;){
    pv->programReady();
    memcpy(pv->audio_output, pv->audio_input, pv->audio_blocksize*2*sizeof(uint32_t));
  }
#else
  if(pv->audio_format == AUDIO_FORMAT_24B32){
    for(;;){
      pv->programReady();
      samples->split32(pv->audio_input, pv->audio_blocksize);
      processor.setParameterValues(pv->parameters);
      processor.patch->processAudio(*samples);
      samples->comb32(pv->audio_output);
    }
#if 0
  }else if(pv->audio_format == AUDIO_FORMAT_24B24){
    for(;;){
      pv->programReady();
      samples->split24(pv->audio_input, pv->audio_blocksize);
      processor.setParameterValues(pv->parameters);
      processor.patch->processAudio(*samples);
      samples->comb24(pv->audio_output);
    }
#endif
  }else{
    for(;;){
      pv->programReady();
      samples->split16(pv->audio_input, pv->audio_blocksize);
      processor.setParameterValues(pv->parameters);
      processor.patch->processAudio(*samples);
      samples->comb16(pv->audio_output);
    }
#endif
  }
}

#if 0
      static int32_t value = 0;
      static int32_t step = 32;
      ProgramVector* pv = getProgramVector();
      for(int i=0; i < pv->audio_blocksize; ++i){
        pv->audio_output[i*2] = value;
        pv->audio_output[i*2+1] = value; // swap(value);
        value += step;
        if(value >= INT32_MAX)
          value = INT32_MIN;
      }
#endif
