#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#define Mask_Amt 3
#define Max_Entities 5

typedef enum {true, false} bool;

typedef u_int32_t uint32_t;

//Queue

struct node_;
struct node_ { uint32_t id; struct node_ * next; };
typedef struct node_ node;

typedef struct {
    node * head;
    node * last;
    uint32_t length;
} Queue;

void new_Queue(Queue * q) { q->length = 0; }
void add_Queue(Queue * q, uint32_t data) {
    node * new_node = malloc(sizeof(node));
    new_node->id = data;
    
    if (q->length == 0) {
        q->head = new_node;
        q->last = new_node;
    }
    else {
        q->last->next = new_node;
        q->last = new_node;
    }
    
    q->length++;
}

uint32_t pop_Queue(Queue * q) {
    if (q->length == 0) { return 0; }
    
    node * ret_node = q->head;
    
    if (q->length == 1) {
        q->head = NULL;
        q->last = NULL;
        q->length--;
        uint32_t res = ret_node->id;
        free(ret_node);
        
        return res;
    }
    
    q->head = ret_node->next;
    q->length--;
    uint32_t res = ret_node->id;
    free(ret_node);
    
    return res;
}

typedef uint32_t Entity;
typedef uint32_t Componet;

// Mask

typedef struct { uint32_t masks[Mask_Amt]; } EntityMask;

void new_EntityMask(EntityMask * em) { for (int i = 0; i < Mask_Amt; i++) { em->masks[i] = 0; } }

void print_EntitMask(EntityMask * em) {
    for (int i = 0; i < Mask_Amt; i++) {
        
        uint32_t mask = em->masks[i];
        
        for (int j = 31; j >= 0; j--) {
            uint32_t tell = 0x1;
            tell = tell << j;
            tell = mask & tell;
            if (tell == 0x0) { printf("0"); }
            else { printf("1"); }
        }
        
        printf("\n");
        
    } 
}

void addComponet_EntityMask(EntityMask * em, Componet c) {
    
    uint32_t mask_id = c / 32;
    uint32_t mask = em->masks[mask_id];
    
    uint32_t comp = 0x1;
    comp = comp << (c % 32);
    em->masks[mask_id] = mask | comp;
}

void delComponet_EntityMask(EntityMask * em, Componet c) {
    
    uint32_t mask_id = c / 32;
    uint32_t mask = em->masks[mask_id];
    
    uint32_t comp = 0x1;
    comp = comp << (c % 32);
    em->masks[mask_id] = mask & (~comp);
}

bool hasComponetEh_EntityMask(EntityMask * em, Componet c) {
    uint32_t comp = em->masks[c / 32] >> (c % 32);
    return comp & 0x1; 
}

/// Entity Pool

typedef struct {
    EntityMask masks[Max_Entities];
    Queue ids;
} EntityPool;

void new_EntityPool(EntityPool * ep) {
    new_Queue(&ep->ids);
    for (int i = 0; i < Max_Entities; i++) { new_EntityMask(&(ep->masks[i])); add_Queue(&ep->ids, i); }
}

Entity addEntity_EntityPool(EntityPool * ep) { return pop_Queue(&ep->ids); }
void removeEntity_EntityPool(EntityPool * ep, Entity e) {
    add_Queue(&ep->ids, e);
    new_EntityMask(&(ep->masks[e]));
}

EntityMask * getEM_EntityPool(EntityPool * ep, Entity e) { return &(ep->masks[e]); }

typedef struct {
    void * data;

    uint32_t cap;
    uint32_t used;

    size_t data_size;

} ComponetPool;

void new_ComponetPool(ComponetPool * cp, size_t size) {
    cp->data = malloc(size * MAX_ENTITIES);
    cp->used = 0;
    cp->cap = MAX_ENTITIES;

    cp->data_size = size;
} 

typedef struct {

    ComponetPool * pools;

    uint32_t cap;
    uint32_t used;

    EntityPool ep;

} ecs;

void new_ecs(ecs * w) {
    w->pools = (ComponetPool*)malloc(MAX_COMPONETS * sizeof(ComponetPool) );
    w->used = 0;
    w->cap = MAX_COMPONETS;

    new_EntityPool(&w->ep);
}

Componet newComponet_ecs(ecs * w, size_t size) {
    
    Componet id = w->used;
    w->used++;

    if (w->used < w->cap) {
        new_ComponetPool(w->pools + w->used, size);
    }

    return id;
}

Entity addEntity_ecs(ecs * w, uint32_t componet_num, ...) {

    Entity e = add_EntityPool(w->ep);

    va_list componets;
    va_start(componets, componet_num);

    for (int i = 0; i < componet_num; i++) {
        Componet c = va_arg(componets, Componet);
        addComponet_EntityMask(em, c);
    }
	
   va_end(componets);

    w->ep.used++;
    return w->ep.used - 1;
}

void addComponet_ecs(ecs * w, Entity e, Componet c) {
    EntityMask* em = w->ep.masks + c;
    addComponet_EntityMask(em, c);
}

void delComponet_ecs(ecs * w, Entity e, Componet c) {
    EntityMask* em = w->ep.masks + c;
    delComponet_EntityMask(em, c);
}

void * getComponet_ecs(ecs * w, Entity e, Componet c) {
    ComponetPool * cp = w->pools + c;
    void * comp = cp->data + e;

    return comp;
}

typedef struct {
    double x;
    double y;
} Pos, Vel;

int main() {

    ecs world;
    new_ecs(&world);

    Componet pos = newComponet_ecs(&world, sizeof(Pos));
    Componet vel = newComponet_ecs(&world, sizeof(Vel));

    Entity test = addEntity_ecs(&world, 2, pos, vel);

    Pos * p = (Pos*)getComponet_ecs(&world, test, pos);
    
    p->x = 10;
    p->y = 10;
    
    return 0;
}
