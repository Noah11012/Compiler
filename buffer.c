#include <stdlib.h>

typedef struct
{
    int length;
    int capacity;
} BufferHeader;

#define buffer_header_get(buffer) ((BufferHeader *)((char *)buffer - sizeof(BufferHeader)))
#define buffer_get(buffer_header) ((void *)((char *)buffer_header + sizeof(BufferHeader)))

#define buffer_check_if_reallocation_is_needed(buffer, item_size) \
if(buffer_length(buffer) >= buffer_capacity(buffer)) \
{ \
    buffer_reallocate((void **)(&buffer), item_size, buffer ? buffer_capacity(buffer) * 2 : 32); \
} 0\

#define buffer_push(buffer, item) \
buffer_check_if_reallocation_is_needed(buffer, sizeof item); \
buffer[buffer_length(buffer)] = item; \
buffer_header_get(buffer)->length++ \

void buffer_reallocate(void **buffer, int item_size, int new_capacity)
{
    if(!*buffer)
    {
        BufferHeader *header = malloc(sizeof *header + (item_size * new_capacity));
        header->length = 0;
        header->capacity = new_capacity;
        
        *buffer = buffer_get(header);
    } else
    {
        BufferHeader *header = buffer_header_get(*buffer);
        header = realloc(header, sizeof *header + (item_size * new_capacity));
        header->capacity = new_capacity;
        
        *buffer = buffer_get(header);
    }
}

#define buffer_length(buffer) (buffer ? buffer_header_get(buffer)->length : 0)
#define buffer_capacity(buffer) (buffer ? buffer_header_get(buffer)->capacity : 0)
