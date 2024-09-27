#include <iostream>
#include <list> // For linked list implementation
#include <cstdlib> // For exit()
#include <unistd.h> // for sbrk()
#include <array>

// Struct to represent a memory chunk
struct MemoryChunk {
    std::size_t requested; // Total size of the partition
    std::size_t used; // Actual size being used
    void* address; // Address of the chunk
};

// Lists to hold free and allocated memory chunks
std::list<MemoryChunk> free;
std::list<MemoryChunk> allocated;

// Fixed partition sizes as mentioned in the specification
const std::array<std::size_t, 5> chunk_sizes = {32, 64, 128, 256, 512};

// Function to find the appropriate chunk size for the allocating memory
std::size_t find_size(std::size_t requested_chunk_size) {
  for (std::size_t size : chunk_sizes) {
    if (requested_chunk_size <= size) {
      return size;
    }
  }
  return 0;
}
