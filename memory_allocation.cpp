// #include <iostream>
// #include <cstdlib> // For exit() and malloc()
// // #include <unistd.h> // for sbrk()
// #include <list>
// #include <array>

// // Define the struct for allocation
// struct MemoryChunk {
//   std::size_t requested; // Total size of the partition
//   std::size_t used; // Actual size being used
//   void* space; // Pointer to the memory space of the allocation
// };

// // Global list for allocated and free memory
// std::list<MemoryChunk> allocated_chunk_list;
// std::list<MemoryChunk> free_chunk_list;

// // Fixed partition sizes as mentioned in the specification
// const std::array<std::size_t, 5> chunk_sizes = {32, 64, 128, 256, 512};

// // Function to find the appropriate chunk size for the allocating memory
// std::size_t find_size(std::size_t requested_chunk_size) {
//   for (std::size_t size : chunk_sizes) {
//     if (requested_chunk_size <= size) {
//       return size;
//     }
//   }
//   return 0;
// }

// // Function to request more memory if exceed fixed partition by using malloc()
// void* request_memory(std::size_t size) {
//   // Apply malloc() to allocate memory
//   void* address = std::malloc(size);
//   if (!address) {
//     std::cerr << "There is an error in memory allocation!" << std::endl;
//     exit(EXIT_FAILURE);
//   }
//   return address;
// }

// // Memory allocation
// void* alloc(std::size_t chunk_size) {
//   std::size_t actual;

//   // If the requested chunk size is bigger than the partition, assign immediately
//   if (chunk_size > 512) {
//     actual = chunk_size;  // No need for partition size, use the requested size
//   } else {
//     actual = find_size(chunk_size);
//     if (actual == 0) {
//     std::cerr << "Invalid chunk size!" << std::endl;
//     return nullptr;
//     }
//   }

//   // Scan through the free list for a free chunk (first fit strategy?)
//   for (auto iterator = free_chunk_list.begin(); iterator != free_chunk_list.end(); ++iterator) {
//     if (iterator->requested >= actual) {
//       // Found a chunk to allocate the memory, move the chunk to allocated list
//       MemoryChunk alloc = *iterator;
//       alloc.used = chunk_size; // Keep track the used size for printing purposes
//       free_chunk_list.erase(iterator); // delete the free space
//       allocated_chunk_list.push_back(alloc);
//       return alloc.space;
//     }
//   }

//   // If no suitable fixed-size chunk is found, make a malloc call to request for more memory
//   void* request_space = request_memory(actual);

//   // Create a new allocation and add it to the allocated
//   MemoryChunk new_allocation = {actual, chunk_size, request_space};
//   allocated_chunk_list.push_back(new_allocation);
//   return request_space;
// }





// // Memory de-allocation
// void dealloc(void* chunk) {
//   // Find the chunk in the allocated linked list
//   for (auto iterator = allocated_chunk_list.begin(); iterator != allocated_chunk_list.end(); ++iterator) {
//     if (iterator->space == chunk) {
//       // Move the chunk back to the free list
//       MemoryChunk deallocated = *iterator;
//       allocated_chunk_list.erase(iterator);  // Remove from the list
//       free_chunk_list.push_back(deallocated);  // Add to the end
//       return;
//     }
//   }

//   // If there is an attempt to de-allocate a non-existent chunk, terminate the program
//   std::cerr << "Attempt to free un-allocated/non-existing memory!" << std::endl;
//   exit(EXIT_FAILURE);
// }



// // Function to print the status
// void print_status() {
//   std::cout << "\n -----Allocated List-----" << std::endl;
//   for (const auto& memory : allocated_chunk_list) {
//     std::cout << "Chunk stored at address: " << memory.space
//               << ", Total allocated size: " << memory.requested
//               << " bytes, Total used size: " << memory.used << " bytes" << std::endl; 
//   }

