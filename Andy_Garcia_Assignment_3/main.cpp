/*
 * =====================================================================================
 * Name of Program: CEO Email Priority System (EECS 348 Assignment 3)
 * Description: Implements a priority queue using a custom list-based MaxHeap class 
 *              to manage and prioritize CEO emails based on sender category and date.
 * Inputs: Commands read from standard input (cin): EMAIL, NEXT, READ, COUNT.
 * Output: Text output sent to terminal (cout) displaying top email or email counts.
 * Author: Andy Garcia
 * Creation Date: 2026-09-28
 * Revision Date: 2026-09-29
 * Revisions: Fixed spacing issue by appending blank lines after COUNT and NEXT outputs.
 * Collaborators: None
 * Other Sources: ChatGPT (Base starter implementation), Google Gemini to assist with comments and revisions
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

// Source: Combination of ChatGPT base code and Andy and Gemini refactoring
// Helper function to trim leading and trailing whitespace from a string
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n"); // Find first non-whitespace character
    if (first == string::npos) return ""; // Return empty string if all spaces
    size_t last = str.find_last_not_of(" \t\r\n"); // Find last non-whitespace character
    return str.substr(first, (last - first + 1)); // Extract trimmed substring
}

// =====================================================================================
// Class: Email
// Represents an individual email entity with priority evaluation methods.
// Source: Authored by Andy Garcia and Gemini (refactored from ChatGPT struct)
// =====================================================================================
class Email {
private:
    string senderCategory; // Category of the sender (e.g., Boss, Subordinate)
    string subject;        // Subject line of the email
    string dateStr;        // Original date string formatted as MM-DD-YYYY
    int dateKey;           // Computed integer key YYYYMMDD for chronological comparison
    int priority;          // Numerical priority based on sender category (5 highest, 1 lowest)

    // Helper method to convert MM-DD-YYYY formatted string into YYYYMMDD integer key
    int parseDateKey(const string& dStr) {
        int m = 0, d = 0, y = 0; // Initialize date variables
        char dash1, dash2;       // Delimiter placeholders
        stringstream ss(dStr);   // Create stringstream from date string
        ss >> m >> dash1 >> d >> dash2 >> y; // Extract month, day, year
        return y * 10000 + m * 100 + d; // Construct YYYYMMDD integer
    }

    // Helper method to assign numerical priority based on category string
    int assignPriority(const string& category) {
        if (category == "Boss") return 5;                           // Highest priority
        if (category == "Subordinate") return 4;                    // Second priority
        if (category == "Peer") return 3;                           // Third priority
        if (category == "Important Person" || category == "ImportantPerson") return 2; // Fourth priority
        if (category == "Other Person" || category == "OtherPerson") return 1;         // Lowest priority
        return 0;                                                   // Unknown category default
    }

public:
    // Default constructor
    Email() : senderCategory(""), subject(""), dateStr(""), dateKey(0), priority(0) {} // Initialize empty email

    // Parameterized constructor
    Email(string cat, string subj, string d) {
        senderCategory = trim(cat);            // Store trimmed category name
        subject = trim(subj);                  // Store trimmed subject line
        dateStr = trim(d);                     // Store trimmed date string
        priority = assignPriority(senderCategory); // Calculate priority rank
        dateKey = parseDateKey(dateStr);       // Calculate comparison key for date
    }

    // Getters
    string getSenderCategory() const { return senderCategory; } // Return sender category
    string getSubject() const { return subject; }               // Return subject line
    string getDateStr() const { return dateStr; }               // Return formatted date string
    int getPriority() const { return priority; }                // Return calculated priority
    int getDateKey() const { return dateKey; }                  // Return integer date key

    // Compares this email with another. Returns true if this email has HIGHER priority.
    bool isHigherPriorityThan(const Email& other) const {
        if (this->priority != other.priority) { // Primary comparison: category priority
            return this->priority > other.priority; // Higher priority number wins
        }
        return this->dateKey > other.dateKey; // Secondary comparison: newer date wins
    }
};

// =====================================================================================
// Class: MaxHeap
// List-based dynamic array implementation of a MaxHeap for priority queuing.
// Source: Authored by ChatGPT and revised by Andy Garcia and Gemini
// =====================================================================================
class MaxHeap {
private:
    Email* heapArray; // Dynamic array storing heap nodes
    int heapSize;     // Current number of elements in heap
    int capacity;     // Total allocated capacity of array

    // Doubles array capacity when array is full
    void resize() {
        int newCapacity = capacity * 2;              // Double current capacity
        Email* newArray = new Email[newCapacity];    // Allocate new dynamic array
        for (int i = 0; i < heapSize; i++) {         // Loop through existing elements
            newArray[i] = heapArray[i];              // Copy each element to new array
        }
        delete[] heapArray;                          // Deallocate old dynamic array
        heapArray = newArray;                        // Update pointer to new array
        capacity = newCapacity;                      // Update capacity member variable
    }

    // Restores MaxHeap property by moving node at index upward
    void heapifyUp(int index) {
        while (index > 0) { // Continue until reaching root
            int parent = (index - 1) / 2; // Compute parent index
            if (heapArray[index].isHigherPriorityThan(heapArray[parent])) { // Check if child > parent
                swap(heapArray[index], heapArray[parent]); // Swap child and parent elements
                index = parent; // Move index up to parent position
            } else {
                break; // MaxHeap property satisfied
            }
        }
    }

    // Restores MaxHeap property by moving node at index downward
    void heapifyDown(int index) {
        while (true) {
            int left = 2 * index + 1;  // Left child index
            int right = 2 * index + 2; // Right child index
            int largest = index;       // Assume current index is largest

            if (left < heapSize && heapArray[left].isHigherPriorityThan(heapArray[largest])) {
                largest = left; // Left child is larger
            }
            if (right < heapSize && heapArray[right].isHigherPriorityThan(heapArray[largest])) {
                largest = right; // Right child is larger
            }
            if (largest == index) { // Heap property satisfied
                break;
            }
            swap(heapArray[index], heapArray[largest]); // Swap current with larger child
            index = largest; // Move index down to child position
        }
    }

public:
    // Constructor: Allocates initial heap memory
    MaxHeap(int initialCapacity = 10) {
        capacity = initialCapacity;           // Set initial capacity
        heapSize = 0;                         // Set initial size to zero
        heapArray = new Email[capacity];      // Allocate dynamic memory
    }

    // Destructor: Deallocates dynamic memory
    ~MaxHeap() {
        delete[] heapArray; // Free heap array from memory
    }

    // Insert new Email into the heap
    void insert(const Email& email) {
        if (heapSize == capacity) { // Check if dynamic array is full
            resize();              // Expand array size if full
        }
        heapArray[heapSize] = email;  // Place email at end of heap array
        heapifyUp(heapSize);          // Rebalance heap upwards
        heapSize++;                   // Increment element count
    }

    // Returns the highest priority email without removing it
    Email peek() const {
        return heapArray[0]; // Return root node
    }

    // Removes and returns the highest priority email
    Email extractMax() {
        Email topEmail = heapArray[0];            // Store root node to return
        heapArray[0] = heapArray[heapSize - 1];   // Move last element to root position
        heapSize--;                               // Decrement element count
        if (heapSize > 0) {                       // If heap is not empty
            heapifyDown(0);                       // Rebalance heap downwards from root
        }
        return topEmail;                          // Return stored root node
    }

    // Returns current number of emails in heap
    int size() const {
        return heapSize; // Return size member variable
    }

    // Checks if heap is empty
    bool isEmpty() const {
        return heapSize == 0; // Return comparison result
    }
};

// =====================================================================================
// Main Processing Loop
// Source: Authored by Andy and Gemini (refactored parsing and output handling)
// =====================================================================================
int main() {
    MaxHeap emailQueue; // Instantiate MaxHeap priority queue object
    string line;        // Variable to hold input lines from cin

    while (getline(cin, line)) { // Read input line by line until EOF
        line = trim(line);       // Trim whitespace from input line
        if (line.empty()) continue; // Skip blank lines

        // Process EMAIL command
        if (line.rfind("EMAIL ", 0) == 0) { // Check if line starts with "EMAIL "
            string payload = line.substr(6); // Strip "EMAIL " prefix

            size_t firstComma = payload.find(','); // Find first comma delimiter
            size_t secondComma = payload.find_last_of(','); // Find last comma delimiter

            if (firstComma != string::npos && secondComma != string::npos && firstComma < secondComma) {
                string category = payload.substr(0, firstComma); // Extract category token
                string subject = payload.substr(firstComma + 1, secondComma - firstComma - 1); // Extract subject token
                string date = payload.substr(secondComma + 1); // Extract date token

                Email newEmail(category, subject, date); // Construct Email object
                emailQueue.insert(newEmail);             // Insert email into priority heap
            }
        }
        // Process NEXT command
        else if (line == "NEXT") {
            if (!emailQueue.isEmpty()) { // Check if unread emails exist
                Email nextEmail = emailQueue.peek(); // Peek at top email node
                cout << "Next email:" << endl; // Print header
                cout << "Sender: " << nextEmail.getSenderCategory() << endl; // Print category
                cout << "Subject: " << nextEmail.getSubject() << endl;       // Print subject
                cout << "Date: " << nextEmail.getDateStr() << endl;         // Print date
                cout << endl; // Output blank line after NEXT output
            }
        }
        // Process READ command
        else if (line == "READ") {
            if (!emailQueue.isEmpty()) { // Check if unread emails exist
                emailQueue.extractMax(); // Remove top priority email silently
            }
        }
        // Process COUNT command
        else if (line == "COUNT") {
            cout << "There are " << emailQueue.size() << " emails to read." << endl; // Print unread count
            cout << endl; // Output blank line after COUNT output
        }
    }

    return 0; // Exit successfully
}