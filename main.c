#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "ecs.h"

#define POS 0
#define VEL 1
#define TAG 2

typedef struct { double x; double y; } Pos, Vel;
typedef enum { PLAYER, ASTEROID, BULLET } Tag;

void kinematics(ecs * world, float dt, void * data) {

  Componet pos = lookUpComponet_ECS(world, POS);
  Componet vel = lookUpComponet_ECS(world, VEL);

  for (Entity e = 0; e < MAX_ENTITIES; e++) {

    if (hasEntityComponets_ECS(world, e, 2, pos, vel)) {
      Pos * p = (Pos*)getEntityComponet_ECS(world, e, pos);
      Vel * v = (Vel*)getEntityComponet_ECS(world, e, vel);

      p->x += v->x * dt;
      p->y += v->y * dt;

      printf("HERE %f, %f\n", p->x, p->y);
    }

  }
}

int main(int argc, char const *argv[]) {

  //Make World
  ecs world;
  new_ECS(&world);

  //ADD Componets                ecs world, size of componet, identifyier <- you get to define it
  Componet pos = addComponet_ECS(&world, sizeof(Pos), POS);
  Componet vel = addComponet_ECS(&world, sizeof(Vel), VEL);
  Componet tag = addComponet_ECS(&world, sizeof(Tag), TAG);

  //Add Entities
  Entity player = addEntity_ECS(&world, 3, pos, vel, tag);
  Entity aster1 = addEntity_ECS(&world, 3, pos, vel, tag);
  Entity test = addEntity_ECS(&world, 1, pos);

  //Modify Componet
  Pos * p = (Pos*)getEntityComponet_ECS(&world, player, pos);
  p->x = 10;

  //Create Pipeline
  Pipeline update;
  new_Pipeline(&update);
  add_Pipeline(&update, &kinematics);

  //Update World with Pipeline
  updateECS_Pipeline(&update, &world, 1, NULL);

  //free up recources
  free_ECS(&world);

  return 0;
}
