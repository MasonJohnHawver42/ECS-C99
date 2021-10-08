
#include <stdlib.h>
#include <stdarg.h>

typedef enum {true, false} bool;

typedef u_int32_t uint32_t;

const uint32_t MAX_COMPONETS = 32;
const uint32_t MAX_ENTITIES = 128;

typedef uint32_t Entity;
typedef uint32_t Componet;


typedef struct { uint32_t mask; } EntityMask;

void new_EntityMask(EntityMask * em) {
    em->mask = 0;
}

void addComponet_EntityMask(EntityMask * em, Componet c) {
    uint32_t comp = 0x1;
    comp = comp << c;
    em->mask = em->mask | comp;
}

void delComponet_EntityMask(EntityMask * em, Componet c) {
    uint32_t comp = 0x1;
    comp = comp << c;
    em->mask = em->mask & (~comp);
}

bool hasComponetEh_EntityMask(EntityMask * em, Componet c) {
    uint32_t comp = em->mask >> c;
    return comp & 0x1; 
}

bool hasComponets_EntityMask(EntityMask * em, int size, Componet * cs) {
    
    uint32_t comps = 0;
    
    for (int i = 0; i < size; i++) {
        Componet c = *(cs + i);
        uint32_t comp = 0x1;
        comp = comp << c;
        
        comps = comps | comp;
    }
    
    return (((em->mask & comps) - comps) == 0);
    
}


typedef struct {
    EntityMask * masks;
    uint32_t cap;
} EntityPool;

void new_EntityPool(EntityPool * ep) {
    ep->masks = (EntityMask*)malloc(sizeof(EntityMask) * MAX_ENTITIES);
    ep->cap = MAX_ENTITIES;
	
	//todo queue sys for ids;
}

Entity add_EntityPool(EntityPool * ep) {
	return 0; 
}

void remove_EntityPool(EntityPool * ep, Entity e) {

}

EntityMask * getMask_EntityPool(EntityPool * ep, Entity e) {
	return 
}

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
