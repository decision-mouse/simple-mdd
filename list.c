//
//  list.c
//  MDD
//
//  Created by Lynn on 7/18/15.
//  Copyright (c) 2015 Lynn. All rights reserved.
//
#include <stdlib.h>
#include "list.h"
#include "mdd.h"

EdgeList* allocEList(){
    EdgeList* list = (EdgeList*)malloc(sizeof(EdgeList));
    list->_head = list->_tail = NULL;
    list->_size = 0;
    return list;
}

void destroyEList(EdgeList* list){
    if(!listIsEmpty(list)){
        int done = 0;
        MDDEdge* edge = list->_head->_edge;
        while(!done){
            removeEdge(edge->_start->_outgoing, edge->_id);
            removeEdge(edge->_end->_incoming, edge->_id);
            free(edge);
            if(list->_size == 0) done = 1;
            else edge = list->_head->_edge;
        }
    }
    free(list);
}

EdgeListNode*  allocEListNode(MDDEdge* edge){
    EdgeListNode* node = (EdgeListNode*) malloc(sizeof(EdgeListNode));
    node->_edge = edge;
    node->_prev = node->_next = NULL;
    return node;
}

void destroyELNode(EdgeListNode* elNode){
    free(elNode);
}

void addEdge(EdgeList* list, MDDEdge* e){
    EdgeListNode* elNode = allocEListNode(e);
    if(listIsEmpty(list)){
        list->_head = elNode;
        list->_tail = elNode;
    }
    else{
        list->_tail->_next = elNode;
        elNode->_prev = list->_tail;
        list->_tail = elNode;
    }
    list->_size++;
}


/*
 * Assumes list has the ELN we are looking for
 */
EdgeListNode* findELNode(EdgeList* list, int id){              //eid is the edge's cost
    EdgeListNode* temp = list->_head;
    while(1){
        if(temp->_edge->_id == id) return temp;                //if we found our edge return it
        else{
            if(temp->_next) temp = temp->_next;                 //continue looking
            else{                                               //can't look any further
                temp = NULL;
                return temp;
            }
        }
    }
}

MDDEdge* removeEdge(EdgeList* list, int id){                  //assumes the edge is in the list
    MDDEdge* edge = NULL;
    assert(!listIsEmpty(list));
    if(!listIsEmpty(list)){
        EdgeListNode* temp = findELNode(list, id);
        if(temp == NULL){
            printf("Error: Could not find edge to be removed. \n"); }
        else if(list->_size == 1){                                    //case of a singleton
            list->_head = NULL;
            list->_tail = NULL;
        }
        else if(temp->_edge->_value == list->_head->_edge->_value){         //if the head is the target node
            list->_head = temp->_next;
            list->_head->_prev = NULL;
        }
        else if(temp->_edge->_value == list->_tail->_edge->_value){         //if the tail is the target node
            list->_tail = temp->_prev;
            list->_tail->_next = NULL;
        }
        else{                                                         //if the targets is inbetween nodes
            temp->_prev->_next = temp->_next;
            temp->_next->_prev = temp->_prev;
        }
        edge = temp->_edge;
        free(temp);
        list->_size--;
        return edge;
    }
    else printf("Error: Unable to remove node from an empty list");
    return edge;
}

int listIsEmpty(EdgeList* list){
    return (int) !list->_head;
}

void printEList(EdgeList* list){
    int done = 0;
    EdgeListNode* temp = list->_head;
    printf("List size: %d", list->_size);
    while(!done){
        printf(" (%d-%d,%d-%d) ",temp->_edge->_start->_layer,temp->_edge->_start->_layerID, temp->_edge->_end->_layer,temp->_edge->_end->_layerID);
        if(temp->_next) temp = temp->_next;
        else{
            done = 1;
            printf(".\n");
        }
    }
}

int validateList(EdgeList* list){
    int done = 0;
    EdgeListNode* temp = list->_head;
    while(!done){
        if(temp->_edge->_value > 3 || temp->_edge->_id>100){
            return 0;
        }
        if(temp->_next) temp = temp->_next;
        else{
            done = 1;
            printf(".\n");
        }
    }
    return 1;
}


/*
 * Clones an Edgelist for a newly created MDDNode
 */
EdgeList* cloneEL(EdgeList* el,MDDNode* newOrigin, MDD* mdd){                       //assume the list isn't empty since if it were it would've been filtered
    int done = 0;
    EdgeList* clonedEList = allocEList();                                           //make a new empty list
    EdgeListNode* currentELN = el->_head;
    //for each node in the original list add a clone to the clone list
    while(!done){
        mdd->_eTracker++;
        MDDEdge* clonedEdge = cloneMDDEdge(currentELN->_edge, newOrigin, mdd->_eTracker);    //clone the edge with a new origin
        if(mdd->_eTracker == 15) {
            //printf("HOLD UP!\n");
        }
        if(clonedEdge->_end->_layer==3 && clonedEdge->_end->_layerID==0){
            //printf("What's going on");
        }
        addEdge(clonedEList, clonedEdge);                                            //add to split node's outgoing list
        addEdge(currentELN->_edge->_end->_incoming, clonedEdge);                     //add to this edge's destination incoming list
        if(currentELN->_next == NULL) done = 1;
        else currentELN = currentELN->_next;
    }
    //printf("Cloned list: \n");
    //printEList(clonedEList);
    return clonedEList;
}
























