/** @file char_buffer.h
 *
 *
 */
#ifndef EDITOR_CHAR_BUFFER_H
#define EDITOR_CHAR_BUFFER_H

#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

#define BUF_INIT {NULL, 0, 0} // practical initialisation macro

/** Basic byte buffer with memory mangement capabilities. */
struct CharBuffer {
  char *buf;
  size_t len;
  size_t capacity;
};

// TODO: Comments
int CharBuffer_init(struct CharBuffer *char_buffer);
// Note this function assumes that the buffer is already initialized
int CharBuffer_grow(struct CharBuffer *char_buffer, size_t needed);

/** Add a byte array at the end of a preallocated CharBuffer
 *
 * @param char_buffer The desination buffer
 * @param s The string to be copied
 * @param len The maximum number of bytes that will be copied if no null
 * terminator is encountered
 *
 * @return 0 if the copy was succesful, -1 if an error was encountered.
 *
 * @note If an error is encountered during the copy, the buffer is left in a
 * state that is functionally identical the the initial state.
 */
int CharBuffer_append_text(struct CharBuffer *char_buffer, const char *s,
                           size_t len);

/**
 * Insert a byte array in a buffer at the given index
 *
 * @param char_buffer The desination buffer
 * @param s The string to be copied
 * @param len The maximum number of bytes that will be copied if no null
 * terminator is encountered
 *
 * @return 0 if the copy was succesful, -1 if an error was encountered.
 *
 * @note If an error is encountered during the copy, the buffer is left in a
 * state that is functionally identical the the initial state.*/
int CharBuffer_insert_text(struct CharBuffer *char_buffer, const char *s,
                           size_t index, size_t len);

/** Remove the given portion of data from the buffer. This function will not
 * overwrite anything beyond the limits of the buffer.
 */
void CharBuffer_remove_chars(struct CharBuffer *char_buffer, size_t index,
                             size_t len);

void CharBuffer_append_int(struct CharBuffer *char_buffer, int n);

/**
 @brief Deallocate the internal buffer, then sets the lenght to 0.

 @note that if you used an externally/stack managed buffer to as internal
 buffer, calling this function may cause a double free and/or a segfault.

 @param char_buffer
 */
void CharBuffer_free(struct CharBuffer *char_buffer);

#ifdef __cplusplus
}
#endif
#endif
