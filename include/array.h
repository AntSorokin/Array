#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>
#include <stdint.h>

typedef enum {
    OK_ERROR,
    OUT_OF_MEM,
    OUT_OF_BOUNDS
} array_error;

/** 
*   Creates a struct that stores the state of a dynamically resizable array
*   @param T Type stored in array struct
*   @example array_struct(char) a;
*/
#define array_struct(T) \
    struct { \
        T* buf; \
        uint64_t capacity; \
        uint64_t size; \
        uint64_t min_capacity; \
        array_error error; \
    }

/** 
*    Initializes all variables in array struct
*   @param T Type stored in array struct
*   @param array_struct Array struct to initialize
*   @param init_capacity Initial and minimum capacity of the resizeable array
*   @warning init_capacity must be >= 1 
*   @warning The buf is stored in the heap and needs to be released by array_free
*   @note Can modify error state to OUT_OF_MEM
*   @example array_init(char, a, 10);
*/ 
#define array_init(T, array_struct, init_capacity) do { \
        array_struct.buf = calloc(init_capacity, sizeof(T)); \
        if(array_struct.buf) { \
            array_struct.size = 0; \
            array_struct.min_capacity = init_capacity; \
            array_struct.capacity = init_capacity; \
            array_struct.error = OK_ERROR; \
        } \
        else { \
            array_struct.error = OUT_OF_MEM; \
        } \
    } while(0)

/**
*   Adds value to array at the tail
*   @param T Type stored in array struct
*   @param array_struct Array struct to add to
*   @param val Value to store
*   @note Will not execute if error state is not OK_ERROR
*   @note Can modify error state to OUT_OF_MEM
*   @example array_add(char, a, 'a');
*/
#define array_add(T, array_struct, val) do { \
        if(array_struct.error == OK_ERROR) { \
            if(array_struct.size == array_struct.capacity) { \
                array_struct.capacity *= 2; \
                T* temp = realloc(array_struct.buf, sizeof(T) * array_struct.capacity); \
                if(!temp) { \
                    array_struct.error = OUT_OF_MEM; \
                    break; \
                } \
                array_struct.buf = temp; \
            } \
            array_struct.buf[array_struct.size++] = val; \
        } \
    } while(0)

/**
*   Adds value to array at index, and shifts every value after index to the right
*   @param T Type stored in array struct
*   @param array_struct Array struct to add to
*   @param index Index to store value at
*   @param val Value to store
*   @note Will not execute if error state is not OK_ERROR
*   @note Can modify error state to OUT_OF_MEM or OUT_OF_BOUNDS
*   @example array_add(char, a, 1, 'b');
*/
#define array_add_index(T, array_struct, index, val) do { \
        if(array_struct.error == OK_ERROR) { \
            if(array_struct.size == array_struct.capacity) { \
                array_struct.capacity *= 2; \
                T* temp = realloc(array_struct.buf, sizeof(T) * array_struct.capacity); \
                if(!temp) { \
                    array_struct.error = OUT_OF_MEM; \
                    break; \
                } \
                array_struct.buf = temp; \
            } \
            if(0 <= index && index <= array_struct.size) { \
                array_struct.size++; \
                for(uint64_t i = array_struct.size - 1; index < i; --i) { \
                    array_struct.buf[i] = array_struct.buf[i - 1]; \
                } \
                array_struct.buf[index] = val; \
            } \
            else { \
                array_struct.error = OUT_OF_BOUNDS; \
            } \
        } \
    } while(0)

/**
* Overwrites value at specified index
* @param array_struct Array struct to modify
* @param index Index value to overwrite
* @param val Value to write at index
* @note Will not execute if error state is not OK_ERROR
* @note Can modify error state to OUT_OF_BOUNDS
* @example array_set(a, 1, 'c');
*/
#define array_set(array_struct, index, val) do { \
        if(array_struct.error == OK_ERROR) { \
            if(0 <= index && index < array_struct.size) { \
                array_struct.buf[index] = val; \
            } \
            else { \
                array_struct.error = OUT_OF_BOUNDS; \
            } \
        } \
    } while(0) 


