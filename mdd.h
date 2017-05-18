#ifndef __MDD_H
#define __MDD_H

#include "list.h"
#include "mddqueue.h"

//mdd structs
//typedef struct MDDNode MDDNode;
//typedef struct MDDEdge MDDEdge;
//typedef struct MDD     MDD;
//list structs
//ztypedef struct EdgeListNode EdgeListNode;
//typedef struct EdgeList EdgeList;


#ifndef TYPEDEF_MDD_STRUCTS_
#define TYPEDEF_MDD_STRUCTS_
typedef struct EdgeListNode EdgeListNode;
typedef struct EdgeList EdgeList;
typedef struct MDDNode MDDNode;
typedef struct MDDEdge MDDEdge;
typedef struct MDD MDD;
#endif



struct MDDNode {
    EdgeList*    _incoming;
    EdgeList*    _outgoing;
    int             _layer;
    int           _layerID;
    int             _value;
    int           _visited;
    int             _split;
};


struct MDDEdge {
    MDDNode*     _start;
    MDDNode*       _end;
    int          _value;
    int             _id;
};

struct EdgeListNode {
    EdgeListNode*    _prev;
    EdgeListNode*    _next;
    MDDEdge*         _edge;
};

struct MDD {
    MDDNode*      _root;
    MDDNode*  _terminal;
    int      _variables;
    int           _size;
    int       _eTracker;
    
};

struct EdgeList{
    EdgeListNode*  _head;
    EdgeListNode*  _tail;
    int            _size;
};

MDD*	    allocMDD(int n);
void		destroyMDD(MDD* m);

MDDNode*	allocMDDNode(int layer,int layerID, int value);
void		destroyMDDNode(MDDNode* n);

MDDEdge*    allocMDDEdge(MDDNode* start, MDDNode* end, int val, int id);
void        destroyMDDEdge(MDDEdge* e);

MDDEdge*    cloneMDDEdge(MDDEdge* e, MDDNode* newOrigin, int id);

int         IsEmptyMDD(MDD* m);

void        propagate(MDD* m, int constraint);
void        filterNode(MDDNode* n, MDD* m);
MDDNode*    splitNode(MDDNode* n, MDDEdge* e, MDD* mdd, int layerID, int pathCost);
void        printMDD(MDD* m);
void        printSplitMDD(MDD* m);
void        printAllSolutions(MDD* m);
#endif

