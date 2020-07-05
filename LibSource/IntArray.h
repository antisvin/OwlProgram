#ifndef __IntArray_h__
#define __IntArray_h__

#include "message.h"
#include <string.h>

template <typename T>
class BaseIntegerArray
{
private:
  T* data;
  int size;
public:
  BaseIntegerArray() = default;
  BaseIntegerArray(T* data, int size) : data(data), size(size) {};

  int getSize() const{
    return size;
  }

  int getSize(){
    return size;
  }

  void setAll(T value){
    for(int n=0; n<size; n++){
      data[n]=value;
    }
  }

  /**
   * Clear the array.
   * Set all the values in the array to 0.
  */
  void clear(){
    setAll(T(0));
  }
  
  /**
   * Copies the content of the array to another array.
   * @param[out] destination the destination array
  */  
  void copyTo(BaseIntegerArray<T> destination) {
    /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
    copyTo(destination, min(size, destination.getSize()));    
  }

  /**
   * Copies the content of the array to a location in memory.
   * @param[out] destination a pointer to the beginning of the memory location to copy to.
   * The **length***sizeof(T) bytes of memory starting at this location must have been allocated before calling this method.
   * @param[in] length number of samples to copy
  */
  void copyTo(T* other, size_t length){
    ASSERT(size >= length, "Array too small");
    memcpy((void *)other, (void *)getData(), length*sizeof(T));
  }

  /**
   * Copies the content of an array into another array.
   * @param[in] source the source array
  */
  void copyFrom(BaseIntegerArray<T> source){
    /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
    copyFrom(source, min(size, source.getSize()));    
  }
  
  /**
   * Copies an array of intgegers into the array.
   * @param[in] source a pointer to the beginning of the portion of memory to read from.
   * @param[in] length number of samples to copy.
  */
  void copyFrom(T* other, size_t length){
    ASSERT(size >= length, "Array too small");
    memcpy((void *)getData(), (void *)other, length*sizeof(float));
  }
  
  /**
   * Copies the content of an array into a subset of the array.
   * Copies **samples** elements from **source** to **destinationOffset** in the current array.
   * @param[in] source the source array
   * @param[in] destinationOffset the offset into the destination array 
   * @param[in] samples the number of samples to copy
   *
  */

  /**
   * Element-wise sum between arrays.
   * Sets each element in **destination** to the sum of the corresponding element of the array and **operand2**
   * @param[in] operand2 second operand for the sum
   * @param[out] destination the destination array
  */
  void add(BaseIntegerArray<T> operand2, BaseIntegerArray<T> destination){
    //allows in-place
    //ASSERT(operand2.size >= size &&  destination.size<=size, "Arrays must be matching size");
    /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
    // Note: it would be overriden by specialized classes on ARM
    for(int n=0; n<size; n++){
      destination[n]=data[n]+operand2[n];
    }
  }
  
  /**
   * Element-wise sum between arrays.
   * Adds each element of **operand2** to the corresponding element in the array.
   * @param operand2 second operand for the sum
  */
  void add(BaseIntegerArray<T> operand2){ //in-place
  /// @note When built for ARM Cortex-M processor series, this method uses the optimized <a href="http://www.keil.com/pack/doc/CMSIS/General/html/index.html">CMSIS library</a>
    add(operand2, *this);
  } //in-place
  
  /**
   * Allows to index the array using array-style brackets.
   * @param index the index of the element
   * @return the value of the **index** element of the array
   * Example usage:
   * @code
   * int size=1000;
   * int32_t content[size]; 
   * IntArray intArray(content, size);
   * for(int n=0; n<size; n++)
   *   content[n]==intArray[n]; //now the IntArray can be indexed as if it was an array
   * @endcode
  */
  T& operator [](const int index){
    return data[index];
  }
  
  /**
   * Allows to index the array using array-style brackets.
   * **const** version of operator[]
  */
  T& operator [](const int index) const{
    return data[index];
  }
  
  /**
   * Compares two arrays.
   * Performs an element-wise comparison of the values contained in the arrays.
   * @param other the array to compare against.
   * @return **true** if the arrays have the same size and the value of each of the elements of the one 
   * match the value of the corresponding element of the other, or **false** otherwise.
  */
  bool equals(const BaseIntegerArray<T>& other) const{
    if(size!=other.getSize()){
      return false;
    }
    for(int n=0; n<size; n++){
      if(data[n]!=other[n]){
        return false;
      }
    }
    return true;
  }
  
  /**
   * Casting operator to T*
   * @return a T* pointer to the data stored in the BaseIntegerArray<T>
  */
  operator T*(){
    return data;
  }
  
  /**
   * Get the data stored in the BaseIntegerArray<T>.
   * @return a T* pointer to the data stored in the BaseIntegerArray<T>
  */
  T* getData(){
    return data;
  }
  T* getData() const {
    return data;
  }
  
  /**
   * Creates a new BaseIntegerArray<T>.
   * Allocates size*sizeof(T) bytes of memory and returns a BaseIntegerArray<T> that points to it.
   * @param size the size of the new BaseIntegerArray<T>.
   * @return a BaseIntegerArray<T> which **data** point to the newly allocated memory and **size** is initialized to the proper value.
   * @remarks a BaseIntegerArray<T> created with this method has to be destroyed invoking the BaseIntegerArray<T>::destroy() method.
  */
  static BaseIntegerArray<T> create(int size){
    BaseIntegerArray<T> fa(new T[size], size);
    fa.clear();
    return fa;
  }
  
  /**
   * Destroys a BaseIntegerArray<T> created with the create() method.
   * @param array the BaseIntegerArray<T> to be destroyed.
   * @remarks the BaseIntegerArray<T> object passed as an argument should not be used again after invoking this method.
   * @remarks a BaseIntegerArray<T> object that has not been created by the BaseIntegerArray::create() method might cause an exception if passed as an argument to this method.
  */
  static void destroy(BaseIntegerArray<T> array){
    delete array.data;
  }

  /**
   * Bitshift the array values, saturating.
   *
   * @param shiftValue number of positions to shift. A positive value will shift left, a negative value will shift right.
   */
  void shift(int shiftValue){
    if (shiftValue > 0) {
      for(int i = 0; i < size; i++){
        data[i] <<= shiftValue;
      }
    }
    else if (shiftValue < 0) {
      for(int i = 0; i < size; i++){
        data[i] >>= -shiftValue;
      }
    }
  }
};

typedef BaseIntegerArray<int32_t> IntArray;
typedef BaseIntegerArray<uint8_t> ByteArray;

#endif /* __IntArray_h__ */
