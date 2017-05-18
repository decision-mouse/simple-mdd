#include "mdd.h"
#include "list.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

MDD* allocMDD(int n){
    MDD* mdd = (MDD*)malloc(sizeof(MDD));
    mdd->_root = mdd->_terminal = NULL;
    mdd->_size = 0;
    mdd->_variables = n;
    mdd->_eTracker = 1;
    return mdd;
}

void destroyMDD(MDD* m){
    free(m);
}

MDDNode* allocMDDNode(int layer,int layerID, int value){
    MDDNode* node = (MDDNode*)malloc(sizeof(MDDNode));
    node->_incoming = allocEList();
    node->_outgoing = allocEList();
    node->_layer = layer;
    node->_layerID = layerID;
    node->_value = value;
    node->_visited = 0;             //need to get rid of this by making an iterator
    return node;
}

void destroyMDDNode(MDDNode* n){
    destroyEList(n->_incoming);
    destroyEList(n->_outgoing);
    free(n);
}

MDDEdge* cloneMDDEdge(MDDEdge* e,MDDNode* newOrigin, int id){
    MDDEdge* clone = allocMDDEdge(newOrigin, e->_end, e->_value, id);
    return clone;
}


MDDEdge* allocMDDEdge(MDDNode* start, MDDNode* end, int val, int id){
    MDDEdge* edge = (MDDEdge*)malloc(sizeof(MDDEdge));
    edge->_start = start;
    edge->_end = end;
    edge->_value = val;
    edge->_id = id;
    return edge;
}

void destroyMDDEdge(MDDEdge* e){
    free(e);
}

/* 
 * Propagate takes a constraint (currently only values for linear inequalities)
 * It begins from the root and ends at the terminal
 */

void propagate(MDD* m, int constraint){                                             // split all possible nodes and filters those that would violate the constraint
    MDDEdge* currentEdge;
    MDDNode* nextNode = NULL;
    int layerID = 1;
    int currentLayer = -1;
    MDDNode* currentN = m->_root;                                                   // start from the root
    mddNodeQueue* queue = allocmddNodeQueue();                                      // make a queue for a BFS of the MDD
    enqueueMDDQueue(queue, currentN);                                               // inserts the MDD's root node into the queue first
    
    while(queue->_size > 0){                                                        //while the queue is not empty split each node for each of its outgoing edges
        currentN = dequeueMddQueue(queue);                                          //extract a node from the queue
        if (currentN->_layer > currentLayer){
            layerID = 1;
            if(nextNode != NULL && nextNode->_layer != m->_variables){
                destroyMDDNode(nextNode);                                           //If next node is NOT the terminal and it was split then destroy it
            }
        }
        currentLayer = currentN->_layer;
        printf("Dequeued Node %d - %d outgoing size: %d \n", currentN->_layer, currentN->_layerID, currentN->_outgoing->_size);
        printEList(currentN->_outgoing);
        if(currentN->_layer == m->_variables - 1){                                  //if the current node is in the penultimate layer
            printf("Terminal Reached \n");
            if( currentN->_incoming->_size == 0){                                   //this should NOT occur
                printf("THERE IS NO SOLUTION\n");
            }
            return;
        }
        EdgeList* currentList = currentN->_outgoing;                                 //focus on current node's outgoing edge list
        EdgeListNode* currentELN = currentList->_head;                               //find the first outgoing edge's ELN
        int done = 0;
        int currentPathCost;
        MDDNode* splitClone;
        while(!done){                                                               //done when the outgoing list has been completely traversed
            currentEdge = currentELN->_edge;                                        //extract the edge from ELN
            nextNode = currentEdge->_end;                                           //nextNode is the result of the transition over this edge
            //PART 1:choose whether or not to split a node
            currentPathCost = currentEdge->_value + currentN->_value;
            if(currentPathCost <= constraint){                                      //IF state info + edge weight satisfies the constraint
                if(nextNode->_layer < m->_variables ){        //IF next node is NOT the terminal or has been previously split
                    assert(findELNode(nextNode->_incoming, currentEdge->_id));
                    splitClone = splitNode(nextNode, currentEdge, m, layerID, currentPathCost); //returns a split version of Nextnode using the currentE
                    layerID++;
                    enqueueMDDQueue(queue, splitClone);                             //enqueue the split node
                    printf("Enqueued Node %d - %d \n",splitClone->_layer, splitClone->_layerID);
                } else printf("Found valid path to Terminal with cost: %d\n", currentPathCost); // Don't split, do nothing!
            }
            //PART2: the constraint is NOT satisfied we need to remove this edge
            else{
                printf("Edge eid: %d violates the constraint.\n", currentEdge->_id);
                removeEdge(currentN->_outgoing, currentEdge->_id);                   //remove this edge from the current node
                removeEdge(nextNode->_incoming, currentEdge->_id);                   //remove this edge from the next node
                free(currentEdge);
                if(currentN->_outgoing->_size == 0){                                //IF removing this edge results in outgoing list becoming empty
                    filterNode(currentN, m);                                        //THEN filter this node (results in its destruction)
                    done = 1;                                                       //there are no more edges since the node has been filtered
                }
                //else there may be more edges to check
                if(nextNode->_incoming->_size == 0 && nextNode->_layer != m->_variables){ // If the next node is the terminal, then don't filter it
                    filterNode(nextNode, m);
                    nextNode = NULL;
                }
            }
            if(currentELN->_next) currentELN = currentELN->_next;                    //look at the next ELN in the list
            else done = 1;                                                           //the list is empty
        }//list traveral loop end
    }//queue loop end
    printf("Completed Propagating Constraint: Sum <= %d\n", constraint);
}

