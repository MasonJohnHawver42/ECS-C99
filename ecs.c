#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "ecs.h"

//Queue

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

//Masks

void new_EntityMask(EntityMask * em) { for (int i = 0; i < MASK_AMT; i++) { em->masks[i] = 0; } }

void print_EntitMask(EntityMask * em) {
    for (int i = 0; i < MASK_AMT; i++) {

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

void new_EntityPool(EntityPool * ep) {
    new_Queue(&ep->ids);
    for (int i = 0; i < MAX_ENTITIES; i++) { new_EntityMask(&(ep->masks[i])); add_Queue(&ep->ids, i); }
}

Entity addEntity_EntityPool(EntityPool * ep) { return pop_Queue(&ep->ids); }
void removeEntity_EntityPool(EntityPool * ep, Entity e) {
    add_Queue(&ep->ids, e);
    new_EntityMask(&(ep->masks[e]));
}

EntityMask * getEM_EntityPool(EntityPool * ep, Entity e) { return &(ep->masks[e]); }

//Componet Pool

void new_ComponetPool(ComponetPool * cp, size_t size) {
    cp->data = malloc(size * MAX_ENTITIES);
    cp->data_size = size;
}

void * getComponet_ComponetPool(ComponetPool * cp, Entity e) { return cp->data + e; }

void free_ComponetPool(ComponetPool * cp) { free(cp->data); }

//ECS

void new_ECS(ecs * w) { w->used = 0; new_EntityPool(&w->ep); }

Componet addComponet_ECS(ecs * w, size_t size, ComponetID id) {
    new_ComponetPool(&(w->cps[w->used]), size);
    w->used++;

    w->look_ups[id % MAX_COMPONETS] = w->used - 1;

    return w->used - 1;
}

Entity addEntity_ECS(ecs * w, uint32_t componet_num, ...) {

    Entity e = addEntity_EntityPool(&w->ep);
    EntityMask * em = getEM_EntityPool(&w->ep, e);

    va_list componets;
    va_start(componets, componet_num);

    for (int i = 0; i < componet_num; i++) {
        Componet c = va_arg(componets, Componet);
        addComponet_EntityMask(em, c);
    }

    va_end(componets);

    return e;
}

void removeEntity_ECS(ecs * w, Entity e) { removeEntity_EntityPool(&w->ep, e); }

void setEntityComponet_ECS(ecs * w, Entity e, Componet c, bool state) {
  EntityMask * em = getEM_EntityPool(&w->ep, e);
  if (state) { addComponet_EntityMask(em, c); }
  else { delComponet_EntityMask(em, c); }
}

void * getEntityComponet_ECS(ecs * w, Entity e, Componet c) {
  return getComponet_ComponetPool(&(w->cps[c]), e);
}

bool hasEntityComponets_ECS(ecs * w, Entity e, int componet_num, ...) {

  EntityMask * em = getEM_EntityPool(&w->ep, e);

  va_list componets;
  va_start(componets, componet_num);

  bool has_comps = true;

  for (int i = 0; i < componet_num; i++) {
      Componet c = va_arg(componets, Componet);
      has_comps = has_comps & hasComponetEh_EntityMask(em, c);
  }

  va_end(componets);

  return has_comps;
}

Componet lookUpComponet_ECS(ecs * w, ComponetID id) { return w->look_ups[id % MAX_COMPONETS]; }

void free_ECS(ecs * world) {
  for (int i = 0; i < world->used; i++) {
    free_ComponetPool(&world->cps[i])
  }

  world->used = 0;
}

// Pipeline

void new_Pipeline(Pipeline * pipe) { pipe->used = 0; }
void add_Pipeline(Pipeline * pipe, System sys) { if (pipe->used >= MAX_SYSTEMS) { return; } pipe->systems[pipe->used] = sys; pipe->used++; }

void updateECS_Pipeline(Pipeline * pipe, ecs * world, float dt, void * data) {
  for (int i = 0; i < pipe->used; i++) {
    System sys = pipe->systems[i];
    sys(world, dt, data);
  }
}
