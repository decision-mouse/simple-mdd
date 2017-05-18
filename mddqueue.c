//
//  mddqueue.c
//  MDD
//
//  Created by Lynn on 7/29/15.
//  Copyright (c) 2015 Lynn. All rights reserved.
//

#include "mddqueue.h"
#include <stdlib.h>

mddNodeContainer*   allocMddNodeContainer(MDDNode* node){
    mddNodeContainer* container = (mddNodeContainer*)malloc(sizeof(mddNodeContainer));
    container->_next = NULL;
    container->_node = node;
    return container;
}

void destroyMddNodeContainer(mddNodeContainer* container){
    //only frees container struct, freeing of node determined at constraint validation
    free(container);
}

mddNodeQueue* allocmddNodeQueue(){
    mddNodeQueue* queue = (mddNodeQueue*)malloc(sizeof(mddNodeQueue));
    queue->_head = queue->_tail = NULL;
    queue->_size = 0;
    return queue;
}

/*
 * Assumes the Queue will be empty when we go to destroy it
 */
void destroyMddNodeQueue(mddNodeQueue* queue){
    free(queue);
}

void enqueueMDDQueue(mddNodeQueue* queue ,MDDNode* node){
    mddNodeContainer* container = allocMddNodeContainer(node);
    if(queue->_tail == NULL){
        queue->_head = queue->_tail =container;
    } else{
        queue->_tail->_next = container;
        queue->_tail = container;
    }
    queue->_size++;
}
MDDNode* dequeueMddQueue(mddNodeQueue* queue){
    mddNodeContainer* container = queue->_head;
    MDDNode* node = container->_node;
    if(container->_next){
        queue->_head = container->_next;
    } else queue->_head = queue->_tail = NULL;
    queue->_size--;
    destroyMddNodeContainer(container);
    return node;
}
