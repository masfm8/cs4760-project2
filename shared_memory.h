#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

void init_shared_memory();
int *attach_shared_memory();
void detach_shared_memory();
void cleanup_shared_memory();

#endif