//   std::cout << "\n -----Free List-----" << std::endl;
//   for (const auto& memory : free_chunk_list) {
//     std::cout << "Chunk stored at address: " << memory.space
//               << ", Total allocated size: " << memory.requested
//               << " bytes" << std::endl; 
//   }
// }

// // Testing purposes only

// int main() {
//     // Example usage of the memory allocator
//     void* chunk1 = alloc(50);  // Allocates 64 bytes
//     void* chunk2 = alloc(200); // Allocates 256 bytes

//     std::cout << "\nAfter allocations:" << std::endl;
//     print_status();

//     dealloc(chunk1); // Deallocate the first chunk
//     dealloc(chunk2); // Deallocate the second chunk

//     std::cout << "\nAfter deallocations:" << std::endl;
//     print_status();

//     // Testing fatal error (attempting to deallocate unallocated memory)
//     // Uncomment the following line to test the error case:
//     // dealloc((void*)0x12345); // This will trigger the fatal error

//     return 0;
// }




#include <unistd.h>

#include <iostream>
#include <cstdlib> 
#include <list>
#include <array>
#include <fstream>
#include <vector>

// Define the struct for allocation
struct MemoryChunk {
  std::size_t requested; // Total size of the partition
  std::size_t used;      // Actual size being used
  void* space;           // Pointer to the memory space of the allocation
};

// Global list for allocated and free memory
std::list<MemoryChunk> allocated_chunk_list;
std::list<MemoryChunk> free_chunk_list;

// Stack to keep track of allocations (LIFO order)
std::vector<void*> allocation_stack; 

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

// // Function to request more memory using malloc()
// void* request_memory(std::size_t size) {
//   void* address = std::malloc(size);
//   if (!address) {
//     std::cerr << "Memory allocation failed!" << std::endl;
//     exit(EXIT_FAILURE);
//   }
//   return address;
// }
// Function to request more memory using sbrk()
void* request_memory(std::size_t size) {
  // Use sbrk to increase the program's data space
  void* address = sbrk(size);
  if (address == (void*)-1) {
    std::cerr << "Memory allocation failed!" << std::endl;
    exit(EXIT_FAILURE);
  }
  return address;
}
// FIRST FIT Memory Allocation
void* first_fit_alloc(std::size_t chunk_size) {
  std::size_t actual;

  // If the requested chunk size is bigger than the largest partition (512), allocate exact size
  if (chunk_size > 512) {
    actual = chunk_size; 
  } else {
    actual = find_size(chunk_size);
    if (actual == 0) {
      std::cerr << "Invalid chunk size!" << std::endl;
      return nullptr;
    }
  }

  // First-fit: Find the first free chunk that can fit the requested size
  for (auto iterator = free_chunk_list.begin(); iterator != free_chunk_list.end(); ++iterator) {
    if (iterator->requested >= actual) {
      MemoryChunk alloc = *iterator;
      alloc.used = chunk_size;  // Keep track of the used size for printing purposes
      free_chunk_list.erase(iterator);  // Remove from free list
      allocated_chunk_list.push_back(alloc);  // Add to allocated list
      std::cout << "First Fit Allocated: " << chunk_size << " bytes at " << alloc.space << std::endl;
      return alloc.space;
    }
  }

  // If no suitable chunk is found, allocate new memory using sbrk
  void* request_space = request_memory(actual);

  // Create a new allocation and add it to the allocated list
  MemoryChunk new_allocation = {actual, chunk_size, request_space};
  allocated_chunk_list.push_back(new_allocation);
  std::cout << "Allocated new memory: " << chunk_size << " bytes at " << request_space << std::endl;
  return request_space;
}