void filterNode(MDDNode* n, MDD* m){
    //for every inbound edge remove the start node's outgoing edge
    printf("Filtering Algorithm Triggered on node %d - %d.\n", n->_layer, n->_layerID);
    int filtered = 0;                                                                   //signifies end of filtering algorithm
    MDDEdge* edge;
    if(!listIsEmpty(n->_incoming)){                                                     //may not need this check<-----------
        edge = n->_incoming->_head->_edge;
        while(!filtered){                                                               //find currentEdge's ID
            removeEdge(edge->_start->_outgoing, edge->_id);                             //remove currentEdge from the incoming list
            removeEdge(edge->_end->_incoming, edge->_id);                               //remove currentEdge from predecessor's outgoing list
            if(edge->_start->_layer > 0 && listIsEmpty(edge->_start->_outgoing)){      //only do for non-root nodes
                filterNode(edge->_start, m);                                            //then recursively filter it!
            }
            else edge = n->_incoming->_head->_edge;
        }
    }
    //for every outbound edge remove the destination node's incoming edge
    filtered = 0;
    if(!listIsEmpty(n->_outgoing)){                                                 //may not need this check<-----------
        edge = n->_outgoing->_head->_edge;
        while(!filtered){
            removeEdge(edge->_start->_outgoing, edge->_id);                             //remove currentEdge from the incoming list
            removeEdge(edge->_end->_incoming, edge->_id);                               //remove currentEdge from predecessor's outgoing list
            if(edge->_end->_layer < m->_size && listIsEmpty(edge->_end->_incoming)){
                    filterNode(edge->_end, m);
            }
            if(!listIsEmpty(n->_outgoing)) filtered = 1;                                //finished if no more edges in n->_outgoing
            else edge = n->_outgoing->_head->_edge;
        }
    }
    destroyMDDNode(n);
    printf("A node has been filtered.\n");
}

MDDNode* splitNode(MDDNode* dest, MDDEdge* e, MDD* mdd, int layerID, int pathCost){    //how to split node depends on constraint
    removeEdge(dest->_incoming, e->_id);                                               //disconnect this edge from Nextnode's incoming list
    mdd->_size++;
    MDDNode* newDest = allocMDDNode(dest->_layer, layerID, pathCost);                  //update newNode's value (old should be 0)
    e->_end = newDest;                                                              //have the edge point to the Split node
    mdd->_eTracker++;                                                               //increase global edge ID
    addEdge(newDest->_incoming, e);                                                 //add the new ELN to the incoming list
    EdgeList* outlist = cloneEL(dest->_outgoing, newDest, mdd);                        //clones outgoing edge list for newNode.
    newDest->_outgoing = outlist;                                                   //assign cloned outgoing to
    printf("newNode %d - %d outgoing size %d \n",newDest->_layer, newDest->_layerID, newDest->_outgoing->_size);
    return newDest;
}

