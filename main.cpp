#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <random>


DWORD WINAPI customer(LPVOID);

int main(void) {

    HANDLE customerThread;
    DWORD ThreadID;
    int clientamount = 300;
    for (int i = 0; i < clientamount; i++){
        customerThread = CreateThread(
                NULL,       // default security attributes
                0,          // default stack size
                (LPTHREAD_START_ROUTINE) customer,
                NULL,       // no thread function arguments
                0,          // default creation flags
                &ThreadID); // receive thread identifier
        if (customerThread == NULL) {
            printf("CreateThread error: %d\n", GetLastError());
            return 1;
        }

        WaitForSingleObject(customerThread, INFINITE);
        CloseHandle(customerThread);
    }



    return 0;
}


DWORD WINAPI customer(LPVOID lpParam) {
    // lpParam not used
    UNREFERENCED_PARAMETER(lpParam);

    std::random_device rd; // obtain a random number from hardware
    std::mt19937 eng(rd()); // seed the generator
    std::uniform_int_distribution<> distr(2000, 20000); // define the range

    HANDLE barberReady;
    HANDLE accessWRSeats;
    HANDLE custReady;
    HANDLE seatBelt;

    barberReady = OpenSemaphore(SEMAPHORE_ALL_ACCESS,// request full access
                                FALSE,// handle not inheritable
                                TEXT("barberReady"));// object name

    if (barberReady == nullptr)
        printf("OpenMutex error: %d\n", GetLastError());
    //else printf("OpenMutex successfully opened the mutex.\n");

    accessWRSeats = OpenSemaphore(SEMAPHORE_ALL_ACCESS,// request full access
                                  FALSE,// handle not inheritable
                                  TEXT("accessWRSeats"));// object name

    if (accessWRSeats == nullptr)
        printf("OpenMutex error: %d\n", GetLastError());
 //   else printf("OpenMutex successfully opened the mutex.\n");

    custReady = OpenSemaphore(SEMAPHORE_ALL_ACCESS,// request full access
                              FALSE,// handle not inheritable
                              TEXT("custReady"));// object name

    if (custReady == nullptr)
        printf("OpenMutex error: %d\n", GetLastError());
  //  else printf("OpenMutex successfully opened the mutex.\n");

    seatBelt = OpenSemaphore(SEMAPHORE_ALL_ACCESS,// request full access
                              FALSE,// handle not inheritable
                              TEXT("seatBelt"));// object name

    if (seatBelt == nullptr)
        printf("OpenMutex error: %d\n", GetLastError());
   // else printf("OpenMutex successfully opened the mutex.\n");

    DWORD dwWaitResult;
    Sleep(distr(eng));
    printf("Customer %d: has arrived to the barbershop\n", GetCurrentThreadId());
    dwWaitResult = WaitForSingleObject(
            accessWRSeats,   // handle to semaphore
            0);           // zero-second time-out interval

    switch (dwWaitResult) {
        // The semaphore object was signaled.
        case WAIT_OBJECT_0:
            printf("Customer %d: is in the waiting room\n", GetCurrentThreadId());
            ReleaseSemaphore(
                    custReady,  // handle to semaphore
                    1,            // increase count by one
                    nullptr);
            printf("Customer %d: is waiting\n", GetCurrentThreadId());
            WaitForSingleObject(barberReady, INFINITE);

            printf("Customer %d: is getting a haircut\n", GetCurrentThreadId());
            WaitForSingleObject(seatBelt, INFINITE);
            printf("Customer %d: is leaving (got a haircut)\n", GetCurrentThreadId());
            break;

            // The semaphore was nonsignaled, so a time-out occurred.
        case WAIT_TIMEOUT:
            printf("Customer %d: has gone (waiting room is full)\n", GetCurrentThreadId());
            break;
    }


    CloseHandle(barberReady);
    CloseHandle(accessWRSeats);
    CloseHandle(custReady);
    CloseHandle(seatBelt);
    return 0;
}