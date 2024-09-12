#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

extern int shm_id;

void setup_shared_memory();
void attach_shared_memory(int **shared_clock_sec, int **shared_clock_ns);
void detach_shared_memory();

#endif
