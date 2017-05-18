//
//  mddqueue.h
//  MDD
//
//  Created by Lynn on 7/29/15.
//  Copyright (c) 2015 Lynn. All rights reserved.
//

#ifndef __MDD__mddqueue__
#define __MDD__mddqueue__
#include <stdio.h>
#include "mdd.h"
#include "list.h"

typedef struct mddNodeContainer mddNodeContainer;
typedef struct mddNodeQueue mddNodeQueue;

#ifndef TYPEDEF_MDD_STRUCTS_
#define TYPEDEF_MDD_STRUCTS_
typedef struct EdgeListNode EdgeListNode;
typedef struct EdgeList EdgeList;
typedef struct MDDNode MDDNode;
typedef struct MDDEdge MDDEdge;
typedef struct MDD MDD;
#endif


struct mddNodeContainer {
    mddNodeContainer*    _next;
    MDDNode*             _node;
};

struct mddNodeQueue{
    mddNodeContainer*  _head;
    mddNodeContainer*  _tail;
    int                _size;
};

mddNodeContainer*    allocMddNodeContainer(MDDNode* node);
void                 destroyMddNodeContainer(mddNodeContainer* container);

mddNodeQueue*        allocmddNodeQueue();
void                 destroyMddNodeQueue(mddNodeQueue* queue);

void                 enqueueMDDQueue(mddNodeQueue* queue, MDDNode* node);
MDDNode*             dequeueMddQueue(mddNodeQueue* queue);


#endif /* defined(__MDD__mddqueue__) */
