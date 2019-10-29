#include <stdlib.h>

typedef struct
{
    int length;
    int capacity;
} BufferHeader;

#define BufferHeaderGet(buffer) ((BufferHeader *)((char *)buffer - sizeof(BufferHeader)))
#define BufferGet(buffer_header) ((void *)((char *)buffer_header + sizeof(BufferHeader)))

#define BufferCheckIfReallocationIsNeeded(buffer, item_size) \
if(BufferLength(buffer) >= BufferCapacity(buffer)) \
{ \
    BufferReallocate((void **)(&buffer), item_size, buffer ? BufferCapacity(buffer) * 2 : 32); \
} 0\

#define BufferPush(buffer, item) \
BufferCheckIfReallocationIsNeeded(buffer, sizeof item); \
buffer[BufferLength(buffer)] = item; \
BufferHeaderGet(buffer)->length++ \

#define BufferFree(buffer) free(BufferHeaderGet(buffer))

#define BufferLength(buffer) (buffer ? BufferHeaderGet(buffer)->length : 0)
#define BufferCapacity(buffer) (buffer ? BufferHeaderGet(buffer)->capacity : 0)

void BufferReallocate(void **buffer, int item_size, int new_capacity)
{
    if(!*buffer)
    {
        BufferHeader *header = malloc(sizeof *header + (item_size * new_capacity));
        header->length = 0;
        header->capacity = new_capacity;
        
        *buffer = BufferGet(header);
    } else
    {
        BufferHeader *header = BufferHeaderGet(*buffer);
        header = realloc(header, sizeof *header + (item_size * new_capacity));
        header->capacity = new_capacity;
        
        *buffer = BufferGet(header);
    }
}