// BEST FIT Memory Allocation
void* best_fit_alloc(std::size_t chunk_size) {
  std::size_t actual;

  // If the requested chunk size is bigger than the largest partition (512), allocate exact size
  if (chunk_size > 512) {
    actual = chunk_size;  
  } else {
    actual = find_size(chunk_size);
    if (actual == 0) {
      std::cerr << "Invalid chunk size!" << std::endl;
      return nullptr;
    }
  }

  // Best-fit: Find the smallest free chunk that can fit the requested size
  auto best_fit_it = free_chunk_list.end();
  std::size_t best_fit_size = SIZE_MAX;

  for (auto iterator = free_chunk_list.begin(); iterator != free_chunk_list.end(); ++iterator) {
    if (iterator->requested >= actual && iterator->requested < best_fit_size) {
      best_fit_it = iterator;
      best_fit_size = iterator->requested;
    }
  }

  // If we found a suitable chunk, allocate it
  if (best_fit_it != free_chunk_list.end()) {
    MemoryChunk alloc = *best_fit_it;
    alloc.used = chunk_size; // Keep track of the used size for printing purposes
    free_chunk_list.erase(best_fit_it); // Remove from free list
    allocated_chunk_list.push_back(alloc); // Add to allocated list
    std::cout << "Best Fit Allocated: " << chunk_size << " bytes at " << alloc.space << std::endl;
    return alloc.space;
  }

  // If no suitable chunk is found, allocate new memory using sbrk
  void* request_space = request_memory(actual);

  // Create a new allocation and add it to the allocated list
  MemoryChunk new_allocation = {actual, chunk_size, request_space};
  allocated_chunk_list.push_back(new_allocation);
  std::cout << "Allocated new memory: " << chunk_size << " bytes at " << request_space << std::endl;
  return request_space;
}

// Memory De-allocation
void dealloc() {
  if (allocation_stack.empty()) {
    std::cerr << "No memory to deallocate!" << std::endl;
    return;
  }

  // Get the last allocated memory from the stack (LIFO)
  void* last_alloc = allocation_stack.back();
  allocation_stack.pop_back();  // Remove from the stack

  // Find the chunk in the allocated list
  for (auto iterator = allocated_chunk_list.begin(); iterator != allocated_chunk_list.end(); ++iterator) {
    if (iterator->space == last_alloc) {
      std::cout << "Deallocating memory at " << last_alloc << std::endl;
      MemoryChunk deallocated = *iterator;
      allocated_chunk_list.erase(iterator);  // Remove from allocated list
      free_chunk_list.push_back(deallocated);  // Add to free list
      return;
    }
  }

  // If there is an attempt to free non-existing memory, report an error
  std::cerr << "Attempt to free un-allocated memory!" << std::endl;
}

// Function to print the status
void print_status() {
  std::cout << "\n----- Allocated List -----" << std::endl;
  for (const auto& memory : allocated_chunk_list) {
    std::cout << "Address: " << memory.space << ", Size: " << memory.requested
              << " bytes, Used: " << memory.used << " bytes" << std::endl;
  }

  std::cout << "\n----- Free List -----" << std::endl;
  for (const auto& memory : free_chunk_list) {
    std::cout << "Address: " << memory.space << ", Size: " << memory.requested << " bytes" << std::endl;
  }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " [firstfit|bestfit] datafile" << std::endl;
        return 1;
    }

    std::string strategy = argv[1];
    std::string datafile = argv[2];

    // Open the datafile for reading
    std::ifstream file(datafile);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open datafile: " << datafile << std::endl;
        return 1;
    }

    std::string operation;
    std::size_t chunk_size;

    while (file >> operation) {
        if (operation == "alloc:") {
            file >> chunk_size;

            void* allocated_space = nullptr;
            if (strategy == "firstfit") {
                allocated_space = first_fit_alloc(chunk_size);
            } else if (strategy == "bestfit") {
                allocated_space = best_fit_alloc(chunk_size);
            } else {
                std::cerr << "Unknown allocation strategy: " << strategy << std::endl;
                return 1;
            }

            if (allocated_space != nullptr) {
                allocation_stack.push_back(allocated_space); // Track allocated space
            }
        } else if (operation == "dealloc") {
            dealloc(); // Deallocate the last allocated memory
        }
    }

    file.close();

    // Print the final status
    print_status();

    return 0;
}
