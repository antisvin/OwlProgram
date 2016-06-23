#include "ProgramVector.h"
#include "ServiceCall.h"
#include "SampleBuffer.hpp"
#include "PatchProcessor.h"
#include "message.h"
#include "StompBox.h"
#include "patch.h"
#include "main.h"
#include "heap.h"

PatchProcessor processor;

PatchProcessor* getInitialisingPatchProcessor(){
  return &processor;
}

void doSetPatchParameter(uint8_t id, int16_t value){
  if(getProgramVector()->checksum >= PROGRAM_VECTOR_CHECKSUM_V12 &&
     getProgramVector()->setPatchParameter != NULL)
    getProgramVector()->setPatchParameter(id, value);
}

void doSetButton(uint8_t id, uint16_t value, uint16_t samples){
  if(getProgramVector()->checksum >= PROGRAM_VECTOR_CHECKSUM_V12 &&
     getProgramVector()->setButton != NULL)
    getProgramVector()->setButton((PatchButtonId)id, value, samples);
}

void onButtonChanged(uint8_t id, uint16_t value, uint16_t samples){
  if(processor.patch != NULL)
    processor.patch->buttonChanged((PatchButtonId)id, value, samples);
}

void onEncoderChanged(uint8_t id, int16_t delta, uint16_t samples){
  if(processor.patch != NULL)
    processor.patch->encoderChanged((PatchParameterId)id, delta, samples);
}

#define REGISTER_PATCH(T, STR, IN, OUT) registerPatch(STR, IN, OUT, new T)

void registerPatch(const char* name, uint8_t inputs, uint8_t outputs, Patch* patch){
  if(patch == NULL)
    error(OUT_OF_MEMORY_ERROR_STATUS, "Out of memory");
  if(getProgramVector()->registerPatch != NULL)
    getProgramVector()->registerPatch(name, inputs, outputs);
  processor.setPatch(patch);
}

SampleBuffer* samples;
void setup(ProgramVector* pv){
#ifdef DEBUG_MEM
#ifdef ARM_CORTEX
  size_t before = xPortGetFreeHeapSize();
#endif
#endif
#include "patch.cpp"
#ifdef DEBUG_MEM
  // todo xPortGetFreeHeapSize() before and after
  // extern uint32_t total_heap_used;
  // pv->heap_bytes_used = total_heap_used;
#ifdef ARM_CORTEX
  getProgramVector()->heap_bytes_used = before - xPortGetFreeHeapSize();
#endif
#endif
  // samples = new SampleBuffer(getBlockSize());
  samples = new SampleBuffer();
}

void processBlock(ProgramVector* pv){
  samples->split(pv->audio_input, pv->audio_blocksize);
  processor.setParameterValues(pv->parameters);
  processor.patch->processAudio(*samples);
  samples->comb(pv->audio_output);
}