void printMDD(MDD* m){
    MDDNode* nextN;
    MDDNode* currentN;
    mddNodeQueue* queue = allocmddNodeQueue();
    enqueueMDDQueue(queue, m->_root);                                                   //enqueue the MDD's root node
    while(queue->_size > 0){
        currentN = dequeueMddQueue(queue);                                              //extract a node                                                //visit the node
        if(currentN->_layer == m->_variables){                                      //if we are looking at the terminal node layer = depth
            printf(" (%d-%d) Terminal Reached\n",currentN->_layer,currentN->_layerID);
            return;
        }
        EdgeList* currentList = currentN->_outgoing;                                //look at node's outgoing list
        EdgeListNode* currentELN = currentList->_head;                              //start from list head
        while(currentELN){                                                          //iterate through edge list
            nextN = currentELN->_edge->_end;                                        //find destination node of current edge
            printf("Start: %d-%d by:%d End: %d-%d \n",currentN->_layer, currentN->_layerID,currentELN->_edge->_value,nextN->_layer,nextN->_layerID);
            enqueueMDDQueue(queue, nextN);
            if(currentELN->_next) currentELN = currentELN->_next;                   //check for the next edge in the list
            else currentELN = NULL;
        }
    }
}


void printSplitMDD(MDD* m){
    MDDNode* nextN;
    MDDNode* currentN;
    mddNodeQueue* queue = allocmddNodeQueue();                                      //The queue of nodes whose outgoing edges need to be printed
    enqueueMDDQueue(queue, m->_root);                                               //enqueue the root's container
    while(queue->_size > 0){
        currentN = dequeueMddQueue(queue);                                          //extract node from queue
        if(currentN->_layer == m->_variables){                                      //if we are looking at the terminal node layer = depth
            printf("Terminal Reached\n");
            return;
        }
        
        EdgeList* currentList = currentN->_outgoing;                                //look at node's outgoing list
        EdgeListNode* currentELN = currentList->_head;                              //start from list head

        while(currentELN){                                                          //iterate through edge list
            nextN = currentELN->_edge->_end;                                        //find destination node of current edge
            printf("Start: %d-%d by:%d End: %d-%d \n",currentN->_layer, currentN->_layerID,currentELN->_edge->_value,nextN->_layer,nextN->_layerID);
            enqueueMDDQueue(queue, nextN);
            if(currentELN->_next) currentELN = currentELN->_next;                   //check for the next edge in the list
            else currentELN = NULL;
        }
    }
}

/*
 * Should be a DFS of all paths from Root to Terminal
 */
void printAllSolutions(MDD* m){
    MDDNode* nextN;
    MDDNode* currentN;
    mddNodeQueue* queue = allocmddNodeQueue();                                          //The queue of nodes whose outgoing edges need to be printed
    enqueueMDDQueue(queue, m->_root);                                                   //enqueue the root's container
    while(queue->_size > 0){
        currentN = dequeueMddQueue(queue);                                              //extract a container
        if(!currentN->_visited){                                                        //if this deq-ed node isn't visited
            currentN->_visited = 1;                                                     //visit the node
            if(currentN->_layer == m->_variables){                                      //if we are looking at the terminal node layer = depth
                printf("Terminal Reached\n");
                return;
            }
            
            EdgeList* currentList = currentN->_outgoing;                                //look at node's outgoing list
            EdgeListNode* currentELN = currentList->_head;                              //start from list head
            
            while(currentELN){                                                          //iterate through edge list
                nextN = currentELN->_edge->_end;                                        //find destination node of current edge
                printf("Start: %d-%d by:%d End: %d-%d \n",currentN->_layer, currentN->_layerID,currentELN->_edge->_value,nextN->_layer,nextN->_layerID);
                enqueueMDDQueue(queue, nextN);
                if(currentELN->_next) currentELN = currentELN->_next;                   //check for the next edge in the list
                else currentELN = NULL;
            }
        }
    }
}
