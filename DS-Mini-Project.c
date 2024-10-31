#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SEATS 10
#define HASH_TABLE_SIZE 20
#define QUEUE_SIZE 50

typedef struct Passenger {
    int id;                     // Unique ID for the passenger
    char name[100];            // Name of the passenger (up to 100 characters)
    int seatNumber;            // Seat number assigned to the passenger
    struct Passenger* next;    // Pointer to the next passenger in the linked list
} Passenger;

typedef struct Flight {
    int flightNumber;          // Unique flight number
    int availableSeats;        // Number of available seats on the flight
    float ticketCost;          // Cost of the ticket for the flight
    char date[12];            // Date of the flight (formatted as a string)
    char departureTime[10];    // Departure time of the flight
    char arrivalTime[10];      // Arrival time of the flight
    char source[50];          // Source location of the flight
    char destination[50];     // Destination of the flight
    Passenger* passengerList;  // Pointer to the first passenger in the linked list for this flight
    struct Flight* next;       // Pointer to the next flight in the linked list
} Flight;

// Hash table to store flights using flight number as key
Flight* flightHashTable[HASH_TABLE_SIZE];

// Queue structure for booking requests
typedef struct BookingRequest {
    int flightNumber;
    int numSeats;
} BookingRequest;

BookingRequest bookingQueue[QUEUE_SIZE];
int front = -1;
int rear = -1;

// Function declarations
void addFlight(int flightNumber, float ticketCost, char* date, char* departureTime, char* arrivalTime, char* source, char* destination);
Flight* findFlight(int flightNumber);
void bookSeats(int flightNumber, int numSeats);
void enqueueBookingRequest(int flightNumber, int numSeats);
BookingRequest dequeueBookingRequest();
void displayFlights();
void displayPassengers(int flightNumber);
void cancelSeat(int passengerId, int flightNumber);

// Hash function for flight numbers
int hashFunction(int flightNumber) {
    return flightNumber % HASH_TABLE_SIZE;
}

void addFlight(int flightNumber, float ticketCost, char* date, char* departureTime, char* arrivalTime, char* source, char* destination) {
    Flight* newFlight = (Flight*)malloc(sizeof(Flight));
    newFlight->flightNumber = flightNumber;
    newFlight->availableSeats = MAX_SEATS;
    newFlight->ticketCost = ticketCost;
    strcpy(newFlight->date, date);
    strcpy(newFlight->departureTime, departureTime);
    strcpy(newFlight->arrivalTime, arrivalTime);
    strcpy(newFlight->source, source);
    strcpy(newFlight->destination, destination);
    newFlight->passengerList = NULL;
    newFlight->next = NULL;

    // Insert flight into hash table
    int hashIndex = hashFunction(flightNumber);
    newFlight->next = flightHashTable[hashIndex];
    flightHashTable[hashIndex] = newFlight;
}

