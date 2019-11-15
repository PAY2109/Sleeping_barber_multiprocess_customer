#include <windows.h>
#include <stdio.h>
#include <iostream>

// This process opens a handle to a mutex created by another process.

DWORD WINAPI customer(LPVOID);

int main(void) {
    // Create worker threads
    HANDLE customerThread;
    DWORD ThreadID;

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


    // Wait for all threads to terminate

    WaitForSingleObject(customerThread, INFINITE);
    CloseHandle(customerThread);

    return 0;
}


DWORD WINAPI customer(LPVOID lpParam) {
    // lpParam not used in this example
    UNREFERENCED_PARAMETER(lpParam);
    HANDLE barberReady;
    HANDLE accessWRSeats;
    HANDLE custReady;

    barberReady = OpenSemaphore(SEMAPHORE_ALL_ACCESS,// request full access
                                FALSE,// handle not inheritable
                                TEXT("barberReady"));// object name

    if (barberReady == nullptr)
        printf("OpenMutex error: %d\n", GetLastError());
    else printf("OpenMutex successfully opened the mutex.\n");

    accessWRSeats = OpenSemaphore(SEMAPHORE_ALL_ACCESS,// request full access
                                  FALSE,// handle not inheritable
                                  TEXT("accessWRSeats"));// object name

    if (accessWRSeats == nullptr)
        printf("OpenMutex error: %d\n", GetLastError());
    else printf("OpenMutex successfully opened the mutex.\n");

    custReady = OpenSemaphore(SEMAPHORE_ALL_ACCESS,// request full access
                              FALSE,// handle not inheritable
                              TEXT("custReady"));// object name

    if (custReady == nullptr)
        printf("OpenMutex error: %d\n", GetLastError());
    else printf("OpenMutex successfully opened the mutex.\n");

    DWORD dwWaitResult;
    int i = 0;
    while (i < 1) {
        // Try to enter the semaphore gate.

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
                break;

                // The semaphore was nonsignaled, so a time-out occurred.
            case WAIT_TIMEOUT:
                printf("Customer %d: has gone (waiting room is full)\n", GetCurrentThreadId());
                break;
        }
        // i++;
        Sleep(2);
    }
    CloseHandle(barberReady);
    CloseHandle(accessWRSeats);
    CloseHandle(custReady);
    return 0;
}
