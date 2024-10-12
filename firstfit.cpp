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

// Global list for allocated and free memory (now holding pointers to MemoryChunk)
std::list<MemoryChunk*> allocated_chunk_list;
std::list<MemoryChunk*> free_chunk_list;

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

// Function to request more memory for the allocation structure using sbrk() or malloc()
MemoryChunk* allocate_memory_chunk() {
    void* space = sbrk(sizeof(MemoryChunk)); // Allocate space for the `MemoryChunk` struct itself
    if (space == nullptr) {
        std::cerr << "Error: Memory allocation failed for MemoryChunk struct!" << std::endl;
        exit(EXIT_FAILURE);
    }
    return static_cast<MemoryChunk*>(space); // Cast the space to a `MemoryChunk` pointer
}

// Function to request more memory using sbrk() or malloc()
void* request_memory(std::size_t size) {
    void* address = sbrk(size);
    if (address == nullptr) {
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
        if ((*iterator)->requested >= actual) {
            MemoryChunk* alloc = *iterator;
            alloc->used = chunk_size;  // Keep track of the used size for printing purposes
            free_chunk_list.erase(iterator);  // Remove from free list
            allocated_chunk_list.push_back(alloc);  // Add to allocated list
            std::cout << "First Fit Allocated: " << chunk_size << " bytes at " << alloc->space << std::endl;
            return alloc->space;
        }
    }

    // If no suitable chunk is found, allocate new memory using sbrk() or malloc() for both memory and the chunk structure
    void* request_space = request_memory(actual);
    MemoryChunk* new_allocation = allocate_memory_chunk(); // Allocate the metadata using sbrk() or malloc()
    new_allocation->requested = actual;
    new_allocation->used = chunk_size;
    new_allocation->space = request_space;
    
    allocated_chunk_list.push_back(new_allocation); // Add to the allocated list
    std::cout << "Allocated new memory: " << chunk_size << " bytes at " << request_space << std::endl;
    return request_space;
}
// Memory De-allocation
void dealloc(void* chunk = nullptr) {
    if (chunk == nullptr) {
        // Case where no chunks were specified, deallocate based on LIFO order
        if (allocation_stack.empty()) {
            std::cerr << "No memory to deallocate!" << std::endl;
            return;
        }
        // Get the last allocated memory from the stack (LIFO)
        chunk = allocation_stack.back();
        allocation_stack.pop_back();  // Remove from the stack
        std::cout << "Deallocating memory using LIFO order at " << chunk << std::endl;
    } else {
        std::cout << "Deallocating specified memory at " << chunk << std::endl;
    }

    // Find the chunk in the allocated list
    for (auto iterator = allocated_chunk_list.begin(); iterator != allocated_chunk_list.end(); ++iterator) {
        if ((*iterator)->space == chunk) {
            MemoryChunk* deallocated = *iterator;
            allocated_chunk_list.erase(iterator);  // Remove from allocated list
            free_chunk_list.push_back(deallocated);  // Add to free list
            return;
        }
    }

    // If there is an attempt to free non-existing memory, report an error
    std::cerr << "Attempt to free un-allocated memory!" << std::endl;
    exit(EXIT_FAILURE); // Terminate the program due to a fatal error of attempting to free non-allocated memory.
}

// Function to print the status
void print_status() {
    std::cout << "\n----- Allocated List -----" << std::endl;
    for (const auto& memory : allocated_chunk_list) {
        std::cout << "Address: " << memory->space << ", Size: " << memory->requested
                  << " bytes, Used: " << memory->used << " bytes" << std::endl;
    }

    std::cout << "\n----- Free List -----" << std::endl;
    for (const auto& memory : free_chunk_list) {
        std::cout << "Address: " << memory->space << ", Size: " << memory->requested << " bytes" << std::endl;
    }
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " datafile" << std::endl;
        return 1;
    }

    std::string datafile = argv[1];

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
            void* allocated_space = first_fit_alloc(chunk_size);
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

