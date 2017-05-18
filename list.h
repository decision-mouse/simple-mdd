//
//  list.h
//  MDD
//
//  Created by Lynn on 7/18/15.
//  Copyright (c) 2015 Lynn. All rights reserved.
//

#ifndef __MDD__list__
#define __MDD__list__

#include "mdd.h"
#include "mddqueue.h"
#include <assert.h>
#include <stdio.h>


#ifndef TYPEDEF_MDD_STRUCTS_
#define TYPEDEF_MDD_STRUCTS_
typedef struct EdgeListNode EdgeListNode;
typedef struct EdgeList EdgeList;
typedef struct MDDNode MDDNode;
typedef struct MDDEdge MDDEdge;
typedef struct MDD MDD;
#endif

EdgeList*      allocEList();
void           destroyEList(EdgeList* list);

EdgeListNode*  allocEListNode(MDDEdge* edge);
void           destroyELNode(EdgeListNode* elNode);

void           addEdge(EdgeList* list, MDDEdge* edge);
EdgeListNode*  findELNode(EdgeList* list, int id);
MDDEdge*       removeEdge(EdgeList* list, int id);
int            listIsEmpty(EdgeList* list);
void           printEList(EdgeList* list);

int             validateList(EdgeList* list);

EdgeList* cloneEL(EdgeList* el,MDDNode* newOrigin, MDD* mdd);

#endif /* defined(__MDD__list__) */
