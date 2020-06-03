#include <stdlib.h>
#include <stdio.h>
#include "plugins.h"

void callEngineOutputMessage(struct EngineInterface *pEngineInterface, int toolId, int status, void * message) {
	pEngineInterface->pOutputMessage(pEngineInterface->handle, toolId, status, message);
}

void * callEngineCreateTempFileName(struct EngineInterface *pEngineInterface, void * ext) {
	return pEngineInterface->pCreateTempFileName(pEngineInterface->handle, ext);
}

unsigned callEngineBrowseEverywhereReserveAnchor(struct EngineInterface *pEngineInterface, int toolId) {
	return pEngineInterface->pBrowseEverywhereReserveAnchor(pEngineInterface->handle, toolId);
}

long callEngineOutputToolProgress(struct EngineInterface *pEngineInterface, int toolId, double dPercentProgress) {
    return pEngineInterface->pOutputToolProgress(pEngineInterface->handle, toolId, dPercentProgress);
}

struct IncomingConnectionInterface* callEngineBrowseEverywhereGetII(struct EngineInterface *pEngineInterface, unsigned browseEverywhereAnchorId, int toolId, void * name) {
	return pEngineInterface->pBrowseEverywhereGetII(pEngineInterface->handle, browseEverywhereAnchorId, toolId, name);
}

struct IncomingConnectionInterface *callEnginePresort(struct EngineInterface *pEngineInterface, int toolId, void * sortXml, struct IncomingConnectionInterface *originalInterface) {
    FILE *f = fopen("C:\\temp\\output.txt", "a");
    fprintf(f, "Started callEnginePresort\n");
    fprintf(f, "toolId: %i\n", toolId);
    fprintf(f, "sortXml: %S\n", sortXml);
    fflush(f);
    struct IncomingConnectionInterface *newIncomingInterface;
    fprintf(f, "Declared *newIncomingInterface\n");
    fflush(f);
    struct PreSortConnectionInterface *newPresortTool;
    fprintf(f, "Declared *newPresortTool\n");
    fflush(f);
    long result = pEngineInterface->pPreSort(pEngineInterface->handle, toolId, sortXml, originalInterface, &newIncomingInterface, &newPresortTool);
    fprintf(f, "called engine pPreSort\n");
	fclose(f);
    return newIncomingInterface;
}

long callInitOutput(struct IncomingConnectionInterface * connection, void * recordMetaInfoXml) {
    return connection->pII_Init(connection->handle, recordMetaInfoXml);
}

long callPushRecord(struct IncomingConnectionInterface * connection, void * record) {
    return connection->pII_PushRecord(connection->handle, record);
}

long callCloseOutput(struct IncomingConnectionInterface * connection) {
    connection->pII_Close(connection->handle);
}

struct IncomingConnectionInterface* newIi() {
    struct IncomingConnectionInterface *ptr;
    ptr = malloc(sizeof(struct IncomingConnectionInterface));
    return ptr;
}

int currentIiIndex = 0;
struct IncomingRecordCache* buffers[1000];
int iiFixedSizes[1000];

void * getIiIndex(){
    int* iiIndex = malloc(sizeof(int));
    *iiIndex = currentIiIndex++;
    return iiIndex;
}

void saveIncomingInterfaceFixedSize(void * handle, int fixedSize) {
    int iiIndex = *((int*)handle);
    struct IncomingRecordCache* cache = malloc(sizeof(struct IncomingRecordCache));
    buffers[iiIndex] = cache;
    iiFixedSizes[iiIndex] = fixedSize;
}

long iiPushRecord(void * handle, void * record) {
    int iiIndex = *((int*)handle);
    int fixedSize = iiFixedSizes[iiIndex];
    struct IncomingRecordCache *buffer = buffers[iiIndex];
    if (buffer->currentBufferIndex == 10) {
        pushRecordCache(handle, &(buffer->buffer), buffer->currentBufferIndex);
        buffer->currentBufferIndex = 0;
    }

    int varSize = *(int*)(record+fixedSize);
    int totalSize = fixedSize + 4 + varSize;
    int bufferSize = buffer->bufferSizes[buffer->currentBufferIndex];
    if (totalSize > bufferSize) {
        if (bufferSize > 0) {
            free(buffer->buffer[buffer->currentBufferIndex]);
        }
        buffer->buffer[buffer->currentBufferIndex] = malloc(totalSize);
        buffer->bufferSizes[buffer->currentBufferIndex] = totalSize;
    }
    memcpy(buffer->buffer[buffer->currentBufferIndex], record, totalSize);
    buffer->currentBufferIndex++;
    buffer->recordCount++;
    return 1;
}

void closeRecordCache(void * handle){
    int iiIndex = *((int*)handle);
    struct IncomingRecordCache *buffer = buffers[iiIndex];
    pushRecordCache(handle, &(buffer->buffer), buffer->currentBufferIndex);
    buffer->currentBufferIndex = 0;
}

void updateProgress(struct IncomingConnectionInterface * connection, double percent){
    connection->pII_UpdateProgress(connection->handle, percent);
}

void freeRecordCache(void * handle) {
    int iiIndex = *((int*)handle);
    struct IncomingRecordCache *buffer = buffers[iiIndex];

    int ceiling = 10;
    if (buffer->recordCount < 10) {
        ceiling = buffer->recordCount;
    }

    for (int i = 0; i < ceiling; i++) {
        free(buffer->buffer[i]);
    }

    free(buffer);
    free(handle);
}
