#include <stdio.h>
#include <stdlib.h>
#include "snap7.h"

#define PLC_IP "192.168.1.151"
#define RACK 0
#define SLOT 1
#define DB_NUMBER 1

int main() {
    // Create an instance of the S7Client structure
    S7Object client = Cli_Create();

    // Connect to the PLC
    int result = Cli_ConnectTo(client, PLC_IP, RACK, SLOT);
    if (result != 0) {
        printf("Failed to connect to the PLC.\n");
        Cli_Destroy(&client);
        return -1;
    }
    printf("Connected to the PLC.\n");



    // Write data to DB1's first word
    uint8_t data[2] = {0x12, 0x34};
    result = Cli_DBWrite(client, DB_NUMBER, 0, 2, data);
    if (result != 0) {
        printf("Failed to write data to DB1.\n");
        Cli_Disconnect(client);
        Cli_Destroy(&client);
        return -1;
    }
    printf("Data written to DB1.\n");

    // Read the first word of DB1
    uint8_t readData[2];
    result = Cli_DBRead(client, DB_NUMBER, 0, 2, readData);
    if (result != 0) {
        printf("Failed to read data from DB1.\n");
    } else {
        printf("Data read from DB1: 0x%x 0x%x\n", readData[0], readData[1]);
    }



    // Disconnect and destroy the client
    Cli_Disconnect(client);
    Cli_Destroy(&client);

    return 0;
}
