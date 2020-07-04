#ifndef __PatchProcessor_h__
#define __PatchProcessor_h__

#include <stdint.h>
#include "Patch.h"
#include "device.h"
#include "MemoryBuffer.hpp"

class ParameterUpdater {
public:
  virtual ~ParameterUpdater(){}
  virtual void update(int16_t value) = 0;
  virtual void setParameter(IntParameter* p){}
  virtual void setParameter(FloatParameter* p){}
};

template<typename T, typename V>
class LinearParameterUpdater : public ParameterUpdater {
private:
  PatchParameter<T>* parameter;
  T minimum;
  T maximum;
  V value;
public:
  LinearParameterUpdater(T min, T max, V initialValue)
    : parameter(NULL), minimum(min), maximum(max), value(initialValue) {}
  void update(int16_t newValue){
    value = (newValue*(maximum-minimum))/4096+minimum;
    if(parameter != NULL)
      parameter->update((T)value);
  }
  void setParameter(PatchParameter<T>* p){
    parameter = p;
  }
};

// void setSkew(float mid){
//   if (maximum > minimum)
//     skew = log (0.5) / log ((mid - minimum) / (maximum - minimum));
// }

template<typename T, typename V>
class ExponentialParameterUpdater : public ParameterUpdater {
private:
  PatchParameter<T>* parameter;
  float skew;
  T minimum;
  T maximum;
  V value;
public:
  ExponentialParameterUpdater(float skw, T min, T max, V initialValue)
    : parameter(NULL), skew(skw), minimum(min), maximum(max), value(initialValue) {
    //    ASSERT(skew > 0.0 && skew <= 2.0, "Invalid exponential skew value");
    ASSERT(skew > 0.0, "Invalid exponential skew value");
  }
  void update(int16_t newValue){
    float v = newValue/4096.0f;
    v = expf(logf(v)/skew);
    value = v*(maximum-minimum)+minimum;
    if(parameter != NULL)
      parameter->update((T)value);
  }
  void setParameter(PatchParameter<T>* p){
    parameter = p;
  }
};



class PatchProcessor {
public:  
  PatchProcessor();
  ~PatchProcessor();
  void clear();
  void setPatch(Patch* patch, const char* name);
  const char* getPatchName(){
    return name;
  }
  int getBlockSize();
  double getSampleRate();
  AudioBuffer* createMemoryBuffer(int channels, int samples);
  void setParameterValues(int16_t* parameters);
  Patch* patch;
  uint8_t index;
  void setPatchParameter(int pid, FloatParameter* param);
  void setPatchParameter(int pid, IntParameter* param);
  template<typename T>
  PatchParameter<T> getParameter(const char* name, T min, T max, T defaultValue, float lambda, float delta, float skew);
private:
  void setDefaultValue(int pid, float value);
  void setDefaultValue(int pid, int value);
  uint8_t bufferCount;
  ParameterUpdater* parameters[MAX_NUMBER_OF_PARAMETERS];
  uint8_t parameterCount;
  AudioBuffer* buffers[MAX_BUFFERS_PER_PATCH];
  const char* name;
};


#endif // __PatchProcessor_h__