Flight* findFlight(int flightNumber) {
    int hashIndex = hashFunction(flightNumber);
    Flight* current = flightHashTable[hashIndex];
    while (current) {
        if (current->flightNumber == flightNumber) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void enqueueBookingRequest(int flightNumber, int numSeats) {
    if ((rear + 1) % QUEUE_SIZE == front) {
        printf("Booking queue is full.\n");
        return;
    }
    rear = (rear + 1) % QUEUE_SIZE;
    bookingQueue[rear].flightNumber = flightNumber;
    bookingQueue[rear].numSeats = numSeats;
    if (front == -1) {
        front = 0;
    }
}

BookingRequest dequeueBookingRequest() {
    if (front == -1) {
        printf("No booking requests in the queue.\n");
        BookingRequest emptyRequest = {0, 0};
        return emptyRequest;
    }
    BookingRequest request = bookingQueue[front];
    if (front == rear) {
        front = rear = -1; // Queue is now empty
    } else {
        front = (front + 1) % QUEUE_SIZE;
    }
    return request;
}

void bookSeats(int flightNumber, int numSeats) {
    Flight* flight = findFlight(flightNumber);
    if (!flight) {
        printf("Error: Flight %d not found.\n\n", flightNumber);
        return;
    }
    if (flight->availableSeats < numSeats) {
        printf("Error: Only %d seats available on flight %d.\n\n", flight->availableSeats, flightNumber);
        return;
    }

    enqueueBookingRequest(flightNumber, numSeats); // Enqueue booking request

    printf("Booking request for %d seats on flight %d has been added to the queue.\n", numSeats, flightNumber);

    // Process the booking request
    BookingRequest request = dequeueBookingRequest();
    if (request.numSeats > 0) {
        float totalCost = request.numSeats * flight->ticketCost;
        printf("Total cost for booking %d seats: $%.2f\n\n", request.numSeats, totalCost);

        for (int i = 0; i < request.numSeats; i++) {
            Passenger* newPassenger = (Passenger*)malloc(sizeof(Passenger));
            newPassenger->id = 10000 + flight->availableSeats; // Assign a unique passenger ID
            newPassenger->seatNumber = MAX_SEATS - flight->availableSeats + 1; // Assign the next available seat number

            // Get passenger name with a limit of 100 characters
            printf("Enter name for passenger %d (max 100 characters): ", i + 1);
            scanf(" %[^\n]s", newPassenger->name); // Use a format specifier to allow spaces in the name

            newPassenger->next = flight->passengerList;
            flight->passengerList = newPassenger;
            flight->availableSeats--;

            printf("Seat %d booked for %s (Passenger ID: %d).\n", newPassenger->seatNumber, newPassenger->name, newPassenger->id);
        }

        printf("Booking confirmed for %d seats on flight %d. Total cost: $%.2f\n\n", request.numSeats, flightNumber, totalCost);
    }
}

void displayFlights() {
    printf("\n| Flight No | Date       | Departure | Arrival | Source        | Destination   | Ticket Cost | Available Seats |\n");
    printf("|-----------|------------|-----------|---------|---------------|---------------|-------------|----------------|\n");
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        Flight* current = flightHashTable[i];
        while (current) {
            printf("| %9d | %10s | %9s | %7s | %-13s | %-13s | $%10.2f | %14d |\n",
                   current->flightNumber, current->date, current->departureTime,
                   current->arrivalTime, current->source, current->destination,
                   current->ticketCost, current->availableSeats);
            current = current->next;
        }
    }
    printf("\n");
}

void displayPassengers(int flightNumber) {
    Flight* flight = findFlight(flightNumber);
    if (!flight) {
        printf("\nFlight %d not found.\n\n", flightNumber);
        return;
    }

    Passenger* passenger = flight->passengerList;
    printf("\nPassengers on Flight %d (Date: %s):\n", flightNumber, flight->date);
    printf("| Passenger ID | Name                                         | Seat Number |\n");
    printf("|--------------|----------------------------------------------|-------------|\n");
    while (passenger) {
        printf("| %12d | %-44s | %11d |\n", passenger->id, passenger->name, passenger->seatNumber);
        passenger = passenger->next;
    }
    printf("\n");
}

void cancelSeat(int passengerId, int flightNumber) {
    Flight* flight = findFlight(flightNumber);
    if (!flight) {
        printf("Error: Flight %d not found.\n\n", flightNumber);
        return;
    }

    Passenger* current = flight->passengerList;
    Passenger* previous = NULL;
    while (current && current->id != passengerId) {
        previous = current;
        current = current->next;
    }

    if (!current) {
        printf("Error: Passenger ID %d not found on flight %d.\n\n", passengerId, flightNumber);
        return;
    }

    if (previous) {
        previous->next = current->next;
    } else {
        flight->passengerList = current->next;
    }
    free(current);
    flight->availableSeats++;

    printf("Cancellation confirmed: Passenger ID %d has been removed from flight %d.\n\n", passengerId, flightNumber);
}

int main() {
    // Sample flights
    addFlight(101, 200.00, "2024-10-29", "10:00", "12:30", "Mumbai", "South Korea");
    addFlight(102, 150.00, "2024-10-29", "14:00", "16:30", "Mumbai", "Maldives");
    addFlight(103, 300.00, "2024-10-29", "18:00", "20:30", "Mumbai", "Chicago");
    addFlight(104, 400.00, "2024-10-29", "22:00", "01:00", "Mumbai", "Paris");

    int choice;
    while (1) {
        printf("\n1. Display Flights\n");
        printf("2. Book Seats\n");
        printf("3. Display Passengers\n");
        printf("4. Cancel Seat\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                displayFlights();
                break;
            case 2: {
                int flightNumber, numSeats;
                printf("Enter Flight Number: ");
                scanf("%d", &flightNumber);
                printf("Enter Number of Seats to Book: ");
                scanf("%d", &numSeats);
                bookSeats(flightNumber, numSeats);
                break;
            }
            case 3: {
                int flightNumber;
                printf("Enter Flight Number: ");
                scanf("%d", &flightNumber);
                displayPassengers(flightNumber);
                break;
            }
            case 4: {
                int passengerId, flightNumber;
                printf("Enter Passenger ID: ");
                scanf("%d", &passengerId);
                printf("Enter Flight Number: ");
                scanf("%d", &flightNumber);
                cancelSeat(passengerId, flightNumber);
                break;
            }
            case 5:
                printf("Exiting program.\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
}