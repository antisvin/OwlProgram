#ifndef __RESOURCE_STORAGE_H__
#define __RESOURCE_STORAGE_H__

#include <stdint.h>
#include <cstring>
#include "OpenWareMidiControl.h"
#include "ServiceCall.h"
#include "ProgramVector.h"
#include "FloatArray.h"


/**
 * Resource class is an interface for accessing data stored on flash. Use one of two methods
 * to access it:
 * - get() returns a read-only reference to data stored on flash
 * - load() copies data to a new buffer in dynamic memory
 *
 * After that you can access raw data with getData() method. This method also has a
 * template version getData<typename Payload>() that is useful when you want to load
 * data into a datastructure.
 *
 * If data stored in resource is an array, it can be converted to FloatArray or other array
 * classes with asArray() template method.
 */
class Resource {
public:
    /**
     * Default constructor
     */
    Resource() = default;

    /**
     * Construct a resource with known name
     */
    Resource(const char* name) {
        setName(name);
    }

    /**
     * Resource destructor
     */
    ~Resource() {
    }

    /**
     * Check if data is available
     */
    bool hasData() const {
        return data != NULL;
    }

    /**
     * Get pointer to data. This may be NULL if no buffer is assigned yet.
     *
     * @param offset: offset in bytes
     */
    uint8_t* getData(uint32_t offset = 0) {
        return data;
    }

    /**
     * Get pointer to data. This may be NULL if no buffer is assigned yet.
     * This is the **const** version
     *
     * @param offset: offset in bytes
     */
    const uint8_t* getData(uint32_t offset = 0) const {
        return const_cast<const uint8_t*>(data + offset);
    }

    /**
     * Template method for converting data to an object of particular class
     *
     * @param offset: offset in bytes
     */
    template<typename Payload>
    const Payload& getData(uint32_t offset = 0) const {
        return *(reinterpret_cast<const Payload*>(data + offset));
    }

    /**
     * Template method for converting data to an object. This is the **const** version
     *
     * @param offset: offset in bytes
     */
    template<typename Payload>
    Payload& getData(uint32_t offset = 0) {
        return *(reinterpret_cast<Payload*>(data + offset));
    }    

    /**
     * Set data
     * 
     * @param src source buffer address
     * @param length number of bytes to use
     * @param copy flag to copy data to a new buffer instead of changing current buffer address
     */
    bool setData(void* src, uint32_t length, bool copy);

    /**
     * Template method for setting data from an object
     * 
     * @param src source object
     * @param copy flag to copy data to a new buffer instead of changing current buffer address
     */
    template<typename Payload>
    bool setData(const Payload& src, bool copy){
        return setData((void*)&src, sizeof(Payload), copy);
    }

    /**
     * Get buffer size in bytes
     */
    uint32_t getSize() const {
        return size;
    }    

    /**
     * Set buffer size
     * 
     * @param new_size new size in bytes
     */
    void setSize(uint32_t new_size) {
        size = new_size;
    }

    /**
     * Get resource name
     */
    const char* getName() const {
        return name;
    }

    /**
     * Set resource neam
     * 
     * @param new_name new name
     */
    void setName(const char* new_name) {
        strncpy(name, new_name, 24);
    }

    /**
     * Check if resource is set to a valid buffer with some data
     */
    bool isValid() const {
        return size > 0 && hasData();
    }

    /**
     * Check if buffer is allocated on the heap
     */
    bool isAllocated() const {
        return allocated;
    }

    /**
     * Array conversion
     *
     * @param offset: offset in bytes
     * @param max_size maximum size, actual size can be smaller depending on object size
     */
    template<typename Array, typename Element>
    const Array asArray(uint32_t offset = 0, uint32_t max_size = 0xFFFFFFFF) const;

    /**
     * Array conversion. This is the **const** version
     *
     * @param offset: offset in bytes
     * @param max_size maximum size, actual size can be smaller depending on object size
     */
    template<typename Array, typename Element>
    Array asArray(uint32_t offset = 0, uint32_t max_size = 0xFFFFFFFF);

    /**
     * Create a new resource with allocated buffer
     * 
     * @param size buffer size in bytes
     */
    static Resource create(uint32_t size);

    /**
     * Destroy allocated buffer for a resource
     */
    static void destroy(Resource resource);

    /**
     * Get resource from storage
     * 
     * This method will never allocate a new buffer, only using data stored on flash
     * directly. For this reason it returns a pointer **const** resource object. This
     * object should be deleted after using, but resource shouldn't be destroyed.
     * 
     * @param name resource name
     * @param offest offset in bytes
     * @param max_size maximum size, actual size can be smaller depending on object size
     */
    static const Resource* get(
        const char* name, uint32_t offset = 0, uint32_t max_size = 0xFFFFFFFF);

    /**
     * Load resource to a new buffer in memory
     * 
     * This method will always allocate a buffer in memory and copy object into it. This
     * buffer is mutable. The object should be destroyed after it's used to deallocated
     * stored buffer.
     * 
     * @param name resource name
     * @param offest offset in bytes
     * @param max_size maximum size, actual size can be smaller depending on object size
     */
    static Resource load(
        const char* name, uint32_t offset = 0, uint32_t max_size = 0xFFFFFFFF);

protected:
    char name[24];
    uint32_t size;
    uint8_t* data = NULL;
    bool allocated;
};
#endif