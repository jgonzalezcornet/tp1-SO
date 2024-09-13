#ifndef SLAVEADT_H
#define SLAVEADT_H
#include <stdio.h>

typedef struct slaveCDT * slaveADT;

/**
 * Creates the slave process with the executable provided in path
 * @returns slaveADT, pointer to the created structure
 * @param path Path to exectuable program run by slave
 * @param argv Parameters for executable program
 * @param envp Environment variables for executable program
 */
slaveADT createSlave(const char * path , char * argv[] , char * envp[]);

/**
 * Blocks the process until at least one slave is available
 * @returns integer with the number of slaves available for reading in slaves
 * @param slaves slaves to be checked for availability
 * @param count number of slaves to be checked
 */
int anyReadable(slaveADT * slaves , size_t count);

/**
 * Checks if specific slave is available for reading. Should be used after 
 * calling the anyReadable function. 
 * @returns integer different from zero if slave is available for reading
 * @param slave slave to be checked for availability
 */
int isReadable(slaveADT slave);

/**
 * Reads from the slave STDOUT up to the buffSize number of characters and
 * writes them into buff.
 * @returns integer with number of charaters read or -1 if slave is not readable
 * @param slave slave to be read from
 * @param buff buffer to where characters should be written
 * @param buffSize size of buffer
 */
int readFromSlave(slaveADT slave , char * buff , size_t buffSize);

/**
 * Writes the content in buff up to buffSize characters into the slave STDIN
 * @returns integer with the number of characters written
 * @param slave slave to be written into
 * @param buff buffer from where characters should be read
 * @param buffSize size of buffer
 */
int writeToSlave(slaveADT slave , char * buff , size_t buffSize);

/**
 * Closes slaves by closing pipes and frees memory allocated for the slaveADT
 * @returns void
 * @param slaves slaves to be closed
 * @param count number of slaves to be closed
 */
void closeSlaves(slaveADT * slaves , size_t count);

#endif