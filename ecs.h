#include <stdlib.h>

#ifndef ECS
#define ECS

#define MASK_AMT 10
#define MAX_ENTITIES 1000000
#define MAX_COMPONETS MASK_AMT * 32
#define MAX_SYSTEMS 1000


typedef enum {false, true} bool;
typedef u_int32_t uint32_t;

//Queue

struct node_; struct node_ { uint32_t id; struct node_ * next; };
typedef struct node_ node;

typedef struct { node * head; node * last; uint32_t length; } Queue;

void new_Queue(Queue * q);
void add_Queue(Queue * q, uint32_t data);
uint32_t pop_Queue(Queue * q);

typedef uint32_t Entity;
typedef uint32_t Componet;

// Mask

typedef struct { uint32_t masks[MASK_AMT]; } EntityMask;

void new_EntityMask(EntityMask * em);
void print_EntitMask(EntityMask * em);
void addComponet_EntityMask(EntityMask * em, Componet c);
void delComponet_EntityMask(EntityMask * em, Componet c);
bool hasComponetEh_EntityMask(EntityMask * em, Componet c);

/// Entity Pool

typedef struct { EntityMask masks[MAX_ENTITIES]; Queue ids; } EntityPool;

void new_EntityPool(EntityPool * ep);
Entity addEntity_EntityPool(EntityPool * ep);
void removeEntity_EntityPool(EntityPool * ep, Entity e);
EntityMask * getEM_EntityPool(EntityPool * ep, Entity e);

//Componet Pool

typedef struct { void * data; size_t data_size; } ComponetPool;

void new_ComponetPool(ComponetPool * cp, size_t size);
void * getComponet_ComponetPool(ComponetPool * cp, Entity e);

void free_ComponetPool(ComponetPool * cp);

//ECS

typedef uint32_t ComponetID;

typedef struct {
  ComponetPool cps[MAX_COMPONETS]; uint32_t used;
  Componet look_ups[MAX_COMPONETS];
  EntityPool ep;
} ecs;

//todo safe gaurd methods bellow

void new_ECS(ecs * w);

Componet addComponet_ECS(ecs * w, size_t size, ComponetID id);

Entity addEntity_ECS(ecs * w, uint32_t componet_num, ...);
void removeEntity_ECS(ecs * w, Entity e);

void setEntityComponet_ECS(ecs * w, Entity e, Componet c, bool state);
bool hasEntityComponets_ECS(ecs * w, Entity e, int componet_num, ...);
void * getEntityComponet_ECS(ecs * w, Entity e, Componet c);

Componet lookUpComponet_ECS(ecs * w, ComponetID id);

void free_ECS(ecs * world);

//System

typedef void (*System)(ecs * world, float dt, void * data);

typedef struct {
  System systems[MAX_SYSTEMS];
  uint32_t used;
} Pipeline;

void new_Pipeline(Pipeline * pipe);
void add_Pipeline(Pipeline * pipe, System sys);

void updateECS_Pipeline(Pipeline * pipe, ecs * world, float dt, void * data);


#endif /* end of include guard: ECS */
