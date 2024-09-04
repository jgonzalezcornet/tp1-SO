#ifndef SLAVEADT_H
#define SLAVEADT_H
#include <stdio.h>

typedef struct slaveCDT * slaveADT;

//creates de slave process with de exectutable provided in path
slaveADT createSlave(const char * path , char * argv[] , char * envp[]);

//returns de ammount of slaves available for reading in slaves, blocks until at least one is
int anyReadable(slaveADT * slaves , size_t count);

//returns != 0 if slave is available for reading
//@TODO:explicar que se debe usar con la anterior(anyReadable())
int isReadable(slaveADT slave);

//reads from slave stdout up to buffSize characters and writes them into buff
//if isReadable(slave) == 0 it returns -1
int readFromSlave(slaveADT slave , char * buff , size_t buffSize);

//writes the content in buff up to buffSize into the slave stdin
int writeToSlave(slaveADT slave , char * buff , size_t buffSize);

void closeSlaves(slaveADT * slaves , size_t count);

#endif