/**
 * Gets value at specified index
 * @param array_struct Array struct to get from
 * @param index Index value to get
 * @param ret_val Where value at index is to be stored
 * @note Will not execute if error state is not OK_ERROR
 * @note Can modify error state to OUT_OF_BOUNDS
 * @example 
 * //Value will be store in temp
 * char temp;
 * array_get(a, 0, temp);
 */
#define array_get(array_struct, index, ret_val) do { \
        if(array_struct.error == OK_ERROR) { \
            if(0 <= index && index < array_struct.size) { \
                ret_val = array_struct.buf[index]; \
            } \
            else { \
                array_struct.error = OUT_OF_BOUNDS; \
            } \
        } \
    } while(0)

/**
*   Removes value at tail
*   @param T Type stored in array struct
*   @param array_struct Array struct to be removed from
*   @note Will not execute if error state is not OK_ERROR
*   @note Can modify error state to OUT_OF_MEM or OUT_OF_BOUNDS
*   @example array_remove(char, a);
*/
#define array_remove(T, array_struct) do { \
        if(array_struct.error == OK_ERROR) { \
            if(array_struct.size > 0) { \
                if(--(array_struct.size) == array_struct.capacity / 2 && array_struct.capacity != array_struct.min_capacity) { \
                    array_struct.capacity /= 2; \
                    T* temp = realloc(array_struct.buf, sizeof(T) * array_struct.capacity); \
                    if(!temp) { \
                        array_struct.error = OUT_OF_MEM; \
                        break; \
                    } \
                    array_struct.buf = temp; \
                } \
            } \
            else { \
                array_struct.error = OUT_OF_BOUNDS; \
            } \
        } \
    } while(0)

/**
*   Removes value at index and shifts all values after index to the left
*   @param T Type stored in array struct
*   @param array_struct Array struct to be removed from
*   @param index Index to remove value at
*   @note Will not execute if error state is not OK_ERROR
*   @note Can modify error state to OUT_OF_MEM or OUT_OF_BOUNDS
*   @example array_remove_index(char, a, 0);
*/
#define array_remove_index(T, array_struct, index) do { \
        if(array_struct.error == OK_ERROR) { \
            if(array_struct.size > 0 && 0 <= index && index < array_struct.size) { \
                for(uint64_t i = index; i < array_struct.size - 1; ++i) { \
                    array_struct.buf[i] = array_struct.buf[i + 1]; \
                } \
                if(--(array_struct.size) == array_struct.capacity / 2 && array_struct.capacity != array_struct.min_capacity) { \
                    array_struct.capacity /= 2; \
                    T* temp = realloc(array_struct.buf, sizeof(T) * array_struct.capacity); \
                    if(!temp) { \
                        array_struct.error = OUT_OF_MEM; \
                        break; \
                    } \
                    array_struct.buf = temp; \
                } \
            } \
            else { \
                array_struct.error = OUT_OF_BOUNDS; \
            } \
        } \
    } while(0)

/** 
* Gets the current array size
* @param array_struct Array struct to return size of
* @return Array size
* @example uint64_t size = array_size(a); 
*/
#define array_size(array_struct) array_struct.size

/**
* Gets the current error state of the array
* @param array_struct Array struct to return error state of
* @returns An enum with a value found in array_error
* @example array_error state = array_error(a); 
*/
#define array_error(array_struct) array_struct.error

/**
* Attempts to free the array from the heap
* @param array_struct Array struct to free the buffer of
* @example array_free(a);
*/
#define array_free(array_struct) do { \
        if(array_struct.buf != NULL) { \
            free(array_struct.buf); \
        } \
    } while(0)
    
#endif


