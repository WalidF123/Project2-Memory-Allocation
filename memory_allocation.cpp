#include <iostream>
#include <cstdlib> // For exit()
#include <unistd.h> // for sbrk()
#include <list>
#include <array>

// Define the struct for allocation
struct MemoryChunk {
  std::size_t requested; // Total size of the partition
  std::size_t used; // Actual size being used
  void* space; // Pointer to the memory space of the allocation
};

// Global list for allocated and free memory
std::list<MemoryChunk> allocated;
std::list<MemoryChunk> free;

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

// Memory allocation
void* alloc(std::size_t chunk_size) {
  std::size_t actual;

  // If the requested chunk size is bigger than the partition, assign immediately
  if (chunk_size > 512) {
    actual = chunk_size;  // No need for partition size, use the requested size
  } else {
    actual = find_size(chunk_size);
    if (actual == 0) {
    std::cerr << "Invalid chunk size!" << std::endl;
    return nullptr;
    }
  }

  // Scan through the free list for a free chunk (first fit strategy?)
  for (auto iterator = free.begin(); iterator != free.end(); ++iterator) {
    if (iterator->requested >= actual) {
      // Found a chunk to allocate the memory, move the chunk to allocated list
      MemoryChunk alloc = *iterator;
      alloc.used = chunk_size; // Keep track the used size for printing purposes
      free.erase(iterator); // delete the free space
      allocated.push_back(alloc);
      return alloc.space;
    }
  }

  // If no suitable fixed-size chunk is found, make a OS call to request for more memory
  void* request_space = sbrk(actual);
  if (request_space == (void*)-1) {
    std::cerr << "There is an error in memory allocation!" << std::endl;
    return nullptr;
  }

  // Create a new allocation and add it to the allocated
  MemoryChunk new_allocation = {actual, chunk_size, request_space};
  allocated.push_back(new_allocation);
  return request_space;
}

// Memory de-allocation
void dealloc(void* chunk) {
  // Find the chunk in the allocated linked list
  for (auto iterator = allocated.begin(); iterator != allocated.end(); ++iterator) {
    if (iterator->space == chunk) {
      // Move the chunk back to the free list
      MemoryChunk deallocated = *iterator;
      allocated.erase(iterator);  // Remove from the list
      free.push_back(deallocated);  // Add to the end
      return;
    }
  }

  // If there is an attempt to de-allocate a non-existent chunk, terminate the program
  std::cerr << "Attempt to free un-allocated/non-existing memory!" << std::endl;
  exit(EXIT_FAILURE);
}

// Function to print the status
void print_status() {
  std::cout << "\n -----Allocated List-----" << std::endl;
  for (const auto& memory : allocated) {
    std::cout << "Chunk stored at address: " << memory.space
              << ", Total allocated size: " << memory.requested
              << " bytes, Total used size: " << memory.used << " bytes" << std::endl; 
  }

  std::cout << "\n -----Free List-----" << std::endl;
  for (const auto& memory : free) {
    std::cout << "Chunk stored at address: " << memory.space
              << ", Total allocated size: " << memory.requested
              << " bytes" << std::endl; 
  }
}