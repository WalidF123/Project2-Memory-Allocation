#include <iostream>
#include <list> // For linked list implementation
#include <cstdlib> // For sbrk()

// Struct to represent a memory chunk
struct MemoryChunk {
    std::size_t size; // Size of the chunk
    void* address; // Address of the chunk
};

// Lists to hold free and allocated memory chunks
std::list<MemoryChunk> free_list;
std::list<MemoryChunk> allocated_list;
