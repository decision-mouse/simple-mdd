//
//  main.c
//  MDD
//
//  Created by Lynn Asselin on 6/17/15.
//  Copyright (c) 2015 Lynn. All rights reserved.
//
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "mdd.h"
#include "list.h"
#include "mddqueue.h"


char* getInput()
{
    char input[256];
    int i;
    char* string = (char*)malloc(sizeof(char)*256);
    fflush(stdin);
    fgets(input,256,stdin);
    for ( i = 0; i < 256; i++ ){
        if ( input[i] == '\n' ){
            string[i] = '\0';
            break;
        }
        else{
            string[i]=input[i];
        }
    }
    return string;
}

/*
    used for debugging ELN Lists
*/
void printELN_List(EdgeList* list){
    int done = 0;
    EdgeListNode* temp = list->_head;
    printf("List size: %d\n", list->_size);
    while(!done){
        printf("(%d),",temp->_edge->_value);
        if(temp->_next){ temp = temp->_next; }
        else{
            done = 1;
            printf(".\n");
        }
    }
}

/*
 used for debugging ELN Lists
 */
void testList(){
    EdgeListNode* found;
    EdgeList* tempList = allocEList();
    MDDNode* node = allocMDDNode(0, 0, 0);
    for (int i=0; i<100; i++){
        MDDEdge* tempEdge = allocMDDEdge(node, node, i, i);
        addEdge(tempList, tempEdge);
    }
    found = findELNode(tempList, 0);
    printf("found %d.\n", found->_edge->_value);
    found = findELNode(tempList, 50);
    printf("found %d.\n", found->_edge->_value);
    found = findELNode(tempList, 99);
    printf("found %d.\n", found->_edge->_value);
    
    removeEdge(tempList, 0);
    removeEdge(tempList, 50);
    removeEdge(tempList, 99);
    printELN_List(tempList);
    
    EdgeList* cloneList = cloneEL(tempList, allocMDDNode(9, 9, 9),allocMDD(100));
    printELN_List(cloneList);
}


int main(int argc, const char * argv[]) {
    //testList();
    
    int variables;
    char* varCount;
    char* domainInput;
    printf("Input the number of variables: ");
    varCount = getInput();
    variables = atoi(varCount);
    
    MDD* mdd = allocMDD(variables);
    printf("Variables = %d \n", mdd->_variables);
    MDDNode* currOrigin = allocMDDNode(0,0,0);                          //(layer, layerID, l(s) value)
    MDDNode* currDest = allocMDDNode(1,0,0);
    mdd->_size = 2;
    mdd->_root = currOrigin;
    
    for(int i=0; i<variables; i++){                                           //for each variable assign domain
        printf("Enter the domain for X%d: ", i);
        domainInput = getInput();
        int domainBuffer[20];                                           //maximum number of values in domain
        int j = 0;                                                      //parser marker
        int k = 0;                                                      //buffer marker
        while(domainInput[j] != '\0'){                                  //parses through domain of Xi
            if (isspace(domainInput[j]))  j++;
            else{
                domainBuffer[k]=domainInput[j] - '0';                   //converts char to into int
                j++;
                k++;
            }
        }                                                               //domainBuffer has a list of ints
        int m;
        for(m=0;m<k;m++){                                               //just checks array of domain
            printf("%d \n",domainBuffer[m]);
            MDDEdge* currentEdge = allocMDDEdge(currOrigin, currDest, domainBuffer[m],mdd->_eTracker);
            mdd->_eTracker++;
            addEdge(currOrigin->_outgoing, currentEdge);             //add edge to origin outgoing
            addEdge(currDest->_incoming, currentEdge);               //add cloned edge to destination incoming
        }
        currOrigin = currDest;
        if(i != variables-1){
            mdd->_size++;
            currDest = allocMDDNode(i+2, 0, 0);
        }
        printf("Finished X%d \n",i);
    }
    mdd->_terminal = currDest;                                          //the last destination is the terminal
    
    //Domain Store MDD built
    printf("Propagating Constraint -------\n");
    
    propagate(mdd, 10);
    printf("Built MDD #1 ----------\n");
    printMDD(mdd);
    printf("Built MDD #2----------\n");
    printSplitMDD(mdd);
    
    printf("Done.");
    return 0;
}
