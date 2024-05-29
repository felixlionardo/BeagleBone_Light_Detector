#include "udp.h"
#include "sampler.h"
#include "shutdown.h"

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define MSG_MAX_LEN 200000
#define PORT 12345

static pthread_t UDPThread;
static char previousCommand[MSG_MAX_LEN];



void *displayUDPcommands(void *args)
{
    (void)args;
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;                // Connection may be from network
    sin.sin_addr.s_addr = htonl(INADDR_ANY); // Host to   Network long
    sin.sin_port = htons(PORT);              // Host to Network short

    // Create the socket for UDP
    int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

    // Bind the socket to the port (PORT) that we specify
    bind(socketDescriptor, (struct sockaddr *)&sin, sizeof(sin));
    while (1)
    {
        // Get the data (blocking)
        // Will change sin (the address) to be the address of the client.
        // Note: sin passes information in and out of call!
        struct sockaddr_in sinRemote;
        unsigned int sin_len = sizeof(sinRemote);
        char messageRx[MSG_MAX_LEN];

        // Pass buffer size - 1 for max # bytes so room for the null (string data)
        int bytesRx = recvfrom(socketDescriptor,
                               messageRx, MSG_MAX_LEN - 1, 0,
                               (struct sockaddr *)&sinRemote, &sin_len);

        // Check for errors (-1)

        // Make it null terminated (so string functions work)
        // - recvfrom given max size - 1, so there is always room for the null
        messageRx[bytesRx] = 0;

        if (strcmp(messageRx, "\n") == 0)
            strcpy(messageRx, previousCommand);

        // command for help or ? (Return a brief summary/list of supported commands.)
        if (strncmp(messageRx, "help", strlen("help")) == 0 || strncmp(messageRx, "?", strlen("?")) == 0)
        {
            char messageTx[MSG_MAX_LEN];
            sprintf(messageTx, "Supported commands: \n"
                               "help: Return a brief summary/list of supported commands.\n"
                               "count: Get the total number of samples taken.\n"
                               "length: Get the number of samples taken in the previously completed second.\n"
                               "dips: Get the number of dips in the previously completed second.\n"
                               "history: Get all the samples in the previously completed second.\n"
                               "stop: Cause the server program to end.\n"
                               "<enter>: Repeat the previous command.\n");
            sin_len = sizeof(sinRemote);
            sendto(socketDescriptor,
                   messageTx, strlen(messageTx),
                   0,
                   (struct sockaddr *)&sinRemote, sin_len);
        }

        // command for count (get the total number of samples taken so far)
        else if (strncmp(messageRx, "count", strlen("count")) == 0)
        {
            char messageTx[MSG_MAX_LEN];
            sprintf(messageTx, "Samples taken total: %lld\n", getSamplesTaken());
            sin_len = sizeof(sinRemote);
            sendto(socketDescriptor,
                   messageTx, strlen(messageTx),
                   0,
                   (struct sockaddr *)&sinRemote, sin_len);
        }

        // command for length (get the number of samples taken in the previously completed second)
        else if (strncmp(messageRx, "length", strlen("length")) == 0)
        {
            char messageTx[MSG_MAX_LEN];
            sprintf(messageTx, "Samples taken last second: %d\n", getHistorySize());
            sin_len = sizeof(sinRemote);
            sendto(socketDescriptor,
                   messageTx, strlen(messageTx),
                   0,
                   (struct sockaddr *)&sinRemote, sin_len);
        }

        // command for dips
        else if (strncmp(messageRx, "dips", strlen("dips")) == 0)
        {
            char messageTx[MSG_MAX_LEN];
            sprintf(messageTx, "Dips: %d\n", getHistoryDips());
            sin_len = sizeof(sinRemote);
            sendto(socketDescriptor,
                   messageTx, strlen(messageTx),
                   0,
                   (struct sockaddr *)&sinRemote, sin_len);
        }

        // command for history
        else if (strncmp(messageRx, "history", strlen("history")) == 0)
        {
            char messageTx[MSG_MAX_LEN];
            memset(messageTx, 0, sizeof(messageTx));

            double *history = getHistoryBuffer();

            // print the history and \n after every 10th element
            for (int i = 0; i < getHistorySize(); i++)
            {
                char temp[MSG_MAX_LEN];
                sprintf(temp, "%.3f,  ", history[i]);
                strcat(messageTx, temp);
                if (i % 10 == 9)
                {
                    strcat(messageTx, "\n");
                }
            }

            strcat(messageTx, "\n");

            sin_len = sizeof(sinRemote);
            sendto(socketDescriptor,
                   messageTx, strlen(messageTx),
                   0,
                   (struct sockaddr *)&sinRemote, sin_len);
        }

        // command for stop
        else if (strncmp(messageRx, "stop", strlen("stop")) == 0)
        {
            char messageTx[MSG_MAX_LEN];
            sprintf(messageTx, "Program Terminating\n");
            sin_len = sizeof(sinRemote);
            sendto(socketDescriptor,
                   messageTx, strlen(messageTx),
                   0,
                   (struct sockaddr *)&sinRemote, sin_len);
            signalShutdown();
            break;
        }

        // command for any other command
        else
        {
            char messageTx[MSG_MAX_LEN];
            sprintf(messageTx, "Command not recognized\n");
            sin_len = sizeof(sinRemote);
            sendto(socketDescriptor, messageTx, strlen(messageTx), 0, (struct sockaddr *)&sinRemote, sin_len);
        }

        strcpy(previousCommand, messageRx);
    }
    close(socketDescriptor);
    return NULL;
}

void createUDPThread()
{
    pthread_create(&UDPThread, NULL, displayUDPcommands, NULL);
}

void joinUDPThread()
{
    pthread_join(UDPThread, NULL);
}