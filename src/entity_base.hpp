//
// Created by remex on 10/3/23.
//

#ifndef BARONY_ENTITY_BASE_HPP
#define BARONY_ENTITY_BASE_HPP

#include "monster.hpp"
#include "light.hpp"
#include "stat.hpp"
#include "game.hpp"
#include "main.hpp"

// entity flags
#define BRIGHT 1
#define INVISIBLE 2
#define NOUPDATE 3
#define UPDATENEEDED 4
#define GENIUS 5
#define OVERDRAW 6
#define SPRITE 7
#define BLOCKSIGHT 8
#define BURNING 9
#define BURNABLE 10
#define UNCLICKABLE 11
#define PASSABLE 12
#define USERFLAG1 14
#define USERFLAG2 15

// number of entity skills and fskills
static const int NUMENTITYSKILLS = 60;
static const int NUMENTITYFSKILLS = 30;

class Entity_Base {

public:
    Entity_Base();

    explicit Entity_Base(Sint32 &interactedByMonster, real_t &highlightForUi, real_t &highlightForUiGlow,
                         real_t &grayscaleGlRender);

    virtual ~Entity_Base();

    [[nodiscard]]
	Uint32 getUID() const {return uid;}
    virtual void setUID(Uint32 new_uid);

    Uint32 ticks{};                      // duration of the entity's existence
    real_t x{}, y{}, z{};                // world coordinates
    real_t yaw{}, pitch{}, roll{};       // rotation
    real_t focalx{}, focaly{}, focalz{}; // focal point for rotation, movement, etc.
    real_t scalex{}, scaley{}, scalez{}; // stretches/squashes the entity visually
    Sint32 sizex{}, sizey{};             // entity bounding box size
    Sint32 sprite{};                     // the entity's sprite index

    // network stuff
    Uint32 lastupdate{};                       // last time since the entity was updated
    Uint32 lastupdateserver{};                 // used to sort out old packets
    real_t vel_x{}, vel_y{}, vel_z{};          // entity velocity vector
    real_t new_x{}, new_y{}, new_z{};          // world coordinates
    real_t new_yaw{}, new_pitch{}, new_roll{}; // rotation

    // entity attributes
    real_t fskill[NUMENTITYFSKILLS]{}; // floating point general purpose variables
    Sint32 skill[NUMENTITYSKILLS]{};  // general purpose variables
    bool flags[16]{};    // engine flags
    char* string{};      // general purpose string
    light_t* light{};    // every entity has a specialized light pointer
    list_t children{};   // every entity has a list of child objects
    Uint32 parent{};     // id of the entity's "parent" entity
    TimerExperiments::EntityStates lerpPreviousState{};
    TimerExperiments::EntityStates lerpCurrentState{};
    TimerExperiments::EntityStates lerpRenderState{};
    real_t lerp_ox{};
    real_t lerp_oy{};
    bool bNeedsRenderPositionInit = true;
    bool bUseRenderInterpolation = false;

    // a pointer to the entity's location in a list (ie the map list of entities)
    node_t* mynode {};

    //--PUBLIC GENERAL ENTITY STUFF--
    Sint32& interactedByMonster;  //skill[47] for use with monsterAllyInteractTarget
    real_t& highlightForUI;       //fskill[29] for highlighting interactibles
    real_t& highlightForUIGlow;   //fskill[28] for highlighting animation
    real_t& grayscaleGLRender;    //fskill[27] for grayscale rendering

    Uint32 uid{}; // entity uid
};


#endif //BARONY_ENTITY_BASE_HPP
