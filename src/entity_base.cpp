//
// Created by remex on 10/3/23.
//

#include "entity_base.hpp"

void Entity_Base::setUID(Uint32 new_uid)
{
    uid = new_uid;
}

Entity_Base::Entity_Base(Sint32 &interactedByMonster, real_t &highlightForUi, real_t &highlightForUiGlow,real_t &grayscaleGlRender) :
    interactedByMonster(interactedByMonster),
    highlightForUI(highlightForUi),
    highlightForUIGlow(highlightForUiGlow),
    grayscaleGLRender(grayscaleGlRender) {}

Entity_Base::~Entity_Base() = default;
