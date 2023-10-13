//
// Created by remex on 10/5/23.
//


#include "main.hpp"
#include "game.hpp"
#include "stat.hpp"
#include "items.hpp"
#include "monster.hpp"
#include "engine/audio/sound.hpp"
#include "interface/interface.hpp"
#include "collision.hpp"
#include "scores.hpp"
#include "creature.h"
#include "magic/magic.hpp"
#include "entity.hpp"
#include "net.hpp"
#include "colors.hpp"
#include "paths.hpp"
#include "book.hpp"
#include "shops.hpp"
#ifdef STEAMWORKS
#include <steam/steam_api.h>
#endif
#include "player.hpp"
#include "menu.hpp"
#include "mod_tools.hpp"
#ifdef __ARM_NEON__
#include <arm_neon.h>
#endif
#include "ui/MainMenu.hpp"
#include "ui/GameUI.hpp"


Creature::Creature(Sint32 in_sprite, Uint32 pos, list_t *entlist, list_t *creaturelist) :
        Entity(in_sprite, pos, entlist, creaturelist),
        monsterState(skill[0]),
        monsterTarget(skill[1]),
        monsterTargetX(fskill[2]),
        monsterTargetY(fskill[3]),
        monsterSpecialTimer(skill[29]),
        monsterSpecialState(skill[33]),
        monsterFootstepType(skill[32]),
        monsterLookTime(skill[4]),
        monsterMoveTime(skill[6]),
        monsterLookDir(fskill[4]),
        playerLevelEntrySpeech(skill[18]),
        playerAliveTime(skill[12]),
        playerVampireCurse(skill[51]),
        playerAutomatonDeathCounter(skill[15]),
        playerCreatedDeathCam(skill[16]),
        monsterShadowInitialMimic(skill[34]),
        monsterShadowDontChangeName(skill[35]),
        monsterLichFireMeleeSeq(skill[34]),
        monsterLichFireMeleePrev(skill[35]),
        monsterLichIceCastSeq(skill[34]),
        monsterLichIceCastPrev(skill[35]),
        monsterLichMagicCastCount(skill[37]),
        monsterLichMeleeSwingCount(skill[38]),
        monsterLichBattleState(skill[27]),
        monsterLichTeleportTimer(skill[40]),
        monsterLichAllyStatus(skill[18]),
        monsterLichAllyUID(skill[17]),
        monsterPathBoundaryXStart(skill[14]),
        monsterPathBoundaryYStart(skill[15]),
        monsterPathBoundaryXEnd(skill[16]),
        monsterPathBoundaryYEnd(skill[17]),
        monsterStoreType(skill[18]),
        monsterStrafeDirection(skill[39]),
        monsterPathCount(skill[38]),
        monsterAllyState(skill[43]),
        monsterAllyPickupItems(skill[44]),
        monsterAllyInteractTarget(skill[45]),
        monsterAllyClass(skill[46]),
        monsterDefend(skill[47]),
        monsterAllySpecial(skill[48]),
        monsterAllySpecialCooldown(skill[49]),
        monsterAllySummonRank(skill[50]),
        monsterKnockbackVelocity(fskill[9]),
        monsterKnockbackUID(skill[51]),
        monsterFearfulOfUid(skill[53]),
        monsterIllusionTauntingThisUid(skill[55]),
        monsterLastDistractedByNoisemaker(skill[55]), // shares with above as above only applies to inner demons.
	    monsterExtraReflexTick(skill[56]),
        monsterSentrybotLookDir(fskill[10]),
        monsterKnockbackTangentDir(fskill[11]),
        playerStrafeVelocity(fskill[12]),
        playerStrafeDir(fskill[13]),
        monsterSpecialAttackUnequipSafeguard(fskill[14]),
        monsterHitTime(skill[7]),
        playerStartDir(skill[1])
{
}

void Creature::initMonster(int mySprite)
{
    if (multiplayer != CLIENT) {
	    sprite = mySprite;
		if ( Stat* myStats = this->getStats() )
		{
			if ( myStats->type != NOTHING )
			{
				int specialNPCModel = MonsterData_t::getSpecialNPCBaseModel(*myStats);
				if ( specialNPCModel != 0 )
				{
					sprite = specialNPCModel;
				}
				else
				{
					if ( skill[3] != 0 ) // MONSTER_INIT, loading a savefile
					{
						auto key = MonsterData_t::getKeyFromSprite(sprite);
						if ( myStats->sex == sex_t::MALE )
						{
							if ( key == "monster female" )
							{
								// need to swap
								int newsprite = MonsterData_t::getSpriteFromKey(sprite, "monster male");
								if ( newsprite != 0 )
								{
									sprite = newsprite;
								}
							}
						}
						else if ( myStats->sex == sex_t::FEMALE )
						{
							if ( key == "monster male" )
							{
								// need to swap
								int newsprite = MonsterData_t::getSpriteFromKey(sprite, "monster female");
								if ( newsprite != 0 )
								{
									sprite = newsprite;
								}
							}
						}
					}
				}
			}
		}
	}

	//Common flags.
	flags[UPDATENEEDED] = true;
	flags[BLOCKSIGHT] = true;
	flags[INVISIBLE] = false;

	Monster monsterType = this->getMonsterTypeFromSprite();

    if (monsterType != SPIDER) {
	    focalx = limbs[monsterType][0][0];
	    focaly = limbs[monsterType][0][1];
	    focalz = limbs[monsterType][0][2];
	} else {
		if (arachnophobia_filter)
		{
		    focalx = limbs[CRAB][0][0];
		    focaly = limbs[CRAB][0][1];
		    focalz = limbs[CRAB][0][2];
		}
		else
		{
		    focalx = limbs[SPIDER][0][0];
		    focaly = limbs[SPIDER][0][1];
		    focalz = limbs[SPIDER][0][2];
		}
	}

	switch ( monsterType )
	{
		case GNOME:
			monsterFootstepType = MONSTER_FOOTSTEP_USE_BOOTS;
			monsterSpellAnimation = MONSTER_SPELLCAST_SMALL_HUMANOID;
			break;
		case KOBOLD:
			monsterFootstepType = MONSTER_FOOTSTEP_USE_BOOTS;
			monsterSpellAnimation = MONSTER_SPELLCAST_SMALL_HUMANOID;
			break;
		case HUMAN:
			monsterFootstepType = MONSTER_FOOTSTEP_USE_BOOTS;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case RAT:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case GOBLIN:
			monsterFootstepType = MONSTER_FOOTSTEP_USE_BOOTS;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case SLIME:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case TROLL:
			monsterFootstepType = MONSTER_FOOTSTEP_STOMP;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case OCTOPUS:
			// unused
			break;
		case SPIDER:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case GHOUL:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case SKELETON:
			monsterFootstepType = MONSTER_FOOTSTEP_SKELETON;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case SCORPION:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case CREATURE_IMP:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case CRAB:
			// unused
			break;
		case DEMON:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case SUCCUBUS:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case MIMIC:
			// unused
			break;
		case LICH:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case MINOTAUR:
			monsterFootstepType = MONSTER_FOOTSTEP_STOMP;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case DEVIL:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case SHOPKEEPER:
			monsterFootstepType = MONSTER_FOOTSTEP_LEATHER;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case SCARAB:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case CRYSTALGOLEM:
			monsterFootstepType = MONSTER_FOOTSTEP_STOMP;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case INCUBUS:
			monsterFootstepType = MONSTER_FOOTSTEP_LEATHER;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case VAMPIRE:
			monsterFootstepType = MONSTER_FOOTSTEP_LEATHER;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case SHADOW:
			monsterFootstepType = MONSTER_FOOTSTEP_USE_BOOTS;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case COCKATRICE:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		case INSECTOID:
			monsterFootstepType = MONSTER_FOOTSTEP_USE_BOOTS;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case GOATMAN:
			monsterFootstepType = MONSTER_FOOTSTEP_USE_BOOTS;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case AUTOMATON:
			monsterFootstepType = MONSTER_FOOTSTEP_USE_BOOTS;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case LICH_ICE:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case LICH_FIRE:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_HUMANOID;
			break;
		case SENTRYBOT:
		case SPELLBOT:
		case GYROBOT:
		case DUMMYBOT:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
		default:
			monsterFootstepType = MONSTER_FOOTSTEP_NONE;
			monsterSpellAnimation = MONSTER_SPELLCAST_NONE;
			break;
	}
	return;
}

//void Creature::actMonsterLimb(bool processLight)
//{
//	//If no longer part of a monster, delete the limb.
//	Entity *parentEnt = nullptr;
//	if ( (parentEnt = uidToEntity(skill[2])) == nullptr )
//	{
//		if ( multiplayer == CLIENT )
//		{
//			if ( light )
//			{
//				list_RemoveNode(light->node);
//				light = nullptr;
//			}
//		}
//		list_RemoveNode(mynode);
//		return;
//	}
//
//	//Do something magical beyond my comprehension.
//	if ( multiplayer != CLIENT )
//	{
//		for ( int i = 0; i < MAXPLAYERS; ++i )
//		{
//			if ( inrange[i] )
//			{
//				if ( client_selected[i] == this || selectedEntity[i] == this )
//				{
//					parentEnt->skill[13] = i + 1;
//				}
//			}
//		}
//	}
//
//	if ( processLight )
//	{
//		//Only run by monsters who can carry stuff (like torches). Sorry, rats.
//		if ( light != nullptr )
//		{
//			list_RemoveNode(light->node);
//			light = nullptr;
//		}
//
//        const char* lightName = nullptr;
//		if ( flags[INVISIBLE] == false )
//		{
//			if ( sprite == 93 )   // torch
//			{
//                lightName = "npc_torch";
//			}
//			else if ( sprite == 94 )     // lantern
//			{
//                lightName = "npc_lantern";
//			}
//			else if ( sprite == 529 )	// crystal shard
//			{
//                lightName = "npc_shard";
//			}
//		}
//
//		if ( lightName )
//		{
//			light = addLight(x / 16, y / 16, lightName);
//		}
//	}
//
//	if ( parentEnt && parentEnt->behavior == &actMonster && dynamic_cast<Creature*>(parentEnt)->monsterEntityRenderAsTelepath == 1 )
//	{
//		monsterEntityRenderAsTelepath = 1;
//	}
//	else
//	{
//		monsterEntityRenderAsTelepath = 0;
//	}
//}


/*-------------------------------------------------------------------------------

Creature::effectTimes

Counts down effect timers and toggles effects whose timers reach zero - server only function

-------------------------------------------------------------------------------*/
void Creature::effectTimes()
{
	Stat* myStats = this->getStats();
	int player;
	spell_t * spell = NULL;
	node_t * node = NULL;
	int count = 0;

	if ( myStats == NULL )
	{
		return;
	}
	if ( this->behavior == &actPlayer )
	{
		player = this->skill[2];
	}
	else
	{
		player = -1;
	}


	spell_t * invisibility_hijacked = nullptr; //If NULL, function proceeds as normal. If points to something, it ignores the invisibility timer since a spell is doing things. //TODO: Incorporate the spell into isInvisible() instead?
	spell_t * levitation_hijacked = nullptr; //If NULL, function proceeds as normal. If points to something, it ignore the levitation timer since a spell is doing things.
	spell_t * reflectMagic_hijacked = nullptr;
	spell_t * amplifyMagic_hijacked = nullptr;
	spell_t * vampiricAura_hijacked = nullptr;
	//Handle magic effects (like invisibility)
	for ( node = myStats->magic_effects.first; node; node = node->next, ++count )
	{
		//printlog( "%s\n", "Potato.");
		//Handle magic effects.
		spell = (spell_t *)node->element;
		if ( !spell->sustain )
		{
			node_t * temp = NULL;
			if ( node->prev )
			{
				temp = node->prev;
			}
			else if ( node->next )
			{
				temp = node->next;
			}
			spell->magic_effects_node = NULL; //To prevent recursive removal, which results in a crash.
			if ( player > 0 && multiplayer == SERVER && !players[player]->isLocalPlayer() )
			{
				strcpy((char*)net_packet->data, "UNCH");
				net_packet->data[4] = player;
				SDLNet_Write32(spell->ID, &net_packet->data[5]);
				net_packet->address.host = net_clients[player - 1].host;
				net_packet->address.port = net_clients[player - 1].port;
				net_packet->len = 9;
				sendPacketSafe(net_sock, -1, net_packet, player - 1);
			}
			list_RemoveNode(node); //Bugger the spell.
			node = temp;
			if ( !node )
			{
				break; //Done with list. Stop.
			}
			continue; //Skip this spell.
		}

		bool unsustain = false;
		switch ( spell->ID )
		{
			case SPELL_INVISIBILITY:
				invisibility_hijacked = spell;
				if ( !myStats->EFFECTS[EFF_INVISIBLE] )
				{
					for ( int c = 0; c < MAXPLAYERS; ++c )
					{
						if ( players[c] && players[c]->entity && players[c]->entity == uidToEntity(spell->caster) )
						{
							messagePlayer(c, MESSAGE_COMBAT, Language::get(591));    //If cure ailments or somesuch bombs the status effects.
						}
					}
					node_t * temp = nullptr;
					if ( node->prev )
					{
						temp = node->prev;
					}
					else if ( node->next )
					{
						temp = node->next;
					}
					unsustain = true;
					list_RemoveNode(node); //Remove this here node.
					node = temp;
				}
				break;
			case SPELL_LEVITATION:
				levitation_hijacked = spell;
				if ( !myStats->EFFECTS[EFF_LEVITATING] )
				{
					for ( int c = 0; c < MAXPLAYERS; ++c )
					{
						if ( players[c] && players[c]->entity && players[c]->entity == uidToEntity(spell->caster) )
						{
							messagePlayer(c, MESSAGE_COMBAT, Language::get(592));
						}
					}
					node_t * temp = nullptr;
					if ( node->prev )
					{
						temp = node->prev;
					}
					else if ( node->next )
					{
						temp = node->next;
					}
					unsustain = true;
					list_RemoveNode(node); //Remove this here node.
					node = temp;
				}
				break;
			case SPELL_REFLECT_MAGIC:
				reflectMagic_hijacked = spell;
				if ( !myStats->EFFECTS[EFF_MAGICREFLECT] )
				{
					for ( int c = 0; c < MAXPLAYERS; ++c )
					{
						if ( players[c] && players[c]->entity && players[c]->entity == uidToEntity(spell->caster) )
						{
							messagePlayer(c, MESSAGE_COMBAT, Language::get(2446));
						}
					}
					node_t * temp = nullptr;
					if ( node->prev )
					{
						temp = node->prev;
					}
					else if ( node->next )
					{
						temp = node->next;
					}
					unsustain = true;
					list_RemoveNode(node); //Remove this here node.
					node = temp;
				}
				break;
			case SPELL_AMPLIFY_MAGIC:
				amplifyMagic_hijacked = spell;
				if ( !myStats->EFFECTS[EFF_MAGICAMPLIFY] )
				{
					for ( int c = 0; c < MAXPLAYERS; ++c )
					{
						if ( players[c] && players[c]->entity && players[c]->entity == uidToEntity(spell->caster) )
						{
							messagePlayer(c, MESSAGE_COMBAT, Language::get(3441));
						}
					}
					node_t * temp = nullptr;
					if ( node->prev )
					{
						temp = node->prev;
					}
					else if ( node->next )
					{
						temp = node->next;
					}
					unsustain = true;
					list_RemoveNode(node); //Remove this here node.
					node = temp;
				}
				break;
			case SPELL_VAMPIRIC_AURA:
				vampiricAura_hijacked = spell;
				if ( !myStats->EFFECTS[EFF_VAMPIRICAURA] )
				{
					for ( int c = 0; c < MAXPLAYERS; ++c )
					{
						if ( players[c] && players[c]->entity && players[c]->entity == uidToEntity(spell->caster) )
						{
							messagePlayer(c, MESSAGE_COMBAT, Language::get(2447));
						}
					}
					node_t * temp = nullptr;
					if ( node->prev )
					{
						temp = node->prev;
					}
					else if ( node->next )
					{
						temp = node->next;
					}
					unsustain = true;
					list_RemoveNode(node); //Remove this here node.
					node = temp;
				}
				break;
			default:
				//Unknown spell, undefined effect. Like, say, a fireball spell wound up in here for some reason. That's a nono.
				printlog("[entityEffectTimes] Warning: magic_effects spell that's not relevant. Should not be in the magic_effects list!\n");
				list_RemoveNode(node);
		}

		if ( unsustain )
		{
			// the node has been removed, tell the client to unsustain in their list.
			if ( player > 0 && multiplayer == SERVER && !players[player]->isLocalPlayer() )
			{
				strcpy((char*)net_packet->data, "UNCH");
				net_packet->data[4] = player;
				SDLNet_Write32(spell->ID, &net_packet->data[5]);
				net_packet->address.host = net_clients[player - 1].host;
				net_packet->address.port = net_clients[player - 1].port;
				net_packet->len = 9;
				sendPacketSafe(net_sock, -1, net_packet, player - 1);
			}
		}

		if ( !node )
		{
			break;    //BREAK OUT. YEAAAAAH. Because otherwise it crashes.
		}
	}
	if ( count )
	{
		//printlog( "Number of magic effects spells: %d\n", count); //Debugging output.
	}

	bool dissipate = true;
	bool updateClient = false;
	spell_t * unsustainSpell = nullptr;

	for ( int c = 0; c < NUMEFFECTS; c++ )
	{
		if ( myStats->EFFECTS_TIMERS[c] > 0 )
		{
			myStats->EFFECTS_TIMERS[c]--;
			if ( c == EFF_POLYMORPH )
			{
				if ( myStats->EFFECTS_TIMERS[c] == TICKS_PER_SECOND * 15 )
				{
					//playSoundPlayer(player, 32, 128);
					messagePlayer(player, MESSAGE_STATUS, Language::get(3193));
				}
			}
			else if ( c == EFF_SHAPESHIFT )
			{
				if ( myStats->EFFECTS_TIMERS[c] == TICKS_PER_SECOND * 15 )
				{
					playSoundPlayer(player, 32, 128);
					messagePlayer(player, MESSAGE_STATUS, Language::get(3475));
				}
			}
			if ( myStats->EFFECTS_TIMERS[c] == 0 )
			{
				myStats->EFFECTS[c] = false;
				switch ( c )
				{
					case EFF_ASLEEP:
						messagePlayer(player, MESSAGE_STATUS, Language::get(593));
						if ( monsterAllyGetPlayerLeader() && monsterAllySpecial == ALLY_SPECIAL_CMD_REST )
						{
							monsterAllySpecial = ALLY_SPECIAL_CMD_NONE;
							myStats->EFFECTS[EFF_HP_REGEN] = false;
							myStats->EFFECTS_TIMERS[EFF_HP_REGEN] = 0;
						}
						break;
					case EFF_HP_REGEN:
						//messagePlayer(player, MESSAGE_STATUS, Language::get(3476));
						updateClient = true;
						break;
					case EFF_MP_REGEN:
						//messagePlayer(player, MESSAGE_STATUS, Language::get(3477));
						updateClient = true;
						break;
					case EFF_POISONED:
						messagePlayer(player, MESSAGE_STATUS, Language::get(594));
						break;
					case EFF_STUNNED:
						//messagePlayer(player, MESSAGE_STATUS, Language::get(595));
						break;
					case EFF_CONFUSED:
						messagePlayer(player, MESSAGE_STATUS, Language::get(596));
						break;
					case EFF_DRUNK:
						messagePlayer(player, MESSAGE_STATUS, Language::get(597));
						break;
					case EFF_INVISIBLE:
						; //To make the compiler shut up: "error: a label can only be part of a statement and a declaration is not a statement"
						dissipate = true; //Remove the effect by default.
						if ( invisibility_hijacked )
						{
							bool sustained = false;
							Entity* caster = uidToEntity(invisibility_hijacked->caster);
							if ( caster )
							{
								//Deduct mana from caster. Cancel spell if not enough mana (simply leave sustained at false).
								bool deducted = caster->safeConsumeMP(1); //Consume 1 mana ever duration / mana seconds
								if ( deducted )
								{
									sustained = true;
									myStats->EFFECTS[c] = true;
									myStats->EFFECTS_TIMERS[c] = invisibility_hijacked->channel_duration;
								}
								else
								{
									int i = 0;
									for ( i = 0; i < MAXPLAYERS; ++i )
									{
										if ( players[i]->entity == caster )
										{
											messagePlayer(i, MESSAGE_STATUS, Language::get(598));
										}
									}
									unsustainSpell = invisibility_hijacked;
									list_RemoveNode(invisibility_hijacked->magic_effects_node); //Remove it from the entity's magic effects. This has the side effect of removing it from the sustained spells list too.
																								//list_RemoveNode(invisibility_hijacked->sustain_node); //Remove it from the channeled spells list.
								}
							}
							if ( sustained )
							{
								dissipate = false;    //Sustained the spell, so do not stop being invisible.
							}
						}
						if ( dissipate )
						{
							if ( !this->isBlind() )
							{
								messagePlayer(player, MESSAGE_STATUS, Language::get(599));
							}
						}
						break;
					case EFF_BLIND:
						if ( !this->isBlind() )
						{
							messagePlayer(player, MESSAGE_STATUS, Language::get(600));
						}
						else
						{
							messagePlayer(player, MESSAGE_STATUS, Language::get(601));
						}
						break;
					case EFF_GREASY:
						messagePlayer(player, MESSAGE_STATUS, Language::get(602));
						break;
					case EFF_MESSY:
						messagePlayer(player, MESSAGE_STATUS, Language::get(603));
						break;
					case EFF_FAST:
						messagePlayer(player, MESSAGE_STATUS, Language::get(604));
						break;
					case EFF_PARALYZED:
						messagePlayer(player, MESSAGE_STATUS, Language::get(605));
						break;
					case EFF_POTION_STR:
						messagePlayer(player, MESSAGE_STATUS, Language::get(3355));
						break;
					case EFF_LEVITATING:
						; //To make the compiler shut up: "error: a label can only be part of a statement and a declaration is not a statement"
						dissipate = true; //Remove the effect by default.
						if ( levitation_hijacked )
						{
							bool sustained = false;
							Entity* caster = uidToEntity(levitation_hijacked->caster);
							if ( caster )
							{
								//Deduct mana from caster. Cancel spell if not enough mana (simply leave sustained at false).
								bool deducted = caster->safeConsumeMP(1); //Consume 1 mana ever duration / mana seconds
								if ( deducted )
								{
									sustained = true;
									myStats->EFFECTS[c] = true;
									myStats->EFFECTS_TIMERS[c] = levitation_hijacked->channel_duration;
								}
								else
								{
									int i = 0;
									for ( i = 0; i < MAXPLAYERS; ++i )
									{
										if ( players[i]->entity == caster )
										{
											messagePlayer(i, MESSAGE_STATUS, Language::get(606));    //TODO: Unhardcode name?
										}
									}
									unsustainSpell = levitation_hijacked;
									list_RemoveNode(levitation_hijacked->magic_effects_node); //Remove it from the entity's magic effects. This has the side effect of removing it from the sustained spells list too.
								}
							}
							if ( sustained )
							{
								dissipate = false;    //Sustained the spell, so do not stop levitating.
							}
						}
						if ( dissipate )
						{
							if ( !isLevitating(myStats) )
							{
								messagePlayer(player, MESSAGE_STATUS, Language::get(607));
							}
						}
						break;
					case EFF_TELEPATH:
						if ( myStats->mask != nullptr && myStats->mask->type == TOOL_BLINDFOLD_TELEPATHY )
						{
							// don't play any messages since we'll reset the counter in due time.
							// likely to happen on level change.
						}
						else
						{
							setEffect(EFF_TELEPATH, false, 0, true);
							messagePlayer(player, MESSAGE_STATUS, Language::get(608));
							if ( player >= 0 && players[player]->isLocalPlayer() )
							{
								for (node_t * mapNode = map.creatures->first; mapNode != nullptr; mapNode = mapNode->next )
								{
									Creature* mapCreature = (Creature*)mapNode->element;
									if ( mapCreature )
									{
										// undo telepath rendering.
										mapCreature->monsterEntityRenderAsTelepath = 0;
									}
								}
							}
						}
						break;
					case EFF_VOMITING:
						messagePlayer(player, MESSAGE_STATUS, Language::get(609));
						if ( myStats->HUNGER > getEntityHungerInterval(player, this, myStats, HUNGER_INTERVAL_OVERSATIATED) )
						{
							messagePlayer(player, MESSAGE_STATUS, Language::get(610));
						}
						else if ( myStats->HUNGER > getEntityHungerInterval(player, this, myStats, HUNGER_INTERVAL_WEAK)
							&& myStats->HUNGER <= getEntityHungerInterval(player, this, myStats, HUNGER_INTERVAL_HUNGRY) )
						{
							messagePlayer(player, MESSAGE_STATUS, Language::get(611));
							playSoundPlayer(player, 32, 128);
						}
						else if ( myStats->HUNGER > getEntityHungerInterval(player, this, myStats, HUNGER_INTERVAL_STARVING)
							&& myStats->HUNGER <= getEntityHungerInterval(player, this, myStats, HUNGER_INTERVAL_WEAK) )
						{
							messagePlayer(player, MESSAGE_STATUS, Language::get(612));
							playSoundPlayer(player, 32, 128);
						}
						else if ( myStats->HUNGER <= getEntityHungerInterval(player, this, myStats, HUNGER_INTERVAL_STARVING) )
						{
							myStats->HUNGER = 50;
							messagePlayer(player, MESSAGE_STATUS, Language::get(613));
							playSoundPlayer(player, 32, 128);
						}
						serverUpdateHunger(player);
						break;
					case EFF_BLEEDING:
						messagePlayer(player, MESSAGE_STATUS, Language::get(614));
						break;
					case EFF_MAGICRESIST:
						messagePlayer(player, MESSAGE_STATUS, Language::get(2470));
						break;
					case EFF_FLUTTER:
						if ( !isLevitating(myStats) )
						{
							messagePlayer(player, MESSAGE_STATUS, Language::get(607));
							if ( behavior == &actPlayer
								&& achievementObserver.playerAchievements[skill[2]].flutterShyCoordinates.first > 0.01
								&& achievementObserver.playerAchievements[skill[2]].flutterShyCoordinates.second > 0.01 )
							{
								int playerx = std::min(std::max<unsigned int>(1, this->x / 16), map.width - 2);
								int playery = std::min(std::max<unsigned int>(1, this->y / 16), map.height - 2);
								if ( map.tiles[0 + playery * MAPLAYERS + playerx * MAPLAYERS * map.height] )
								{
									// there's ground..
									achievementObserver.playerAchievements[skill[2]].checkPathBetweenObjects(this, nullptr, AchievementObserver::BARONY_ACH_FLUTTERSHY);
								}
							}
						}
						break;
					case EFF_MAGICREFLECT:
						dissipate = true; //Remove the effect by default.
						if ( reflectMagic_hijacked )
						{
							bool sustained = false;
							Entity* caster = uidToEntity(reflectMagic_hijacked->caster);
							if ( caster )
							{
								//Deduct mana from caster. Cancel spell if not enough mana (simply leave sustained at false).
								bool deducted = caster->safeConsumeMP(1); //Consume 1 mana ever duration / mana seconds
								if ( deducted )
								{
									sustained = true;
									myStats->EFFECTS[c] = true;
									myStats->EFFECTS_TIMERS[c] = reflectMagic_hijacked->channel_duration;
								}
								else
								{
									int i = 0;
									for ( i = 0; i < MAXPLAYERS; ++i )
									{
										if ( players[i]->entity == caster )
										{
											messagePlayer(i, MESSAGE_STATUS, Language::get(2474));
										}
									}
									unsustainSpell = reflectMagic_hijacked;
									list_RemoveNode(reflectMagic_hijacked->magic_effects_node); //Remove it from the entity's magic effects. This has the side effect of removing it from the sustained spells list too.
																								//list_RemoveNode(reflectMagic_hijacked->sustain_node); //Remove it from the channeled spells list.
								}
							}
							if ( sustained )
							{
								dissipate = false; //Sustained the spell, so do not stop being invisible.
							}
						}
						if ( dissipate )
						{
							messagePlayer(player, MESSAGE_STATUS, Language::get(2471));
							updateClient = true;
						}
						break;
					case EFF_MAGICAMPLIFY:
						dissipate = true; //Remove the effect by default.
						if ( amplifyMagic_hijacked )
						{
							bool sustained = false;
							Entity* caster = uidToEntity(amplifyMagic_hijacked->caster);
							if ( caster )
							{
								//Deduct mana from caster. Cancel spell if not enough mana (simply leave sustained at false).
								bool deducted = caster->safeConsumeMP(1); //Consume 1 mana ever duration / mana seconds
								if ( deducted )
								{
									sustained = true;
									myStats->EFFECTS[c] = true;
									myStats->EFFECTS_TIMERS[c] = amplifyMagic_hijacked->channel_duration;
								}
								else
								{
									int i = 0;
									for ( i = 0; i < MAXPLAYERS; ++i )
									{
										if ( players[i]->entity == caster )
										{
											messagePlayer(i, MESSAGE_STATUS, Language::get(3443));
										}
									}
									unsustainSpell = amplifyMagic_hijacked;
									list_RemoveNode(amplifyMagic_hijacked->magic_effects_node); //Remove it from the entity's magic effects. This has the side effect of removing it from the sustained spells list too.
								}
							}
							if ( sustained )
							{
								dissipate = false; //Sustained the spell, so do not stop being invisible.
							}
						}
						if ( dissipate )
						{
							messagePlayer(player, MESSAGE_STATUS, Language::get(3441));
							updateClient = true;
						}
						break;
					case EFF_VAMPIRICAURA:
						dissipate = true; //Remove the effect by default.
						if ( vampiricAura_hijacked )
						{
							bool sustained = false;
							Creature* caster = uidToCreature(vampiricAura_hijacked->caster);
							if ( caster )
							{
								//Deduct mana from caster. Cancel spell if not enough mana (simply leave sustained at false).
								bool deducted = caster->safeConsumeMP(1); //Consume 3 mana ever duration / mana seconds
								if ( deducted )
								{
									sustained = true;
									myStats->EFFECTS[c] = true;
									myStats->EFFECTS_TIMERS[c] = vampiricAura_hijacked->channel_duration;

									// monsters have a chance to un-sustain the spell each MP consume.
									if ( caster->behavior == &actMonster && local_rng.rand() % 20 == 0 )
									{
										sustained = false;
										list_RemoveNode(vampiricAura_hijacked->magic_effects_node);
									}
								}
								else
								{
									int i = 0;
									for ( i = 0; i < MAXPLAYERS; ++i )
									{
										if ( players[i]->entity == caster )
										{
											//messagePlayer(player, MESSAGE_STATUS, Language::get(2449));
										}
									}
									unsustainSpell = vampiricAura_hijacked;
									list_RemoveNode(vampiricAura_hijacked->magic_effects_node); //Remove it from the entity's magic effects. This has the side effect of removing it from the sustained spells list too.
																								//list_RemoveNode(reflectMagic_hijacked->sustain_node); //Remove it from the channeled spells list.
								}
							}
							if ( sustained )
							{
								dissipate = false; //Sustained the spell, so do not stop being invisible.
							}
						}
						if ( dissipate )
						{
							//if ( myStats->HUNGER > 250 )
							//{
							//	myStats->HUNGER = 252; // set to above 250 to trigger the hunger sound/messages when it decrements to 250.
							//	serverUpdateHunger(player);
							//}
							messagePlayer(player, MESSAGE_STATUS, Language::get(2449));
							updateClient = true;
						}
						break;
					case EFF_SLOW:
						messagePlayer(player, MESSAGE_STATUS, Language::get(604)); // "You return to your normal speed."
						break;
					case EFF_POLYMORPH:
						effectPolymorph = 0;
						serverUpdateEntitySkill(this, 50);
						if ( !myStats->EFFECTS[EFF_SHAPESHIFT] )
						{
							messagePlayer(player, MESSAGE_STATUS, Language::get(3185));
						}
						else
						{
							messagePlayer(player, MESSAGE_STATUS, Language::get(4303)); // wears out, no mention of 'normal' form
						}

						playSoundEntity(this, 400, 92);
						createParticleDropRising(this, 593, 1.f);
						serverSpawnMiscParticles(this, PARTICLE_EFFECT_RISING_DROP, 593);
						updateClient = true;
						break;
					case EFF_SHAPESHIFT:
						effectShapeshift = 0;
						serverUpdateEntitySkill(this, 53);
						if ( !myStats->EFFECTS[EFF_POLYMORPH] )
						{
							messagePlayer(player, MESSAGE_STATUS, Language::get(3417));
						}
						else
						{
							messagePlayer(player, MESSAGE_STATUS, Language::get(4302)); // return to your 'abnormal' form
						}

						playSoundEntity(this, 400, 92);
						createParticleDropRising(this, 593, 1.f);
						serverSpawnMiscParticles(this, PARTICLE_EFFECT_RISING_DROP, 593);
						updateClient = true;
						break;
					case EFF_TROLLS_BLOOD:
						messagePlayer(player, MESSAGE_STATUS, Language::get(3491));
						updateClient = true;
						break;
					case EFF_KNOCKBACK:
						break;
					case EFF_WITHDRAWAL:
						if ( player >= 0 && player < MAXPLAYERS )
						{
							if ( myStats->EFFECTS[EFF_DRUNK] )
							{
								// we still drunk! no need for hangover just yet...
								// extend another 15 seconds.
								myStats->EFFECTS_TIMERS[EFF_WITHDRAWAL] = TICKS_PER_SECOND * 15;
							}
							else
							{
								playSoundPlayer(player, 32, 128);
								messagePlayer(player, MESSAGE_STATUS, Language::get(3247 + local_rng.rand() % 3));
								messagePlayer(player, MESSAGE_STATUS, Language::get(3222));
								this->setEffect(EFF_WITHDRAWAL, true, -2, true); // set effect as "active"
							}
						}
						break;
					case EFF_FEAR:
						this->monsterFearfulOfUid = 0;
						messagePlayer(player, MESSAGE_STATUS, Language::get(3439));
						updateClient = true;
						break;
					case EFF_PACIFY:
					case EFF_SHADOW_TAGGED:
					case EFF_WEBBED:
						updateClient = true;
						break;
					default:
						break;
				}
				if ( player > 0 && multiplayer == SERVER )
				{
					serverUpdateEffects(player);
				}
			}
			else if ( myStats->EFFECTS_TIMERS[c] == ((TICKS_PER_SECOND * 5) - 1) )
			{
				if ( player > 0 && multiplayer == SERVER )
				{
					serverUpdateEffects(player);
				}
			}
		}
		if ( unsustainSpell )
		{
			// we need to tell the client to un-sustain from their list.
			if ( player > 0 && multiplayer == SERVER && !players[player]->isLocalPlayer() )
			{
				strcpy((char*)net_packet->data, "UNCH");
				net_packet->data[4] = player;
				SDLNet_Write32(unsustainSpell->ID, &net_packet->data[5]);
				net_packet->address.host = net_clients[player - 1].host;
				net_packet->address.port = net_clients[player - 1].port;
				net_packet->len = 9;
				sendPacketSafe(net_sock, -1, net_packet, player - 1);
			}
		}
		unsustainSpell = nullptr;
	}

	if ( updateClient )
	{
		//Only a select few effects have something that needs to be handled on the client's end.
		//(such as spawning particles for the magic reflection effect)
		//Only update the entity's effects in that case.
		serverUpdateEffectsForEntity(true);
	}
}

void Creature::succubusChooseWeapon(const Entity* target, double dist)
{

	Stat *myStats = getStats();
	if ( !myStats )
	{
		return;
	}

	if ( monsterSpecialState != 0 && monsterSpecialTimer != 0 )
	{
		return;
	}

	if ( monsterSpecialTimer == 0 && (ticks % 10 == 0) && monsterAttack == 0 )
	{
		Stat* targetStats = target->getStats();
		if ( !targetStats )
		{
			return;
		}

		// try to charm enemy.
		int specialRoll = -1;
		int bonusFromHP = 0;
		specialRoll = local_rng.rand() % 40;
		if ( myStats->HP <= myStats->MAXHP * 0.8 )
		{
			bonusFromHP += 1; // +2.5% chance if on low health
		}
		if ( myStats->HP <= myStats->MAXHP * 0.4 )
		{
			bonusFromHP += 1; // +extra 2.5% chance if on lower health
		}

		int requiredRoll = (1 + bonusFromHP + (targetStats->EFFECTS[EFF_CONFUSED] ? 4 : 0)
			+ (targetStats->EFFECTS[EFF_DRUNK] ? 2 : 0)); // +2.5% base, + extra if target is inebriated

		if ( dist < 40 )
		{
			requiredRoll += 1; //+extra 2.5% chance if dist is smaller.
		}

		if ( specialRoll < requiredRoll )
		{
			node_t * node = nullptr;
			node = itemNodeInInventory(myStats, -1, SPELLBOOK);
			if ( node != nullptr )
			{
				swapMonsterWeaponWithInventoryItem(this, myStats, node, true, true);
				monsterSpecialState = SUCCUBUS_CHARM;
				return;
			}
		}
	}
}

bool Creature::automatonCanWieldItem(const Item& item) const
{
	Stat* myStats = getStats();
	if ( !myStats )
	{
		return false;
	}

	if ( monsterAllyIndex >= 0 && (monsterAllyClass != ALLY_CLASS_MIXED || item.interactNPCUid == getUID()) )
	{
		return monsterAllyEquipmentInClass(item);
	}

	switch ( itemCategory(&item) )
	{
		case WEAPON:
			return true;
		case ARMOR:
			{
				int equipType = checkEquipType(&item);
				if ( equipType == TYPE_HAT )
				{
					return false; //No can wear hats, beep boop
				}
				return true;
			}
		case THROWN:
			return true;
		case TOOL:
			if ( itemTypeIsQuiver(item.type) )
			{
				return true;
			}
			break;
		case MAGICSTAFF:
			return true;
		default:
			return false;
	}

	return false;
}

bool Creature::shadowCanWieldItem(const Item& item) const
{
	Stat* myStats = getStats();
	if ( !myStats )
	{
		return false;
	}

	switch ( itemCategory(&item) )
	{
		case WEAPON:
			return true;
		case THROWN:
			return true;
		case ARMOR:
			if ( checkEquipType(&item) == TYPE_SHIELD )
			{
				return true; //Can only wear shields, out of armor.
			}
			return false;
		default:
			return false;
	}
}

void Creature::lichIceTeleport()
{
	monsterLichTeleportTimer = 0;
	Entity* spellTimer = createParticleTimer(this, 40, 593);
	if ( monsterState == MONSTER_STATE_LICHICE_TELEPORT_STATIONARY )
	{
		spellTimer->particleTimerEndAction = PARTICLE_EFFECT_LICHICE_TELEPORT_STATIONARY; // teleport behavior of timer.
	}
	else
	{
		spellTimer->particleTimerEndAction = PARTICLE_EFFECT_LICH_TELEPORT_ROAMING; // teleport behavior of timer.
	}
	spellTimer->particleTimerEndSprite = 593; // sprite to use for end of timer function.
	spellTimer->particleTimerCountdownAction = PARTICLE_TIMER_ACTION_SHOOT_PARTICLES;
	spellTimer->particleTimerCountdownSprite = 593;
	if ( multiplayer == SERVER )
	{
		serverSpawnMiscParticles(this, spellTimer->particleTimerEndAction, 593);
	}
}

void Creature::automatonRecycleItem()
{
	Stat* myStats = getStats();
	if ( !myStats )
	{
		return;
	}

	if ( this->monsterSpecialTimer > 0 && !(this->monsterSpecialState == AUTOMATON_RECYCLE_ANIMATION_COMPLETE) )
	{
		// if we're on cooldown, skip checking
		// also we need the callback from my->attack() to set monsterSpecialState = AUTOMATON_RECYCLE_ANIMATION_COMPLETE once the animation completes.
		return;
	}

	node_t * node = nullptr;
	node_t * nextnode = nullptr;
	int numItemsHeld = list_Size(&myStats->inventory);
	//messagePlayer(0, "Numitems: %d", numItemsHeld);
	if ( numItemsHeld < 2 )
	{
		return;
	}

	if ( this->monsterSpecialTimer == 0 && this->monsterSpecialState == AUTOMATON_RECYCLE_ANIMATION_WAITING )
	{
		// put the skill on cooldown.
		this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_AUTOMATON_RECYCLE;
	}

	int i = 0;
	int itemIndex = 0;
	int chances[10]; // max 10 items
	for ( int i = 0; i < 10; ++i )
	{
		chances[i] = -1;
	}
	int matches = 0;

	// search for valid recyclable items, set chance for valid index.
	for ( node = myStats->inventory.first; node != nullptr; node = nextnode )
	{
		if ( matches >= 10 )
		{
			break;
		}
		nextnode = node->next;
		Item* item = (Item*)node->element;
		if ( item != nullptr )
		{
			if ( (itemCategory(item) == WEAPON || itemCategory(item) == THROWN || itemCategory(item) == ARMOR)
				&& item->status > BROKEN )
			{
				chances[matches] = itemIndex;
				++matches;
			}
		}
		++itemIndex;
	}

	//messagePlayer(0, "Found %d items", matches);

	if ( matches < 2 ) // not enough valid items found.
	{
		this->monsterSpecialTimer = 250; // reset cooldown to 5 seconds to check again quicker.
		return;
	}

	if ( this->monsterSpecialState == AUTOMATON_RECYCLE_ANIMATION_WAITING )
	{
		// this is the first run of the check, we'll execute the casting animation and wait for this to be set to 1 when it ends.
		this->attack(MONSTER_POSE_SPECIAL_WINDUP1, 0, nullptr);
		return;
	}

	this->monsterSpecialState = AUTOMATON_RECYCLE_ANIMATION_WAITING; // reset my special state after the previous lines.
	int pickItem1 = local_rng.rand() % matches; // pick random valid item index in inventory
	int pickItem2 = local_rng.rand() % matches;
	while ( pickItem2 == pickItem1 )
	{
		pickItem2 = local_rng.rand() % matches; // make sure index 2 is unique
	}

	itemIndex = 0;
	Item* item1 = nullptr;
	Item* item2 = nullptr;

	// search again for the 2 indexes, store the items and nodes.
	for ( node = myStats->inventory.first; node != nullptr; node = nextnode )
	{
		nextnode = node->next;
		if ( chances[pickItem1] == itemIndex )
		{
			item1 = (Item*)node->element;
		}
		else if ( chances[pickItem2] == itemIndex )
		{
			item2 = (Item*)node->element;
		}
		++itemIndex;
	}

	if ( item1 == nullptr || item2 == nullptr )
	{
		return;
	}

	//messagePlayer(0, "made it past");

	int maxGoldValue = ((items[item1->type].value + items[item2->type].value) * 2) / 3;
	if ( local_rng.rand() % 2 == 0 )
	{
		maxGoldValue = ((items[item1->type].value + items[item2->type].value) * 1) / 2;
	}
	int minGoldValue = ((items[item1->type].value + items[item2->type].value) * 1) / 3;
	ItemType type;
	// generate a weapon/armor piece and add it into the inventory.
	switch ( local_rng.rand() % 10 )
	{
		case 0:
		case 1:
		case 2:
			type = itemTypeWithinGoldValue(WEAPON, minGoldValue, maxGoldValue);
			break;
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			type = itemTypeWithinGoldValue(ARMOR, minGoldValue, maxGoldValue);
			break;
		case 9:
			type = itemTypeWithinGoldValue(THROWN, minGoldValue, maxGoldValue);
			break;
		default:
			break;
	}

	if ( type != GEM_ROCK ) // found an item in category
	{
		Item* item = nullptr;
		// recycle item1 or item2, reduce durability.
		if ( local_rng.rand() % 2 == 0 )
		{
			item = newItem(type, item1->status, item1->beatitude, 1, local_rng.rand(), item1->identified, &myStats->inventory);
			item1->status = static_cast<Status>(std::max(0, item1->status - 2));
		}
		else
		{
			item = newItem(type, item2->status, item2->beatitude, 1, local_rng.rand(), item2->identified, &myStats->inventory);
			item2->status = static_cast<Status>(std::max(0, item2->status - 2));
		}
		// drop newly created item. To pickup if possible or leave behind if overburdened.
		dropItemMonster(item, this, myStats);
		//messagePlayer(0, "Generated %d!", type);
		if ( myStats->leader_uid != 0 )
		{
			for ( int c = 0; c < MAXPLAYERS; ++c )
			{
				if ( players[c] && players[c]->entity )
				{
					uint32_t playerUid = players[c]->entity->getUID();
					if ( playerUid == myStats->leader_uid
						&& (item1->ownerUid == playerUid || item2->ownerUid == playerUid) )
					{
						steamAchievementClient(c, "BARONY_ACH_RECYCLER");
						break;
					}
				}
			}
		}
		//messagePlayer(0, "%d, %d", item1->ownerUid, item2->ownerUid);
	}

	return;
}

void Creature::shadowSpecialAbility(bool initialMimic)
{
	//1. Turn invisible.
	//2. Mimic target's weapon & shield (only on initial cast).
	//3. Random chance to mimic other things.
	//4. Teleport to target.

	Stat *myStats = getStats();
	if ( !myStats )
	{
		return;
	}

	Entity *target = uidToEntity(monsterTarget);
    Creature* targetCrtr = (Creature*)(*&target);
	if ( !target )
	{
		//messagePlayer(clientnum, "Shadow's target deaded!");
		monsterReleaseAttackTarget();
		return;
	}

	Stat* targetStats = target->getStats();
	if ( !targetStats )
	{
		monsterReleaseAttackTarget(true); //Force get rid of the target since it has no stats -- it's useless to us!
		return;
	}

	//1. Turn invisible.
	//myStats->EFFECTS[EFF_INVISIBLE] = true;
	//myStats->EFFECTS_TIMERS[EFF_INVISIBLE] = 0; //Does not deactivate until it attacks.
	//messagePlayer(clientnum, "Turned invisible!");

	int numSpellsToMimic = 2;
	int numSkillsToMimic = 3;

	//2. Copy target's weapon & shield on initial activation of this ability only.
	if ( initialMimic )
	{
		if ( !monsterShadowDontChangeName )
		{
			std::basic_string<char> newName = "Shadow of ";
			if ( strcmp(targetStats->name, "") != 0 )
			{
				newName += targetStats->name;
			}
			else
			{
				newName += monstertypename[targetStats->type];
			}
			strcpy(myStats->name, newName.c_str());
		}

		monsterShadowInitialMimic = 0;
		//messagePlayer(clientnum, "[DEBUG: Creature::shadowSpecialAbility() ] Initial mimic.");
		//TODO: On initial mimic, need to reset some the tracking info on what's already been mimic'ed.
		//Such as dropping already equipped items.
		bool shadowAlreadyStartedWithWeapon = (myStats->weapon != nullptr);
		if ( !shadowAlreadyStartedWithWeapon )
		{
			if ( itemCategory(myStats->weapon) == SPELLBOOK )
			{
				//Don't want to drop spellbooks, though. Then the shadow would lose the spell.
				addItemToMonsterInventory(myStats->weapon);
				myStats->weapon = nullptr;
			}
			else
			{
				dropItemMonster(myStats->weapon, this, myStats);
			}
		}
		dropItemMonster(myStats->shield, this, myStats);

		//Skills do not get reset.
		//Attributes do not get reset.
		//Spells do not get reset.

		//On initial mimic, copy best melee weapon and shield from target's hands or inventory.
		Item *bestMeleeWeapon = target->getBestMeleeWeaponIHave();
		Item *bestShield = target->getBestShieldIHave();

		if ( bestMeleeWeapon && !shadowAlreadyStartedWithWeapon )
		{
			Item* wieldedCopy = new Item();
			copyItem(wieldedCopy, bestMeleeWeapon);
			wieldedCopy->appearance = MONSTER_ITEM_UNDROPPABLE_APPEARANCE;
			monsterEquipItem(*wieldedCopy, &myStats->weapon);
		}

		if ( bestShield )
		{
			Item* wieldedCopy = new Item();
			copyItem(wieldedCopy, bestShield);
			wieldedCopy->appearance = MONSTER_ITEM_UNDROPPABLE_APPEARANCE;
			monsterEquipItem(*wieldedCopy, &myStats->shield);
		}

		//On initial mimic, copy more spells & skills.
		numSkillsToMimic += local_rng.rand()%3 + 1;
		numSpellsToMimic += local_rng.rand()%3 + 1;

		if ( targetCrtr && targetCrtr->behavior == actPlayer )
		{
			messagePlayer(target->skill[2], MESSAGE_HINT, Language::get(2516));
		}
	}
	else
	{
		if ( targetCrtr && targetCrtr->behavior == actPlayer )
		{
			messagePlayer(target->skill[2], MESSAGE_HINT, Language::get(2517));
		}
	}

	//3. Random chance to mimic other things.
	//Mimic target's skills (proficiencies).
	//First, get proficiencies to mimic:
	std::vector<int> skillsCanMimic;
	for ( int i = 0; i < NUMPROFICIENCIES; ++i )
	{
		if ( targetStats->PROFICIENCIES[i] > myStats->PROFICIENCIES[i] )
		{
			//Target is better, can mimic this proficiency.
			skillsCanMimic.push_back(i);
		}
	}
	//Now choose a random skill and copy it over.
	for ( int skillsMimicked = 0; skillsCanMimic.size() && skillsMimicked < numSkillsToMimic; ++skillsMimicked )
	{
		int choosen = local_rng.rand()%skillsCanMimic.size();
		myStats->PROFICIENCIES[skillsCanMimic[choosen]] = targetStats->PROFICIENCIES[skillsCanMimic[choosen]];

		//messagePlayer(clientnum, "DEBUG: Shadow mimicked skill %d.", skillsCanMimic[choosen]);
		skillsCanMimic.erase(skillsCanMimic.begin() + choosen); //No longer an eligible skill.
	}

	//Mimick spells.
	//First, search the target's inventory for spells the shadow likes.
	//For a player, it searches for the spell item.
	//For a monster, it searches for spellbooks (since monsters don't learn spells the normal way.
	std::vector<int> spellsCanMimic; //Array of spell IDs.
	if ( targetCrtr && targetCrtr->behavior == actMonster && itemCategory(targetStats->weapon) == SPELLBOOK )
	{
		int spellID = getSpellIDFromSpellbook(targetStats->weapon->type);
		if ( spellID != SPELL_NONE && shadowCanMimickSpell(spellID) && !monsterHasSpellbook(getSpellIDFromSpellbook(targetStats->weapon->type)) )
		{
			spellsCanMimic.push_back(spellID);
		}
	}
	for (node_t * node = targetStats->inventory.first; node; node = node->next)
	{
		Item* item = static_cast<Item*>(node->element);
		if ( !item )
		{
			continue;
		}

		if ( targetCrtr && targetCrtr->behavior == actPlayer )
		{
			//Search player's inventory for the special spell item.
			if ( itemCategory(item) != SPELL_CAT )
			{
				continue;
			}

			spell_t *spell = getSpellFromItem(target->skill[2], item); //Do not free or delete this.
			if ( !spell )
			{
				continue;
			}

			int spellbookType = getSpellbookFromSpellID(spell->ID);
			if ( spellbookType == WOODEN_SHIELD )
			{
				continue;
			}
			Item* spellbook = newItem(static_cast<ItemType>(spellbookType), static_cast<Status>(DECREPIT), 0, 1, local_rng.rand(), true, nullptr);
			if ( !spellbook )
			{
				continue;
			}

			if ( shadowCanMimickSpell(spell->ID) && !monsterHasSpellbook(getSpellIDFromSpellbook(spellbook->type)) )
			{
				spellsCanMimic.push_back(spell->ID);
			}

			free(spellbook);
		}
		else
		{
			//Search monster's inventory for spellbooks.
			if ( itemCategory(item) != SPELLBOOK )
			{
				continue;
			}

			spell_t *spell = getSpellFromID(getSpellIDFromSpellbook(item->type));

			if ( shadowCanMimickSpell(spell->ID) && !monsterHasSpellbook(getSpellIDFromSpellbook(item->type)) )
			{
				spellsCanMimic.push_back(spell->ID);
			}
		}
	}
	//Now randomly choose & copy over a spell.
	for ( int spellsMimicked = 0; spellsCanMimic.size() && spellsMimicked < numSkillsToMimic; ++spellsMimicked )
	{
		int choosen = local_rng.rand()%spellsCanMimic.size();

		int spellbookType = getSpellbookFromSpellID(spellsCanMimic[choosen]);
		if ( spellbookType == WOODEN_SHIELD )
		{
			continue;
		}
		Item* spellbook = newItem(static_cast<ItemType>(spellbookType), static_cast<Status>(DECREPIT), 0, 1, local_rng.rand(), true, nullptr);
		if ( !spellbook )
		{
			continue;
		}

		if ( spellbook )
		{
			spellbook->appearance = MONSTER_ITEM_UNDROPPABLE_APPEARANCE;
			addItemToMonsterInventory(spellbook);

			//TODO: Delete debug.
			spell_t * spell = getSpellFromID(getSpellIDFromSpellbook(spellbook->type));
			//messagePlayer(clientnum, "DEBUG: Shadow mimicked spell %s.", spell->getSpellName());
		}

		spellsCanMimic.erase(spellsCanMimic.begin() + choosen); //No longer an eligible spell.
	}

	//shadowTeleportToTarget(target);
}

void Creature::lichIceCreateCannon()
{
	//spellTimer->particleTimerCountdownAction = PARTICLE_TIMER_ACTION_SHOOT_PARTICLES;
	for ( int i = 0; i < 6; ++i )
	{
		Entity* spellOrbit = castOrbitingMagicMissile(SPELL_MAGICMISSILE, 8.0, i * PI / 3, 500);
		spellOrbit->z = -25;
		spellOrbit->actmagicOrbitStartZ = spellOrbit->z;
	}
	createParticleDropRising(this, 678, 1.0);
	if ( multiplayer == SERVER )
	{
		serverSpawnMiscParticles(this, PARTICLE_EFFECT_RISING_DROP, 678);
	}
}

Entity* Creature::lichThrowProjectile(double angle)
{
	Entity* projectile = newEntity(items[STEEL_CHAKRAM].index, 1, map.entities, nullptr); // thrown item
	projectile->parent = uid;
	projectile->x = x;
	projectile->y = y;
	projectile->z = z;
	projectile->yaw = yaw + angle;
	projectile->sizex = 1;
	projectile->sizey = 1;
	projectile->behavior = &actThrown;
	projectile->flags[UPDATENEEDED] = true;
	projectile->flags[PASSABLE] = true;
	projectile->skill[10] = STEEL_CHAKRAM;
	projectile->skill[11] = EXCELLENT;
	projectile->skill[12] = 0;
	projectile->skill[13] = 1;
	projectile->skill[14] = 0;
	projectile->skill[15] = 1;

	// todo: change velocity of chakram/shuriken?
	projectile->vel_x = 6 * cos(yaw + angle);
	projectile->vel_y = 6 * sin(yaw + angle);
	projectile->vel_z = -.3;

	return projectile;
}

void Creature::incubusChooseWeapon(const Entity* target, double dist)
{
    Creature* targetCrtr = (Creature*)(*&target);
	if ( monsterSpecialState != 0 )
	{
		//Holding a weapon assigned from the special attack. Don't switch weapons.
		if ( monsterSpecialState == INCUBUS_TELEPORT_STEAL && monsterSpecialTimer == 0 )
		{
			// handle steal weapon random teleportation
			incubusTeleportRandom();
			monsterSpecialState = 0;
			serverUpdateEntitySkill(this, 33); // for clients to keep track of animation
			attack(MONSTER_POSE_INCUBUS_TELEPORT, 0, nullptr);
		}
		else if ( monsterSpecialState == INCUBUS_TELEPORT && monsterSpecialTimer == 0 )
		{
			// handle idle teleporting to target
			monsterSpecialState = 0;
			serverUpdateEntitySkill(this, 33); // for clients to keep track of animation
		}
		return;
	}

	Stat *myStats = getStats();
	if ( !myStats )
	{
		return;
	}

	if ( !strncmp(myStats->name, "inner demon", strlen("inner demon")) )
	{
		return;
	}

	int specialRoll = -1;
	int bonusFromHP = 0;

	if ( ticks % 10 == 0 && monsterSpecialState != INCUBUS_TELEPORT_STEAL )
	{
		// teleport to target, higher chance at greater distance or lower HP
		specialRoll = local_rng.rand() % 50;
		if ( specialRoll < (1 + (dist > 80 ? 4 : 0) + (myStats->HP <= myStats->MAXHP * 0.8 ? 4 : 0)) )
		{
			monsterSpecialState = INCUBUS_TELEPORT;
			incubusTeleportToTarget(target);
			return;
		}
	}

	if ( monsterSpecialTimer == 0 && (ticks % 10 == 0) && monsterAttack == 0 )
	{
		Stat* targetStats = target->getStats();
		if ( !targetStats )
		{
			return;
		}

		bool tryCharm = true;
		bool trySteal = true;
		if ( targetStats->EFFECTS[EFF_PACIFY] )
		{
			tryCharm = false;
		}
		if ( !targetStats->weapon )
		{
			trySteal = false;
		}

		if ( trySteal || tryCharm )
		{
			if ( myStats->HP <= myStats->MAXHP * 0.8 )
			{
				bonusFromHP += 1; // +2.5% chance if on low health
			}
			if ( myStats->HP <= myStats->MAXHP * 0.4 )
			{
				bonusFromHP += 1; // +extra 2.5% chance if on lower health
			}

			int requiredRoll = (1 + bonusFromHP + (targetStats->EFFECTS[EFF_CONFUSED] ? 4 : 0)
				+ (targetStats->EFFECTS[EFF_DRUNK] ? 2 : 0)
				+ (targetStats->EFFECTS[EFF_PACIFY] ? 2 : 0)); // +2.5% base, + extra if target is inebriated

			if ( trySteal && tryCharm )
			{
				if ( local_rng.rand() % 8 == 0 )
				{
					trySteal = false; // try charm 12.5% of the time.
				}
				else
				{
					tryCharm = false;
				}
			}

			if ( trySteal )
			{
				// try to steal weapon if target is holding.
				// occurs less often against fellow monsters.
				specialRoll = local_rng.rand() % (40 + 40 * (targetCrtr && targetCrtr->behavior == &actMonster));
			}
			else if ( tryCharm )
			{
				specialRoll = local_rng.rand() % 40;
			}

			if ( trySteal )
			{
				requiredRoll += (myStats->weapon == nullptr ? 3 : 0); // bonus if no weapon held
			}

			if ( specialRoll < requiredRoll )
			{
				node_t * node = nullptr;
				if ( trySteal )
				{
					node = itemNodeInInventory(myStats, SPELLBOOK_STEAL_WEAPON, static_cast<Category>(-1));
					if ( node != nullptr )
					{
						swapMonsterWeaponWithInventoryItem(this, myStats, node, true, true);
						monsterSpecialState = INCUBUS_STEAL;
						serverUpdateEntitySkill(this, 33); // for clients to keep track of animation
						return;
					}
				}
				else if ( tryCharm )
				{
					node = itemNodeInInventory(myStats, SPELLBOOK_CHARM_MONSTER, static_cast<Category>(-1));
					if ( node != nullptr )
					{
						swapMonsterWeaponWithInventoryItem(this, myStats, node, true, true);
						monsterSpecialState = INCUBUS_CHARM;
						serverUpdateEntitySkill(this, 33); // for clients to keep track of animation
						return;
					}
				}
			}
		}

		// try new roll for alternate potion throw special.
		// occurs less often against fellow monsters.
		specialRoll = local_rng.rand() % (30 + 30 * (targetCrtr && targetCrtr->behavior == &actMonster));
		if ( specialRoll < (2 + bonusFromHP) ) // +5% base
		{
			node_t * node = nullptr;
			node = itemNodeInInventory(myStats, -1, POTION);
			if ( node != nullptr )
			{
				swapMonsterWeaponWithInventoryItem(this, myStats, node, true, true);
				monsterSpecialState = INCUBUS_CONFUSION;
				return;
			}
		}
	}

	bool inMeleeRange = monsterInMeleeRange(target, dist);

	if ( inMeleeRange )
	{
		//Switch to a melee weapon if not already wielding one. Unless monster special state is overriding the AI.
		if ( !myStats->weapon || !isMeleeWeapon(*myStats->weapon) )
		{
			node_t * weaponNode = getMeleeWeaponItemNodeInInventory(myStats);
			if ( !weaponNode )
			{
				return; //Resort to fists.
			}

			bool swapped = swapMonsterWeaponWithInventoryItem(this, myStats, weaponNode, false, false);
			if ( !swapped )
			{
				//Don't return so that monsters will at least equip ranged weapons in melee range if they don't have anything else.
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}
	}

	//Switch to a thrown weapon or a ranged weapon.
	if ( !myStats->weapon || isMeleeWeapon(*myStats->weapon) )
	{
		node_t *weaponNode = getRangedWeaponItemNodeInInventory(myStats, true);
		if ( !weaponNode )
		{
			return; //Nothing available
		}
		bool swapped = swapMonsterWeaponWithInventoryItem(this, myStats, weaponNode, false, false);
		return;
	}
	return;
}

void Creature::lichIceSummonMonster(Monster creature)
{
	Entity* target = nullptr;
	for (node_t * searchNode = map.entities->first; searchNode != nullptr; searchNode = searchNode->next )
	{
		target = (Entity*)searchNode->element;
		if ( target->behavior == &actDevilTeleport
			&& target->sprite == 128 )
		{
			break; // found specified center of map
		}
	}
	if ( target )
	{
		int tries = 25; // max iteration in while loop, fail safe.
		long spawn_x = (target->x / 16) - 11 + local_rng.rand() % 23;
		long spawn_y = (target->y / 16) - 11 + local_rng.rand() % 23;
		int index = (spawn_x)* MAPLAYERS + (spawn_y)* MAPLAYERS * map.height;
		while ( tries > 0 &&
				(map.tiles[OBSTACLELAYER + index] == 1
				|| map.tiles[index] == 0
				|| swimmingtiles[map.tiles[index]]
				|| lavatiles[map.tiles[index]])
			)
		{
			// find a spot that isn't wall, no floor or lava/water tiles.
			spawn_x = (target->x / 16) - 11 + local_rng.rand() % 23;
			spawn_y = (target->y / 16) - 11 + local_rng.rand() % 23;
			index = (spawn_x)* MAPLAYERS + (spawn_y)* MAPLAYERS * map.height;
			--tries;
		}
		if ( tries > 0 )
		{
			Entity* timer = createParticleTimer(this, 70, 174);
			timer->x = spawn_x * 16.0 + 8;
			timer->y = spawn_y * 16.0 + 8;
			timer->z = 0;
			timer->particleTimerCountdownAction = PARTICLE_TIMER_ACTION_SUMMON_MONSTER;
			timer->particleTimerCountdownSprite = 174;
			timer->particleTimerEndAction = PARTICLE_EFFECT_SUMMON_MONSTER;
			timer->particleTimerVariable1 = creature;
			serverSpawnMiscParticlesAtLocation(spawn_x, spawn_y, 0, PARTICLE_EFFECT_SUMMON_MONSTER, 174);
		}
	}
}

void Creature::goatmanChooseWeapon(const Entity* target, double dist)
{
	if ( monsterSpecialState != 0 )
	{
		//Holding a weapon assigned from the special attack. Don't switch weapons.
		//messagePlayer()
		return;
	}

	//TODO: I don't like this function getting called every frame. Find a better place to put it.
	//Although if I do that, can't do this dirty little hack for the goatman's special...

	//TODO: If applying attack animations that will involve holding a potion for several frames while this code has a chance to run, do a check here to cancel the function if holding a potion.

	Stat *myStats = getStats();
	if ( !myStats )
	{
		return;
	}

	if ( myStats->weapon && (itemCategory(myStats->weapon) == SPELLBOOK) )
	{
		return;
	}

	int specialRoll = -1;
	bool usePotionSpecial = false;

	/*
	 * For the goatman's special:
	 * * If specialRoll == 0, want to use a booze or healing potion (prioritize healing potion if damaged enough).
	 * * If no have potion, try to use THROWN in melee.
	 * * If in melee, if potion is not a healing potion, check if have any THROWN and then 50% chance to use those instead.
	 */

	node_t * hasPotion = nullptr;
	bool isHealingPotion = false;

	if ( monsterSpecialTimer == 0 && (ticks % 10 == 0) && monsterAttack == 0 )
	{
		//messagePlayer(clientnum, "Cooldown done!");
		specialRoll = local_rng.rand()%10;

		if ( specialRoll == 0 )
		{
			if ( myStats->HP <= myStats->MAXHP / 3 * 2 )
			{
				//Try to get a health potion.
				hasPotion = itemNodeInInventory(myStats, POTION_EXTRAHEALING, static_cast<Category>(-1));
				if ( !hasPotion )
				{
					hasPotion = itemNodeInInventory(myStats, POTION_HEALING, static_cast<Category>(-1));
					if ( hasPotion )
					{
						//Equip and chuck it now.
						bool swapped = swapMonsterWeaponWithInventoryItem(this, myStats, hasPotion, false, false);
						if ( !swapped )
						{
							//printlog("Error in Creature::goatmanChooseWeapon(): failed to swap healing potion into hand!");
							//Don't return, want to try equipping either a potion of booze, or one of the other weapon routes (e.h. a THROWN special if in melee or just an axe if worst comes to worst).
						}
						else
						{
							monsterSpecialState = GOATMAN_POTION;
							//monsterHitTime = 2 * HITRATE;
							return;
						}
					}
				}
				else
				{
					//Equip and chuck it now.
					bool swapped = swapMonsterWeaponWithInventoryItem(this, myStats, hasPotion, false, false);
					if ( !swapped )
					{
						//printlog("Error in Creature::goatmanChooseWeapon(): failed to swap healing potion into hand!");
						//Don't return, want to try equipping either a potion of booze, or one of the other weapon routes (e.h. a THROWN special if in melee or just an axe if worst comes to worst).
					}
					else
					{
						monsterSpecialState = GOATMAN_POTION;
						//monsterHitTime = 2 * HITRATE;
						return;
					}
				}
			}

			if ( !hasPotion )
			{
				//Couldn't find a healing potion? Try for a potion of booze.
				hasPotion = itemNodeInInventory(myStats, POTION_BOOZE, static_cast<Category>(-1));
				if ( hasPotion )
				{
					//Equip and chuck it now.
					bool swapped = swapMonsterWeaponWithInventoryItem(this, myStats, hasPotion, false, false);
					if ( !swapped )
					{
						//printlog("Error in Creature::goatmanChooseWeapon(): failed to swap healing potion into hand!");
						//Don't return, want to try equipping either a potion of booze, or one of the other weapon routes (e.h. a THROWN special if in melee or just an axe if worst comes to worst).
					}
					else
					{
						monsterSpecialState = GOATMAN_POTION;
						//monsterHitTime = 2 * HITRATE;
						return;
					}
				}
			}
		}
	}

	bool inMeleeRange = monsterInMeleeRange(target, dist);

	if ( inMeleeRange )
	{
		if ( monsterSpecialTimer == 0 && (ticks % 10 == 0) && monsterAttack == 0 && specialRoll == 0 )
		{
			bool tryChakram = true;
			if ( hasPotion && local_rng.rand()%10 )
			{
				tryChakram = false;
			}

			if ( tryChakram )
			{
				//Grab a chakram instead.
				node_t * thrownNode = itemNodeInInventory(myStats, -1, THROWN);
				if ( thrownNode )
				{
					bool swapped = swapMonsterWeaponWithInventoryItem(this, myStats, thrownNode, false, false);
					if ( !swapped )
					{
						//printlog("Error in Creature::goatmanChooseWeapon(): failed to swap THROWN into hand! Cursed? (%d)", myStats->weapon->beatitude);
						//Don't return, make sure holding a melee weapon at least.
					}
					else
					{
						monsterSpecialState = GOATMAN_THROW;
						return;
					}
				}
			}
		}

		//Switch to a melee weapon if not already wielding one. Unless monster special state is overriding the AI.
		if ( !myStats->weapon || !isMeleeWeapon(*myStats->weapon) )
		{
			node_t * weaponNode = getMeleeWeaponItemNodeInInventory(myStats);
			if ( !weaponNode )
			{
				if ( myStats->weapon && myStats->weapon->type == MAGICSTAFF_SLOW )
				{
					monsterUnequipSlotFromCategory(myStats, &myStats->weapon, MAGICSTAFF);
				}
				return; //Resort to fists.
			}

			bool swapped = swapMonsterWeaponWithInventoryItem(this, myStats, weaponNode, false, false);
			if ( !swapped )
			{
				//printlog("Error in Creature::goatmanChooseWeapon(): failed to swap melee weapon into hand! Cursed? (%d)", myStats->weapon->beatitude);
				//Don't return so that monsters will at least equip ranged weapons in melee range if they don't have anything else.
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}
	}

	//if ( hasPotion )
	//{
	//	//Try to equip the potion first. If fails, then equip normal ranged.
	//	bool swapped = swapMonsterWeaponWithInventoryItem(this, myStats, hasPotion, false, false);
	//	if ( !swapped )
	//	{
	//		printlog("Error in Creature::goatmanChooseWeapon(): failed to swap non-healing potion into hand! (non-melee block) Cursed? (%d)", myStats->weapon->beatitude);
	//	}
	//	else
	//	{
	//		monsterSpecialState = GOATMAN_POTION;
	//		return;
	//	}
	//}

	//Switch to a thrown weapon or a ranged weapon. Potions are reserved as a special attack.
	if ( !myStats->weapon || isMeleeWeapon(*myStats->weapon) )
	{
		//First search the inventory for a THROWN weapon.
		node_t *weaponNode = nullptr;
		if ( monsterSpecialTimer == 0 && (ticks % 10 == 0) && monsterAttack == 0 && local_rng.rand() % 10 == 0 )
		{
			weaponNode = itemNodeInInventory(myStats, -1, THROWN);
			if ( weaponNode )
			{
				if ( swapMonsterWeaponWithInventoryItem(this, myStats, weaponNode, false, false) )
				{
					monsterSpecialState = GOATMAN_THROW;
					return;
				}
			}
		}
		if ( !weaponNode )
		{
			//If couldn't find any, search the inventory for a ranged weapon.
			weaponNode = getRangedWeaponItemNodeInInventory(myStats, true);
		}

		bool swapped = swapMonsterWeaponWithInventoryItem(this, myStats, weaponNode, false, false);
		return;
	}

	return;
}

bool Creature::insectoidCanWieldItem(const Item& item) const
{
	Stat* myStats = getStats();
	if ( !myStats )
	{
		return false;
	}

	switch ( itemCategory(&item) )
	{
		case WEAPON:
			return true;
		case POTION:
			return false;
		case THROWN:
			return true;
		case ARMOR:
		{ //Little baby compiler stop whining, wah wah.
			int equipType = checkEquipType(&item);
			if ( equipType == TYPE_HAT || equipType == TYPE_HELM )
			{
				return false; //No can wear hats, because antennae.
			}
			return true; //Can wear all other armor.
		}
		default:
			return false;
	}

	return false;
}

void Creature::incubusTeleportToTarget(const Entity* target)
{
	Entity* spellTimer = createParticleTimer(this, 40, 593);
	spellTimer->particleTimerEndAction = PARTICLE_EFFECT_INCUBUS_TELEPORT_TARGET; // teleport behavior of timer.
	spellTimer->particleTimerEndSprite = 593; // sprite to use for end of timer function.
	spellTimer->particleTimerCountdownAction = PARTICLE_TIMER_ACTION_SHOOT_PARTICLES;
	spellTimer->particleTimerCountdownSprite = 593;
	if ( target != nullptr )
	{
		spellTimer->particleTimerTarget = static_cast<int32_t>(target->getUID()); // get the target to teleport around.
	}
	spellTimer->particleTimerVariable1 = 3; // distance of teleport in tiles
	if ( multiplayer == SERVER )
	{
		serverSpawnMiscParticles(this, PARTICLE_EFFECT_INCUBUS_TELEPORT_TARGET, 593);
	}
}

void Creature::insectoidChooseWeapon(const Entity* target, double dist)
{
    Creature* targetCrtr = (Creature*)(*&target);
	if ( monsterSpecialState != 0 )
	{
		//Holding a weapon assigned from the special attack. Don't switch weapons.
		//messagePlayer()
		return;
	}

	Stat *myStats = getStats();
	if ( !myStats )
	{
		return;
	}

	/*if ( myStats->weapon && (itemCategory(myStats->weapon) == MAGICSTAFF || itemCategory(myStats->weapon) == SPELLBOOK) )
	{
		return;
	}*/

	int specialRoll = -1;
	int bonusFromHP = 0;

	// throwing weapons
	// occurs less often against fellow monsters.
	if ( monsterSpecialTimer == 0 && (ticks % 10 == 0) && monsterAttack == 0 )
	{
		specialRoll = local_rng.rand() % (40 + 40 * (target != nullptr && targetCrtr && targetCrtr->behavior == &actMonster));
		//messagePlayer(0, "rolled: %d", specialRoll);
		if ( myStats->HP <= myStats->MAXHP * 0.6 )
		{
			bonusFromHP += 1; // +5% chance if on low health
		}
		if ( myStats->HP <= myStats->MAXHP * 0.3 )
		{
			bonusFromHP += 1; // +extra 5% chance if on lower health
		}
		if ( specialRoll < (1 + bonusFromHP) ) // +5% base
		{
			node_t * node = itemNodeInInventory(myStats, -1, THROWN);
			if ( node != nullptr )
			{
				bool swapped = swapMonsterWeaponWithInventoryItem(this, myStats, node, true, true);
				if ( swapped )
				{
					if ( myStats->weapon->count > 1 )
					{
						monsterSpecialState = INSECTOID_DOUBLETHROW_FIRST + local_rng.rand() % 2; // 50% for double throw.
					}
					else
					{
						monsterSpecialState = INSECTOID_DOUBLETHROW_SECOND;
					}
				}
				return;
			}
		}
	}

	bool inMeleeRange = monsterInMeleeRange(target, dist);

	if ( inMeleeRange )
	{
		//Switch to a melee weapon if not already wielding one. Unless monster special state is overriding the AI.
		if ( !myStats->weapon || !isMeleeWeapon(*myStats->weapon) )
		{
			node_t * weaponNode = getMeleeWeaponItemNodeInInventory(myStats);
			if ( !weaponNode )
			{
				return; //Resort to fists.
			}

			bool swapped = swapMonsterWeaponWithInventoryItem(this, myStats, weaponNode, false, false);
			if ( !swapped )
			{
				//Don't return so that monsters will at least equip ranged weapons in melee range if they don't have anything else.
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}
	}

	//Switch to a thrown weapon or a ranged weapon.
	if ( !myStats->weapon || isMeleeWeapon(*myStats->weapon) )
	{
		node_t *weaponNode = getRangedWeaponItemNodeInInventory(myStats, true);
		if ( !weaponNode )
		{
			return; //Nothing available
		}
		bool swapped = swapMonsterWeaponWithInventoryItem(this, myStats, weaponNode, false, false);
		return;
	}
	return;
}

bool Creature::shadowCanMimickSpell(int spellID)
{
	switch ( spellID )
	{
		case SPELL_FORCEBOLT:
		case SPELL_MAGICMISSILE:
		case SPELL_COLD:
		case SPELL_FIREBALL:
		case SPELL_LIGHTNING:
		case SPELL_SLEEP:
		case SPELL_CONFUSE:
		case SPELL_SLOW:
		case SPELL_STONEBLOOD:
		case SPELL_BLEED:
		case SPELL_ACID_SPRAY:
			return true;
		default:
			return false;
	}
}

void Creature::incubusTeleportRandom()
{
	Entity* spellTimer = createParticleTimer(this, 80, 593);
	spellTimer->particleTimerEndAction = PARTICLE_EFFECT_INCUBUS_TELEPORT_STEAL; // teleport behavior of timer.
	spellTimer->particleTimerEndSprite = 593; // sprite to use for end of timer function.
	spellTimer->particleTimerCountdownAction = PARTICLE_TIMER_ACTION_SHOOT_PARTICLES;
	spellTimer->particleTimerCountdownSprite = 593;
	spellTimer->particleTimerPreDelay = 40;
	if ( multiplayer == SERVER )
	{
		serverSpawnMiscParticles(this, PARTICLE_EFFECT_INCUBUS_TELEPORT_STEAL, 593);
	}
}

void Creature::shadowTeleportToTarget(const Entity* target, int range)
{
	Entity* spellTimer = createParticleTimer(this, 60, 625);
	spellTimer->particleTimerPreDelay = 20; // wait 20 ticks before animation.
	spellTimer->particleTimerEndAction = PARTICLE_EFFECT_SHADOW_TELEPORT; // teleport behavior of timer.
	spellTimer->particleTimerEndSprite = 625; // sprite to use for end of timer function.
	spellTimer->particleTimerCountdownAction = 1;
	spellTimer->particleTimerCountdownSprite = 625;
	if ( target != nullptr )
	{
		spellTimer->particleTimerTarget = static_cast<int32_t>(target->getUID()); // get the target to teleport around.
	}
	spellTimer->particleTimerVariable1 = range; // distance of teleport in tiles
	if ( multiplayer == SERVER )
	{
		serverSpawnMiscParticles(this, PARTICLE_EFFECT_SHADOW_TELEPORT, 625);
	}
}

void Creature::shadowChooseWeapon(const Entity* target, double dist)
{
    Creature* targetCrtr = (Creature*)(*&target);
	if ( monsterSpecialState != 0 )
	{
		//Holding a weapon assigned from the special attack. Don't switch weapons.
		//messagePlayer(clientnum, "Shadow not choosing.");
		// handle idle teleporting to target
		if ( monsterSpecialState == SHADOW_TELEPORT_ONLY && monsterSpecialTimer == 0 )
		{
			monsterSpecialState = 0;
			serverUpdateEntitySkill(this, 33); // for clients to keep track of animation
		}
		return;
	}
	//messagePlayer(clientnum, "Shadow choosing.");

	Stat *myStats = getStats();
	if ( !myStats )
	{
		return;
	}

	int specialRoll = -1;

	bool inMeleeRange = monsterInMeleeRange(target, dist);

	if ( monsterSpecialTimer == 0 && (ticks % 10 == 0) && monsterAttack == 0 )
	{
		//messagePlayer(clientnum, "Preliminary special check.");
		Stat* targetStats = target->getStats();
		if ( !targetStats )
		{
			return;
		}

		/* THIS NEEDS TO BE ELSEWHERE, TO BE CALLED CONSTANTLY TO ALLOW SHADOW TO TELEPORT IF NO PATH/ DISTANCE IS TOO GREAT */

		// occurs less often against fellow monsters.
		specialRoll = local_rng.rand() % (20 + 50 * (targetCrtr && targetCrtr->behavior == &actMonster));

		int requiredRoll = 10;

		// check the roll
		if ( specialRoll < requiredRoll )
		//if ( local_rng.rand() % 150 )
		{
			//messagePlayer(clientnum, "Rolled the special!");
			node_t * node = nullptr;
			bool telemimic  = (local_rng.rand() % 4 == 0); //By default, 25% chance it'll telepotty instead of casting a spell.
			if ( monsterState != MONSTER_STATE_ATTACK )
			{
				//If it's hunting down the player, always want it to teleport and find them.
				telemimic = true;
				//messagePlayer(clientnum, "Forcing tele-mimic!");
			}

			if ( telemimic )
			{
				//Do the tele-mimic-invisibility special ability.
				//messagePlayer(clientnum, "Executing telemimic.");
				//monsterShadowInitialMimic = 0; //False!
				monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_SHADOW_TELEMIMICINVISI_ATTACK;
				attack(MONSTER_POSE_MAGIC_WINDUP3, 0, nullptr);
				return;
			}

			//messagePlayer(clientnum, "Defaulting to spell.");
			node = chooseAttackSpellbookFromInventory();
			if ( node != nullptr )
			{
				//messagePlayer(clientnum, "Shadow equipped a spell!");
				swapMonsterWeaponWithInventoryItem(this, myStats, node, true, true);
				monsterSpecialState = SHADOW_SPELLCAST;
				serverUpdateEntitySkill(this, 33); // for clients to keep track of animation
				monsterHitTime = HITRATE * 2; // force immediate attack
				return;
			}
			else
			{
				//Always set the cooldown, even if didn't cast anything.
				monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_SHADOW_SPELLCAST;
			}
		}
	}

	if ( inMeleeRange ) //Shadows are paunchy people, they don't like to shoost much.
	{
		//Switch to a melee weapon if not already wielding one. Unless monster special state is overriding the AI.
		if ( !myStats->weapon || !isMeleeWeapon(*myStats->weapon) )
		{
			node_t * weaponNode = getMeleeWeaponItemNodeInInventory(myStats);
			if ( !weaponNode )
			{
				return; //Resort to fists.
			}

			bool swapped = swapMonsterWeaponWithInventoryItem(this, myStats, weaponNode, false, false);
			if ( !swapped )
			{
				//Don't return so that monsters will at least equip ranged weapons in melee range if they don't have anything else.
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}
	}
	return;
}

bool Creature::goatmanCanWieldItem(const Item& item) const
{
	Stat* myStats = getStats();
	if ( !myStats )
	{
		return false;
	}

	if ( monsterAllyIndex >= 0 && (monsterAllyClass != ALLY_CLASS_MIXED || item.interactNPCUid == getUID()) )
	{
		return monsterAllyEquipmentInClass(item);
	}

	switch ( itemCategory(&item) )
	{
		case WEAPON:
			return true;
		case POTION:
			switch ( item.type )
			{
				case POTION_BOOZE:
					return true;
				case POTION_HEALING:
					return true;
				default:
					return false;
			}
			break;
		case TOOL:
			if ( itemTypeIsQuiver(item.type) )
			{
				return true;
			}
			break;
		case THROWN:
			return true;
		case ARMOR:
			{ //Little baby compiler stop whining, wah wah.
				int equipType = checkEquipType(&item);
				if ( equipType == TYPE_HAT || equipType == TYPE_HELM )
				{
					return false; //No can wear hats, because horns.
				}
				return true; //Can wear all other armor.
			}
		default:
			return false;
	}

	return false;
}

bool Creature::humanCanWieldItem(const Item& item) const
{
	Stat* myStats = getStats();
	if ( !myStats )
	{
		return false;
	}

	if ( monsterAllyIndex >= 0 && (monsterAllyClass != ALLY_CLASS_MIXED || item.interactNPCUid == getUID()) )
	{
		return monsterAllyEquipmentInClass(item);
	}

	switch ( itemCategory(&item) )
	{
		case WEAPON:
			return true;
		case ARMOR:
			return true;
		case MAGICSTAFF:
			return false;
		case THROWN:
			return false;
		case TOOL:
			if ( itemTypeIsQuiver(item.type) )
			{
				return true;
			}
			break;
		default:
			return false;
	}

	return false;
}

void Creature::humanSetLimbsClient(int bodypart)
{
	int skinColor = 0;
	int sex = MALE;

	// get the skinColor/sex from the head sprite.

	if ( (sprite >= 113 && sprite < 118)
		|| (sprite >= 125 && sprite < 130)
		|| (sprite >= 332 && sprite < 334) )
	{
		skinColor = 0; // light.
		if ( (sprite >= 125 && sprite < 130)
			|| sprite == 333 )
		{
			sex = FEMALE;
		}
	}
	else if ( (sprite >= 341 && sprite < 347)
		|| (sprite >= 354 && sprite < 360) )
	{
		skinColor = 1; // medium.
		if ( sprite >= 354 && sprite < 360 )
		{
			sex = FEMALE;
		}
	}
	else if ( (sprite >= 367 && sprite < 373)
		|| (sprite >= 380 && sprite < 386) )
	{
		skinColor = 2; // dark.
		if ( sprite >= 380 && sprite < 386 )
		{
			sex = FEMALE;
		}
	}

	node_t * limbNode = list_Node(&this->children, bodypart);
	Entity* limb = nullptr;
	if ( limbNode )
	{
		limb = (Entity*)limbNode->element;
	}

	if ( !limb )
	{
		return;
	}

	switch ( bodypart )
	{
		case LIMB_HUMANOID_TORSO:
			switch ( skinColor )
			{
				case 1:
					limb->sprite = 334 + 13 * sex;
					break;
				case 2:
					limb->sprite = 360 + 13 * sex;
					break;
				default:
					limb->sprite = 106 + 12 * sex;
					break;
			}
			break;
		case LIMB_HUMANOID_RIGHTLEG:
			switch ( skinColor )
			{
				case 1:
					limb->sprite = 335 + 13 * sex;
					break;
				case 2:
					limb->sprite = 361 + 13 * sex;
					break;
				default:
					limb->sprite = 107 + 12 * sex;
					break;
			}
			break;
		case LIMB_HUMANOID_LEFTLEG:
			switch ( skinColor )
			{
				case 1:
					limb->sprite = 336 + 13 * sex;
					break;
				case 2:
					limb->sprite = 362 + 13 * sex;
					break;
				default:
					limb->sprite = 108 + 12 * sex;
					break;
			}
			break;
		case LIMB_HUMANOID_RIGHTARM:
			switch ( skinColor )
			{
				case 1:
					limb->sprite = 337 + 13 * sex;
					break;
				case 2:
					limb->sprite = 363 + 13 * sex;
					break;
				default:
					limb->sprite = 109 + 12 * sex;
					break;
			}
			break;
		case LIMB_HUMANOID_LEFTARM:
			switch ( skinColor )
			{
				case 1:
					limb->sprite = 338 + 13 * sex;
					break;
				case 2:
					limb->sprite = 364 + 13 * sex;
					break;
				default:
					limb->sprite = 110 + 12 * sex;
					break;
			}
			break;
	}
}

/*-------------------------------------------------------------------------------

Creature::checkBetterEquipment

Checks the tiles immediately surrounding the given entity for items and
replaces the entity's equipment with those items if they are better

-------------------------------------------------------------------------------*/

void Creature::checkBetterEquipment(Stat* myStats)
{
	if ( !myStats )
	{
		return; //Can't continue without these.
	}

	list_t * items = nullptr;
	//X and Y in terms of tiles.
	int tx = x / 16;
	int ty = y / 16;
	getItemsOnTile(tx, ty, &items); //Check the tile the goblin is on for items.
	getItemsOnTile(tx - 1, ty, &items); //Check tile to the left.
	getItemsOnTile(tx + 1, ty, &items); //Check tile to the right.
	getItemsOnTile(tx, ty - 1, &items); //Check tile up.
	getItemsOnTile(tx, ty + 1, &items); //Check tile down.
	getItemsOnTile(tx - 1, ty - 1, &items); //Check tile diagonal up left.
	getItemsOnTile(tx + 1, ty - 1, &items); //Check tile diagonal up right.
	getItemsOnTile(tx - 1, ty + 1, &items); //Check tile diagonal down left.
	getItemsOnTile(tx + 1, ty + 1, &items); //Check tile diagonal down right.
	int currentAC, newAC;
	Item* oldarmor = nullptr;

	node_t * node = nullptr;

	bool glovesandshoes = false;
	if ( myStats->type == HUMAN )
	{
		glovesandshoes = true;
	}

	if ( items )
	{
		/*
		* Rundown of the function:
		* Loop through all items.
		* Check the monster's item. Compare and grab the best item.
		*/

		for ( node = items->first; node != nullptr; node = node->next )
		{
			//Turn the entity into an item.
			if ( node->element )
			{
				Entity* entity = (Entity*)node->element;
				Item* item = nullptr;
				if ( entity != nullptr )
				{
					item = newItemFromEntity(entity);
				}
				if ( !item )
				{
					continue;
				}
				if ( !canWieldItem(*item) )
				{
					free(item);
					continue;
				}

				//If weapon.
				if ( itemCategory(item) == WEAPON )
				{
					if ( myStats->weapon == nullptr ) //Not currently holding a weapon.
					{
						myStats->weapon = item; //Assign the monster's weapon.
						item = nullptr;
						list_RemoveNode(entity->mynode);
					}
					else
					{
						//Ok, the monster has a weapon already. First check if the monster's weapon is cursed. Can't drop it if it is.
						if ( myStats->weapon->beatitude >= 0 && itemCategory(myStats->weapon) != MAGICSTAFF && itemCategory(myStats->weapon) != POTION && itemCategory(myStats->weapon) != THROWN && itemCategory(myStats->weapon) != GEM )
						{
							//Next compare the two weapons. If the item on the ground is better, drop the weapon it's carrying and equip that one.
							int weapon_tohit = myStats->weapon->weaponGetAttack();
							int new_weapon_tohit = item->weaponGetAttack();

							//If the new weapon does more damage than the current weapon.
							if ( new_weapon_tohit > weapon_tohit )
							{
								dropItemMonster(myStats->weapon, this, myStats);
								myStats->weapon = item;
								item = nullptr;
								list_RemoveNode(entity->mynode);
							}
						}
					}
				}
				else if ( itemCategory(item) == ARMOR )
				{
					if ( checkEquipType(item) == TYPE_HAT ) // hats
					{
						if ( myStats->helmet == nullptr ) // nothing on head currently
						{
							// goblins love hats.
							myStats->helmet = item; // pick up the hat.
							item = nullptr;
							list_RemoveNode(entity->mynode);
						}
					}
					else if ( checkEquipType(item) == TYPE_HELM ) // helmets
					{
						if ( myStats->helmet == nullptr ) // nothing on head currently
						{
							myStats->helmet = item; // pick up the helmet.
							item = nullptr;
							list_RemoveNode(entity->mynode);
						}
						else
						{
							if ( myStats->helmet->beatitude >= 0 ) // if the armor is not cursed, proceed. Won't do anything if the armor is cursed.
							{
								// to compare the armors, we use the AC function to check the Armor Class of the equipment the goblin
								// is currently wearing versus the Armor Class that the goblin would have if it had the new armor.
								currentAC = AC(myStats);
								oldarmor = myStats->helmet;
								myStats->helmet = item;
								newAC = AC(myStats);
								myStats->helmet = oldarmor;

								//If the new armor is better than the current armor.
								if ( newAC > currentAC )
								{
									dropItemMonster(myStats->helmet, this, myStats);
									myStats->helmet = item;
									item = nullptr;
									list_RemoveNode(entity->mynode);
								}
							}
						}
					}
					else if ( checkEquipType(item) == TYPE_SHIELD )     // shields
					{
						if ( myStats->shield == nullptr ) // nothing in left hand currently
						{
							myStats->shield = item; // pick up the shield.
							item = nullptr;
							list_RemoveNode(entity->mynode);
						}
						else
						{
							if ( myStats->shield->beatitude >= 0 )   // if the armor is not cursed, proceed. Won't do anything if the armor is cursed.
							{
								// to compare the armors, we use the AC function to check the Armor Class of the equipment the goblin
								// is currently wearing versus the Armor Class that the goblin would have if it had the new armor.
								currentAC = AC(myStats);
								oldarmor = myStats->shield;
								myStats->shield = item;
								newAC = AC(myStats);
								myStats->shield = oldarmor;

								//If the new armor is better than the current armor (OR we're not carrying anything)
								if ( newAC > currentAC || !myStats->shield )
								{
									dropItemMonster(myStats->shield, this, myStats);
									myStats->shield = item;
									item = nullptr;
									list_RemoveNode(entity->mynode);
								}
							}
						}
					}
					else if ( checkEquipType(item) == TYPE_BREASTPIECE ) // breastpieces
					{
						if ( myStats->breastplate == nullptr ) // nothing on torso currently
						{
							myStats->breastplate = item; // pick up the armor.
							item = nullptr;
							list_RemoveNode(entity->mynode);
						}
						else
						{
							if ( myStats->breastplate->beatitude >= 0 ) // if the armor is not cursed, proceed. Won't do anything if the armor is cursed.
							{
								// to compare the armors, we use the AC function to check the Armor Class of the equipment the goblin
								// is currently wearing versus the Armor Class that the goblin would have if it had the new armor.
								currentAC = AC(myStats);
								oldarmor = myStats->breastplate;
								myStats->breastplate = item;
								newAC = AC(myStats);
								myStats->breastplate = oldarmor;

								//If the new armor is better than the current armor.
								if ( newAC > currentAC )
								{
									dropItemMonster(myStats->breastplate, this, myStats);
									myStats->breastplate = item;
									item = nullptr;
									list_RemoveNode(entity->mynode);
								}
							}
						}
					}
					else if ( checkEquipType(item) == TYPE_CLOAK ) // cloaks
					{
						if ( myStats->cloak == nullptr ) // nothing on back currently
						{
							myStats->cloak = item; // pick up the armor.
							item = nullptr;
							list_RemoveNode(entity->mynode);
						}
						else
						{
							if ( myStats->cloak->beatitude >= 0 )   // if the armor is not cursed, proceed. Won't do anything if the armor is cursed.
							{
								// to compare the armors, we use the AC function to check the Armor Class of the equipment the goblin
								// is currently wearing versus the Armor Class that the goblin would have if it had the new armor.
								currentAC = AC(myStats);
								oldarmor = myStats->cloak;
								myStats->cloak = item;
								newAC = AC(myStats);
								myStats->cloak = oldarmor;

								//If the new armor is better than the current armor.
								if ( newAC > currentAC )
								{
									dropItemMonster(myStats->cloak, this, myStats);
									myStats->cloak = item;
									item = nullptr;
									list_RemoveNode(entity->mynode);
								}
							}
						}
					}
					if ( glovesandshoes && item != nullptr )
					{
						if ( checkEquipType(item) == TYPE_BOOTS ) // boots
						{
							if ( myStats->shoes == nullptr )
							{
								myStats->shoes = item; // pick up the armor
								item = nullptr;
								list_RemoveNode(entity->mynode);
							}
							else
							{
								if ( myStats->shoes->beatitude >= 0 ) // if the armor is not cursed, proceed. Won't do anything if the armor is cursed.
								{
									// to compare the armors, we use the AC function to check the Armor Class of the equipment the goblin
									// is currently wearing versus the Armor Class that the goblin would have if it had the new armor.
									currentAC = AC(myStats);
									oldarmor = myStats->shoes;
									myStats->shoes = item;
									newAC = AC(myStats);
									myStats->shoes = oldarmor;

									//If the new armor is better than the current armor.
									if ( newAC > currentAC )
									{
										dropItemMonster(myStats->shoes, this, myStats);
										myStats->shoes = item;
										item = nullptr;
										list_RemoveNode(entity->mynode);
									}
								}
							}
						}
						else if ( checkEquipType(item) == TYPE_GLOVES )
						{
							if ( myStats->gloves == nullptr )
							{
								myStats->gloves = item; // pick up the armor
								item = nullptr;
								list_RemoveNode(entity->mynode);
							}
							else
							{
								if ( myStats->gloves->beatitude >= 0 ) // if the armor is not cursed, proceed. Won't do anything if the armor is cursed.
								{
									// to compare the armors, we use the AC function to check the Armor Class of the equipment the goblin
									// is currently wearing versus the Armor Class that the goblin would have if it had the new armor.
									currentAC = AC(myStats);
									oldarmor = myStats->gloves;
									myStats->gloves = item;
									newAC = AC(myStats);
									myStats->gloves = oldarmor;

									//If the new armor is better than the current armor.
									if ( newAC > currentAC )
									{
										dropItemMonster(myStats->gloves, this, myStats);
										myStats->gloves = item;
										item = nullptr;
										list_RemoveNode(entity->mynode);
									}
								}
							}
						}
					}
				}
				else if ( itemCategory(item) == POTION )
				{
					if ( myStats->weapon == nullptr ) //Not currently holding a weapon.
					{
						myStats->weapon = item; //Assign the monster's weapon.
						item = nullptr;
						list_RemoveNode(entity->mynode);
					}
					//Don't pick up if already wielding something.
				}
				else if ( itemCategory(item) == THROWN )
				{
					if ( myStats->weapon == nullptr ) //Not currently holding a weapon.
					{
						if ( !entity->itemNotMoving && entity->parent && entity->parent != uid )
						{
							//Don't pick up the item.
						}
						else
						{
							myStats->weapon = item; //Assign the monster's weapon.
							item = nullptr;
							list_RemoveNode(entity->mynode);
						}
					}
					//Don't pick up if already wielding something.
				}

				if ( item != nullptr )
				{
					free(item);
				}
			}
		}

		list_FreeAll(items);
		free(items);
	}
}

void Creature::monsterRollLevelUpStats(int increasestat[3])
{
	// monsters use this.
	increasestat[0] = local_rng.rand() % 6;
	int r = local_rng.rand() % 6;
	while ( r == increasestat[0] ) {
		r = local_rng.rand() % 6;
	}
	increasestat[1] = r;
	r = local_rng.rand() % 6;
	while ( r == increasestat[0] || r == increasestat[1] ) {
		r = local_rng.rand() % 6;
	}
	increasestat[2] = r;
}

static ConsoleVariable<bool> cvar_noxp("/noxp", false);

/*-------------------------------------------------------------------------------

Creature::isMobile

returns true if the given entity can move, or false if it cannot

-------------------------------------------------------------------------------*/

bool Creature::isMobile()
{
	Stat* entitystats;
	if ( (entitystats = getStats()) == nullptr )
	{
		return true;
	}

	if ( behavior == &actPlayer && (entitystats->EFFECTS[EFF_PACIFY] || entitystats->EFFECTS[EFF_FEAR]) )
	{
		return false;
	}
	else if ( behavior == &actPlayer && entitystats->HP <= 0 )
	{
		return false;
	}

	if ( behavior == &actPlayer &&
		(this->skill[9] == MONSTER_POSE_SPECIAL_WINDUP1 || this->skill[9] == PLAYER_POSE_GOLEM_SMASH) // special strike attack
		)
	{
		return false;
	}

	if ( behavior == &actMonster && MainMenu::isCutsceneActive() )
	{
		return false;
	}

	// paralyzed
	if ( entitystats->EFFECTS[EFF_PARALYZED] )
	{
		return false;
	}

	// asleep
	if ( entitystats->EFFECTS[EFF_ASLEEP] )
	{
		return false;
	}

	// stunned
	if ( entitystats->EFFECTS[EFF_STUNNED] )
	{
		return false;
	}

	if ( (entitystats->type == LICH_FIRE || entitystats->type == LICH_ICE)
		&& monsterLichBattleState < LICH_BATTLE_READY )
	{
		return false;
	}

	if ( entitystats->type == GYROBOT
		&& (monsterSpecialState == GYRO_RETURN_LANDING
			|| monsterSpecialState == GYRO_INTERACT_LANDING
			|| monsterSpecialState == GYRO_START_FLYING) )
	{
		return false;
	}
	else if ( (entitystats->type == DUMMYBOT || entitystats->type == SENTRYBOT || entitystats->type == SPELLBOT)
		&& (monsterSpecialState == DUMMYBOT_RETURN_FORM) )
	{
		return false;
	}

	if ( entitystats->MISC_FLAGS[STAT_FLAG_NPC] != 0 && !strcmp(entitystats->name, "scriptNPC") )
	{
		return false;
	}

	return true;
}

/*-------------------------------------------------------------------------------

Creature::attack

Causes an entity to attack using whatever weapon it's holding

-------------------------------------------------------------------------------*/

void Creature::attack(int pose, int charge, Entity* target)
{
	Stat* hitstats = nullptr;
	Stat* myStats = nullptr;
	int player, playerhit = -1;
	double dist;
	int c, i;
	int weaponskill = -1;
	node_t * node = nullptr;
	double tangent;

	if ( (myStats = getStats()) == nullptr )
	{
		return;
	}

	// get the player number, if applicable
	if ( behavior == &actPlayer )
	{
		player = skill[2];
	}
	else
	{
		player = -1; // not a player
	}

	if ( multiplayer != CLIENT )
	{
		// animation
		if ( player >= 0 )
		{
			players[player]->entity->skill[10] = 0; // PLAYER_ATTACKTIME
			if ( pose == MONSTER_POSE_SPECIAL_WINDUP1 || pose == PLAYER_POSE_GOLEM_SMASH || pose == MONSTER_POSE_SPECIAL_WINDUP2 )
			{
				players[player]->entity->skill[9] = pose; // PLAYER_ATTACK
				if ( pose == MONSTER_POSE_SPECIAL_WINDUP1 || pose == MONSTER_POSE_SPECIAL_WINDUP2 )
				{
					if ( multiplayer == SERVER )
					{
						if ( player >= 0 && player < MAXPLAYERS )
						{
							serverUpdateEntitySkill(players[player]->entity, 9);
							serverUpdateEntitySkill(players[player]->entity, 10);
						}
					}
					return;
				}
				else if ( pose == PLAYER_POSE_GOLEM_SMASH )
				{
					players[player]->entity->skill[10] = 1; // to avoid resetting the animation
				}
			}
			else if ( stats[player]->weapon != nullptr )
			{
				if ( stats[player]->type == CREATURE_IMP && itemCategory(stats[player]->weapon) != MAGICSTAFF )
				{
					players[player]->entity->skill[9] = 1;
				}
				else
				{
					players[player]->entity->skill[9] = pose; // PLAYER_ATTACK
				}
			}
			else
			{
				players[player]->entity->skill[9] = 1; // special case for punch to eliminate spanking motion :p
			}
		}
		else
		{
			if ( pose >= MONSTER_POSE_MELEE_WINDUP1 && pose <= MONSTER_POSE_SPECIAL_WINDUP3 )
			{
				// calls animation, but doesn't actually attack
				// this branch executes for most monsters
				monsterAttack = pose;
				if (myStats->type != SCARAB) {
				    monsterAttackTime = 0;
				}
				if ( multiplayer == SERVER )
				{
					// be sure to update the clients with the new wind-up pose.
					serverUpdateEntitySkill(this, 8);
					if (myStats->type != SLIME && myStats->type != RAT && myStats->type != SCARAB) {
					    serverUpdateEntitySkill(this, 9);
					}
				}
				return; // don't execute the attack, let the monster animation call the attack() function again.
			}
			else if ( (myStats->type == INCUBUS && (pose == MONSTER_POSE_INCUBUS_TELEPORT || pose == MONSTER_POSE_INCUBUS_TAUNT))
				|| (myStats->type == VAMPIRE && (pose == MONSTER_POSE_VAMPIRE_DRAIN || pose == MONSTER_POSE_VAMPIRE_AURA_CHARGE))
				|| (myStats->type == LICH_FIRE && pose == MONSTER_POSE_MAGIC_CAST1)
				|| (myStats->type == LICH_ICE && pose == MONSTER_POSE_MAGIC_CAST1)
				|| (myStats->type == LICH_ICE
						&& (monsterLichIceCastPrev == LICH_ATK_CHARGE_AOE
							|| monsterLichIceCastPrev == LICH_ATK_RISING_RAIN
							|| monsterLichIceCastPrev == LICH_ATK_FALLING_DIAGONAL
							|| monsterState == MONSTER_STATE_LICH_CASTSPELLS
							)
					)
			)
			{
				// calls animation, but doesn't actually attack
				// this branch executes in special cases for certain monsters
				monsterAttack = pose;
				monsterAttackTime = 0;
				if ( multiplayer == SERVER )
				{
					// be sure to update the clients with the new wind-up pose.
					serverUpdateEntitySkill(this, 8);
					serverUpdateEntitySkill(this, 9);
				}
				return; // don't execute the attack, let the monster animation call the attack() function again.
			}
			else if ( myStats->type == VAMPIRE && pose == MONSTER_POSE_VAMPIRE_AURA_CAST )
			{
				monsterAttack = 0;
			}
			else if ( myStats->weapon != nullptr || myStats->type == CRYSTALGOLEM || myStats->type == COCKATRICE )
			{
				monsterAttack = pose;
			}
			else
			{
				monsterAttack = 1;    // punching
			}
			monsterAttackTime = 0;
		}

		// special AoE attack.
		if ( behavior == &actPlayer && pose == MONSTER_POSE_AUTOMATON_MALFUNCTION )
		{
			list_t * aoeTargets = nullptr;
			getTargetsAroundEntity(this, this, 24, PI, MONSTER_TARGET_ALL, &aoeTargets);
			if ( aoeTargets )
			{
				for ( node = aoeTargets->first; node != NULL; node = node->next )
				{
					Entity* tmpEntity = (Entity*)node->element;
                    Creature* tmpEntityCrtr = (Creature*)(*&tmpEntity);
					if ( tmpEntity != nullptr )
					{
						spawnExplosion(tmpEntity->x, tmpEntity->y, tmpEntity->z);
						Stat* tmpStats = tmpEntity->getStats();
						if ( tmpStats )
						{
							int explodeDmg = (10 + local_rng.rand() % 10 + myStats->LVL) * Creature::getDamageTableMultiplier(tmpEntity, *tmpStats, DAMAGE_TABLE_MAGIC); // check base magic damage resist.
							Entity* gib = spawnGib(tmpEntity);
							serverSpawnGibForClient(gib);
							if ( tmpEntityCrtr && tmpEntityCrtr->behavior == &actPlayer )
							{
								playerhit = tmpEntity->skill[2];
								if ( playerhit > 0 && multiplayer == SERVER && !players[playerhit]->isLocalPlayer() )
								{
									strcpy((char*)net_packet->data, "SHAK");
									net_packet->data[4] = 20; // turns into .1
									net_packet->data[5] = 20;
									net_packet->address.host = net_clients[playerhit - 1].host;
									net_packet->address.port = net_clients[playerhit - 1].port;
									net_packet->len = 6;
									sendPacketSafe(net_sock, -1, net_packet, playerhit - 1);
								}
								else if ( playerhit == 0 || (splitscreen && playerhit > 0) )
								{
									cameravars[playerhit].shakex += 0.2;
									cameravars[playerhit].shakey += 20;
								}
								if ( playerhit >= 0 )
								{
									uint32_t color = makeColorRGB(255, 0, 0);
									messagePlayerColor(playerhit, MESSAGE_STATUS, color, Language::get(2523));
								}
							}
							tmpEntity->modHP(-explodeDmg);
						}
					}
				}
				//Free the list.
				list_FreeAll(aoeTargets);
				free(aoeTargets);
			}
			return;
		}

		if ( multiplayer == SERVER )
		{
			if ( player >= 0 && player < MAXPLAYERS )
			{
				serverUpdateEntitySkill(players[player]->entity, 9);
				serverUpdateEntitySkill(players[player]->entity, 10);
			}
			else
			{
				serverUpdateEntitySkill(this, 8);
				if (myStats->type != SLIME && myStats->type != RAT && myStats->type != SCARAB) {
				    serverUpdateEntitySkill(this, 9);
				}
			}
		}

		if ( myStats->type == SHADOW )
		{
			if ( myStats->EFFECTS[EFF_INVISIBLE] )
			{
				//Shadows lose invisibility when they attack.
				//TODO: How does this play with the passive invisibility?
				setEffect(EFF_INVISIBLE, false, 0, true);
			}
		}

		if ( behavior == &actMonster && monsterAllyIndex != -1 )
		{
			Entity* myTarget = uidToEntity(monsterTarget);
            Creature* myTargetCrtr = (Creature*)(&myTarget);
			if ( myTarget )
			{
				if ( myTarget->monsterAllyIndex != -1 || (myTargetCrtr && myTargetCrtr->behavior == &actPlayer ))
				{
					this->monsterReleaseAttackTarget(true); // stop attacking player allies or players after this hit executes.
				}
			}
		}
		/*if ( myStats->type == SHOPKEEPER )
		{
			if ( Entity* myTarget = uidToEntity(monsterTarget) )
			{
				if ( Stat* targetStats = myTarget->getStats() )
				{
					if ( targetStats->type == SHOPKEEPER )
					{
						this->monsterReleaseAttackTarget(true);
					}
				}
			}
		}*/

		bool shapeshifted = false;
		if ( this->behavior == &actPlayer && this->effectShapeshift != NOTHING )
		{
			shapeshifted = true;
		}

		bool isIllusion = false;
		if ( myStats->type == INCUBUS
			&& !strncmp(myStats->name, "inner demon", strlen("inner demon")) )
		{
			isIllusion = true;
		}

		if ( myStats->weapon != nullptr
			&& (!shapeshifted || (shapeshifted && myStats->type == CREATURE_IMP && itemCategory(myStats->weapon) == MAGICSTAFF)) )
		{
			// if non-shapeshifted, or you're an imp with a staff then process throwing/magic weapons

			// magical weapons
			if ( myStats->weapon->type == TOOL_BEARTRAP )
			{
				Item* item = myStats->weapon;
				item_ToolBeartrap(item, this);
				return;
			}
			else if ( itemCategory(myStats->weapon) == SPELLBOOK || itemCategory(myStats->weapon) == MAGICSTAFF )
			{
				if ( itemCategory(myStats->weapon) == MAGICSTAFF )
				{
					switch ( myStats->weapon->type )
					{
						case MAGICSTAFF_LIGHT:
							castSpell(uid, &spell_light, true, false);
							break;
						case MAGICSTAFF_DIGGING:
							castSpell(uid, &spell_dig, true, false);
							break;
						case MAGICSTAFF_LOCKING:
							castSpell(uid, &spell_locking, true, false);
							break;
						case MAGICSTAFF_MAGICMISSILE:
							castSpell(uid, &spell_magicmissile, true, false);
							break;
						case MAGICSTAFF_OPENING:
							castSpell(uid, &spell_opening, true, false);
							break;
						case MAGICSTAFF_SLOW:
							castSpell(uid, &spell_slow, true, false);
							break;
						case MAGICSTAFF_COLD:
							castSpell(uid, &spell_cold, true, false);
							break;
						case MAGICSTAFF_FIRE:
							castSpell(uid, &spell_fireball, true, false);
							break;
						case MAGICSTAFF_LIGHTNING:
							castSpell(uid, &spell_lightning, true, false);
							break;
						case MAGICSTAFF_SLEEP:
							castSpell(uid, &spell_sleep, true, false);
							break;
						case MAGICSTAFF_SUMMON:
							castSpell(uid, &spell_summon, true, false);
							break;
						case MAGICSTAFF_STONEBLOOD:
							castSpell(uid, &spell_stoneblood, true, false);
							break;
						case MAGICSTAFF_BLEED:
							castSpell(uid, &spell_bleed, true, false);
							break;
						case MAGICSTAFF_CHARM:
							castSpell(uid, &spell_charmMonster, true, false);
							break;
						case MAGICSTAFF_POISON:
							castSpell(uid, &spell_poison, true, false);
							break;
						default:
							messagePlayer(player, MESSAGE_DEBUG | MESSAGE_MISC, "This is my wish stick! Wishy wishy wish!");
							break;
					}

					// magicstaffs deplete themselves for each use
					bool degradeWeapon = true;
					if ( myStats->type == SHADOW || myStats->type == LICH_FIRE || myStats->type == LICH_ICE )
					{
						degradeWeapon = false; //certain monster's weapons don't degrade.
					}
					bool forceDegrade = false;
					if ( degradeWeapon )
					{
						if ( myStats->weapon->type == MAGICSTAFF_CHARM )
						{
							if ( myStats->weapon->status <= WORN )
							{
								forceDegrade = true;
							}
						}
					}

					if ( (local_rng.rand() % 3 == 0 && degradeWeapon && !(svFlags & SV_FLAG_HARDCORE)) || forceDegrade
						|| ((svFlags & SV_FLAG_HARDCORE) && local_rng.rand() % 6 == 0 && degradeWeapon) )
					{
						if ( player >= 0 && players[player]->isLocalPlayer() )
						{
							if ( myStats->weapon->count > 1 )
							{
								newItem(myStats->weapon->type, myStats->weapon->status, myStats->weapon->beatitude, myStats->weapon->count - 1, myStats->weapon->appearance, myStats->weapon->identified, &myStats->inventory);
							}
						}
						myStats->weapon->count = 1;
						myStats->weapon->status = static_cast<Status>(myStats->weapon->status - 1);
						if ( myStats->weapon->status != BROKEN )
						{
							messagePlayer(player, MESSAGE_EQUIPMENT, Language::get(659));
						}
						else
						{
							if ( itemCategory(myStats->weapon) == MAGICSTAFF && myStats->weapon->beatitude < 0 )
							{
								steamAchievementClient(player, "BARONY_ACH_ONE_MANS_TRASH");
							}
							messagePlayer(player, MESSAGE_EQUIPMENT, Language::get(660));
							if ( player >= 0 && players[player]->isLocalPlayer() && client_classes[player] == CLASS_MESMER )
							{
								if ( myStats->weapon->type == MAGICSTAFF_CHARM )
								{
									bool foundCharmSpell = false;
									for (node_t * spellnode = stats[player]->inventory.first; spellnode != nullptr; spellnode = spellnode->next )
									{
										Item* item = (Item*)spellnode->element;
										if ( item && itemCategory(item) == SPELL_CAT )
										{
											spell_t * spell = getSpellFromItem(player, item);
											if ( spell && spell->ID == SPELL_CHARM_MONSTER )
											{
												foundCharmSpell = true;
												break;
											}
										}
									}
									if ( !foundCharmSpell )
									{
										steamAchievement("BARONY_ACH_WHAT_NOW");
									}
								}
							}
						}
						if ( player > 0 && multiplayer == SERVER && !players[player]->isLocalPlayer() )
						{
							strcpy((char*)net_packet->data, "ARMR");
							net_packet->data[4] = 5;
							net_packet->data[5] = myStats->weapon->status;
							net_packet->address.host = net_clients[player - 1].host;
							net_packet->address.port = net_clients[player - 1].port;
							net_packet->len = 6;
							sendPacketSafe(net_sock, -1, net_packet, player - 1);
						}
					}
				}
				else
				{
					// this is mostly used for monsters that "cast" spells
					switch ( myStats->weapon->type )
					{
						case SPELLBOOK_FORCEBOLT:
							if ( myStats->type == SPELLBOT )
							{
								Entity* cast = castSpell(uid, &spell_forcebolt, true, false);
								if ( cast )
								{
									cast->z -= 1;
									cast->x = this->x + 2 * cos(this->yaw);
									cast->y = this->y + 2 * sin(this->yaw);
								}
							}
							else
							{
								castSpell(uid, &spell_forcebolt, true, false);
							}
							break;
						case SPELLBOOK_MAGICMISSILE:
							if ( myStats->type == SPELLBOT )
							{
								Entity* cast = castSpell(uid, &spell_magicmissile, true, false);
								if ( cast )
								{
									cast->z -= 1;
									cast->x = this->x + 2 * cos(this->yaw);
									cast->y = this->y + 2 * sin(this->yaw);
								}
							}
							else
							{
								castSpell(uid, &spell_magicmissile, true, false);
							}
							break;
						case SPELLBOOK_COLD:
							castSpell(uid, &spell_cold, true, false);
							break;
						case SPELLBOOK_FIREBALL:
							castSpell(uid, &spell_fireball, true, false);
							break;
						case SPELLBOOK_LIGHTNING:
							castSpell(uid, &spell_lightning, true, false);
							break;
						case SPELLBOOK_SLEEP:
							castSpell(uid, &spell_sleep, true, false);
							break;
						case SPELLBOOK_CONFUSE:
							castSpell(uid, &spell_confuse, true, false);
							break;
						case SPELLBOOK_SLOW:
							castSpell(uid, &spell_slow, true, false);
							break;
						case SPELLBOOK_DIG:
							castSpell(uid, &spell_dig, true, false);
							break;
						case SPELLBOOK_STONEBLOOD:
							castSpell(uid, &spell_stoneblood, true, false);
							break;
						case SPELLBOOK_BLEED:
							castSpell(uid, &spell_bleed, true, false);
							break;
						case SPELLBOOK_SUMMON:
							castSpell(uid, &spell_summon, true, false);
							break;
						case SPELLBOOK_ACID_SPRAY:
							castSpell(uid, &spell_acidSpray, true, false);
							break;
						case SPELLBOOK_STEAL_WEAPON:
							castSpell(uid, &spell_stealWeapon, true, false);
							break;
						case SPELLBOOK_DRAIN_SOUL:
							castSpell(uid, &spell_drainSoul, true, false);
							break;
						case SPELLBOOK_VAMPIRIC_AURA:
							castSpell(uid, &spell_vampiricAura, true, false);
							break;
						case SPELLBOOK_CHARM_MONSTER:
							castSpell(uid, &spell_charmMonster, true, false);
							break;
						case SPELLBOOK_POISON:
							castSpell(uid, &spell_poison, true, false);
							break;
						case SPELLBOOK_SPRAY_WEB:
							castSpell(uid, &spell_sprayWeb, true, false);
							break;
						case SPELLBOOK_SPEED:
							castSpell(uid, &spell_speed, true, false);
							break;
						case SPELLBOOK_HEALING:
							castSpell(uid, &spell_healing, true, false);
							break;
						case SPELLBOOK_EXTRAHEALING:
							castSpell(uid, &spell_extrahealing, true, false);
							break;
						case SPELLBOOK_TROLLS_BLOOD:
							castSpell(uid, &spell_trollsBlood, true, false);
							break;
						case SPELLBOOK_REFLECT_MAGIC:
							castSpell(uid, &spell_reflectMagic, true, false);
							break;
						case SPELLBOOK_DASH:
							castSpell(uid, &spell_dash, true, false);
							break;
						case SPELLBOOK_FEAR:
							castSpell(uid, &spell_fear, true, false);
							break;
						//case SPELLBOOK_REFLECT_MAGIC: //TODO: Test monster support. Maybe better to just use a special ability that directly casts the spell.
						//castSpell(uid, &spell_reflectMagic, true, false)
						//break;
						default:
							break;
					}

					// DEPRECATED!!
					/*if( myStats->MP>0 ) {
					castMagic(my);

					// spells deplete MP
					myStats->MP--;
					if( multiplayer==SERVER && player!=clientnum ) {
					strcpy((char *)net_packet->data,"UPMP");
					SDLNet_Write32((Uint32)myStats->MP,&net_packet->data[4]);
					net_packet->address.host = net_clients[player-1].host;
					net_packet->address.port = net_clients[player-1].port;
					net_packet->len = 8;
					sendPacketSafe(net_sock, -1, net_packet, player-1);
					}
					} else {
					messagePlayer(player,"You lack the energy to cast magic!");
					}*/
				}
				return;
			}

			// ranged weapons (bows)
			else if ( isRangedWeapon(*myStats->weapon) )
			{
				// damage weapon if applicable
				int bowDegradeChance = 50;
				if ( behavior == &actPlayer )
				{
					bowDegradeChance += (stats[skill[2]]->PROFICIENCIES[PRO_RANGED] / 20) * 10;
				}
				if ( myStats->type == GOBLIN )
				{
					bowDegradeChance += 20;
					if ( myStats->PROFICIENCIES[PRO_RANGED] < SKILL_LEVEL_LEGENDARY )
					{
						bowDegradeChance = std::min(bowDegradeChance, 90);
					}
				}
				if ( myStats->type == SKELETON && behavior == &actMonster && monsterAllySummonRank > 0 )
				{
					bowDegradeChance = 100; // conjured skeleton weapon doesn't break.
				}
				if ( bowDegradeChance < 100 && local_rng.rand() % bowDegradeChance == 0 && myStats->weapon->type != ARTIFACT_BOW )
				{
					if ( myStats->weapon != NULL )
					{
						if ( player >= 0 && players[player]->isLocalPlayer() )
						{
							if ( myStats->weapon->count > 1 )
							{
								newItem(myStats->weapon->type, myStats->weapon->status, myStats->weapon->beatitude, myStats->weapon->count - 1, myStats->weapon->appearance, myStats->weapon->identified, &myStats->inventory);
							}
						}
						myStats->weapon->count = 1;
						myStats->weapon->status = static_cast<Status>(myStats->weapon->status - 1);
						if ( myStats->weapon->status != BROKEN )
						{
							messagePlayer(player, MESSAGE_EQUIPMENT, Language::get(661), myStats->weapon->getName());
						}
						else
						{
							playSoundEntity(this, 76, 64);
							messagePlayer(player, MESSAGE_EQUIPMENT, Language::get(662), myStats->weapon->getName());
						}
						if ( player > 0 && multiplayer == SERVER && !players[player]->isLocalPlayer() )
						{
							strcpy((char*)net_packet->data, "ARMR");
							net_packet->data[4] = 5;
							net_packet->data[5] = myStats->weapon->status;
							net_packet->address.host = net_clients[player - 1].host;
							net_packet->address.port = net_clients[player - 1].port;
							net_packet->len = 6;
							sendPacketSafe(net_sock, -1, net_packet, player - 1);
						}
					}
				}
				Entity* entity = nullptr;
				if ( myStats->weapon->type == SLING )
				{
					entity = newEntity(78, 1, map.entities, nullptr); // rock
					playSoundEntity(this, 239 + local_rng.rand() % 3, 96);
				}
				else if ( myStats->weapon->type == CROSSBOW || myStats->weapon->type == HEAVY_CROSSBOW )
				{
					entity = newEntity(167, 1, map.entities, nullptr); // bolt
					if ( myStats->weapon->type == HEAVY_CROSSBOW )
					{
						playSoundEntity(this, 411 + local_rng.rand() % 3, 128);
						if ( this->behavior == &actPlayer && this->skill[2] > 0 )
						{
							this->setEffect(EFF_KNOCKBACK, true, 30, false);
						}
					}
					else
					{
						playSoundEntity(this, 239 + local_rng.rand() % 3, 96);
					}
				}
				else
				{
					entity = newEntity(166, 1, map.entities, nullptr); // arrow
					playSoundEntity(this, 239 + local_rng.rand() % 3, 96);
				}
				if ( !entity )
				{
					return;
				}
				entity->parent = uid;
				entity->x = x;
				entity->y = y;
				entity->z = z;
				if ( myStats->type == SENTRYBOT )
				{
					entity->z -= 1;
				}
				entity->yaw = yaw;
				entity->sizex = 1;
				entity->sizey = 1;
				entity->behavior = &actArrow;
				entity->flags[UPDATENEEDED] = true;
				entity->flags[PASSABLE] = true;

				// set properties of the arrow.
				if ( pose == MONSTER_POSE_RANGED_SHOOT2 && myStats->weapon->type == ARTIFACT_BOW )
				{
					entity->setRangedProjectileAttack(*this, *myStats, QUIVER_SILVER + local_rng.rand() % 7);
				}
				else
				{
					entity->setRangedProjectileAttack(*this, *myStats);
				}

				if ( entity->arrowQuiverType != 0 && myStats->shield && itemTypeIsQuiver(myStats->shield->type) )
				{
					//TODO: Refactor this so that we don't have to copy paste this check a million times whenever some-one uses up an item.
					if ( behavior == &actPlayer && pose != MONSTER_POSE_RANGED_SHOOT2 )
					{
						myStats->shield->count--;
						if ( myStats->shield->count <= 0 )
						{
							if ( myStats->shield->node )
							{
								list_RemoveNode(myStats->shield->node);
							}
							else
							{
								free(myStats->shield);
							}
							myStats->shield = nullptr;
						}
					}
				}
				return;
			}

			// potions & gems (throwing), and thrown weapons
			if ( itemCategory(myStats->weapon) == POTION
				|| itemCategory(myStats->weapon) == GEM
				|| itemCategory(myStats->weapon) == THROWN
				|| myStats->weapon->type == FOOD_CREAMPIE
				|| itemIsThrowableTinkerTool(myStats->weapon) )
			{
				bool drankPotion = false;
				if ( behavior == &actMonster && myStats->type == GOATMAN && itemCategory(myStats->weapon) == POTION )
				{
					//Goatmen chug potions & then toss them at you.
					if ( myStats->weapon->type == POTION_BOOZE && !myStats->EFFECTS[EFF_DRUNK] )
					{
						item_PotionBooze(myStats->weapon, this, this, false);
						drankPotion = true;
					}
					else if ( myStats->weapon->type == POTION_HEALING )
					{
						item_PotionHealing(myStats->weapon, this, this, false);
						drankPotion = true;
					}
					else if ( myStats->weapon->type == POTION_EXTRAHEALING )
					{
						item_PotionExtraHealing(myStats->weapon, this, this, false);
						drankPotion = true;
					}
				}

				if ( myStats->weapon->type == BOOMERANG )
				{
					playSoundEntity(this, 75, 64);
					//playSoundEntity(this, 427 + local_rng.rand() % 4, 128);

				}
				else
				{
					playSoundEntity(this, 75, 64);
				}
				Entity* entity = nullptr;
				if ( drankPotion )
				{
					Item* emptyBottle = newItem(POTION_EMPTY, myStats->weapon->status, myStats->weapon->beatitude, 1, myStats->weapon->appearance, myStats->weapon->appearance, nullptr);
					entity = newEntity(itemModel(emptyBottle), 1, map.entities, nullptr); // thrown item
					entity->parent = uid;
					entity->x = x;
					entity->y = y;
					entity->z = z;
					entity->yaw = yaw;
					entity->sizex = 1;
					entity->sizey = 1;
					entity->behavior = &actThrown;
					entity->flags[UPDATENEEDED] = true;
					entity->flags[PASSABLE] = true;
					entity->skill[10] = emptyBottle->type;
					entity->skill[11] = emptyBottle->status;
					entity->skill[12] = emptyBottle->beatitude;
					entity->skill[13] = 1;
					entity->skill[14] = emptyBottle->appearance;
					entity->skill[15] = emptyBottle->identified;
				}
				else
				{
					entity = newEntity(itemModel(myStats->weapon), 1, map.entities, nullptr); // thrown item
					entity->parent = uid;
					entity->x = x;
					entity->y = y;
					entity->z = z;
					entity->yaw = yaw;
					entity->sizex = 1;
					entity->sizey = 1;
					entity->behavior = &actThrown;
					entity->flags[UPDATENEEDED] = true;
					entity->flags[PASSABLE] = true;
					entity->skill[10] = myStats->weapon->type;
					entity->skill[11] = myStats->weapon->status;
					entity->skill[12] = myStats->weapon->beatitude;
					entity->skill[13] = 1;
					entity->skill[14] = myStats->weapon->appearance;
					entity->skill[15] = myStats->weapon->identified;
				}

				if ( itemCategory(myStats->weapon) == THROWN )
				{
					double speed = 5.f;
					double normalisedCharge = (charge * 1.5 / MAXCHARGE); // 0-1.5
					if ( myStats->weapon->type == BOOMERANG )
					{
						speed = 3.75 + normalisedCharge; //3.75
					}
					else
					{
						speed = 5.f + normalisedCharge;
					}

					// thrown items have slightly faster velocities
					if ( (myStats->weapon->type == STEEL_CHAKRAM || myStats->weapon->type == CRYSTAL_SHURIKEN) )
					{
						if ( this->behavior == &actPlayer )
						{
							// todo: change velocity of chakram/shuriken?
							entity->vel_x = speed * cos(players[player]->entity->yaw);
							entity->vel_y = speed * sin(players[player]->entity->yaw);
							entity->vel_z = -.3;
						}
						else if ( this->behavior == &actMonster )
						{
							// todo: change velocity of chakram/shuriken?
							entity->vel_x = 6 * cos(this->yaw);
							entity->vel_y = 6 * sin(this->yaw);
							entity->vel_z = -.3;
						}
					}
					else if ( myStats->weapon->type == BOOMERANG )
					{
						entity->sprite = 977;
						entity->pitch = PI;
						entity->yaw -= PI / 2;
						if ( this->behavior == &actPlayer )
						{
							entity->vel_x = speed * cos(players[player]->entity->yaw);
							entity->vel_y = speed * sin(players[player]->entity->yaw);
							entity->vel_z = -.1;
						}
						else if ( this->behavior == &actMonster )
						{
							entity->vel_x = 6 * cos(this->yaw);
							entity->vel_y = 6 * sin(this->yaw);
							entity->vel_z = -.1;
						}
					}
					else
					{
						if ( this->behavior == &actPlayer )
						{
							entity->vel_x = speed * cos(players[player]->entity->yaw);
							entity->vel_y = speed * sin(players[player]->entity->yaw);
							entity->vel_z = -.3;
						}
						else if ( this->behavior == &actMonster )
						{
							entity->vel_x = 6 * cos(this->yaw);
							entity->vel_y = 6 * sin(this->yaw);
							entity->vel_z = -.3;
						}
					}
					entity->thrownProjectilePower = this->getThrownAttack();
					if ( behavior == &actPlayer )
					{
						entity->thrownProjectileCharge = normalisedCharge * 10;
					}
				}
				else if ( itemIsThrowableTinkerTool(myStats->weapon) )
				{
					double normalisedCharge = (charge * 0.5);
					normalisedCharge /= MAXCHARGE;
					entity->sizex = 4;
					entity->sizey = 4;
					if ( myStats->weapon->type >= TOOL_BOMB && myStats->weapon->type <= TOOL_TELEPORT_BOMB )
					{
						entity->sizex = 2;
						entity->sizey = 2;
					}
					if ( behavior == &actPlayer )
					{
						entity->vel_x = (1.f + normalisedCharge) * cos(players[player]->entity->yaw);
						entity->vel_y = (1.f + normalisedCharge) * sin(players[player]->entity->yaw);
					}
					entity->vel_z = -.3;
					entity->roll -= (PI / 2 - 0.1 + (local_rng.rand() % 10) * 0.02);
					if ( myStats->type == GYROBOT )
					{
						entity->vel_x = 0.0;
						entity->vel_y = 0.0;
						if ( monsterAllyGetPlayerLeader() )
						{
							entity->parent = monsterAllyGetPlayerLeader()->getUID();
						}
					}
				}
				else
				{
					double speed = 5.f;
					if ( itemCategory(myStats->weapon) == GEM )
					{
						double normalisedCharge = (charge * 1.5 / MAXCHARGE); // 0-1.5
						speed = 3.f + normalisedCharge;
						if ( behavior == &actPlayer )
						{
							entity->thrownProjectileCharge = normalisedCharge * 10;
						}
					}
					entity->thrownProjectilePower = this->getThrownAttack();
					if ( this->behavior == &actPlayer )
					{
						entity->vel_x = speed * cos(players[player]->entity->yaw);
						entity->vel_y = speed * sin(players[player]->entity->yaw);
						entity->vel_z = -.5;
					}
					else if ( this->behavior == &actMonster )
					{
						entity->vel_x = speed * cos(this->yaw);
						entity->vel_y = speed * sin(this->yaw);
						entity->vel_z = -.5;
					}
				}

				//TODO: Refactor this so that we don't have to copy paste this check a million times whenever some-one uses up an item.
				myStats->weapon->count--;
				if ( myStats->weapon->count <= 0 )
				{
					if ( myStats->weapon->node )
					{
						list_RemoveNode(myStats->weapon->node);
					}
					else
					{
						free(myStats->weapon);
					}
					myStats->weapon = nullptr;
				}
				return;
			}
		}
		bool whip = myStats->weapon && myStats->weapon->type == TOOL_WHIP;
		// normal attacks
		if ( target == nullptr )
		{
			if ( whip )
			{
				dist = lineTrace(this, x, y, yaw, STRIKERANGE * 1.5, LINETRACE_ATK_CHECK_FRIENDLYFIRE, false);
				playSoundEntity(this, 23 + local_rng.rand() % 5, 128); // whoosh noise
			}
			else
			{
				playSoundEntity(this, 23 + local_rng.rand() % 5, 128); // whoosh noise
				dist = lineTrace(this, x, y, yaw, STRIKERANGE, LINETRACE_ATK_CHECK_FRIENDLYFIRE, false);
			}
		}
		else
		{
			hit.entity = target;
		}

		if ( hit.entity != nullptr )
		{
			if ( !(svFlags & SV_FLAG_FRIENDLYFIRE) )
			{
				// test for friendly fire
				if ( checkFriend(hit.entity) )
				{
					return;
				}
			}
			else if ( (myStats->type == LICH_FIRE && hit.entity->getRace() == LICH_ICE)
				|| (myStats->type == LICH_ICE && hit.entity->getRace() == LICH_FIRE) )
			{
				// friendship <3
				return;
			}

			int32_t previousMonsterState = -1;

			if ( hit.entity->behavior == &actBoulder )
			{
				if ( (myStats->weapon != nullptr && !shapeshifted) || pose == PLAYER_POSE_GOLEM_SMASH )
				{
					if ( (myStats->weapon && myStats->weapon->type == TOOL_PICKAXE && !shapeshifted) || pose == PLAYER_POSE_GOLEM_SMASH )
					{
						// spawn several rock items
						if ( pose == PLAYER_POSE_GOLEM_SMASH )
						{
							createParticleRock(hit.entity);
							if ( multiplayer == SERVER )
							{
								serverSpawnMiscParticles(hit.entity, PARTICLE_EFFECT_ABILITY_ROCK, 78);
							}
						}
						else
						{
							int i = 8 + local_rng.rand() % 4;
							int c;
							for ( c = 0; c < i; c++ )
							{
								Entity* entity = newEntity(-1, 1, map.entities, nullptr); //Rock/item entity.
								entity->flags[INVISIBLE] = true;
								entity->flags[UPDATENEEDED] = true;
								entity->x = hit.entity->x - 4 + local_rng.rand() % 8;
								entity->y = hit.entity->y - 4 + local_rng.rand() % 8;
								entity->z = -6 + local_rng.rand() % 12;
								entity->sizex = 4;
								entity->sizey = 4;
								entity->yaw = local_rng.rand() % 360 * PI / 180;
								entity->vel_x = (local_rng.rand() % 20 - 10) / 10.0;
								entity->vel_y = (local_rng.rand() % 20 - 10) / 10.0;
								entity->vel_z = -.25 - (local_rng.rand() % 5) / 10.0;
								entity->flags[PASSABLE] = true;
								entity->behavior = &actItem;
								entity->flags[USERFLAG1] = true; // no collision: helps performance
								entity->skill[10] = GEM_ROCK;    // type
								entity->skill[11] = WORN;        // status
								entity->skill[12] = 0;           // beatitude
								entity->skill[13] = 1;           // count
								entity->skill[14] = 0;           // appearance
								entity->skill[15] = 1;			 // identified
							}
						}

						double ox = hit.entity->x;
						double oy = hit.entity->y;

						if ( player >= 0 && (abs(hit.entity->vel_x) > 0.01 || abs(hit.entity->vel_y) > 0.01) )
						{
							// boulder rolling, check if rolling towards player.
							bool lastResort = false;
							int boulderDirection = 0;
							if ( abs(hit.entity->yaw - (PI / 2)) < 0.1 )
							{
								boulderDirection = 1;
							}
							else if ( abs(hit.entity->yaw - (PI)) < 0.1 )
							{
								boulderDirection = 2;
							}
							else if ( abs(hit.entity->yaw - (3 * PI / 2)) < 0.1 )
							{
								boulderDirection = 3;
							}

							switch ( boulderDirection )
							{
								case 0: // east
									if ( static_cast<int>(oy / 16) == static_cast<int>(y / 16)
										&& static_cast<int>(ox / 16) <= static_cast<int>(x / 16) )
									{
										lastResort = true;
									}
									break;
								case 1: // south
									if ( static_cast<int>(ox / 16) == static_cast<int>(x / 16)
										&& static_cast<int>(oy / 16) <= static_cast<int>(y / 16) )
									{
										lastResort = true;
									}
									break;
								case 2: // west
									if ( static_cast<int>(oy / 16) == static_cast<int>(y / 16)
										&& static_cast<int>(ox / 16) >= static_cast<int>(x / 16) )
									{
										lastResort = true;
									}
									break;
								case 3: // north
									if ( static_cast<int>(ox / 16) == static_cast<int>(x / 16)
										&& static_cast<int>(oy / 16) >= static_cast<int>(y / 16) )
									{
										lastResort = true;
									}
									break;
								default:
									break;
							}
							if ( lastResort )
							{
								steamAchievementClient(player, "BARONY_ACH_LAST_RESORT");
							}
						}

						boulderLavaOrArcaneOnDestroy(hit.entity, hit.entity->sprite, nullptr);

						// destroy the boulder
						playSoundEntity(hit.entity, 67, 128);
						list_RemoveNode(hit.entity->mynode);
						messagePlayer(player, MESSAGE_COMBAT, Language::get(663));
						if ( myStats->weapon && local_rng.rand() % 2 && pose != PLAYER_POSE_GOLEM_SMASH )
						{
							myStats->weapon->status = static_cast<Status>(myStats->weapon->status - 1);
							if ( myStats->weapon->status < BROKEN )
							{
								myStats->weapon->status = BROKEN; // bounds checking.
							}
							if ( myStats->weapon->status == BROKEN )
							{
								messagePlayer(player, MESSAGE_COMBAT, Language::get(664));
								playSoundEntity(this, 76, 64);
							}
							else
							{
								messagePlayer(player, MESSAGE_COMBAT, Language::get(665));
							}
							if ( player > 0 && multiplayer == SERVER && !players[player]->isLocalPlayer() )
							{
								strcpy((char*)net_packet->data, "ARMR");
								net_packet->data[4] = 5;
								net_packet->data[5] = myStats->weapon->status;
								net_packet->address.host = net_clients[player - 1].host;
								net_packet->address.port = net_clients[player - 1].port;
								net_packet->len = 6;
								sendPacketSafe(net_sock, -1, net_packet, player - 1);
							}
						}

						// on sokoban, destroying boulders spawns scorpions
						if ( !strcmp(map.name, "Sokoban") )
						{
							Entity* monster = nullptr;
							if ( local_rng.rand() % 2 == 0 )
							{
								monster = summonMonster(INSECTOID, ox, oy);
							}
							else
							{
								monster = summonMonster(SCORPION, ox, oy);
							}
							if ( monster )
							{
								int c;
								for ( c = 0; c < MAXPLAYERS; c++ )
								{
									uint32_t color = makeColorRGB(255, 128, 0);
									messagePlayerColor(c, MESSAGE_HINT, color, Language::get(406));
								}
							}
							boulderSokobanOnDestroy(false);
						}
					}
					else
					{
						spawnBang(hit.x - cos(yaw) * 2, hit.y - sin(yaw) * 2, 0);
					}
				}
				else
				{
					//spawnBang(hit.x - cos(my->yaw)*2,hit.y - sin(my->yaw)*2,0);
					playSoundPos(hit.x, hit.y, 183, 64);
				}
			}
			else if (Creature* hitCrtr = (Creature *)(&hit.entity); hitCrtr && hitCrtr->behavior == &actMonster )
			{
				previousMonsterState = hitCrtr->monsterState;
				hitstats = hit.entity->getStats();
				if ( hitstats )
				{
					bool alertTarget = true;
					if ( behavior == &actMonster && monsterAllyIndex != -1 && hit.entity->monsterAllyIndex != -1 )
					{
						// if we're both allies of players, don't alert the hit target.
						alertTarget = false;
					}

					// alert the monster!
					if ( hitCrtr->monsterState != MONSTER_STATE_ATTACK && (hitstats->type < LICH || hitstats->type >= SHOPKEEPER) )
					{
						if ( alertTarget )
						{
							hitCrtr->monsterAcquireAttackTarget(*this, MONSTER_STATE_PATH, true);
						}
					}

					// alert other monsters too
					if ( alertTarget )
					{
						hitCrtr->alertAlliesOnBeingHit(this);
					}
					hitCrtr->updateEntityOnHit(this, alertTarget);
				}
			}
			else if ( Creature* hitCrtr = (Creature *)(&hit.entity); hitCrtr && hitCrtr->behavior == &actPlayer )
			{
				hitstats = stats[hit.entity->skill[2]];
				playerhit = hit.entity->skill[2];

				bool alertAllies = true;
				if ( behavior == &actMonster && monsterAllyIndex != -1 )
				{
					// if I'm a player ally, don't alert other allies.
					alertAllies = false;
				}

				// alert the player's followers!
				//Maybe should send a signal to each follower, with some kind of attached priority, which determines if they change their target to bumrush the player's assailant.
				for ( node = hitstats->FOLLOWERS.first; node != nullptr && alertAllies; node = node->next )
				{
					uint32_t * c = (uint32_t *)node->element;
					Creature* entity = nullptr;
					if ( c )
					{
						entity = uidToCreature(*c);
					}
					Entity* ohitentity = hit.entity;
					if ( entity )
					{
						Stat* buddystats = entity->getStats();
						if ( buddystats != nullptr )
						{
							if ( entity->monsterState == MONSTER_STATE_WAIT || (entity->monsterState == MONSTER_STATE_HUNT && entity->monsterTarget != uid) ) // monster is waiting or hunting
							{
								if ( entity->monsterAllyState == ALLY_STATE_DEFEND )
								{
									// monster is defending, make em stay put unless line of sight.
									tangent = atan2(entity->y - ohitentity->y, entity->x - ohitentity->x);
									lineTrace(ohitentity, ohitentity->x, ohitentity->y, tangent, 1024, 0, false);
									if ( hit.entity == entity )
									{
										Entity* attackTarget = uidToEntity(uid);
										if ( attackTarget )
										{
											entity->monsterAcquireAttackTarget(*attackTarget, MONSTER_STATE_PATH);
										}
									}
								}
								else
								{
									Entity* attackTarget = uidToEntity(uid);
									if ( attackTarget )
									{
										entity->monsterAcquireAttackTarget(*attackTarget, MONSTER_STATE_PATH);
									}
								}
							}
						}
					}
					hit.entity = ohitentity;
				}
			}
			else if ( hit.entity->behavior == &actColliderDecoration
				&& hit.entity->colliderDiggable != 0
				&& ((myStats->weapon && myStats->weapon->type == TOOL_PICKAXE && !shapeshifted) || pose == PLAYER_POSE_GOLEM_SMASH) )
			{
				magicDig(this, nullptr, 1, 0);

				if ( myStats->weapon && local_rng.rand() % 2 && pose != PLAYER_POSE_GOLEM_SMASH )
				{
					myStats->weapon->status = static_cast<Status>(myStats->weapon->status - 1);
					if ( myStats->weapon->status < BROKEN )
					{
						myStats->weapon->status = BROKEN; // bounds checking.
					}
					if ( myStats->weapon->status == BROKEN )
					{
						messagePlayer(player, MESSAGE_COMBAT, Language::get(664));
						playSoundEntity(this, 76, 64);
					}
					else
					{
						messagePlayer(player, MESSAGE_COMBAT, Language::get(665));
					}
					if ( player > 0 && multiplayer == SERVER && !players[player]->isLocalPlayer() )
					{
						strcpy((char*)net_packet->data, "ARMR");
						net_packet->data[4] = 5;
						net_packet->data[5] = myStats->weapon->status;
						net_packet->address.host = net_clients[player - 1].host;
						net_packet->address.port = net_clients[player - 1].port;
						net_packet->len = 6;
						sendPacketSafe(net_sock, -1, net_packet, player - 1);
					}
				}
			}
			else if ( hit.entity->behavior == &actDoor || hit.entity->behavior == &::actFurniture || hit.entity->behavior == &::actChest
				|| (hit.entity->isDamageableCollider() && hit.entity->isColliderDamageableByMelee()) )
			{
				int axe = 0;
				int damage = 1;
				int weaponskill = -1;
				if ( myStats->weapon && !shapeshifted )
				{
					weaponskill = getWeaponSkill(myStats->weapon);
					if ( hit.entity->behavior == &actColliderDecoration )
					{
						if ( weaponskill >= 0 && hit.entity->isColliderWeakToSkill(weaponskill) )
						{
							axe = (myStats->PROFICIENCIES[weaponskill] / 20);
							if ( myStats->PROFICIENCIES[weaponskill] >= SKILL_LEVEL_LEGENDARY )
							{
								axe = 9;
							}
						}
					}
					else if ( weaponskill == PRO_AXE )
					{
						axe = (myStats->PROFICIENCIES[weaponskill] / 20);
						if ( myStats->PROFICIENCIES[weaponskill] >= SKILL_LEVEL_LEGENDARY )
						{
							axe = 9;
						}
					}
				}
				else
				{
					weaponskill = PRO_UNARMED;
					if ( hit.entity->behavior == &actColliderDecoration )
					{
						if ( hit.entity->isColliderWeakToSkill(weaponskill) )
						{
							axe = (myStats->PROFICIENCIES[weaponskill] / 20);
							if ( myStats->PROFICIENCIES[weaponskill] >= SKILL_LEVEL_LEGENDARY )
							{
								axe = 9;
							}
						}
					}
					else if ( hit.entity->behavior != &::actChest )
					{
						axe = (myStats->PROFICIENCIES[PRO_UNARMED] / 20);
						if ( myStats->PROFICIENCIES[PRO_UNARMED] >= SKILL_LEVEL_LEGENDARY )
						{
							axe = 9;
						}
					}
				}
				if ( pose == PLAYER_POSE_GOLEM_SMASH )
				{
					if ( hit.entity->behavior == &actDoor || hit.entity->behavior == &::actFurniture )
					{
						axe += 20;
					}
					else if ( hit.entity->behavior == &::actChest )
					{
						axe = std::min(axe + 50, 50);
					}
					damage += axe;
				}
				else
				{
					damage += axe;
					if ( charge >= MAXCHARGE / 2 )
					{
						damage *= 2;
					}
				}

				int& entityHP = hit.entity->behavior == &actColliderDecoration ? hit.entity->colliderCurrentHP :
					(hit.entity->behavior == &::actChest ? hit.entity->chestHealth :
					(hit.entity->behavior == &actDoor ? hit.entity->doorHealth :
						hit.entity->furnitureHealth));

				entityHP -= damage;

				if ( whip )
				{
					playSoundEntity(hit.entity, 407 + local_rng.rand() % 3, 64);
				}
				else
				{
					int sound = 28; //damage.ogg
					if ( hit.entity->behavior == &actColliderDecoration && hit.entity->getColliderSfxOnHit() > 0 )
					{
						sound = hit.entity->getColliderSfxOnHit();
					}
					playSoundEntity(hit.entity, sound, 64);
				}
				if ( entityHP > 0 )
				{
					if ( hit.entity->behavior == &actDoor )
					{
						messagePlayer(player, MESSAGE_COMBAT_BASIC, Language::get(666));
					}
					else if ( hit.entity->behavior == &::actChest )
					{
						messagePlayer(player, MESSAGE_COMBAT_BASIC, Language::get(667));
					}
					else if ( hit.entity->isDamageableCollider() )
					{
						messagePlayer(player, MESSAGE_COMBAT_BASIC, Language::get(hit.entity->getColliderOnHitLangEntry()),
							Language::get(hit.entity->getColliderLangName()));
					}
					else if ( hit.entity->behavior == &::actFurniture )
					{
						switch ( hit.entity->furnitureType )
						{
							case FURNITURE_CHAIR:
								messagePlayer(player, MESSAGE_COMBAT_BASIC, Language::get(669));
								break;
							case FURNITURE_TABLE:
								messagePlayer(player, MESSAGE_COMBAT_BASIC, Language::get(668));
								break;
							case FURNITURE_BED:
								messagePlayer(player, MESSAGE_COMBAT_BASIC, Language::get(2509), Language::get(2505));
								break;
							case FURNITURE_BUNKBED:
								messagePlayer(player, MESSAGE_COMBAT_BASIC, Language::get(2509), Language::get(2506));
								break;
							case FURNITURE_PODIUM:
								messagePlayer(player, MESSAGE_COMBAT_BASIC, Language::get(2509), Language::get(2507));
								break;
							default:
								break;
						}
					}
				}
				else
				{
					entityHP = 0;
					if ( hit.entity->behavior == &actDoor )
					{
						messagePlayer(player, MESSAGE_COMBAT, Language::get(670));
						if ( !hit.entity->skill[0] )
						{
							hit.entity->skill[6] = (x > hit.entity->x);
						}
						else
						{
							hit.entity->skill[6] = (y < hit.entity->y);
						}
					}
					else if ( hit.entity->behavior == &::actChest )
					{
						messagePlayer(player, MESSAGE_COMBAT, Language::get(671));
					}
					else if ( hit.entity->isDamageableCollider() )
					{
						messagePlayer(player, MESSAGE_COMBAT, Language::get(hit.entity->getColliderOnBreakLangEntry()),
							Language::get(hit.entity->getColliderLangName()));
					}
					else if ( hit.entity->behavior == &::actFurniture )
					{
						switch ( hit.entity->furnitureType )
						{
							case FURNITURE_CHAIR:
								messagePlayer(player, MESSAGE_COMBAT, Language::get(673));
								break;
							case FURNITURE_TABLE:
								messagePlayer(player, MESSAGE_COMBAT, Language::get(672));
								break;
							case FURNITURE_BED:
								messagePlayer(player, MESSAGE_COMBAT, Language::get(2510), Language::get(2505));
								break;
							case FURNITURE_BUNKBED:
								messagePlayer(player, MESSAGE_COMBAT, Language::get(2510), Language::get(2506));
								break;
							case FURNITURE_PODIUM:
								messagePlayer(player, MESSAGE_COMBAT, Language::get(2510), Language::get(2507));
								break;
							default:
								break;
						}
					}
				}
				if ( hit.entity->behavior == &actDoor )
				{
					updateEnemyBar(this, hit.entity, Language::get(674), entityHP, hit.entity->skill[9], false,
						DamageGib::DMG_DEFAULT);
				}
				else if ( hit.entity->behavior == &::actChest )
				{
					updateEnemyBar(this, hit.entity, Language::get(675), entityHP, hit.entity->skill[8], false,
						DamageGib::DMG_DEFAULT);
				}
				else if ( hit.entity->isDamageableCollider() )
				{
					updateEnemyBar(this, hit.entity, Language::get(hit.entity->getColliderLangName()), entityHP, hit.entity->colliderMaxHP, false,
						DamageGib::DMG_DEFAULT);
				}
				else if ( hit.entity->behavior == &::actFurniture )
				{
					switch ( hit.entity->furnitureType )
					{
						case FURNITURE_CHAIR:
							updateEnemyBar(this, hit.entity, Language::get(677), entityHP, hit.entity->furnitureMaxHealth, false,
								DamageGib::DMG_DEFAULT);
							break;
						case FURNITURE_TABLE:
							updateEnemyBar(this, hit.entity, Language::get(676), entityHP, hit.entity->furnitureMaxHealth, false,
								DamageGib::DMG_DEFAULT);
							break;
						case FURNITURE_BED:
							updateEnemyBar(this, hit.entity, Language::get(2505), entityHP, hit.entity->furnitureMaxHealth, false,
								DamageGib::DMG_DEFAULT);
							break;
						case FURNITURE_BUNKBED:
							updateEnemyBar(this, hit.entity, Language::get(2506), entityHP, hit.entity->furnitureMaxHealth, false,
								DamageGib::DMG_DEFAULT);
							break;
						case FURNITURE_PODIUM:
							updateEnemyBar(this, hit.entity, Language::get(2507), entityHP, hit.entity->furnitureMaxHealth, false,
								DamageGib::DMG_DEFAULT);
							break;
						default:
							break;
					}
				}
			}
			else if ( hit.entity->behavior == &actSink )
			{
				if ( whip )
				{
					playSoundEntity(hit.entity, 407 + local_rng.rand() % 3, 64);
				}
				else
				{
					playSoundEntity(hit.entity, 28, 64);
				}
				playSoundEntity(hit.entity, 140 + local_rng.rand() % 2, 64);
				messagePlayer(player, MESSAGE_COMBAT, Language::get(678));
				if ( hit.entity->skill[0] > 0 )
				{
					hit.entity->skill[0]--; //Deplete one usage.

											//50% chance spawn a slime.
					if ( local_rng.rand() % 2 == 0 )
					{
						// spawn slime
						Entity* monster = summonMonster(SLIME, x, y);
						if ( monster )
						{
							messagePlayer(player, MESSAGE_HINT, Language::get(582));
							Stat* monsterStats = monster->getStats();
							monsterStats->LVL = 4;
						}
					}

					if ( hit.entity->skill[0] == 0 )   //Depleted.
					{
						messagePlayer(player, MESSAGE_COMBAT, Language::get(585)); //TODO: Alert all players that see (or otherwise in range) it?
						playSoundEntity(hit.entity, 132, 64);
						serverUpdateEntitySkill(hit.entity, 0);
					}
				}
			}
			else
			{
				if ( myStats->weapon && !shapeshifted && pose != PLAYER_POSE_GOLEM_SMASH )
				{
					// bang
					spawnBang(hit.x - cos(yaw) * 2, hit.y - sin(yaw) * 2, 0);
				}
				else
				{
					playSoundPos(hit.x, hit.y, 183, 64);
				}
			}

			if ( hitstats != nullptr )
			{
				// hit chance
				//int hitskill=5; // for unarmed combat

				if ( behavior == &actPlayer )
				{
					if ( !players[skill[2]]->isLocalPlayer() )
					{
						if ( achievementRangedMode[skill[2]] && !playerFailedRangedOnlyConduct[skill[2]] )
						{
							messagePlayer(skill[2], MESSAGE_HINT, Language::get(3923)); // prevent attack.
							return;
						}
						if ( achievementRangedMode[skill[2]] )
						{
							messagePlayer(skill[2], MESSAGE_HINT, Language::get(3924)); // notify no longer eligible for achievement but still atk.
						}
						if ( !playerFailedRangedOnlyConduct[skill[2]] )
						{
							playerFailedRangedOnlyConduct[skill[2]] = true;
							serverUpdatePlayerConduct(skill[2], CONDUCT_RANGED_ONLY, 0);
						}
					}
					else
					{
						if ( achievementRangedMode[skill[2]] && conductGameChallenges[CONDUCT_RANGED_ONLY] )
						{
							messagePlayer(skill[2], MESSAGE_HINT, Language::get(3923)); // prevent attack.
							return;
						}
						if ( achievementRangedMode[skill[2]] )
						{
							messagePlayer(skill[2], MESSAGE_HINT, Language::get(3924)); // notify no longer eligible for achievement but still atk.
						}
						conductGameChallenges[CONDUCT_RANGED_ONLY] = 0;
					}
				}

				weaponskill = getWeaponSkill(myStats->weapon);
				//if ( behavior == &actMonster && weaponskill == PRO_UNARMED )
				//{
				//	was -1 for legacy monster punching and damage variance. != &actMonster added to PRO_UNARMED check for damage variability.
				//	weaponskill = -1;
				//}
				if ( shapeshifted || pose == PLAYER_POSE_GOLEM_SMASH )
				{
					weaponskill = PRO_UNARMED;
				}

				double weaponMultipliers = 0.0;
				if ( weaponskill == PRO_UNARMED )
				{
					weaponMultipliers = Creature::getDamageTableMultiplier(hit.entity, *hitstats, DAMAGE_TABLE_UNARMED);
				}
				else if ( weaponskill == PRO_RANGED )
				{
					weaponMultipliers = Creature::getDamageTableMultiplier(hit.entity, *hitstats, DAMAGE_TABLE_RANGED);
				}
				else if ( weaponskill >= 0 )
				{
					DamageTableType dmgType = static_cast<DamageTableType>(weaponskill - PRO_SWORD);
					weaponMultipliers = Creature::getDamageTableMultiplier(hit.entity, *hitstats, dmgType);
				}

				bool dyrnwynSmite = false;
				bool gugnirProc = false;
				if ( weaponskill == PRO_SWORD && myStats->weapon && myStats->weapon->type == ARTIFACT_SWORD )
				{
					switch ( hitstats->type )
					{
						case SKELETON:
						case CREATURE_IMP:
						case GHOUL:
						case DEMON:
						case SUCCUBUS:
						case INCUBUS:
						case VAMPIRE:
						case LICH:
						case LICH_ICE:
						case LICH_FIRE:
						case DEVIL:
						{
							// smite these creatures
							double amount = 0.0;
							double percent = getArtifactWeaponEffectChance(myStats->weapon->type, *myStats, &amount);
							if ( local_rng.rand() % 100 < static_cast<int>(percent) )
							{
								weaponMultipliers += amount;
								dyrnwynSmite = true;
								spawnMagicEffectParticles(hit.entity->x, hit.entity->y, hit.entity->z, 981);
								//playSoundEntity(hit.entity, 249, 64);
							}
							break;
						}
						default:
							break;
					}
				}
				/*if( weaponskill>=0 )
				hitskill = myStats->PROFICIENCIES[weaponskill]/5;
				c = local_rng.rand()%20 + hitskill + (weaponskill==PRO_POLEARM);
				bool hitsuccess=false;
				if( myStats->weapon ) {
				if( myStats->weapon->type == ARTIFACT_SPEAR ) {
				hitsuccess=true; // Gungnir always lands a hit!
				}
				}
				if( c > 10+std::min(std::max(-3,hit.entity->getDEX()-my->getDEX()),3) ) {
				hitsuccess=true;
				}
				if( hitsuccess )*/
				{
					// calculate and perform damage to opponent
					int damage = 0;
					int damagePreMultiplier = 1;

					if ( (myStats->type == CRYSTALGOLEM && pose == MONSTER_POSE_GOLEM_SMASH)
						|| (myStats->type == LICH_FIRE && pose == 3) )
					{
						damagePreMultiplier = 2;
					}
					else if ( player >= 0 && pose == PLAYER_POSE_GOLEM_SMASH )
					{
						damagePreMultiplier = 2;
					}

					int myAttack = std::max(0, (Creature::getAttack(this, myStats, behavior == &actPlayer) * damagePreMultiplier) + getBonusAttackOnTarget(*hitstats));
					int enemyAC = AC(hitstats);
					if ( weaponskill == PRO_POLEARM && myStats->weapon && myStats->weapon->type == ARTIFACT_SPEAR )
					{
						double amount = 0.f;
						double percent = getArtifactWeaponEffectChance(ARTIFACT_SPEAR, *myStats, &amount);
						if ( (local_rng.rand() % 100 < static_cast<int>(percent)) )
						{
							enemyAC *= amount;
							gugnirProc = true;
						}
					}
                    Creature* hitCrtr = (Creature *)(&hit.entity);
					double targetACEffectiveness = Creature::getACEffectiveness(hit.entity, hitstats, (hitCrtr && hitCrtr->behavior == &actPlayer), this, myStats);
					int attackAfterReductions = static_cast<int>(std::max(0.0, ((myAttack * targetACEffectiveness - enemyAC))) + (1.0 - targetACEffectiveness) * myAttack);
					if ( weaponskill == PRO_UNARMED )
					{
						damage = attackAfterReductions * weaponMultipliers;
					}
					else if ( weaponskill == PRO_RANGED )
					{
						damage = attackAfterReductions * weaponMultipliers;
					}
					else if ( weaponskill >= 0 )
					{
						damage = attackAfterReductions * weaponMultipliers;
					}
					else
					{
						damage = attackAfterReductions;
					}

					damage = std::max(0, damage);

					if ( weaponskill == PRO_AXE )
					{
						damage++;
					}
					if ( myStats->type == LICH_FIRE && !hitstats->defending )
					{
						if ( damage <= 8 )
						{
							damage += (8 - damage) + local_rng.rand() % 9; // 8 - 16 minimum damage.
						}
					}
					if ( behavior == &actMonster && myStats->EFFECTS[EFF_VAMPIRICAURA] )
					{
						damage += 5; // 5 bonus damage after reductions.
					}

					bool backstab = false;
					bool flanking = false;
					if ( player >= 0 && !monsterIsImmobileTurret(hit.entity, hitstats) )
					{
						double hitAngle = hit.entity->yawDifferenceFromPlayer(player);
						if ( (hitAngle >= 0 && hitAngle <= 2 * PI / 3) ) // 120 degree arc
						{
							int stealthCapstoneBonus = 1;
							if ( skillCapstoneUnlockedEntity(PRO_STEALTH) )
							{
								stealthCapstoneBonus = 2;
							}

							if ( previousMonsterState == MONSTER_STATE_WAIT
								|| previousMonsterState == MONSTER_STATE_PATH
								|| (previousMonsterState == MONSTER_STATE_HUNT && uidToEntity(monsterTarget) == nullptr) )
							{
								// unaware monster, get backstab damage.
								backstab = true;
								damage += (stats[player]->PROFICIENCIES[PRO_STEALTH] / 20 + 2) * (2 * stealthCapstoneBonus);
                                Creature* hitCrtr = (Creature *)(&hit.entity);
								if ( local_rng.rand() % 4 > 0 && (!hitCrtr || hitCrtr->behavior != &actPlayer ))
								{
									this->increaseSkill(PRO_STEALTH);
								}
							}
							else if ( local_rng.rand() % 2 == 0 )
							{
								// monster currently engaged in some form of combat maneuver
								// 1 in 2 chance to flank defenses.
								flanking = true;
								damage += (stats[player]->PROFICIENCIES[PRO_STEALTH] / 20 + 1) * (stealthCapstoneBonus);
                                Creature* hitCrtr = (Creature *)(&hit.entity);
								if ( local_rng.rand() % 20 == 0 && (!hitCrtr || hitCrtr->behavior != &actPlayer ))
								{
									this->increaseSkill(PRO_STEALTH);
								}
							}
						}
					}

					bool gungnir = false;
					if ( myStats->weapon )
					{
						if ( myStats->weapon->type == ARTIFACT_SPEAR )
						{
							gungnir = true;
						}
					}

					static ConsoleVariable<bool> cvar_atkonhit("/enemy_debugatkonhit", false);
					if ( (weaponskill >= PRO_SWORD && weaponskill < PRO_SHIELD) || (weaponskill == PRO_UNARMED && behavior != &actMonster) || weaponskill == PRO_RANGED )
					{
						if ( *cvar_atkonhit )
						{
							int chance = 0;
							if ( weaponskill == PRO_POLEARM )
							{
								chance = (damage / 3) * (100 - myStats->PROFICIENCIES[weaponskill]) / 100.f;
							}
							else
							{
								chance = (damage / 2) * (100 - myStats->PROFICIENCIES[weaponskill]) / 100.f;
							}
							messagePlayer(0, MESSAGE_DEBUG, "Old range minmax: %d-%d", damage - chance, damage);
						}
						/*if ( chance > 0 )
						{
							damage = (damage - chance) + (local_rng.rand() % chance) + 1;
						}*/
						 double variance = 20;
						 double baseSkillModifier = 50.0; // 40-60 base
						if ( weaponskill == PRO_UNARMED )
						{
							variance = 30.0;
							baseSkillModifier = 45.0; // 30-60 base
						}
						else if ( weaponskill == PRO_POLEARM )
						{
							if ( gungnir )
							{
								variance = 0.0;
								baseSkillModifier = 60.0;
							}
							else
							{
								variance = 10.0;
								baseSkillModifier = 55.0; // 50-60 base
							}
						}
						double skillModifier = baseSkillModifier - (variance / 2) + (myStats->PROFICIENCIES[weaponskill] / 2.0);
						skillModifier += (local_rng.rand() % (1 + static_cast<int>(variance)));
						skillModifier /= 100.0;
						skillModifier = std::min(skillModifier, 1.0);
						damage = damage - static_cast<int>((1.0 - skillModifier) * damage);
						if ( *cvar_atkonhit )
						{
							messagePlayer(0, MESSAGE_DEBUG, "New Dmg: %.f%%: %d", 100.0 * skillModifier, damage);
						}
					}

					if ( *cvar_atkonhit )
					{
						if ( weaponskill >= 0 )
						{
							messagePlayer(0, MESSAGE_DEBUG, "Old damage max: %d", (int)((std::max(0, (myAttack - enemyAC)) * weaponMultipliers)));
						}
					}


					int olddamage = damage;
					damage *= std::max(charge, MAXCHARGE / 2) / ((double)(MAXCHARGE / 2));
					bool parashuProc = false;
					if ( myStats->weapon )
					{
						if ( myStats->weapon->type == ARTIFACT_AXE )
						{
							double amount = 0.0;
							double percent = getArtifactWeaponEffectChance(myStats->weapon->type, *myStats, &amount);

							if ( local_rng.rand() % 100 < static_cast<int>(percent) )
							{
								if ( !shapeshifted )
								{
									damage *= amount; // Parashu sometimes multiplier damage
								}
								parashuProc = true;
							}
						}
					}

					if ( hitstats->type == DUMMYBOT )
					{
						// higher level dummy bots have damage cap limits on hit.
						if ( myStats->type != MINOTAUR && myStats->type != LICH_FIRE )
						{
							if ( hitstats->LVL >= 10 )
							{
								damage = std::min(damage, 15);
							}
							else if ( hitstats->LVL >= 5 )
							{
								damage = std::min(damage, 30);
							}
						}
					}

					hit.entity->modHP(-damage); // do the damage
					bool skillIncreased = false;
					// skill increase
					// can raise skills up to skill level 20 on dummybots...
					bool doSkillIncrease = true;
					if ( monsterIsImmobileTurret(hit.entity, hitstats) )
					{
						if ( hitstats->type == DUMMYBOT && hitstats->HP > 0 )
						{
							doSkillIncrease = true; // can train on dummybots.
						}
						else
						{
							doSkillIncrease = false; // no skill for killing/hurting other turrets.
						}
					}
					if ( hitCrtr && hitCrtr->behavior == &actPlayer && behavior == &actPlayer )
					{
						doSkillIncrease = false; // no skill for killing/hurting players
					}
					if ( doSkillIncrease
						&& ((weaponskill >= PRO_SWORD && weaponskill <= PRO_POLEARM) || weaponskill == PRO_UNARMED || (whip && weaponskill == PRO_RANGED)) )
					{
						if ( myStats->weapon &&
							(myStats->weapon->type == CRYSTAL_BATTLEAXE
								|| myStats->weapon->type == CRYSTAL_MACE
								|| myStats->weapon->type == CRYSTAL_SWORD
								|| myStats->weapon->type == CRYSTAL_SPEAR) )
						{
							int chance = 6;
							bool notify = true;
							if ( myStats->type == GOBLIN )
							{
								chance = 10;
								notify = true;
							}

							if ( local_rng.rand() % chance == 0 )
							{
								if ( hitstats->type != DUMMYBOT || (hitstats->type == DUMMYBOT && myStats->PROFICIENCIES[weaponskill] < SKILL_LEVEL_BASIC) )
								{
									this->increaseSkill(weaponskill, notify);
									skillIncreased = true;
								}
							}
						}
						else if ( hitstats->HP <= 0 )
						{
							if ( player >= 0 && weaponskill == PRO_UNARMED
								&& stats[player]->type == GOATMAN
								&& stats[player]->EFFECTS[EFF_DRUNK] )
							{
								steamStatisticUpdateClient(player, STEAM_STAT_BARFIGHT_CHAMP, STEAM_STAT_INT, 1);
							}
							int chance = 8;
							bool notify = true;
							if ( myStats->type == GOBLIN )
							{
								chance = 12;
								notify = true;
							}
							if ( local_rng.rand() % chance == 0 )
							{
								this->increaseSkill(weaponskill, notify);
								skillIncreased = true;
							}
						}
						else
						{
							int chance = 10;
							bool notify = true;
							if ( myStats->type == GOBLIN && weaponskill != PRO_RANGED )
							{
								chance = 14;
								notify = true;
							}
							if ( local_rng.rand() % chance == 0 )
							{
								if ( hitstats->type != DUMMYBOT || (hitstats->type == DUMMYBOT && myStats->PROFICIENCIES[weaponskill] < SKILL_LEVEL_BASIC) )
								{
									this->increaseSkill(weaponskill, notify);
									skillIncreased = true;
								}
							}
						}
					}

					if ( skillIncreased && myStats->type == GOBLIN && weaponskill != PRO_RANGED )
					{
						// goblins level up all combat skills at once.
						if ( weaponskill != PRO_SWORD )
						{
							this->increaseSkill(PRO_SWORD, false);
						}
						if ( weaponskill != PRO_MACE )
						{
							this->increaseSkill(PRO_MACE, false);
						}
						if ( weaponskill != PRO_AXE )
						{
							this->increaseSkill(PRO_AXE, false);
						}
						if ( weaponskill != PRO_POLEARM )
						{
							this->increaseSkill(PRO_POLEARM, false);
						}
						if ( weaponskill != PRO_UNARMED )
						{
							this->increaseSkill(PRO_UNARMED, false);
						}
						if ( player >= 0 )
						{
							uint32_t color = makeColorRGB(255, 255, 0);
							messagePlayerColor(player, MESSAGE_PROGRESSION, color, Language::get(3446));
						}
					}

					// write the obituary
					if ( hit.entity != this )
					{
						killedByMonsterObituary(hit.entity);
					}

					// damage weapon if applicable

					bool isWeakWeapon = false;
					bool artifactWeapon = false;
					bool degradeWeapon = false;
					ItemType weaponType = static_cast<ItemType>(WOODEN_SHIELD);
					bool hasMeleeGloves = false;
					if ( myStats->gloves && !shapeshifted )
					{
						switch ( myStats->gloves->type )
						{
							case BRASS_KNUCKLES:
							case IRON_KNUCKLES:
							case SPIKED_GAUNTLETS:
								hasMeleeGloves = true;
								break;
							default:
								break;
						}
					}

					Item** weaponToBreak = nullptr;

					if ( myStats->weapon )
					{
						weaponToBreak = &myStats->weapon;
					}
					else if ( hasMeleeGloves )
					{
						weaponToBreak = &myStats->gloves;
					}

					if ( weaponToBreak != nullptr && !shapeshifted )
					{
						weaponType = (*weaponToBreak)->type;
						if ( weaponType == ARTIFACT_AXE || weaponType == ARTIFACT_MACE || weaponType == ARTIFACT_SPEAR || weaponType == ARTIFACT_SWORD )
						{
							artifactWeapon = true;
						}
						else if ( weaponType == CRYSTAL_BATTLEAXE || weaponType == CRYSTAL_MACE || weaponType == CRYSTAL_SWORD || weaponType == CRYSTAL_SPEAR )
						{
							// crystal weapons degrade faster.
							isWeakWeapon = true;
						}

						if ( myStats->type == GOBLIN )
						{
							isWeakWeapon = false;
						}

						if ( !artifactWeapon )
						{
							// normal weapons chance to not degrade 75% chance on 0 dmg, else 98%
							int degradeOnZeroDMG = 4 + (myStats->type == GOBLIN ? 4 : 0);
							int degradeOnNormalDMG = 50 + (myStats->type == GOBLIN ? 20 : 0);

							if ( !myStats->weapon && (*weaponToBreak) )
							{
								// unarmed glove weapons, 87.5% to not degrade on 0 dmg, else 99%
								degradeOnZeroDMG = 8 + (myStats->type == GOBLIN ? 4 : 0);
								degradeOnNormalDMG = 100 + (myStats->type == GOBLIN ? 20 : 0);
							}
							else if ( isWeakWeapon )
							{
								// crystal weapons chance to not degrade 66% chance on 0 dmg, else 97.5%
								degradeOnZeroDMG = 3;
								degradeOnNormalDMG = 40;
							}

							if ( behavior == &actPlayer && ((weaponskill >= PRO_SWORD && weaponskill <= PRO_POLEARM)
								|| weaponskill == PRO_UNARMED || weaponskill == PRO_RANGED) )
							{
								int skillLVL = myStats->PROFICIENCIES[weaponskill] / 20;
								degradeOnZeroDMG += skillLVL; // increase by 1-5
								degradeOnNormalDMG += (skillLVL * 10); // increase by 10-50
							}
							if ( myStats->weapon && myStats->weapon->type == TOOL_WHIP )
							{
								degradeOnZeroDMG = degradeOnNormalDMG; // don't degrade faster on 0 dmg.
							}
							if ( behavior == &actPlayer && (svFlags & SV_FLAG_HARDCORE) )
							{
								// double durability.
								degradeOnZeroDMG *= 2;
								degradeOnNormalDMG *= 2;
							}

							if	( (local_rng.rand() % degradeOnZeroDMG == 0 && damage == 0)
									|| (local_rng.rand() % degradeOnNormalDMG == 0 && damage > 0)
								)
							{
								degradeWeapon = true;
							}

							if ( behavior == &actPlayer && skillCapstoneUnlocked(skill[2], weaponskill) )
							{
								// don't degrade on capstone skill.
								degradeWeapon = false;
							}
							else if ( myStats->type == SHADOW || myStats->type == LICH_FIRE || myStats->type == LICH_ICE )
							{
								degradeWeapon = false; //certain monster's weapons don't degrade.
							}
							else if ( myStats->type == SKELETON && behavior == &actMonster && monsterAllySummonRank != 0 )
							{
								degradeWeapon = false;
							}
							else if ( isIllusion )
							{
								degradeWeapon = false;
							}
							else if ( myStats->weapon && myStats->weapon->type == TOOL_WHIP )
							{
								degradeWeapon = false;
							}

							if ( degradeWeapon )
							{
								if ( (player >= 0 && players[player]->isLocalPlayer()) || player < 0 )
								{
									if ( (*weaponToBreak)->count > 1 )
									{
										newItem((*weaponToBreak)->type, (*weaponToBreak)->status, (*weaponToBreak)->beatitude,
											(*weaponToBreak)->count - 1, (*weaponToBreak)->appearance, (*weaponToBreak)->identified, &myStats->inventory);
									}
								}
								(*weaponToBreak)->count = 1;
								(*weaponToBreak)->status = static_cast<Status>((*weaponToBreak)->status - 1);
								if ( (*weaponToBreak)->status != BROKEN )
								{
									messagePlayer(player, MESSAGE_EQUIPMENT, Language::get(679));
								}
								else
								{
									playSoundEntity(this, 76, 64);
									messagePlayer(player, MESSAGE_EQUIPMENT, Language::get(680));
								}
								if ( player > 0 && multiplayer == SERVER && !players[player]->isLocalPlayer() )
								{
									strcpy((char*)net_packet->data, "ARMR");
									if ( weaponToBreak == &myStats->weapon )
									{
										net_packet->data[4] = 5;
									}
									else
									{
										net_packet->data[4] = 2;
									}
									net_packet->data[5] = (*weaponToBreak)->status;
									net_packet->address.host = net_clients[player - 1].host;
									net_packet->address.port = net_clients[player - 1].port;
									net_packet->len = 6;
									sendPacketSafe(net_sock, -1, net_packet, player - 1);
								}
								if ( (*weaponToBreak)->status == BROKEN && behavior == &actMonster && playerhit >= 0 )
								{
									steamStatisticUpdateClient(playerhit, STEAM_STAT_TOUGH_AS_NAILS, STEAM_STAT_INT, 1);
								}
							}
						}
					}

					// damage opponent armor if applicable
					Item* armor = NULL;
					int armornum = 0;
					bool isWeakArmor = false;

					if ( damage > 0 || (damage == 0 && !(hitstats->shield && hitstats->defending)) )
					{
						// choose random piece of equipment to target
						armornum = hitstats->pickRandomEquippedItemToDegradeOnHit(&armor, true, false, false, true);
						if ( armor != NULL && armor->status > BROKEN )
						{
							switch ( armor->type )
							{
								case CRYSTAL_HELM:
								case CRYSTAL_SHIELD:
								case CRYSTAL_BREASTPIECE:
								case CRYSTAL_BOOTS:
								case CRYSTAL_GLOVES:
									isWeakArmor = true;
									break;
								default:
									isWeakArmor = false;
									break;
							}
						}

						int armorDegradeChance = 25;
						if ( isWeakArmor )
						{
							armorDegradeChance = 15;
							if ( weaponskill == PRO_MACE )
							{
								armorDegradeChance = 5;
							}
						}
						else
						{
							if ( weaponskill == PRO_MACE )
							{
								armorDegradeChance = 10;
							}
						}

						if ( hitstats->type == GOBLIN )
						{
							armorDegradeChance += 10;
						}

						if ( hitCrtr && hitCrtr->behavior == &actPlayer && armornum == 4 )
						{
							armorDegradeChance += (hitstats->PROFICIENCIES[PRO_SHIELD] / 10);
							if ( itemCategory(hitstats->shield) == ARMOR )
							{
								armorDegradeChance += (hitstats->PROFICIENCIES[PRO_SHIELD] / 10); // 2x shield bonus offhand
							}
							if ( skillCapstoneUnlocked(hit.entity->skill[2], PRO_SHIELD) )
							{
								armorDegradeChance = 100; // don't break.
							}
						}

						// crystal golem special attack increase chance for armor to break if hit. (25-33%)
						// special attack only degrades armor if primary target.
						if ( (pose == MONSTER_POSE_GOLEM_SMASH || pose == PLAYER_POSE_GOLEM_SMASH) && target == nullptr )
						{
							if ( isWeakArmor )
							{
								// 66% chance to be deselected from degrading.
								if ( local_rng.rand() % 3 > 0 )
								{
									armor = NULL;
									armornum = 0;
								}
							}
							else
							{
								// 75% chance to be deselected from degrading.
								if ( local_rng.rand() % 4 > 0 )
								{
									armor = NULL;
									armornum = 0;
								}
							}
						}
						else
						{
							if ( armorDegradeChance == 100 || (local_rng.rand() % armorDegradeChance > 0) )
							{
								armor = NULL;
								armornum = 0;
							}
						}
					}

					// if nothing chosen to degrade, check extra shield chances to degrade
					if ( hitstats->shield != NULL && hitstats->shield->status > BROKEN && armor == NULL
						&& !itemTypeIsQuiver(hitstats->shield->type) && itemCategory(hitstats->shield) != SPELLBOOK
						&& hitstats->shield->type != TOOL_TINKERING_KIT )
					{
						if ( hitstats->shield->type == TOOL_CRYSTALSHARD && hitstats->defending )
						{
							// shards degrade by 1 stage each hit.
							armor = hitstats->shield;
							armornum = 4;
						}
						else if ( hitstats->shield->type == MIRROR_SHIELD && hitstats->defending )
						{
							// mirror shield degrade by 1 stage each hit.
							armor = hitstats->shield;
							armornum = 4;
						}
						else
						{
							// if no armor piece was chosen to break, grant chance to improve shield skill.
							if ( itemCategory(hitstats->shield) == ARMOR
								|| (hitstats->defending) )
							{
								if ( (local_rng.rand() % 15 == 0 && damage > 0) || (damage == 0 && local_rng.rand() % 8 == 0) )
								{
									bool increaseSkill = true;
									if ( hitCrtr && hitCrtr->behavior == &actPlayer && behavior == &actPlayer )
									{
										increaseSkill = false;
									}
									else if ( hitstats->EFFECTS[EFF_SHAPESHIFT] )
									{
										increaseSkill = false;
									}
									else if ( itemCategory(hitstats->shield) != ARMOR
										&& hitstats->PROFICIENCIES[PRO_SHIELD] >= SKILL_LEVEL_SKILLED )
									{
										increaseSkill = false; // non-shield offhands dont increase skill past 40.
									}
									if ( increaseSkill )
									{
										hit.entity->increaseSkill(PRO_SHIELD); // increase shield skill
									}
								}
							}

							// shield still has chance to degrade after raising skill.
							// crystal golem special attack increase chance for shield to break if defended. (33%)
							// special attack only degrades shields if primary target.
							int shieldDegradeChance = 10;
							if ( svFlags & SV_FLAG_HARDCORE )
							{
								shieldDegradeChance = 40;
							}
							if ( hitstats->type == GOBLIN )
							{
								shieldDegradeChance += 10;
							}
							if ( hitCrtr && hitCrtr->behavior == &actPlayer )
							{
								shieldDegradeChance += (hitstats->PROFICIENCIES[PRO_SHIELD] / 10);
								if ( itemCategory(hitstats->shield) == ARMOR )
								{
									shieldDegradeChance += (hitstats->PROFICIENCIES[PRO_SHIELD] / 10); // 2x shield bonus offhand
								}
								if ( skillCapstoneUnlocked(hit.entity->skill[2], PRO_SHIELD) )
								{
									shieldDegradeChance = 100; // don't break.
								}
							}
							if ( shieldDegradeChance < 100 && armor == NULL &&
								(	(hitstats->defending && local_rng.rand() % shieldDegradeChance == 0)
									|| (hitstats->defending && pose == MONSTER_POSE_GOLEM_SMASH && target == nullptr && local_rng.rand() % 3 == 0)
								)
								)
							{
								armor = hitstats->shield;
								armornum = 4;
							}
						}
					}

					if ( armor != NULL && armor->status > BROKEN  && hitCrtr)
					{
						hitCrtr->degradeArmor(*hitstats, *armor, armornum);
						if ( armor->status == BROKEN )
						{
							if ( player >= 0 && hitCrtr->behavior == &actMonster )
							{
								steamStatisticUpdateClient(player, STEAM_STAT_UNSTOPPABLE_FORCE, STEAM_STAT_INT, 1);
							}
						}
					}

					bool statusInflicted = false;
					bool paralyzeStatusInflicted = false;
					bool slowStatusInflicted = false;
					bool bleedStatusInflicted = false;
					bool swordExtraDamageInflicted = false;
					bool knockbackInflicted = false;
					bool dyrnwynBurn = false;

					// special weapon effects
					if ( myStats->weapon && !shapeshifted )
					{
						if ( myStats->weapon->type == ARTIFACT_SWORD )
						{
							double amount = 0.0;
							double percent = getArtifactWeaponEffectChance(myStats->weapon->type, *myStats, &amount);
							if ( dyrnwynSmite || (local_rng.rand() % 100 < static_cast<int>(percent)) )
							{
								if ( hit.entity->flags[BURNABLE] )
								{
									if ( hitstats )
									{
										hitstats->burningInflictedBy = static_cast<int32_t>(uid);
									}

									bool wasBurning = hit.entity->flags[BURNING];
									// Attempt to set the Entity on fire
									hit.entity->SetEntityOnFire();

									if ( !wasBurning && hit.entity->flags[BURNING] )
									{
										// 6 ticks maximum burning.
										hit.entity->char_fire = std::min(hit.entity->char_fire, static_cast<int>(TICKS_TO_PROCESS_FIRE * (6 + amount)));
										dyrnwynBurn = true;
									}

									// If a Player was hit, and they are now on fire, tell them what set them on fire
									if ( playerhit > 0 && hit.entity->flags[BURNING] )
									{
										messagePlayer(playerhit, MESSAGE_COMBAT, Language::get(683)); // "Dyrnwyn sets you on fire!"
									}
								}
							}
						}
						else if ( myStats->weapon->type == ARTIFACT_AXE && parashuProc )
						{
							int duration = 100; // 2 seconds
							if ( hitstats->HP > 0 && hit.entity->setEffect(EFF_SLOW, true, duration, true) )
							{
								slowStatusInflicted = true;
								playSoundEntity(hit.entity, 396 + local_rng.rand() % 3, 64);
								spawnMagicEffectParticles(hit.entity->x, hit.entity->y, hit.entity->z, 171);
							}
						}
					}

					if ( (hitstats->EFFECTS[EFF_WEBBED] || pose == PLAYER_POSE_GOLEM_SMASH)
						&& !hitstats->EFFECTS[EFF_KNOCKBACK] && hit.entity->setEffect(EFF_KNOCKBACK, true, 30, false) )
					{
						double baseMultiplier = 0.7;
						if ( pose == PLAYER_POSE_GOLEM_SMASH )
						{
							baseMultiplier = 0.9;
						}
						double pushbackMultiplier = baseMultiplier;
						if ( !hitCrtr || !hitCrtr->isMobile() )
						{
							pushbackMultiplier += 0.3;
						}
						double tangent = atan2(hit.entity->y - this->y, hit.entity->x - this->x);
						if ( hitCrtr && hitCrtr->behavior == &actMonster )
						{
							hitCrtr->vel_x = cos(tangent) * pushbackMultiplier;
							hitCrtr->vel_y = sin(tangent) * pushbackMultiplier;
							hitCrtr->monsterKnockbackVelocity = 0.05;
							hitCrtr->monsterKnockbackUID = this->getUID();
							hitCrtr->monsterKnockbackTangentDir = tangent;
							hitCrtr->lookAtEntity(*this);
							if ( !(backstab || flanking) )
							{
								if ( hitCrtr->monsterAttack == 0 )
								{
									hitCrtr->monsterHitTime = std::max(HITRATE - 12, hitCrtr->monsterHitTime);
								}
							}
						}
						else if ( hitCrtr && hitCrtr->behavior == &actPlayer )
						{
							// normalize tangent
							while ( tangent < 0 )
							{
								tangent += 2 * PI;
							}
							while ( tangent > 2 * PI )
							{
								tangent -= 2 * PI;
							}
							if ( !players[hit.entity->skill[2]]->isLocalPlayer() )
							{
								hitCrtr->monsterKnockbackVelocity = pushbackMultiplier;
								hitCrtr->monsterKnockbackTangentDir = tangent;
								serverUpdateEntityFSkill(hit.entity, 11);
								serverUpdateEntityFSkill(hit.entity, 9);
							}
							else
							{
								hitCrtr->monsterKnockbackVelocity = pushbackMultiplier;
								hitCrtr->monsterKnockbackTangentDir = tangent;
							}
						}
						knockbackInflicted = true;
					}

					// player weapon skills
					if ( damage > 0 && weaponskill == PRO_UNARMED && behavior == &actPlayer && (charge >= MAXCHARGE - 3) )
					{
						int chance = 0;
						bool inflictParalyze = false;
						switch ( myStats->PROFICIENCIES[PRO_UNARMED] / 20 )
						{
							case 0:
								break;
							case 1:
								break;
							case 2:
								break;
							case 3:
								break;
							case 4:
								break;
							case 5:
								chance = 1;
								break;
							default:
								break;
						}
						if ( chance > 0 && backstab && !hitstats->EFFECTS[EFF_PARALYZED] && hitstats->HP > 0 )
						{
							int duration = 50;
							if ( hitstats->HP > 0 && hit.entity->setEffect(EFF_PARALYZED, true, duration, true) )
							{
								paralyzeStatusInflicted = true;
								playSoundEntity(hit.entity, 172, 64); //TODO: Paralyze spell sound.
								spawnMagicEffectParticles(hit.entity->x, hit.entity->y, hit.entity->z, 170);
								if ( behavior == &actPlayer ) // redundant; but if this code ever changes...
								{
									steamAchievementClient(skill[2], "BARONY_ACH_ONE_PUNCH_MAN");
								}
							}
							hit.entity->modHP(-5); // do extra damage.
						}
						if ( !knockbackInflicted && hasMeleeGloves
							&& hit.entity->setEffect(EFF_KNOCKBACK, true, 30, false) )
						{
							double baseMultiplier = 0.5;
							if ( myStats->gloves )
							{
								switch ( myStats->gloves->type )
								{
									case BRASS_KNUCKLES:
										baseMultiplier = 0.25;
										break;
									case IRON_KNUCKLES:
										baseMultiplier = 0.35;
										break;
									case SPIKED_GAUNTLETS:
										baseMultiplier = 0.5;
										break;
									default:
										break;
								}
							}
							double pushbackMultiplier = baseMultiplier + 0.1 * (myStats->PROFICIENCIES[PRO_UNARMED] / 20);
							/*if ( myStats->shield && hasMeleeGloves )
							{
								pushbackMultiplier /= 2;
							}*/
							if ( !hitCrtr || !hitCrtr->isMobile() )
							{
								pushbackMultiplier += 0.3;
							}
							double tangent = atan2(hit.entity->y - this->y, hit.entity->x - this->x);
							if ( hitCrtr && hitCrtr->behavior == &actMonster )
							{
								hitCrtr->vel_x = cos(tangent) * pushbackMultiplier;
								hitCrtr->vel_y = sin(tangent) * pushbackMultiplier;
								hitCrtr->monsterKnockbackVelocity = 0.05;
								hitCrtr->monsterKnockbackUID = this->getUID();
								hitCrtr->monsterKnockbackTangentDir = tangent;
								hitCrtr->lookAtEntity(*this);
								if ( !(backstab || flanking) )
								{
									if ( hitCrtr->monsterAttack == 0 )
									{
										hitCrtr->monsterHitTime = std::max(HITRATE - 12, hitCrtr->monsterHitTime);
									}
								}
							}
							else if ( hitCrtr && hitCrtr->behavior == &actPlayer )
							{
								// normalize tangent
								while ( tangent < 0 )
								{
									tangent += 2 * PI;
								}
								while ( tangent > 2 * PI )
								{
									tangent -= 2 * PI;
								}
								if ( !players[hit.entity->skill[2]]->isLocalPlayer() )
								{
									hitCrtr->monsterKnockbackVelocity = pushbackMultiplier;
									hitCrtr->monsterKnockbackTangentDir = tangent;
									serverUpdateEntityFSkill(hit.entity, 11);
									serverUpdateEntityFSkill(hit.entity, 9);
								}
								else
								{
									hitCrtr->monsterKnockbackVelocity = pushbackMultiplier;
									hitCrtr->monsterKnockbackTangentDir = tangent;
								}
							}
							knockbackInflicted = true;
						}
					}
					else if ( damage > 0 && behavior == &actPlayer && weaponskill >= PRO_SWORD && weaponskill <= PRO_POLEARM && (charge >= MAXCHARGE - 3) )
					{
						// special weapon effects.
						int capstoneDamage = 5;
						if ( weaponskill == PRO_AXE )
						{
							capstoneDamage = 10;
						}
						int chance = 0;
						switch ( myStats->PROFICIENCIES[weaponskill] / 20 )
						{
							case 0:
							case 1:
							case 2:
							case 3:
							case 4:
								break;
							case 5:
								chance = 4;
								break;
							default:
								break;
						}

						if ( weaponskill == PRO_POLEARM )
						{
							// knockback.
							if ( chance > 0 )
							{
								if ( hitCrtr && hitCrtr->behavior == &actMonster && hit.entity->setEffect(EFF_KNOCKBACK, true, 20, false) )
								{
									double pushbackMultiplier = 0.3 + 0.075 * (myStats->PROFICIENCIES[PRO_POLEARM] / 20);
									if ( !hitCrtr->isMobile() )
									{
										pushbackMultiplier += 0.3;
									}
									double tangent = atan2(hit.entity->y - this->y, hit.entity->x - this->x);
									hitCrtr->vel_x = cos(tangent) * pushbackMultiplier;
									hitCrtr->vel_y = sin(tangent) * pushbackMultiplier;
									hitCrtr->monsterKnockbackVelocity = 0.05;
									hitCrtr->monsterKnockbackUID = this->getUID();
									hitCrtr->monsterKnockbackTangentDir = tangent;
									hitCrtr->lookAtEntity(*this);
									if ( !(backstab || flanking) )
									{
										if ( hitCrtr->monsterAttack == 0 )
										{
											hitCrtr->monsterHitTime = std::max(HITRATE - 12, hitCrtr->monsterHitTime);
										}
									}
									knockbackInflicted = true;
								}
								hit.entity->modHP(-capstoneDamage); // do the damage
							}
						}
						else if ( weaponskill == PRO_MACE && hitstats->HP > 0 )
						{
							// paralyze.
							if ( chance > 0 ) // chance based paralyze
							{
								if ( local_rng.rand() % chance == 0 && !hitstats->EFFECTS[EFF_PARALYZED] )
								{
									int duration = 75; // 1.5 seconds
									if ( hitstats->HP > 0 && hit.entity->setEffect(EFF_PARALYZED, true, duration, true) )
									{
										paralyzeStatusInflicted = true;
										playSoundEntity(hit.entity, 172, 64); //TODO: Paralyze spell sound.
										spawnMagicEffectParticles(hit.entity->x, hit.entity->y, hit.entity->z, 170);
									}
								}
								hit.entity->modHP(-capstoneDamage); // do the damage
							}
						}
						else if ( weaponskill == PRO_AXE && hitstats->HP > 0 )
						{
							// slow.
							if ( chance > 0 ) // always
							{
								int duration = 150; // 3 seconds
								if ( hitstats->HP > 0 && hit.entity->setEffect(EFF_SLOW, true, duration, true) && !slowStatusInflicted )
								{
									slowStatusInflicted = true;
									playSoundEntity(hit.entity, 396 + local_rng.rand() % 3, 64);
									spawnMagicEffectParticles(hit.entity->x, hit.entity->y, hit.entity->z, 171);
								}
								hit.entity->modHP(-capstoneDamage); // do the damage
								// don't re-notify if already inflicted slow from Parashu.
							}
						}
						else if ( weaponskill == PRO_SWORD && hitstats->HP > 0 )
						{
							// bleed.
							if ( chance > 0 ) // always
							{
								spawnMagicEffectParticles(hit.entity->x, hit.entity->y, hit.entity->z, 643);
								playSoundEntity(hit.entity, 173, 128);
								if ( gibtype[hitstats->type] > 0 && gibtype[(int)hitstats->type] != 5 )
								{
									bleedStatusInflicted = true;
									for ( int gibs = 0; gibs < 10; ++gibs )
									{
										Entity* gib = spawnGib(hit.entity);
										serverSpawnGibForClient(gib);
									}
									hit.entity->modHP(-capstoneDamage); // do the damage
								}
								else
								{
									swordExtraDamageInflicted = true;
									int extraDamage = 5;
									hit.entity->modHP(-(extraDamage + capstoneDamage)); // do the damage
								}
							}
						}
					}

					bool playerPoisonedTarget = false;

					// special monster effects
					if ( myStats->type == CRYSTALGOLEM && pose == MONSTER_POSE_GOLEM_SMASH )
					{
						if ( damage >= 150 && hitCrtr && hitCrtr->behavior == &actPlayer && playerhit >= 0 )
						{
							if ( hitstats && hitstats->HP > 0 )
							{
								steamAchievementClient(playerhit, "BARONY_ACH_SPONGE");
							}
						}
						if ( multiplayer != CLIENT )
						{
							createParticleRock(hit.entity);
							if ( multiplayer == SERVER )
							{
								serverSpawnMiscParticles(hit.entity, PARTICLE_EFFECT_ABILITY_ROCK, 78);
							}
							if ( target == nullptr )
							{
								// only play sound once on primary target.
								playSoundEntity(hit.entity, 181, 64);
							}
						}
					}
					else if ( (damage > 0 || hitstats->EFFECTS[EFF_PACIFY] || hitstats->EFFECTS[EFF_FEAR]) && local_rng.rand() % 4 == 0 )
					{
						switch ( myStats->type )
						{
						case SCORPION:
							hitstats->EFFECTS[EFF_PARALYZED] = true;
							hitstats->EFFECTS_TIMERS[EFF_PARALYZED] = std::max(50, 150 - hit.entity->getCON() * 5);
							messagePlayer(playerhit, MESSAGE_COMBAT, Language::get(684));
							messagePlayer(playerhit, MESSAGE_COMBAT, Language::get(685));
							serverUpdateEffects(playerhit);
							break;
						case SPIDER:
						{
							bool applyPoison = true;
							if ( behavior == &actPlayer )
							{
								if ( charge >= MAXCHARGE - 3 ) // fully charged strike injects venom.
								{
									applyPoison = true;
								}
								else
								{
									applyPoison = false;
								}
							}
							if ( applyPoison )
							{
								playerPoisonedTarget = true;
								hitstats->EFFECTS[EFF_POISONED] = true;
								hitstats->EFFECTS_TIMERS[EFF_POISONED] = std::max(200, 600 - hit.entity->getCON() * 20);
								hitstats->poisonKiller = getUID();
								if ( arachnophobia_filter ) {
									messagePlayer(playerhit, MESSAGE_COMBAT, Language::get(4089));
								}
								else {
									messagePlayer(playerhit, MESSAGE_COMBAT, Language::get(686));
								}
								messagePlayer(playerhit, MESSAGE_COMBAT, Language::get(687));
								serverUpdateEffects(playerhit);
								for ( int tmp = 0; tmp < 3; ++tmp )
								{
									Entity* gib = spawnGib(hit.entity, 211);
									serverSpawnGibForClient(gib);
								}
							}
							break;
						}
						case SUCCUBUS:
						{
							Item* armor = nullptr;
							int armornum = 0;
							if ( behavior == &actPlayer )
							{
								armor = nullptr;
							}
							else
							{
								if ( !monsterAllyGetPlayerLeader() )
								{
									if ( currentlevel >= 10 )
									{
										armornum = hitstats->pickRandomEquippedItem(&armor, true, false, false, false);
									}
									else
									{
										if ( local_rng.rand() % 4 == 0 )
										{
											armornum = hitstats->pickRandomEquippedItem(&armor, true, false, false, false);
										}
									}
								}
								else
								{
									if ( local_rng.rand() % 8 == 0 )
									{
										armornum = hitstats->pickRandomEquippedItem(&armor, true, false, false, false);
									}
								}
							}
							if ( armor != nullptr )
							{
								if ( (playerhit >= 0 && players[playerhit]->isLocalPlayer()) || playerhit < 0 )
								{
									if ( armor->count > 1 )
									{
										newItem(armor->type, armor->status, armor->beatitude, armor->count - 1, armor->appearance, armor->identified, &hitstats->inventory);
									}
								}
								armor->count = 1;
								messagePlayer(playerhit, MESSAGE_COMBAT, Language::get(688), armor->getName());
								Item* stolenArmor = newItem(armor->type, armor->status, armor->beatitude, armor->count, armor->appearance, armor->identified, &myStats->inventory);
								stolenArmor->ownerUid = hit.entity->getUID();
								Item** slot = itemSlot(hitstats, armor);
								if ( slot )
								{
									*slot = NULL;
								}
								if ( armor->node )
								{
									list_RemoveNode(armor->node);
								}
								else
								{
									free(armor);
								}
								if ( playerhit > 0 && multiplayer == SERVER && !players[playerhit]->isLocalPlayer() )
								{
									strcpy((char*)net_packet->data, "STLA");
									net_packet->data[4] = armornum;
									net_packet->address.host = net_clients[playerhit - 1].host;
									net_packet->address.port = net_clients[playerhit - 1].port;
									net_packet->len = 5;
									sendPacketSafe(net_sock, -1, net_packet, playerhit - 1);
								}
								teleportRandom();

								// the succubus loses interest after this
								monsterState = 0;
								monsterTarget = 0;
							}
							break;
						}
						default:
							break;
						}
					}
					else if ( damage == 0 && !(hitstats->defending) )
					{
						// special chance effects when damage is 0.
						if ( local_rng.rand() % 20 == 0 )
						{
							switch ( myStats->type )
							{
								case SCORPION:
									hitstats->EFFECTS[EFF_PARALYZED] = true;
									hitstats->EFFECTS_TIMERS[EFF_PARALYZED] = std::max(50, 150 - hit.entity->getCON() * 5);
									messagePlayer(playerhit, MESSAGE_COMBAT, Language::get(684));
									messagePlayer(playerhit, MESSAGE_COMBAT, Language::get(685));
									serverUpdateEffects(playerhit);
									statusInflicted = true;
									break;
								case SPIDER:
									if ( behavior != &actPlayer )
									{
										hitstats->EFFECTS[EFF_POISONED] = true;
										hitstats->EFFECTS_TIMERS[EFF_POISONED] = std::max(200, 300 - hit.entity->getCON() * 20);
										if (arachnophobia_filter) {
										    messagePlayer(playerhit, MESSAGE_COMBAT, Language::get(4089));
										} else {
										    messagePlayer(playerhit, MESSAGE_COMBAT, Language::get(686));
										}
										messagePlayer(playerhit, MESSAGE_COMBAT, Language::get(687));
										serverUpdateEffects(playerhit);
										statusInflicted = true;
									}
									break;
								default:
									break;
							}
						}
					}

					if ( player >= 0 && hitCrtr && hitCrtr->behavior == &actMonster )
					{
						if ( damage > 0 )
						{
							updateAchievementRhythmOfTheKnight(player, hit.entity, false);
						}
						else
						{
							if ( !achievementStatusRhythmOfTheKnight[player] )
							{
								achievementRhythmOfTheKnightVec[player].clear(); // didn't inflict damage.
							}
						}
					}

					bool artifactWeaponProc = parashuProc || dyrnwynSmite || dyrnwynBurn || gugnirProc;

					// send messages
					if ( !strcmp(hitstats->name, "") )
					{
						uint32_t color = makeColorRGB(0, 255, 0);
						uint32_t colorSpecial = color;// makeColorRGB(255, 0, 255);
						if ( hitstats->HP > 0 )
						{
							if ( !artifactWeaponProc )
							{
								if ( damage > olddamage )
								{
									// critical hit
									messagePlayerMonsterEvent(player, color, *hitstats, Language::get(689), Language::get(689), MSG_COMBAT);
								}
								else
								{
									// normal hit
									messagePlayerMonsterEvent(player, color, *hitstats, Language::get(690), Language::get(690), MSG_COMBAT_BASIC);
								}
							}

							if ( dyrnwynSmite )
							{
								messagePlayerMonsterEvent(player, colorSpecial, *hitstats, Language::get(3754), Language::get(3755), MSG_COMBAT);
							}
							else if ( dyrnwynBurn )
							{
								messagePlayerMonsterEvent(player, colorSpecial, *hitstats, Language::get(3756), Language::get(3757), MSG_COMBAT);
							}
							else if ( parashuProc )
							{
								messagePlayerMonsterEvent(player, colorSpecial, *hitstats, Language::get(3758), Language::get(3759), MSG_COMBAT);
							}
							else if ( gugnirProc )
							{
								messagePlayerMonsterEvent(player, colorSpecial, *hitstats, Language::get(3760), Language::get(3761), MSG_COMBAT);
							}

							if ( damage == 0 )
							{
								// blow bounces off
								messagePlayer(player, MESSAGE_COMBAT_BASIC, Language::get(691));
							}
							else
							{
								if ( flanking )
								{
									// flank defenses
									messagePlayerMonsterEvent(player, color, *hitstats, Language::get(2545), Language::get(2545), MSG_COMBAT);
								}
								else if ( backstab )
								{
									// backstab on unaware enemy
									messagePlayerMonsterEvent(player, color, *hitstats, Language::get(2543), Language::get(2543), MSG_COMBAT);
									if ( player >= 0 && hitstats->EFFECTS[EFF_SHADOW_TAGGED] && this->creatureShadowTaggedThisUid == hit.entity->getUID() )
									{
										achievementObserver.awardAchievementIfActive(player, this, AchievementObserver::BARONY_ACH_OHAI_MARK);
									}
								}
							}

							if ( playerPoisonedTarget )
							{
								messagePlayerMonsterEvent(player, color, *hitstats, Language::get(3478), Language::get(3479), MSG_COMBAT);
							}
							if ( paralyzeStatusInflicted )
							{
								messagePlayerMonsterEvent(player, color, *hitstats, Language::get(3206), Language::get(3205), MSG_COMBAT);
							}
							else if ( slowStatusInflicted )
							{
								messagePlayerMonsterEvent(player, color, *hitstats, Language::get(394), Language::get(393), MSG_COMBAT);
							}
							else if ( swordExtraDamageInflicted )
							{
								messagePlayerMonsterEvent(player, color, *hitstats, Language::get(3211), Language::get(3210), MSG_COMBAT);
							}
							else if ( knockbackInflicted )
							{
								messagePlayerMonsterEvent(player, color, *hitstats, Language::get(3215), Language::get(3214), MSG_COMBAT);
							}
						}
						else
						{
							// HP <= 0
							if ( backstab )
							{
								// assassinate monster
								messagePlayerMonsterEvent(player, color, *hitstats, Language::get(2547), Language::get(2547), MSG_COMBAT);
								if ( hitstats->type == COCKATRICE )
								{
									steamAchievementClient(player, "BARONY_ACH_SCALES_IN_FAVOR");
								}
								if ( player >= 0 && stats[player]->type == VAMPIRE && isInvisible() )
								{
									steamStatisticUpdateClient(player, STEAM_STAT_BLOOD_SPORT, STEAM_STAT_INT, 1);
								}
								if ( player >= 0 && hitstats->EFFECTS[EFF_SHADOW_TAGGED] && this->creatureShadowTaggedThisUid == hit.entity->getUID() )
								{
									achievementObserver.awardAchievementIfActive(player, this, AchievementObserver::BARONY_ACH_OHAI_MARK);
								}
							}
							else
							{
								// kill monster
								messagePlayerMonsterEvent(player, color, *hitstats, Language::get(692), Language::get(692), MSG_COMBAT);
								if ( player >= 0 && hitCrtr && hitCrtr->behavior == &actMonster )
								{
									double hitAngle = hitCrtr->yawDifferenceFromPlayer(player);
									if ( (hitAngle >= 0 && hitAngle <= 2 * PI / 3) ) // 120 degree arc
									{
										if ( hitCrtr->monsterState == MONSTER_STATE_ATTACK && hitCrtr->monsterTarget != 0
											&& hitCrtr->monsterTarget != getUID() )
										{
											bool angelOfDeath = false;
											// monster is attacking another entity.
											for ( int i = 0; i < MAXPLAYERS; ++i )
											{
												if ( players[i] && players[i]->entity )
												{
													if ( players[i]->entity->getUID() == hitCrtr->monsterTarget )
													{
														// monster is attacking another player.
														angelOfDeath = true;
														break;
													}
													Entity* tmpEnt = uidToEntity(hitCrtr->monsterTarget);
													if ( tmpEnt )
													{
														Stat* tmpStats = tmpEnt->getStats();
														if ( tmpStats )
														{
															if ( tmpStats->leader_uid == players[i]->entity->getUID() )
															{
																// monster is attacking an allied NPC of a player.
																angelOfDeath = true;
																break;
															}
														}
													}
												}
											}
											if ( angelOfDeath )
											{
												steamAchievementClient(player, "BARONY_ACH_ANGEL_OF_DEATH");
											}
										}
									}
								}
							}
							awardXP(hit.entity, true, true);
							if ( player >= 0 && myStats->weapon && this->checkEnemy(hit.entity) )
							{
								if ( myStats->weapon->ownerUid == hit.entity->getUID() )
								{
									achievementObserver.awardAchievementIfActive(player, hit.entity, AchievementObserver::BARONY_ACH_IRONIC_PUNISHMENT);
								}
								if ( myStats->weapon->type == TOOL_WHIP )
								{
									achievementObserver.awardAchievementIfActive(player, hit.entity, AchievementObserver::BARONY_ACH_COWBOY_FROM_HELL);
								}
								if ( weaponskill == PRO_AXE && client_classes[player] == CLASS_PUNISHER )
								{
									if ( hitstats->EFFECTS[EFF_DISORIENTED] || hitstats->EFFECTS[EFF_PARALYZED]
										|| hitstats->EFFECTS[EFF_SLOW] || hitstats->EFFECTS[EFF_ASLEEP] )
									{
										steamStatisticUpdateClient(player, STEAM_STAT_CHOPPING_BLOCK, STEAM_STAT_INT, 1);
									}
								}
							}
						}
					}
					else
					{
						uint32_t color = makeColorRGB(0, 255, 0);
						uint32_t colorSpecial = color;// makeColorRGB(255, 0, 255);
						if ( hitstats->HP > 0 )
						{
							if ( !artifactWeaponProc )
							{
								if ( damage > olddamage )
								{
									// critical hit
									messagePlayerMonsterEvent(player, color, *hitstats, Language::get(689), Language::get(693), MSG_COMBAT);
								}
								else
								{
									// normal hit
									messagePlayerMonsterEvent(player, color, *hitstats, Language::get(690), Language::get(694), MSG_COMBAT_BASIC);
								}
							}

							if ( dyrnwynSmite )
							{
								messagePlayerMonsterEvent(player, colorSpecial, *hitstats, Language::get(3754), Language::get(3755), MSG_COMBAT);
							}
							else if ( dyrnwynBurn )
							{
								messagePlayerMonsterEvent(player, colorSpecial, *hitstats, Language::get(3756), Language::get(3757), MSG_COMBAT);
							}
							else if ( parashuProc )
							{
								messagePlayerMonsterEvent(player, colorSpecial, *hitstats, Language::get(3758), Language::get(3759), MSG_COMBAT);
							}
							else if ( gugnirProc )
							{
								messagePlayerMonsterEvent(player, colorSpecial, *hitstats, Language::get(3760), Language::get(3761), MSG_COMBAT);
							}

							if ( damage == 0 )
							{
								// blow bounces off
								if ( hitstats->sex )
								{
									messagePlayerMonsterEvent(player, 0xFFFFFFFF, *hitstats, Language::get(691), Language::get(695), MSG_COMBAT_BASIC);
								}
								else
								{
									messagePlayerMonsterEvent(player, 0xFFFFFFFF, *hitstats, Language::get(691), Language::get(696), MSG_COMBAT_BASIC);
								}
							}
							else
							{
								if ( flanking )
								{
									// flank defenses
									messagePlayerMonsterEvent(player, color, *hitstats, Language::get(2545), Language::get(2546), MSG_COMBAT);
								}
								else if ( backstab )
								{
									// backstab on unaware enemy
									messagePlayerMonsterEvent(player, color, *hitstats, Language::get(2543), Language::get(2544), MSG_COMBAT);
									if ( player >= 0 && hitstats->EFFECTS[EFF_SHADOW_TAGGED] && this->creatureShadowTaggedThisUid == hit.entity->getUID() )
									{
										achievementObserver.awardAchievementIfActive(player, this, AchievementObserver::BARONY_ACH_OHAI_MARK);
									}
								}
							}

							if ( playerPoisonedTarget )
							{
								messagePlayerMonsterEvent(player, color, *hitstats, Language::get(3478), Language::get(3479), MSG_COMBAT);
							}
							if ( paralyzeStatusInflicted )
							{
								messagePlayerMonsterEvent(player, color, *hitstats, Language::get(3206), Language::get(3205), MSG_COMBAT);
							}
							else if ( slowStatusInflicted )
							{
								messagePlayerMonsterEvent(player, color, *hitstats, Language::get(394), Language::get(393), MSG_COMBAT);
							}
							else if ( swordExtraDamageInflicted )
							{
								messagePlayerMonsterEvent(player, color, *hitstats, Language::get(3211), Language::get(3210), MSG_COMBAT);
							}
							else if ( knockbackInflicted )
							{
								messagePlayerMonsterEvent(player, color, *hitstats, Language::get(3215), Language::get(3214), MSG_COMBAT);
							}
						}
						else
						{
							// HP <= 0
							if ( backstab )
							{
								// assassinate monster
								messagePlayerMonsterEvent(player, color, *hitstats, Language::get(2547), Language::get(2548), MSG_COMBAT);
								if ( hitstats->type == COCKATRICE )
								{
									steamAchievementClient(player, "BARONY_ACH_SCALES_IN_FAVOR");
								}
								if ( player >= 0 && stats[player]->type == VAMPIRE && isInvisible() )
								{
									steamStatisticUpdateClient(player, STEAM_STAT_BLOOD_SPORT, STEAM_STAT_INT, 1);
								}
								if ( player >= 0 && hitstats->EFFECTS[EFF_SHADOW_TAGGED] && this->creatureShadowTaggedThisUid == hit.entity->getUID() )
								{
									achievementObserver.awardAchievementIfActive(player, this, AchievementObserver::BARONY_ACH_OHAI_MARK);
								}
							}
							else
							{
								// kill monster
								messagePlayerMonsterEvent(player, color, *hitstats, Language::get(692), Language::get(697), MSG_COMBAT);
								if ( player >= 0 && hitCrtr && hitCrtr->behavior == &actMonster )
								{
									double hitAngle = hit.entity->yawDifferenceFromPlayer(player);
									if ( (hitAngle >= 0 && hitAngle <= 2 * PI / 3) ) // 120 degree arc
									{
										if ( hitCrtr->monsterState == MONSTER_STATE_ATTACK && hitCrtr->monsterTarget != 0
											&& hitCrtr->monsterTarget != getUID() )
										{
											bool angelOfDeath = false;
											// monster is attacking another entity.
											for ( int i = 0; i < MAXPLAYERS; ++i )
											{
												if ( players[i] && players[i]->entity )
												{
													if ( players[i]->entity->getUID() == hitCrtr->monsterTarget )
													{
														// monster is attacking another player.
														angelOfDeath = true;
														break;
													}
													Entity* tmpEnt = uidToEntity(hitCrtr->monsterTarget);
													if ( tmpEnt )
													{
														Stat* tmpStats = tmpEnt->getStats();
														if ( tmpStats )
														{
															if ( tmpStats->leader_uid == players[i]->entity->getUID() )
															{
																// monster is attacking an allied NPC of a player.
																angelOfDeath = true;
																break;
															}
														}
													}
												}
											}
											if ( angelOfDeath )
											{
												steamAchievementClient(player, "BARONY_ACH_ANGEL_OF_DEATH");
											}
										}
									}
								}
							}
							awardXP(hit.entity, true, true);
							if ( player >= 0 && myStats->weapon && this->checkEnemy(hit.entity) )
							{
								if ( myStats->weapon->ownerUid == hit.entity->getUID() )
								{
									achievementObserver.awardAchievementIfActive(player, hit.entity, AchievementObserver::BARONY_ACH_IRONIC_PUNISHMENT);
								}
								if ( myStats->weapon->type == TOOL_WHIP )
								{
									achievementObserver.awardAchievementIfActive(player, hit.entity, AchievementObserver::BARONY_ACH_COWBOY_FROM_HELL);
								}
								if ( weaponskill == PRO_AXE && client_classes[player] == CLASS_PUNISHER )
								{
									if ( hitstats->EFFECTS[EFF_DISORIENTED] || hitstats->EFFECTS[EFF_PARALYZED]
										|| hitstats->EFFECTS[EFF_SLOW] || hitstats->EFFECTS[EFF_ASLEEP] )
									{
										steamStatisticUpdateClient(player, STEAM_STAT_CHOPPING_BLOCK, STEAM_STAT_INT, 1);
									}
								}
							}
						}
					}

					bool disarmed = false;
					if ( hitstats->HP > 0 )
					{
						if ( !whip && hitstats->EFFECTS[EFF_DISORIENTED] )
						{
							hit.entity->setEffect(EFF_DISORIENTED, false, 0, false);
						}
						else if ( whip && (hitstats->EFFECTS[EFF_DISORIENTED]
							|| (!hitCrtr || !hitCrtr->isMobile())
							|| (hitstats->EFFECTS[EFF_DRUNK] && local_rng.rand() % 3 == 0)
							|| (hitstats->EFFECTS[EFF_CONFUSED] && local_rng.rand() % 3 == 0))
							)
						{
							if ( hitCrtr && hitCrtr->behavior == &actMonster && !hit.entity->isBossMonster() )
							{
								uint32_t color = makeColorRGB(0, 255, 0);
								if ( hitstats->weapon
									&& itemCategory(hitstats->weapon) != SPELLBOOK )
								{
									Entity* dropped = dropItemMonster(hitstats->weapon, hit.entity, hitstats);
									if ( dropped )
									{
										if ( hitstats->EFFECTS[EFF_DISORIENTED] && !hitstats->shield )
										{
											hit.entity->setEffect(EFF_DISORIENTED, false, 0, false);
										}
										playSoundEntity(hit.entity, 406, 128);
										dropped->itemDelayMonsterPickingUp = TICKS_PER_SECOND * 5;
										double tangent = atan2(hit.entity->y - y, hit.entity->x - x) + PI;
										dropped->yaw = tangent + PI;
										dropped->vel_x = (1.5 + .025 * (local_rng.rand() % 11)) * cos(tangent);
										dropped->vel_y = (1.5 + .025 * (local_rng.rand() % 11)) * sin(tangent);
										dropped->vel_z = (-10 - local_rng.rand() % 20) * .01;
										dropped->flags[USERFLAG1] = false;
										messagePlayerMonsterEvent(player, color, *hitstats, Language::get(3454), Language::get(3455), MSG_COMBAT);
										disarmed = true;
										dropped->itemOriginalOwner = hit.entity->getUID();
										if ( player >= 0 )
										{
											achievementObserver.addEntityAchievementTimer(hit.entity, AchievementObserver::BARONY_ACH_IRONIC_PUNISHMENT, -1, true, 0);
											achievementObserver.playerAchievements[player].ironicPunishmentTargets.insert(hit.entity->getUID());
										}
									}
								}
								else if ( hitstats->shield )
								{
									Entity* dropped = dropItemMonster(hitstats->shield, hit.entity, hitstats);
									if ( dropped )
									{
										if ( hitstats->EFFECTS[EFF_DISORIENTED] )
										{
											hit.entity->setEffect(EFF_DISORIENTED, false, 0, false);
										}
										playSoundEntity(hit.entity, 406, 128);
										dropped->itemDelayMonsterPickingUp = TICKS_PER_SECOND * 5;
										double tangent = atan2(hit.entity->y - y, hit.entity->x - x) + PI;
										dropped->yaw = tangent;
										dropped->vel_x = (1.5 + .025 * (local_rng.rand() % 11)) * cos(tangent);
										dropped->vel_y = (1.5 + .025 * (local_rng.rand() % 11)) * sin(tangent);
										dropped->vel_z = (-10 - local_rng.rand() % 20) * .01;
										dropped->flags[USERFLAG1] = false;
										messagePlayerMonsterEvent(player, color, *hitstats, Language::get(3456), Language::get(3457), MSG_COMBAT);
										disarmed = true;
										dropped->itemOriginalOwner = hit.entity->getUID();
									}
								}
								else
								{
									if ( hitstats->EFFECTS[EFF_DISORIENTED] )
									{
										hit.entity->setEffect(EFF_DISORIENTED, false, 0, false);
									}
								}
							}
							else
							{
								if ( hitstats->EFFECTS[EFF_DISORIENTED] )
								{
									hit.entity->setEffect(EFF_DISORIENTED, false, 0, false);
								}
							}
						}
					}

					if ( playerhit > 0 && multiplayer == SERVER && !players[playerhit]->isLocalPlayer() )
					{
						if ( pose == MONSTER_POSE_GOLEM_SMASH )
						{
							if ( target == nullptr )
							{
								// primary target
								strcpy((char*)net_packet->data, "SHAK");
								net_packet->data[4] = 20; // turns into .2
								net_packet->data[5] = 20;
								net_packet->address.host = net_clients[playerhit - 1].host;
								net_packet->address.port = net_clients[playerhit - 1].port;
								net_packet->len = 6;
								sendPacketSafe(net_sock, -1, net_packet, playerhit - 1);
							}
							else
							{
								// secondary target
								strcpy((char*)net_packet->data, "SHAK");
								net_packet->data[4] = 10; // turns into .1
								net_packet->data[5] = 10;
								net_packet->address.host = net_clients[playerhit - 1].host;
								net_packet->address.port = net_clients[playerhit - 1].port;
								net_packet->len = 6;
								sendPacketSafe(net_sock, -1, net_packet, playerhit - 1);
							}

							strcpy((char*)net_packet->data, "UPHP");
							SDLNet_Write32((uint32_t)hitstats->HP, &net_packet->data[4]);
							SDLNet_Write32((uint32_t)myStats->type, &net_packet->data[8]);
							net_packet->address.host = net_clients[playerhit - 1].host;
							net_packet->address.port = net_clients[playerhit - 1].port;
							net_packet->len = 12;
							sendPacketSafe(net_sock, -1, net_packet, playerhit - 1);
						}
						else
						{
							strcpy((char*)net_packet->data, "SHAK");
							net_packet->data[4] = 10; // turns into .1
							net_packet->data[5] = 10;
							net_packet->address.host = net_clients[playerhit - 1].host;
							net_packet->address.port = net_clients[playerhit - 1].port;
							net_packet->len = 6;
							sendPacketSafe(net_sock, -1, net_packet, playerhit - 1);
						}
					}
					else if ( playerhit == 0 || (splitscreen && playerhit > 0) )
					{
						if ( pose == MONSTER_POSE_GOLEM_SMASH || pose == PLAYER_POSE_GOLEM_SMASH )
						{
							if ( target == nullptr )
							{
								// primary target
								cameravars[playerhit].shakex += .2;
								cameravars[playerhit].shakey += 20;
							}
							else
							{
								// secondary target
								cameravars[playerhit].shakex += .1;
								cameravars[playerhit].shakey += 10;
							}
						}
						else if ( damage > 0 )
						{
							cameravars[playerhit].shakex += .1;
							cameravars[playerhit].shakey += 10;
						}
						else
						{
							cameravars[playerhit].shakex += .05;
							cameravars[playerhit].shakey += 5;
						}
					}

					if ( damage > 0 )
					{
						Entity* gib = spawnGib(hit.entity);
						serverSpawnGibForClient(gib);
						uint32_t color = makeColorRGB(255, 0, 0);
						messagePlayerMonsterEvent(playerhit, color, *myStats, Language::get(698), Language::get(699), MSG_ATTACKS);
						if ( playerhit >= 0 )
						{
							if ( !achievementStatusRhythmOfTheKnight[playerhit] )
							{
								achievementRhythmOfTheKnightVec[playerhit].clear();
							}
							if ( !achievementStatusThankTheTank[playerhit] )
							{
								achievementThankTheTankPair[playerhit] = std::make_pair(0, 0);
							}
							if ( behavior == &actMonster )
							{
								updateAchievementBaitAndSwitch(playerhit, false);
							}
							//messagePlayer(0, "took damage!");
							if ( paralyzeStatusInflicted )
							{
								messagePlayerMonsterEvent(playerhit, color, *myStats, Language::get(3208), Language::get(3207), MSG_COMBAT);
							}
							else if ( slowStatusInflicted )
							{
								messagePlayerMonsterEvent(playerhit, color, *myStats, Language::get(395), Language::get(395), MSG_COMBAT);
							}
							else if ( swordExtraDamageInflicted )
							{
								messagePlayerMonsterEvent(playerhit, color, *myStats, Language::get(3213), Language::get(3212), MSG_COMBAT);
							}
							else if ( knockbackInflicted )
							{
								messagePlayerMonsterEvent(playerhit, color, *myStats, Language::get(3216), Language::get(3216), MSG_COMBAT);
							}
						}
					}
					else
					{
						// display 'blow bounces off' message
						if ( !statusInflicted )
						{
							messagePlayerMonsterEvent(playerhit, 0xFFFFFFFF, *myStats, Language::get(2457), Language::get(2458), MSG_COMBAT_BASIC);
						}
						if ( myStats->type == COCKATRICE && hitstats->defending )
						{
							steamAchievementClient(playerhit, "BARONY_ACH_COCK_BLOCK");
						}
						else if ( myStats->type == MINOTAUR && !hitstats->defending )
						{
							steamAchievementClient(playerhit, "BARONY_ACH_ONE_WHO_KNOCKS");
						}
						if ( playerhit >= 0 )
						{
							if ( hitstats->defending )
							{
								updateAchievementRhythmOfTheKnight(playerhit, this, true);
								updateAchievementThankTheTank(playerhit, this, false);
							}
							else if ( !achievementStatusRhythmOfTheKnight[playerhit] )
							{
								achievementRhythmOfTheKnightVec[playerhit].clear();
								//messagePlayer(0, "used AC!");
							}
						}
					}

					if ( !strncmp(hitstats->name, "inner demon", strlen("inner demon")) )
					{
						hit.entity->modHP(damage); // undo melee damage.
					}

					DamageGib dmgGib = DMG_DEFAULT;
					bool charged = std::max(charge, MAXCHARGE / 2) / ((double)(MAXCHARGE / 2)) > 1;
					if ( weaponMultipliers >= 1.15 )
					{
						dmgGib = DMG_STRONGER;
						if ( charged )
						{
							dmgGib = DMG_STRONGEST;
						}
					}
					else if ( charged )
					{
						dmgGib = DMG_STRONGER;
					}
					else if ( weaponMultipliers <= 0.85 )
					{
						dmgGib = DMG_WEAKER;
					}
					// update enemy bar for attacker
					if ( !strcmp(hitstats->name, "") )
					{
						updateEnemyBar(this, hit.entity, getMonsterLocalizedName(hitstats->type).c_str(), hitstats->HP, hitstats->MAXHP, false,
							dmgGib);
					}
					else
					{
						updateEnemyBar(this, hit.entity, hitstats->name, hitstats->HP, hitstats->MAXHP, false,
							dmgGib);
					}

					if ( hitstats->type == INCUBUS
						&& !strncmp(hitstats->name, "inner demon", strlen("inner demon")) )
					{
						// conjuration deals damage back to attacker.
						Entity* illusionParent = uidToEntity(hit.entity->parent);
                        Creature* illusionParentCrtr = (Creature*)(*&illusionParent);
						this->modHP(-(std::max(2, damage / 2)) );
						playSoundEntity(this, 173, 64);
						if ( illusionParent )
						{
							if ( myStats->HP <= 0 )
							{
								illusionParent->awardXP(this, true, true);
								if ( illusionParentCrtr && illusionParentCrtr->behavior == &actPlayer )
								{
									steamStatisticUpdateClient(illusionParent->skill[2], STEAM_STAT_SELF_FLAGELLATION, STEAM_STAT_INT, 1);
								}
							}
							if ( player > 0 && multiplayer == SERVER && !players[player]->isLocalPlayer() )
							{
								strcpy((char*)net_packet->data, "SHAK");
								net_packet->data[4] = 10; // turns into .1
								net_packet->data[5] = 10;
								net_packet->address.host = net_clients[player - 1].host;
								net_packet->address.port = net_clients[player - 1].port;
								net_packet->len = 6;
								sendPacketSafe(net_sock, -1, net_packet, player - 1);
							}
							else if ( player >= 0 && players[player]->isLocalPlayer() )
							{
								cameravars[player].shakex += 0.1;
								cameravars[player].shakey += 10;
							}

							spawnMagicEffectParticles(this->x, this->y, this->z, 983);
							if ( illusionParentCrtr && illusionParentCrtr->behavior == &actPlayer && illusionParent != this )
							{
								// update enemy bar for attacker
								if ( !strcmp(myStats->name, "") )
								{
									updateEnemyBar(illusionParent, this, getMonsterLocalizedName(myStats->type).c_str(), myStats->HP, myStats->MAXHP, false,
										DamageGib::DMG_DEFAULT);
								}
								else
								{
									updateEnemyBar(illusionParent, this, myStats->name, myStats->HP, myStats->MAXHP, false,
										DamageGib::DMG_DEFAULT);
								}
							}
						}
					}

					if ( !disarmed )
					{
						if ( whip )
						{
							playSoundEntity(hit.entity, 407 + local_rng.rand() % 3, 64);
						}
						else
						{
							playSoundEntity(hit.entity, 28, 64);
						}
					}

					// chance of bleeding
					bool wasBleeding = hitstats->EFFECTS[EFF_BLEEDING]; // check if currently bleeding when this roll occurred.
					if ( gibtype[(int)hitstats->type] > 0 && gibtype[(int)hitstats->type] != 5 )
					{
						if ( bleedStatusInflicted || (hitstats->HP > 5 && damage > 0) )
						{
							if ( bleedStatusInflicted || (local_rng.rand() % 20 == 0 && (weaponskill > PRO_SWORD && weaponskill <= PRO_POLEARM) )
								|| (local_rng.rand() % 10 == 0 && weaponskill == PRO_SWORD)
								|| (whip && ( (flanking && local_rng.rand() % 5 == 0) || (backstab && local_rng.rand() % 2 == 0) || disarmed) )
								|| (local_rng.rand() % 4 == 0 && pose == MONSTER_POSE_GOLEM_SMASH)
								|| (local_rng.rand() % 4 == 0 && pose == PLAYER_POSE_GOLEM_SMASH)
								|| (local_rng.rand() % 10 == 0 && myStats->type == VAMPIRE && myStats->weapon == nullptr)
								|| (local_rng.rand() % 8 == 0 && myStats->EFFECTS[EFF_VAMPIRICAURA] && (myStats->weapon == nullptr || myStats->type == LICH_FIRE))
							)
							{
								bool heavyBleedEffect = false; // heavy bleed will have a greater starting duration, and add to existing duration.
								if ( pose == MONSTER_POSE_GOLEM_SMASH )
								{
									heavyBleedEffect = true;
								}
								else if ( bleedStatusInflicted )
								{
									heavyBleedEffect = false;
								}
								else if ( (myStats->type == VAMPIRE && this->behavior == &actMonster) || myStats->EFFECTS[EFF_VAMPIRICAURA] )
								{
									if ( local_rng.rand() % 2 == 0 ) // 50% for heavy bleed effect.
									{
										heavyBleedEffect = true;
									}
								}

								char playerHitMessage[1024] = "";
								char monsterHitMessage[1024] = "";

								if ( (!wasBleeding && !heavyBleedEffect) || bleedStatusInflicted )
								{
									// normal bleed effect
									if ( bleedStatusInflicted ) // from sword capstone
									{
										// 5 seconds bleeding minimum
										hitstats->EFFECTS_TIMERS[EFF_BLEEDING] = std::max(hitstats->EFFECTS_TIMERS[EFF_BLEEDING], 250);
									}
									else if ( myStats->weapon && myStats->weapon->type == TOOL_WHIP )
									{
										// 5 seconds bleeding minimum
										hitstats->EFFECTS_TIMERS[EFF_BLEEDING] = std::max(hitstats->EFFECTS_TIMERS[EFF_BLEEDING], 250);
										spawnMagicEffectParticles(hit.entity->x, hit.entity->y, hit.entity->z, 643);
										for ( int gibs = 0; gibs < 5; ++gibs )
										{
											Entity* gib = spawnGib(hit.entity);
											serverSpawnGibForClient(gib);
										}
									}
									else
									{
										hitstats->EFFECTS_TIMERS[EFF_BLEEDING] = std::max(480 + (int)local_rng.rand() % 360 - hit.entity->getCON() * 100, 120); // 2.4-16.8 seconds
									}
									hitstats->EFFECTS[EFF_BLEEDING] = true;
									strcpy(playerHitMessage, Language::get(701));
									if ( !strcmp(hitstats->name, "") )
									{
										strcpy(monsterHitMessage, Language::get(702));
									}
									else
									{
										strcpy(monsterHitMessage, Language::get(703));
									}
								}
								else if ( heavyBleedEffect )
								{
									if ( !wasBleeding )
									{
										hitstats->EFFECTS_TIMERS[EFF_BLEEDING] = std::max(500 + (int)local_rng.rand() % 500 - hit.entity->getCON() * 10, 250); // 5-20 seconds
										hitstats->EFFECTS[EFF_BLEEDING] = true;
										strcpy(playerHitMessage, Language::get(2451));
										if ( !strcmp(hitstats->name, "") )
										{
											strcpy(monsterHitMessage, Language::get(2452));
										}
										else
										{
											strcpy(monsterHitMessage, Language::get(2453));
										}
									}
									else
									{
										hitstats->EFFECTS_TIMERS[EFF_BLEEDING] += std::max((int)local_rng.rand() % 350 - hit.entity->getCON() * 5, 100); // 2-7 seconds in addition
										hitstats->EFFECTS[EFF_BLEEDING] = true;
										strcpy(playerHitMessage, Language::get(2454));
										if ( !strcmp(hitstats->name, "") )
										{
											strcpy(monsterHitMessage, Language::get(2455));
										}
										else
										{
											strcpy(monsterHitMessage, Language::get(2456));
										}
									}
								}

								// message player of effect, skip if hit entity was already bleeding.
								if ( hitstats->EFFECTS[EFF_BLEEDING] && (!wasBleeding || heavyBleedEffect) )
								{
									hitstats->bleedInflictedBy = static_cast<int32_t>(this->getUID());
									if ( heavyBleedEffect )
									{
										hitstats->EFFECTS[EFF_SLOW] = true;
										hitstats->EFFECTS_TIMERS[EFF_SLOW] = 60;
									}
									if ( hitCrtr && hitCrtr->behavior == &actPlayer && multiplayer == SERVER )
									{
										serverUpdateEffects(hit.entity->skill[2]);
									}

									if ( playerhit >= 0 )
									{
										uint32_t color = makeColorRGB(255, 0, 0);
										messagePlayerColor(playerhit, MESSAGE_STATUS, color, playerHitMessage);
									}
									else
									{
										uint32_t color = makeColorRGB(0, 255, 0);
										if ( !strcmp(hitstats->name, "") )
										{
											messagePlayerColor(player, MESSAGE_COMBAT, color, monsterHitMessage, getMonsterLocalizedName(hitstats->type).c_str());
										}
										else
										{
											messagePlayerColor(player, MESSAGE_COMBAT, color, monsterHitMessage, hitstats->name);
										}
									}

									// energize if wearing punisher hood!
									if ( myStats->helmet && myStats->helmet->type == PUNISHER_HOOD )
									{
										this->modMP(1 + local_rng.rand() % 2);
										uint32_t color = makeColorRGB(0, 255, 0);
										this->setEffect(EFF_MP_REGEN, true, 250, true);
										if ( behavior == &actPlayer )
										{
											messagePlayerColor(player, MESSAGE_HINT, color, Language::get(3753));
											steamStatisticUpdateClient(player, STEAM_STAT_ITS_A_LIVING, STEAM_STAT_INT, 1);
										}
										playSoundEntity(this, 168, 128);
									}
								}
							}
						}
					}
					// apply AoE attack
					list_t * aoeTargets = nullptr;
					list_t * shakeTargets = nullptr;
					Entity* tmpEntity = nullptr;
					if ( pose == MONSTER_POSE_GOLEM_SMASH && target == nullptr )
					{
						getTargetsAroundEntity(this, hit.entity, STRIKERANGE, PI / 3, MONSTER_TARGET_ENEMY, &aoeTargets);
						if ( aoeTargets )
						{
							for ( node = aoeTargets->first; node != NULL; node = node->next )
							{
								tmpEntity = (Entity*)node->element;
								if ( tmpEntity != nullptr )
								{
									this->attack(MONSTER_POSE_GOLEM_SMASH, charge, tmpEntity);
								}
							}
							//Free the list.
							list_FreeAll(aoeTargets);
							free(aoeTargets);
						}
						getTargetsAroundEntity(this, hit.entity, STRIKERANGE, PI, MONSTER_TARGET_PLAYER, &shakeTargets);
						if ( shakeTargets )
						{
							// shake nearby players that were not the primary target.
							for ( node = shakeTargets->first; node != NULL; node = node->next )
							{
								tmpEntity = (Entity*)node->element;
								playerhit = tmpEntity->skill[2];
								if ( playerhit > 0 && multiplayer == SERVER && !players[playerhit]->isLocalPlayer() )
								{
									strcpy((char*)net_packet->data, "SHAK");
									net_packet->data[4] = 10; // turns into .1
									net_packet->data[5] = 10;
									net_packet->address.host = net_clients[playerhit - 1].host;
									net_packet->address.port = net_clients[playerhit - 1].port;
									net_packet->len = 6;
									sendPacketSafe(net_sock, -1, net_packet, playerhit - 1);
								}
								else if ( playerhit >= 0 && players[playerhit]->isLocalPlayer() )
								{
									cameravars[playerhit].shakex += 0.1;
									cameravars[playerhit].shakey += 10;
								}
							}
							//Free the list.
							list_FreeAll(shakeTargets);
							free(shakeTargets);
						}
					}
					else if ( pose == MONSTER_POSE_AUTOMATON_MALFUNCTION )
					{
						getTargetsAroundEntity(this, this, 24, PI, MONSTER_TARGET_ALL, &aoeTargets);
						if ( aoeTargets )
						{
							for ( node = aoeTargets->first; node != NULL; node = node->next )
							{
								tmpEntity = (Entity*)node->element;
								if ( tmpEntity != nullptr )
								{
									spawnExplosion(tmpEntity->x, tmpEntity->y, tmpEntity->z);
									Stat* tmpStats = tmpEntity->getStats();
									if ( tmpStats )
									{
										int explodeDmg = (40 + myStats->HP) * Creature::getDamageTableMultiplier(tmpEntity, *tmpStats, DAMAGE_TABLE_MAGIC); // check base magic damage resist.
										Entity* gib = spawnGib(tmpEntity);
										serverSpawnGibForClient(gib);
										playerhit = tmpEntity->skill[2];
										if ( playerhit > 0 && multiplayer == SERVER && !players[playerhit]->isLocalPlayer() )
										{
											strcpy((char*)net_packet->data, "SHAK");
											net_packet->data[4] = 20; // turns into .1
											net_packet->data[5] = 20;
											net_packet->address.host = net_clients[playerhit - 1].host;
											net_packet->address.port = net_clients[playerhit - 1].port;
											net_packet->len = 6;
											sendPacketSafe(net_sock, -1, net_packet, playerhit - 1);
										}
										else if ( playerhit >= 0 && players[playerhit]->isLocalPlayer() )
										{
											cameravars[playerhit].shakex += 0.2;
											cameravars[playerhit].shakey += 20;
										}
										tmpEntity->modHP(-explodeDmg);
										if ( playerhit >= 0 )
										{
											uint32_t color = makeColorRGB(255, 0, 0);
											messagePlayerColor(playerhit, MESSAGE_STATUS, color, Language::get(2523));
										}
									}
								}
							}
							//Free the list.
							list_FreeAll(aoeTargets);
							free(aoeTargets);
						}
					}
					// lifesteal
					bool tryLifesteal = false;
					bool forceLifesteal = false;
					int lifeStealAmount = damage;
					if ( damage > 0 )
					{
						if ( behavior == &actPlayer )
						{
							if ( myStats->weapon == nullptr || shapeshifted )
							{
								if ( myStats->EFFECTS_TIMERS[EFF_VAMPIRICAURA] == -2 )
								{
									if ( backstab || flanking )
									{
										if ( hitstats->HP <= 0 )
										{
											forceLifesteal = true;
										}
									}
								}
								else if ( myStats->EFFECTS[EFF_VAMPIRICAURA] && myStats->EFFECTS_TIMERS[EFF_VAMPIRICAURA] > 0 )
								{
									tryLifesteal = true;
									if ( backstab || flanking )
									{
										if ( hitstats->HP <= 0 )
										{
											forceLifesteal = true;
										}
									}
								}
								lifeStealAmount = std::max(0, hitstats->OLDHP - hitstats->HP);
								lifeStealAmount /= 4;
								lifeStealAmount = std::max(3, lifeStealAmount);
							}
						}
						else if ( (myStats->EFFECTS[EFF_VAMPIRICAURA] && (myStats->weapon == nullptr || myStats->type == LICH_FIRE)) )
						{
							tryLifesteal = true;
						}
						else if ( myStats->type == VAMPIRE && behavior == &actMonster )
						{
							tryLifesteal = true;
						}

						// special strike spell animation
						if ( pose == PLAYER_POSE_GOLEM_SMASH )
						{
							spawnMagicEffectParticles(hit.entity->x, hit.entity->y, hit.entity->z, 643);
							for ( int gibs = 0; gibs < 10; ++gibs )
							{
								Entity* gib = spawnGib(hit.entity);
								serverSpawnGibForClient(gib);
							}
							playSoundEntity(hit.entity, 181, 128);
						}
					}

					if ( tryLifesteal || forceLifesteal )
					{
						bool lifestealSuccess = false;
						if ( forceLifesteal )
						{
							this->modHP(lifeStealAmount);
							spawnMagicEffectParticles(x, y, z, 169);
							playSoundEntity(this, 168, 128);
							lifestealSuccess = true;
						}
						else if ( !wasBleeding && hitstats->EFFECTS[EFF_BLEEDING] )
						{
							// attack caused the target to bleed, trigger lifesteal tick
							this->modHP(lifeStealAmount);
							spawnMagicEffectParticles(x, y, z, 169);
							playSoundEntity(this, 168, 128);
							lifestealSuccess = true;
						}
						else if ( (local_rng.rand() % 4 == 0) && (myStats->type == VAMPIRE && behavior == &actMonster && myStats->EFFECTS[EFF_VAMPIRICAURA]) )
						{
							// vampires under aura have higher chance.
							this->modHP(lifeStealAmount);
							spawnMagicEffectParticles(x, y, z, 169);
							playSoundEntity(this, 168, 128);
							lifestealSuccess = true;
						}
						else if ( local_rng.rand() % 8 == 0 )
						{
							// else low chance for lifesteal.
							this->modHP(lifeStealAmount);
							spawnMagicEffectParticles(x, y, z, 169);
							playSoundEntity(this, 168, 128);
							lifestealSuccess = true;
						}

						if ( lifestealSuccess )
						{
							if ( player >= 0 )
							{
								myStats->HUNGER = std::min(1499, myStats->HUNGER + 100);
								serverUpdateHunger(player);
								if ( stats[player]->type == VAMPIRE )
								{
									steamStatisticUpdateClient(player, STEAM_STAT_BAD_BLOOD, STEAM_STAT_INT, lifeStealAmount);
								}
							}
							if ( playerhit >= 0 )
							{
								uint32_t color = makeColorRGB(255, 0, 0);
								messagePlayerColor(playerhit, MESSAGE_STATUS, color, Language::get(2441));
							}
							else
							{
								uint32_t color = makeColorRGB(0, 255, 0);
								if ( !strcmp(hitstats->name, "") )
								{
									messagePlayerColor(player, MESSAGE_COMBAT, color, Language::get(2440), getMonsterLocalizedName(hitstats->type).c_str());
								}
								else
								{
									messagePlayerColor(player, MESSAGE_COMBAT, color, Language::get(2439), hitstats->name);
								}
							}
						}
					}
					// vampire blood drops.
					bool tryBloodVial = false;
					if ( hitstats->HP <= 0 && hitCrtr && hitCrtr->behavior == &actMonster
						&& (gibtype[hitstats->type] == 1 || gibtype[hitstats->type] == 2) )
					{
						for ( c = 0; c < MAXPLAYERS; ++c )
						{
							if ( playerRequiresBloodToSustain(c) )
							{
								tryBloodVial = true;
								break;
							}
						}
					}
					if ( tryBloodVial )
					{
						bool spawnBloodVial = false;
						bool spawnSecondVial = false;
						if ( (backstab || flanking) && hitstats->HP <= 0 )
						{
							spawnBloodVial = true;
						}
						else if ( hitstats->EFFECTS[EFF_BLEEDING] || myStats->EFFECTS[EFF_VAMPIRICAURA] )
						{
							if ( hitstats->EFFECTS_TIMERS[EFF_BLEEDING] >= 250 )
							{
								spawnBloodVial = (local_rng.rand() % 2 == 0);
							}
							else if ( hitstats->EFFECTS_TIMERS[EFF_BLEEDING] >= 150 )
							{
								spawnBloodVial = (local_rng.rand() % 4 == 0);
							}
							else
							{
								spawnBloodVial = (local_rng.rand() % 8 == 0);
							}

							if ( local_rng.rand() % 5 == 0 )
							{
								spawnSecondVial = true;
							}
						}
						else
						{
							spawnBloodVial = (local_rng.rand() % 10 == 0);
						}

						if ( spawnBloodVial )
						{
							Item* blood = newItem(FOOD_BLOOD, EXCELLENT, 0, 1, gibtype[hitstats->type] - 1, true, &hitstats->inventory);
							if ( spawnSecondVial )
							{
								blood = newItem(FOOD_BLOOD, EXCELLENT, 0, 1, gibtype[hitstats->type] - 1, true, &hitstats->inventory);
							}
						}
					}
				}
			}
		}
		else
		{
			if ( (dist != STRIKERANGE && !whip) || (dist != STRIKERANGE * 1.5 && whip) )
			{
				// hit a wall
				if ( pose == PLAYER_POSE_GOLEM_SMASH )
				{
					if ( hit.mapx >= 1 && hit.mapx < map.width - 1 && hit.mapy >= 1 && hit.mapy < map.height - 1 )
					{
						if ( magicDig(this, nullptr, 0, 0) )
						{
							playSoundPos(hit.x, hit.y, 67, 128); // bust wall
							if ( player >= 0 && myStats->type == TROLL )
							{
								serverUpdatePlayerGameplayStats(player, STATISTICS_FORUM_TROLL, AchievementObserver::FORUM_TROLL_BREAK_WALL);
							}
							for ( int c = 0; c < 5; c++ )
							{
								Entity* entity = newEntity(78, 1, map.entities, nullptr); //Particle entity.
								entity->sizex = 1;
								entity->sizey = 1;
								entity->x = hit.x + (-4 + local_rng.rand() % 9);
								entity->y = hit.y + (-4 + local_rng.rand() % 9);
								entity->z = 7.5;
								entity->yaw = c * 2 * PI / 5;//(local_rng.rand() % 360) * PI / 180.0;
								entity->roll = (local_rng.rand() % 360) * PI / 180.0;

								entity->vel_x = 0.2 * cos(entity->yaw);
								entity->vel_y = 0.2 * sin(entity->yaw);
								entity->vel_z = 3;// 0.25 - (local_rng.rand() % 5) / 10.0;

								entity->skill[0] = 50; // particle life
								entity->skill[1] = 0; // particle direction, 0 = upwards, 1 = downwards.

								entity->behavior = &actParticleRock;
								entity->flags[PASSABLE] = true;
								entity->flags[NOUPDATE] = true;
								entity->flags[UNCLICKABLE] = true;
								if ( multiplayer != CLIENT )
								{
									entity_uids--;
								}
								entity->setUID(-3);
							}
						}
					}
					else
					{
						messagePlayer(player, MESSAGE_HINT, Language::get(706));
					}
				}
				else if ( myStats->weapon != NULL && !shapeshifted )
				{
					if ( myStats->weapon->type == TOOL_PICKAXE )
					{
						if ( hit.mapx >= 1 && hit.mapx < map.width - 1 && hit.mapy >= 1 && hit.mapy < map.height - 1 )
						{
							bool degradePickaxe = true;
							if ( MFLAG_DISABLEDIGGING )
							{
								uint32_t color = makeColorRGB(255, 0, 255);
								if ( this->behavior == &actPlayer )
								{
									messagePlayerColor(this->skill[2], MESSAGE_HINT, color, Language::get(2380)); // disabled digging.
								}
								playSoundPos(hit.x, hit.y, 66, 128); // strike wall
								// bang
								spawnBang(hit.x - cos(yaw) * 2, hit.y - sin(yaw) * 2, 0);
							}
							else if ( swimmingtiles[map.tiles[OBSTACLELAYER + hit.mapy * MAPLAYERS + hit.mapx * MAPLAYERS * map.height]]
								|| lavatiles[map.tiles[OBSTACLELAYER + hit.mapy * MAPLAYERS + hit.mapx * MAPLAYERS * map.height]] )
							{
								// no effect for lava/water tiles.
								degradePickaxe = false;
							}
							else if ( !mapTileDiggable(hit.mapx, hit.mapy) )
							{
								spawnBang(hit.x - cos(yaw) * 2, hit.y - sin(yaw) * 2, 0);
								messagePlayer(player, MESSAGE_HINT, Language::get(706));
							}
							else
							{
								playSoundPos(hit.x, hit.y, 67, 128); // bust wall
								// spawn several rock items
								i = 8 + local_rng.rand() % 4;
								for ( c = 0; c < i; c++ )
								{
									Entity* entity = newEntity(-1, 1, map.entities, nullptr); //Rock/item entity.
									entity->flags[INVISIBLE] = true;
									entity->flags[UPDATENEEDED] = true;
									entity->x = hit.mapx * 16 + 4 + local_rng.rand() % 8;
									entity->y = hit.mapy * 16 + 4 + local_rng.rand() % 8;
									entity->z = -6 + local_rng.rand() % 12;
									entity->sizex = 4;
									entity->sizey = 4;
									entity->yaw = local_rng.rand() % 360 * PI / 180;
									entity->vel_x = (local_rng.rand() % 20 - 10) / 10.0;
									entity->vel_y = (local_rng.rand() % 20 - 10) / 10.0;
									entity->vel_z = -.25 - (local_rng.rand() % 5) / 10.0;
									entity->flags[PASSABLE] = true;
									entity->behavior = &actItem;
									entity->flags[USERFLAG1] = true; // no collision: helps performance
									entity->skill[10] = GEM_ROCK;    // type
									entity->skill[11] = WORN;        // status
									entity->skill[12] = 0;           // beatitude
									entity->skill[13] = 1;           // count
									entity->skill[14] = 0;           // appearance
									entity->skill[15] = 1;			 // identified
								}

								if ( map.tiles[OBSTACLELAYER + hit.mapy * MAPLAYERS + hit.mapx * MAPLAYERS * map.height] >= 41
									&& map.tiles[OBSTACLELAYER + hit.mapy * MAPLAYERS + hit.mapx * MAPLAYERS * map.height] <= 49 )
								{
									steamAchievementClient(player, "BARONY_ACH_BAD_REVIEW");
								}

								map.tiles[OBSTACLELAYER + hit.mapy * MAPLAYERS + hit.mapx * MAPLAYERS * map.height] = 0;
								// send wall destroy info to clients
								if ( multiplayer == SERVER )
								{
									for ( c = 1; c < MAXPLAYERS; c++ )
									{
										if ( players[c]->isLocalPlayer() || client_disconnected[c] == true )
										{
											continue;
										}
										strcpy((char*)net_packet->data, "WALD");
										SDLNet_Write16((uint16_t)hit.mapx, &net_packet->data[4]);
										SDLNet_Write16((uint16_t)hit.mapy, &net_packet->data[6]);
										net_packet->address.host = net_clients[c - 1].host;
										net_packet->address.port = net_clients[c - 1].port;
										net_packet->len = 8;
										sendPacketSafe(net_sock, -1, net_packet, c - 1);
									}
								}
								// Update the paths so that monsters know they can walk through it
								generatePathMaps();
							}
							int chance = 2 + (myStats->type == GOBLIN ? 2 : 0);
							if ( local_rng.rand() % chance && degradePickaxe )
							{
								myStats->weapon->status = static_cast<Status>(myStats->weapon->status - 1);
								if ( myStats->weapon->status == BROKEN )
								{
									messagePlayer(player, MESSAGE_EQUIPMENT, Language::get(704));
									playSoundEntity(this, 76, 64);
								}
								else
								{
									messagePlayer(player, MESSAGE_EQUIPMENT, Language::get(705));
								}
								if ( player > 0 && multiplayer == SERVER && !players[player]->isLocalPlayer() )
								{
									strcpy((char*)net_packet->data, "ARMR");
									net_packet->data[4] = 5;
									net_packet->data[5] = myStats->weapon->status;
									net_packet->address.host = net_clients[player - 1].host;
									net_packet->address.port = net_clients[player - 1].port;
									net_packet->len = 6;
									sendPacketSafe(net_sock, -1, net_packet, player - 1);
								}
							}
						}
						else
						{
							spawnBang(hit.x - cos(yaw) * 2, hit.y - sin(yaw) * 2, 0);
							messagePlayer(player, MESSAGE_HINT, Language::get(706));
						}
					}
					else
					{
						// bang
						spawnBang(hit.x - cos(yaw) * 2, hit.y - sin(yaw) * 2, 0);
					}
				}
				else
				{
					// bang
					//spawnBang(hit.x - cos(my->yaw)*2,hit.y - sin(my->yaw)*2,0);
					playSoundPos(hit.x, hit.y, 183, 64);
				}
			}

			// apply AoE shake effect
			if ( (pose == MONSTER_POSE_GOLEM_SMASH || pose == PLAYER_POSE_GOLEM_SMASH) && target == nullptr )
			{
				list_t * shakeTargets = nullptr;
				Entity* tmpEntity = nullptr;
				getTargetsAroundEntity(this, hit.entity, STRIKERANGE, PI, MONSTER_TARGET_PLAYER, &shakeTargets);
				if ( shakeTargets )
				{
					// shake nearby players that were not the primary target.
					for ( node = shakeTargets->first; node != NULL; node = node->next )
					{
						tmpEntity = (Entity*)node->element;
						playerhit = tmpEntity->skill[2];
						if ( playerhit > 0 && multiplayer == SERVER && !players[playerhit]->isLocalPlayer() )
						{
							strcpy((char*)net_packet->data, "SHAK");
							net_packet->data[4] = 10; // turns into .1
							net_packet->data[5] = 10;
							net_packet->address.host = net_clients[playerhit - 1].host;
							net_packet->address.port = net_clients[playerhit - 1].port;
							net_packet->len = 6;
							sendPacketSafe(net_sock, -1, net_packet, playerhit - 1);
						}
						else if ( playerhit >= 0 && players[playerhit]->isLocalPlayer() )
						{
							cameravars[playerhit].shakex += .1;
							cameravars[playerhit].shakey += 10;
						}
					}
					//Free the list.
					list_FreeAll(shakeTargets);
					free(shakeTargets);
				}
			}
		}
	}
	else
	{
		if ( player == -1 )
		{
			return;    // clients are NOT supposed to invoke monster attacks in the gamestate!
		}
		strcpy((char*)net_packet->data, "ATAK");
		net_packet->data[4] = player;
		net_packet->data[5] = pose;
		net_packet->data[6] = charge;
		net_packet->address.host = net_server.host;
		net_packet->address.port = net_server.port;
		net_packet->len = 7;
		sendPacketSafe(net_sock, -1, net_packet, 0);
	}
}

void Creature::playerLevelEntrySpeechSecond()
{
	int timeDiff = playerAliveTime - 300;
	int orangeSpeechVolume = 128;
	int blueSpeechVolume = 112;
	if ( timeDiff > 0 && playerLevelEntrySpeech > 0 && !secretlevel )
	{
		switch ( currentlevel )
		{
			case 26:
				switch ( playerLevelEntrySpeech )
				{
					case 1:
						if ( timeDiff == 200 )
						{
							playSound(342, orangeSpeechVolume);
							messageLocalPlayersColor(uint32ColorOrange, MESSAGE_WORLD, Language::get(2616));
							playerLevelEntrySpeech = 0;
						}
						break;
					case 2:
						if ( timeDiff == 200 )
						{
							playSound(344, blueSpeechVolume);
							messageLocalPlayersColor(uint32ColorBaronyBlue, MESSAGE_WORLD, Language::get(2618));
						}
						else if ( timeDiff == 350 )
						{
							playSound(345, orangeSpeechVolume);
							messageLocalPlayersColor(uint32ColorOrange, MESSAGE_WORLD, Language::get(2619));
							playerLevelEntrySpeech = 0;
						}
						break;
					case 3:
						if ( timeDiff == 200 )
						{
							playSound(347, blueSpeechVolume);
							messageLocalPlayersColor(uint32ColorBaronyBlue, MESSAGE_WORLD, Language::get(2621));
						}
						else if ( timeDiff == 350 )
						{
							playSound(348, orangeSpeechVolume);
							messageLocalPlayersColor(uint32ColorOrange, MESSAGE_WORLD, Language::get(2622));
							playerLevelEntrySpeech = 0;
						}
						break;
					default:
						break;
				}
				break;
			case 28:
				switch ( playerLevelEntrySpeech )
				{
					case 1:
						if ( timeDiff == 200 )
						{
							playSound(350, orangeSpeechVolume);
							messageLocalPlayersColor(uint32ColorOrange, MESSAGE_WORLD, Language::get(2630));
						}
						else if ( timeDiff == 350 )
						{
							playSound(351, blueSpeechVolume);
							messageLocalPlayersColor(uint32ColorBaronyBlue, MESSAGE_WORLD, Language::get(2631));
							playerLevelEntrySpeech = 0;
						}
						break;
					case 2:
						if ( timeDiff == 350 )
						{
							playSound(353, blueSpeechVolume);
							messageLocalPlayersColor(uint32ColorBaronyBlue, MESSAGE_WORLD, Language::get(2633));
							playerLevelEntrySpeech = 0;
						}
						break;
					case 3:
						if ( timeDiff == 200 )
						{
							playSound(355, orangeSpeechVolume);
							messageLocalPlayersColor(uint32ColorOrange, MESSAGE_WORLD, Language::get(2635));
							playerLevelEntrySpeech = 0;
						}
						break;
					default:
						break;
				}
				break;
			case 30:
				switch ( playerLevelEntrySpeech )
				{
					case 1:
						if ( timeDiff == 350 )
						{
							playSound(357, orangeSpeechVolume);
							messageLocalPlayersColor(uint32ColorOrange, MESSAGE_WORLD, Language::get(2637));
						}
						else if ( timeDiff == 500 )
						{
							messageLocalPlayersColor(uint32ColorOrange, MESSAGE_WORLD, Language::get(2652));
							playerLevelEntrySpeech = 0;
						}
						break;
					default:
						break;
				}
				break;
			case 31:
				switch ( playerLevelEntrySpeech )
				{
					case 1:
						if ( timeDiff == 350 )
						{
							playSound(359, orangeSpeechVolume);
							messageLocalPlayersColor(uint32ColorOrange, MESSAGE_WORLD, Language::get(2639));
						}
						else if ( timeDiff == 510 )
						{
							messageLocalPlayersColor(uint32ColorOrange, MESSAGE_WORLD, Language::get(2653));
							playerLevelEntrySpeech = 0;
						}
						break;
					default:
						break;
				}
				break;
			case 33:
				switch ( playerLevelEntrySpeech )
				{
					case 1:
						if ( timeDiff == 200 )
						{
							playSound(361, blueSpeechVolume);
							messageLocalPlayersColor(uint32ColorBaronyBlue, MESSAGE_WORLD, Language::get(2641));
							playerLevelEntrySpeech = 0;
						}
						break;
					case 2:
						if ( timeDiff == 350 )
						{
							playSound(363, orangeSpeechVolume);
							messageLocalPlayersColor(uint32ColorOrange, MESSAGE_WORLD, Language::get(2643));
							playerLevelEntrySpeech = 0;
						}
						break;
					default:
						break;
				}
				break;
			case 35:
				switch ( playerLevelEntrySpeech )
				{
					case 1:
						if ( timeDiff == 310 )
						{
							playSound(365, blueSpeechVolume);
							messageLocalPlayersColor(uint32ColorBaronyBlue, MESSAGE_WORLD, Language::get(2645));
							playerLevelEntrySpeech = 0;
						}
						break;
					default:
						break;
				}
				break;
			case 27:
			case 29:
			case 32:
			case 34:
				if ( minotaurlevel )
				{
					switch ( playerLevelEntrySpeech )
					{
						case 1:
							if ( timeDiff == 200 )
							{
								playSound(367, orangeSpeechVolume);
								messageLocalPlayersColor(uint32ColorOrange, MESSAGE_WORLD, Language::get(2624));
								playerLevelEntrySpeech = 0;
							}
							break;
						case 2:
							if ( timeDiff == 200 )
							{
								playSound(369, blueSpeechVolume);
								messageLocalPlayersColor(uint32ColorBaronyBlue, MESSAGE_WORLD, Language::get(2626));
								playerLevelEntrySpeech = 0;
							}
							break;
						case 3:
							if ( timeDiff == 200 )
							{
								playSound(371, orangeSpeechVolume);
								messageLocalPlayersColor(uint32ColorOrange, MESSAGE_WORLD, Language::get(2628));
								playerLevelEntrySpeech = 0;
							}
							break;
						default:
							break;
					}
					break;
				}
				break;
			default:
				break;
		}

	}
}

bool Creature::isPlayerHeadSprite(const int sprite)
{
	switch ( sprite )
	{
		case 113:
		case 114:
		case 115:
		case 116:
		case 117:
		case 125:
		case 126:
		case 127:
		case 128:
		case 129:
		case 332:
		case 333:
		case 341:
		case 342:
		case 343:
		case 344:
		case 345:
		case 346:
		case 354:
		case 355:
		case 356:
		case 357:
		case 358:
		case 359:
		case 367:
		case 368:
		case 369:
		case 370:
		case 371:
		case 372:
		case 380:
		case 381:
		case 382:
		case 383:
		case 384:
		case 385:
		case 686:
		case 694:
		case 702:
		case 710:
		case 718:
		case 726:
		case 734:
		case 742:
		case 752:
		case 756:
		case 760:
		case 768:
		case 770:
		case 814:
		case 817:
		case 823:
		case 827:
		case 1001:
		case 1043:
		case 1044:
		case 1049:
			return true;
			break;
		default:
			break;
	}
	return false;
}

bool Creature::isPlayerHeadSprite() const
{
	return Creature::isPlayerHeadSprite(sprite);
}

Monster Creature::getMonsterFromPlayerRace(int playerRace)
{
    return ::getMonsterFromPlayerRace(playerRace);
}

bool Creature::handleMonsterSpecialAttack(Stat* myStats, Entity* target, double dist, bool forceDeinit)
{
	int specialRoll = 0;
	node_t * node = nullptr;
	int enemiesNearby = 0;
	int bonusFromHP = 0;
	bool hasrangedweapon = this->hasRangedWeapon();

	if ( myStats != nullptr )
	{
		if ( myStats->type == LICH
			|| myStats->type == DEVIL
			|| myStats->type == SHOPKEEPER
			|| myStats->type == LICH_FIRE
			|| myStats->type == LICH_ICE )
		{
			// monster should attack after this function is called.
			return true;
		}

		if ( this->monsterSpecialTimer == 0 && !forceDeinit )
		{
			if ( myStats->MISC_FLAGS[STAT_FLAG_MONSTER_CAST_INVENTORY_SPELLBOOKS] > 0
				&& (monsterSpecialState == MONSTER_SPELLCAST_GENERIC || monsterSpecialState == MONSTER_SPELLCAST_GENERIC2) )
			{
				monsterSpecialState = 0;
				if ( myStats->weapon && itemCategory(myStats->weapon) == SPELLBOOK )
				{
					node = itemNodeInInventory(myStats, -1, WEAPON); // find weapon to re-equip
					if ( node != nullptr )
					{
						swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
						return true;
					}
					node = itemNodeInInventory(myStats, -1, MAGICSTAFF); // find weapon to re-equip
					if ( node != nullptr )
					{
						swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
						return true;
					}
					else
					{
						monsterUnequipSlotFromCategory(myStats, &myStats->weapon, SPELLBOOK);
					}
				}
				return true;
			}

			switch ( myStats->type )
			{
				case KOBOLD:
					if ( (hasrangedweapon && !(myStats->weapon && itemCategory(myStats->weapon) == SPELLBOOK)) || myStats->weapon == nullptr )
					{
						specialRoll = local_rng.rand() % 20;
						//messagePlayer(0, "Rolled: %d", specialRoll);
						if ( myStats->HP < myStats->MAXHP / 2 )
						{
							if ( (dist < 40 && specialRoll < 10) || (dist < 100 && specialRoll < 5) ) // 50%/25% chance
							{
								node = itemNodeInInventory(myStats, -1, SPELLBOOK);
								if ( node != nullptr )
								{
									swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
									this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_KOBOLD;
								}
							}
						}
						else if ( myStats->HP < (0.8 * myStats->MAXHP) )
						{
							if ( (dist < 40 && specialRoll < 5) || (dist < 100 && specialRoll < 2) ) // 25%/10% chance
							{
								node = itemNodeInInventory(myStats, -1, SPELLBOOK);
								if ( node != nullptr )
								{
									swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
									this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_KOBOLD;
								}
							}
						}
					}
					break;
				case SUCCUBUS:
					if ( monsterSpecialState == SUCCUBUS_CHARM )
					{
						// special handled in succubusChooseWeapon()
						this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_SUCCUBUS_CHARM;
						break;
					}
					break;
				case CRYSTALGOLEM:
					specialRoll = local_rng.rand() % 20;
					enemiesNearby = numTargetsAroundEntity(this, STRIKERANGE, PI, MONSTER_TARGET_ENEMY);
					if ( enemiesNearby > 1 )
					{
						enemiesNearby = std::min(enemiesNearby, 4);
						if ( specialRoll < enemiesNearby * 2 ) // 10% for each enemy > 1, capped at 40%
						{
							this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_GOLEM;
							break;
						}
					}

					specialRoll = local_rng.rand() % 20;
					if ( myStats->HP > myStats->MAXHP * 0.8 )
					{
						if ( specialRoll < 2 ) // 10%
						{
							this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_GOLEM;
						}
					}
					else if ( myStats->HP > myStats->MAXHP * 0.6 )
					{
						if ( specialRoll < 3 ) // 15%
						{
							this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_GOLEM;
						}
					}
					else if ( myStats->HP > myStats->MAXHP * 0.4 )
					{
						if ( specialRoll < 4 ) // 20%
						{
							this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_GOLEM;
						}
					}
					else if ( myStats->HP > myStats->MAXHP * 0.2 )
					{
						if ( specialRoll < 5 ) // 25%
						{
							this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_GOLEM;
						}
					}
					else if ( myStats->HP > myStats->MAXHP * 0.2 )
					{
						if ( specialRoll < 5 ) // 25%
						{
							this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_GOLEM;
						}
					}
					break;
				case COCKATRICE:
					specialRoll = local_rng.rand() % 20;
					//specialRoll = 0;
					// check for paralyze first
					enemiesNearby = std::min(numTargetsAroundEntity(this, STRIKERANGE * 2, PI, MONSTER_TARGET_ENEMY), 4);

					if ( myStats->HP <= myStats->MAXHP * 0.5 )
					{
						bonusFromHP = 4; // +20% chance if on low health
					}
					if ( specialRoll < (enemiesNearby * 2 + bonusFromHP) ) // +10% for each enemy, capped at 40%
					{
						node = itemNodeInInventory(myStats, -1, SPELLBOOK);
						if ( node != nullptr )
						{
							swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
							this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_COCKATRICE_STONE;
						}
						break;
					}

					// nothing selected, look for double attack.
					specialRoll = local_rng.rand() % 20;
					if ( myStats->HP > myStats->MAXHP * 0.8 )
					{
						if ( specialRoll < 2 ) // 10%
						{
							this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_COCKATRICE_ATK;
						}
					}
					else if ( myStats->HP > myStats->MAXHP * 0.6 )
					{
						if ( specialRoll < 2 ) // 10%
						{
							this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_COCKATRICE_ATK;
						}
					}
					else if ( myStats->HP > myStats->MAXHP * 0.4 )
					{
						if ( specialRoll < 3 ) // 15%
						{
							this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_COCKATRICE_ATK;
						}
					}
					else if ( myStats->HP > myStats->MAXHP * 0.2 )
					{
						if ( specialRoll < 4 ) // 20%
						{
							this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_COCKATRICE_ATK;
						}
					}
					else if ( myStats->HP <= myStats->MAXHP * 0.2 )
					{
						if ( specialRoll < 5 ) // 25%
						{
							this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_COCKATRICE_ATK;
						}
					}
					break;
				case SPIDER:
					// spray web
					if ( dist < STRIKERANGE * 2 )
					{
						specialRoll = local_rng.rand() % 20;
						enemiesNearby = std::min(numTargetsAroundEntity(this, STRIKERANGE * 2, PI, MONSTER_TARGET_ENEMY), 4);
						//messagePlayer(0, "spider roll %d", specialRoll);
						if ( myStats->getAttribute("special_npc") != "" )
						{
							if ( myStats->HP <= myStats->MAXHP * 0.4 )
							{
								bonusFromHP = 20; // +100% chance
							}
							else if ( myStats->HP <= myStats->MAXHP * 1.0 )
							{
								bonusFromHP = 10; // +50% chance
							}
						}
						else
						{
							if ( myStats->HP <= myStats->MAXHP * 0.4 )
							{
								bonusFromHP = 8; // +40% chance
							}
							else if ( myStats->HP <= myStats->MAXHP * 0.8 )
							{
								bonusFromHP = 4; // +20% chance
							}
						}
						if ( specialRoll < (enemiesNearby * 2 + bonusFromHP) ) // +10% for each enemy, capped at 40%
						{
							node = itemNodeInInventory(myStats, -1, SPELLBOOK);
							if ( node != nullptr )
							{
								monsterSpecialState = SPIDER_CAST;
								swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
								this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_SPIDER_CAST;
								serverUpdateEntitySkill(this, 33); // for clients to handle animation
							}
							else
							{
								if ( myStats->weapon && itemCategory(myStats->weapon) == SPELLBOOK )
								{
									monsterSpecialState = SPIDER_CAST;
									this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_SPIDER_CAST;
									serverUpdateEntitySkill(this, 33); // for clients to handle animation
								}
							}
							break;
						}
					}
					break;
				case INSECTOID:
					if ( monsterSpecialState == INSECTOID_DOUBLETHROW_FIRST || monsterSpecialState == INSECTOID_DOUBLETHROW_SECOND )
					{
						this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_INSECTOID_THROW;
						break;
					}

					// spray acid
					if ( dist < STRIKERANGE * 2 )
					{
						specialRoll = local_rng.rand() % 20;
						enemiesNearby = std::min(numTargetsAroundEntity(this, STRIKERANGE * 2, PI, MONSTER_TARGET_ENEMY), 4);
						//messagePlayer(0, "insectoid roll %d", specialRoll);
						if ( myStats->HP <= myStats->MAXHP * 0.8 )
						{
							bonusFromHP = 4; // +20% chance if on low health
						}
						if ( specialRoll < (enemiesNearby * 2 + bonusFromHP) ) // +10% for each enemy, capped at 40%
						{
							node = itemNodeInInventory(myStats, -1, SPELLBOOK);
							if ( node != nullptr )
							{
								monsterSpecialState = INSECTOID_ACID;
								swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
								this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_INSECTOID_ACID;
								serverUpdateEntitySkill(this, 33); // for clients to handle animation
							}
							else
							{
								if ( myStats->weapon && itemCategory(myStats->weapon) == SPELLBOOK )
								{
									monsterSpecialState = INSECTOID_ACID;
									this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_INSECTOID_ACID;
									serverUpdateEntitySkill(this, 33); // for clients to handle animation
								}
							}
							break;
						}
					}
					// throwing weapon special handled in insectoidChooseWeapon()
					break;
				case INCUBUS:
					if ( monsterSpecialState == INCUBUS_CONFUSION )
					{
						// throwing weapon special handled in incubusChooseWeapon()
						this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_INCUBUS_CONFUSION;
						break;
					}
					else if ( monsterSpecialState == INCUBUS_STEAL )
					{
						// special handled in incubusChooseWeapon()
						this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_INCUBUS_STEAL;
						break;
					}
					else if ( monsterSpecialState == INCUBUS_TELEPORT )
					{
						// special handled in incubusChooseWeapon()
						this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_INCUBUS_TELEPORT_TARGET;
						break;
					}
					else if ( monsterSpecialState == INCUBUS_CHARM )
					{
						// special handled in incubusChooseWeapon()
						this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_INCUBUS_CHARM;
						break;
					}
					break;
				case VAMPIRE:
					if ( monsterSpecialState == VAMPIRE_CAST_AURA )
					{
						// special handled in vampireChooseWeapon()
						this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_VAMPIRE_AURA;
					}
					else if ( monsterSpecialState == VAMPIRE_CAST_DRAIN )
					{
						// special handled in vampireChooseWeapon()
						this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_VAMPIRE_DRAIN;
					}
					break;
				case SHADOW:
					if ( monsterSpecialState == SHADOW_SPELLCAST )
					{
						monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_SHADOW_SPELLCAST;
					}
					else if ( monsterSpecialState == SHADOW_TELEPORT_ONLY )
					{
						// special handled in shadowChooseWeapon(), teleport code in path state.
						this->monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_SHADOW_TELEPORT;
						break;
					}
				case GOATMAN:
					if ( monsterSpecialState == GOATMAN_POTION )
					{
						monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_GOATMAN_DRINK;
					}
					else if ( monsterSpecialState == GOATMAN_THROW )
					{
						monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_GOATMAN_THROW;
					}
					break;
				default:
					break;
			}

			if ( monsterSpecialTimer > 0 )
			{
				monsterSpecialAttackUnequipSafeguard = (double)TICKS_PER_SECOND * 2;
			}
		}
		else if ( this->monsterSpecialTimer > 0 || forceDeinit )
		{
			bool shouldAttack = true;
			bool deinitSuccess = false;

			if ( myStats->MISC_FLAGS[STAT_FLAG_MONSTER_CAST_INVENTORY_SPELLBOOKS] > 0 )
			{
				if ( monsterSpecialState == MONSTER_SPELLCAST_GENERIC )
				{
					monsterSpecialState = MONSTER_SPELLCAST_GENERIC2;
					return true;
				}
				else if ( monsterSpecialState == MONSTER_SPELLCAST_GENERIC2 || forceDeinit )
				{
					monsterSpecialState = 0;
					node = itemNodeInInventory(myStats, -1, WEAPON); // find weapon to re-equip
					if ( node != nullptr )
					{
						swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
						monsterSpecialAttackUnequipSafeguard = (double)MONSTER_SPECIAL_SAFEGUARD_TIMER_BASE;
						return true;
					}
					node = itemNodeInInventory(myStats, -1, MAGICSTAFF); // find weapon to re-equip
					if ( node != nullptr )
					{
						swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
						monsterSpecialAttackUnequipSafeguard = (double)MONSTER_SPECIAL_SAFEGUARD_TIMER_BASE;
						return true;
					}
					else
					{
						monsterUnequipSlotFromCategory(myStats, &myStats->weapon, SPELLBOOK);
					}
					monsterSpecialAttackUnequipSafeguard = (double)MONSTER_SPECIAL_SAFEGUARD_TIMER_BASE;
					return true;
				}
			}

			switch ( myStats->type )
			{
				case KOBOLD:
					node = itemNodeInInventory(myStats, -1, WEAPON); // find weapon to re-equip
					if ( node != nullptr )
					{
						swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
					}
					else
					{
						monsterUnequipSlotFromCategory(myStats, &myStats->weapon, SPELLBOOK);
					}
					deinitSuccess = true;
					break;
				case SUCCUBUS:
					if ( monsterSpecialState == SUCCUBUS_CHARM || forceDeinit )
					{
						node = itemNodeInInventory(myStats, -1, WEAPON); // find weapon to re-equip
						if ( node != nullptr )
						{
							swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
						}
						else
						{
							monsterUnequipSlotFromCategory(myStats, &myStats->weapon, SPELLBOOK);
						}
						shouldAttack = false;
						monsterSpecialState = 0;
						serverUpdateEntitySkill(this, 33); // for clients to handle animation
						deinitSuccess = true;
					}
					break;
				case SPIDER:
					if ( monsterSpecialState == SPIDER_CAST || forceDeinit )
					{
						monsterSpecialState = 0;
						serverUpdateEntitySkill(this, 33); // for clients to handle animation
						monsterUnequipSlotFromCategory(myStats, &myStats->weapon, SPELLBOOK);
						shouldAttack = false;
						deinitSuccess = true;
					}
					break;
				case INSECTOID:
					if ( monsterSpecialState == INSECTOID_ACID || forceDeinit )
					{
						monsterSpecialState = 0;
						node = itemNodeInInventory(myStats, -1, WEAPON); // find weapon to re-equip
						if ( node != nullptr )
						{
							swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
						}
						else
						{
							monsterUnequipSlotFromCategory(myStats, &myStats->weapon, SPELLBOOK);
						}
						shouldAttack = false;
						serverUpdateEntitySkill(this, 33); // for clients to handle animation
						deinitSuccess = true;
					}
					else if ( monsterSpecialState == INSECTOID_DOUBLETHROW_SECOND )
					{
						monsterSpecialState = 0;
						node = itemNodeInInventory(myStats, -1, WEAPON); // find weapon to re-equip
						if ( node != nullptr )
						{
							swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
						}
						else
						{
							monsterUnequipSlotFromCategory(myStats, &myStats->weapon, THROWN);
						}
						shouldAttack = false;
						serverUpdateEntitySkill(this, 33); // for clients to handle animation
						deinitSuccess = true;
					}
					break;
				case COCKATRICE:
					monsterUnequipSlotFromCategory(myStats, &myStats->weapon, SPELLBOOK);
					deinitSuccess = true;
					break;
				case INCUBUS:
					if ( monsterSpecialState == INCUBUS_CONFUSION )
					{
						node = itemNodeInInventory(myStats, -1, WEAPON); // find weapon to re-equip
						if ( node != nullptr )
						{
							swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
						}
						else
						{
							monsterUnequipSlotFromCategory(myStats, &myStats->weapon, POTION);
						}
						shouldAttack = false;
						monsterSpecialState = 0;
						serverUpdateEntitySkill(this, 33); // for clients to keep track of animation
						deinitSuccess = true;
					}
					else if ( monsterSpecialState == INCUBUS_STEAL )
					{
						node = itemNodeInInventory(myStats, -1, WEAPON); // find weapon to re-equip
						if ( node != nullptr )
						{
							swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
						}
						else
						{
							monsterUnequipSlotFromCategory(myStats, &myStats->weapon, SPELLBOOK);
						}
						shouldAttack = false;
						monsterSpecialState = 0;
						serverUpdateEntitySkill(this, 33); // for clients to keep track of animation
						deinitSuccess = true;
					}
					else if ( monsterSpecialState == INCUBUS_TELEPORT_STEAL )
					{
						// this flag will be cleared in incubusChooseWeapon
					}
					else if ( monsterSpecialState == INCUBUS_TELEPORT )
					{
						// this flag will be cleared in incubusChooseWeapon
					}
					else if ( monsterSpecialState == INCUBUS_CHARM || forceDeinit )
					{
						node = itemNodeInInventory(myStats, -1, WEAPON); // find weapon to re-equip
						if ( node != nullptr )
						{
							swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
						}
						else
						{
							monsterUnequipSlotFromCategory(myStats, &myStats->weapon, SPELLBOOK);
						}
						shouldAttack = false;
						monsterSpecialState = 0;
						serverUpdateEntitySkill(this, 33); // for clients to keep track of animation
						deinitSuccess = true;
					}
					break;
				case VAMPIRE:
					if ( monsterSpecialState == VAMPIRE_CAST_AURA || forceDeinit )
					{
						node = itemNodeInInventory(myStats, -1, WEAPON); // find weapon to re-equip
						if ( node != nullptr )
						{
							swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
						}
						else
						{
							monsterUnequipSlotFromCategory(myStats, &myStats->weapon, SPELLBOOK);
						}
						shouldAttack = false;
						monsterSpecialState = 0;
						serverUpdateEntitySkill(this, 33); // for clients to keep track of animation
						deinitSuccess = true;
					}
					else if ( monsterSpecialState == VAMPIRE_CAST_DRAIN )
					{
						node = itemNodeInInventory(myStats, -1, WEAPON); // find weapon to re-equip
						if ( node != nullptr )
						{
							swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
						}
						else
						{
							monsterUnequipSlotFromCategory(myStats, &myStats->weapon, SPELLBOOK);
						}
						shouldAttack = false;
						monsterSpecialState = 0;
						serverUpdateEntitySkill(this, 33); // for clients to keep track of animation
						deinitSuccess = true;
					}
					break;
				case SHADOW:
					if ( monsterSpecialState == SHADOW_SPELLCAST || forceDeinit ) //TODO: This code is destroying spells?
					{
						//TODO: Nope, this code isn't destroying spells. Something *before* this code is.
						//messagePlayer(clientnum, "[DEBUG: handleMonsterSpecialAttack()] Resolving shadow's spellcast.");
						node = itemNodeInInventory(myStats, -1, WEAPON); // find weapon to re-equip
						if ( node != nullptr )
						{
							swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
						}
						else
						{
							monsterUnequipSlotFromCategory(myStats, &myStats->weapon, SPELLBOOK);
						}
						/*Item *spellbook = newItem(static_cast<ItemType>(0), static_cast<Status>(0), 0, 1, local_rng.rand(), 0, &myStats->inventory);
						copyItem(spellbook, myStats->weapon);
						dropItemMonster(myStats->weapon, this, myStats, 1);*/
						shouldAttack = false;
						monsterSpecialState = 0;
						monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_SHADOW_SPELLCAST;
						serverUpdateEntitySkill(this, 33); // for clients to keep track of animation
						deinitSuccess = true;
					}
					break;
				case GOATMAN:
					if ( monsterSpecialState == GOATMAN_POTION || forceDeinit )
					{
						node = itemNodeInInventory(myStats, -1, WEAPON); // find weapon to re-equip
						if ( node != nullptr )
						{
							swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
						}
						else
						{
							monsterUnequipSlotFromCategory(myStats, &myStats->weapon, POTION);
						}
						shouldAttack = false;
						monsterSpecialState = 0;
						serverUpdateEntitySkill(this, 33); // for clients to keep track of animation
						deinitSuccess = true;
					}
					else if ( monsterSpecialState == GOATMAN_THROW )
					{
						node = itemNodeInInventory(myStats, -1, WEAPON); // find weapon to re-equip
						if ( node != nullptr )
						{
							swapMonsterWeaponWithInventoryItem(this, myStats, node, false, true);
						}
						else
						{
							monsterUnequipSlotFromCategory(myStats, &myStats->weapon, THROWN);
						}
						shouldAttack = false;
						monsterSpecialState = 0;
						serverUpdateEntitySkill(this, 33); // for clients to keep track of animation
						deinitSuccess = true;
					}
					break;
				default:
					break;
			}

			if ( deinitSuccess )
			{
				monsterSpecialAttackUnequipSafeguard = (double)MONSTER_SPECIAL_SAFEGUARD_TIMER_BASE;
			}

			// Whether monster should attack following the unequip action.
			return shouldAttack;
		}
	}
	// monster should attack after this function is called.
	return true;
}

void Creature::monsterMoveBackwardsAndPath()
{
	while ( yaw < 0 )
	{
		yaw += 2 * PI;
	}
	while ( yaw >= 2 * PI )
	{
		yaw -= 2 * PI;
	}
	int x1 = ((int)floor(x)) >> 4;
	int y1 = ((int)floor(y)) >> 4;
	int u, v;
	std::vector<std::pair<int, int>> areaToTry;
	if ( yaw <= PI / 4 || yaw > 7 * PI / 4 )
	{
		areaToTry.push_back(std::pair<int, int>(x1 - 1, y1));
		areaToTry.push_back(std::pair<int, int>(x1 - 2, y1));
		areaToTry.push_back(std::pair<int, int>(x1 - 2, y1 + 1));
		areaToTry.push_back(std::pair<int, int>(x1 - 2, y1 - 1));
		areaToTry.push_back(std::pair<int, int>(x1 - 3, y1));
	}
	else if ( yaw > PI / 4 && yaw <= 3 * PI / 4 )
	{
		areaToTry.push_back(std::pair<int, int>(x1, y1 - 1));
		areaToTry.push_back(std::pair<int, int>(x1, y1 - 2));
		areaToTry.push_back(std::pair<int, int>(x1 - 1, y1 - 2));
		areaToTry.push_back(std::pair<int, int>(x1 + 1, y1 - 2));
		areaToTry.push_back(std::pair<int, int>(x1, y1 - 3));
	}
	else if ( yaw > 3 * PI / 4 && yaw <= 5 * PI / 4 )
	{
		areaToTry.push_back(std::pair<int, int>(x1 + 1, y1));
		areaToTry.push_back(std::pair<int, int>(x1 + 2, y1));
		areaToTry.push_back(std::pair<int, int>(x1 + 2, y1 + 1));
		areaToTry.push_back(std::pair<int, int>(x1 + 2, y1 - 1));
		areaToTry.push_back(std::pair<int, int>(x1 + 3, y1));
	}
	else if ( yaw > 5 * PI / 4 && yaw <= 7 * PI / 4 )
	{
		areaToTry.push_back(std::pair<int, int>(x1, y1 + 1));
		areaToTry.push_back(std::pair<int, int>(x1, y1 + 2));
		areaToTry.push_back(std::pair<int, int>(x1 - 1, y1 + 2));
		areaToTry.push_back(std::pair<int, int>(x1 + 1, y1 + 2));
		areaToTry.push_back(std::pair<int, int>(x1, y1 + 3));
	}
	bool foundplace = false;
	for ( int tries = 0; tries < areaToTry.size() && !foundplace; ++tries )
	{
		std::pair<int, int> tmpPair = areaToTry[local_rng.rand() % areaToTry.size()];
		u = tmpPair.first;
		v = tmpPair.second;
		if ( !checkObstacle((u << 4) + 8, (v << 4) + 8, this, nullptr) )
		{
			x1 = u;
			y1 = v;
			foundplace = true;
		}
	}
	path = generatePath((int)floor(x / 16), (int)floor(y / 16), x1, y1, this, this,
		GeneratePathTypes::GENERATE_PATH_MONSTER_MOVE_BACKWARDS);
	if ( children.first != NULL )
	{
		list_RemoveNode(children.first);
	}
	node_t * node = list_AddNodeFirst(&this->children);
	node->element = path;
	node->deconstructor = &listDeconstructor;
	monsterState = MONSTER_STATE_HUNT; // hunt state
}

bool Creature::monsterHasLeader()
{
	Stat* myStats = this->getStats();
	if ( myStats )
	{
		if ( myStats->leader_uid != 0 )
		{
			return true;
		}
	}
	return false;
}

void Creature::monsterAllySendCommand(int command, int destX, int destY, uint32_t uid)
{
	if ( multiplayer == CLIENT )
	{
		return;
	}
	if ( command == -1 || command == ALLY_CMD_CANCEL || monsterAllyIndex == -1 || monsterAllyIndex > MAXPLAYERS )
	{
		return;
	}
	if ( !players[monsterAllyIndex] )
	{
		return;
	}
	if ( !players[monsterAllyIndex]->entity )
	{
		return;
	}
	if ( monsterTarget == players[monsterAllyIndex]->entity->getUID() )
	{
		// angry at owner.
		return;
	}

	Stat* myStats = getStats();
	if ( !myStats )
	{
		return;
	}

	if ( !isMobile() )
	{
		// doesn't respond.
		if ( monsterAllySpecial == ALLY_SPECIAL_CMD_REST && myStats->EFFECTS[EFF_ASLEEP]
			&& (command == ALLY_CMD_MOVETO_CONFIRM || command == ALLY_CMD_ATTACK_CONFIRM
				|| command == ALLY_CMD_MOVEASIDE) )
		{
			myStats->EFFECTS[EFF_ASLEEP] = false; // wake up
			myStats->EFFECTS_TIMERS[EFF_ASLEEP] = 0;
			myStats->EFFECTS[EFF_HP_REGEN] = false; // stop regen
			myStats->EFFECTS_TIMERS[EFF_HP_REGEN] = 0;
			monsterAllySpecial = ALLY_SPECIAL_CMD_NONE;
		}
		else
		{
			messagePlayerMonsterEvent(monsterAllyIndex, 0xFFFFFFFF, *myStats, Language::get(514), Language::get(515), MSG_COMBAT);
		}
		return;
	}

	bool isTinkeringFollower = FollowerMenu[monsterAllyIndex].isTinkeringFollower(myStats->type);
	int tinkeringLVL = 0;
	int skillLVL = 0;
	if ( stats[monsterAllyIndex] )
	{
		tinkeringLVL = stats[monsterAllyIndex]->PROFICIENCIES[PRO_LOCKPICKING] + statGetPER(stats[monsterAllyIndex], players[monsterAllyIndex]->entity);
		skillLVL = stats[monsterAllyIndex]->PROFICIENCIES[PRO_LEADERSHIP] + statGetCHR(stats[monsterAllyIndex], players[monsterAllyIndex]->entity);
		if ( isTinkeringFollower )
		{
			skillLVL = tinkeringLVL;
		}
	}

	if ( myStats->type != GYROBOT )
	{
		if ( FollowerMenu[monsterAllyIndex].monsterGyroBotOnlyCommand(command) )
		{
			return;
		}
	}
	else if ( myStats->type == GYROBOT )
	{
		if ( FollowerMenu[monsterAllyIndex].monsterGyroBotDisallowedCommands(command) )
		{
			return;
		}
	}

	// do a final check if player can use this command.
	/*if ( FollowerMenu.optionDisabledForCreature(skillLVL, myStats->type, command) != 0 )
	{
		messagePlayerMonsterEvent(monsterAllyIndex, 0xFFFFFFFF,
			*myStats, Language::get(3638), Language::get(3639), MSG_COMBAT);
		return;
	}*/

	switch ( command )
	{
		case ALLY_CMD_RETURN_SOUL:
			if ( monsterAllySummonRank != 0 )
			{
				float manaToRefund = myStats->MAXMP * (myStats->HP / static_cast<float>(myStats->MAXHP));
				setMP(static_cast<int>(manaToRefund));
				setHP(0);
				if ( stats[monsterAllyIndex] && stats[monsterAllyIndex]->MP == 0 )
				{
					steamAchievementClient(monsterAllyIndex, "BARONY_ACH_EXTERNAL_BATTERY");
				}
			}
			break;
		case ALLY_CMD_ATTACK_CONFIRM:
			if ( uid != 0 && uid != getUID() )
			{
				Creature* target = uidToCreature(uid);
				if ( target )
				{
					if ( target->behavior == &actMonster || target->behavior == &actPlayer )
					{
						if ( stats[monsterAllyIndex] ) // check owner's proficiency.
						{
							if ( skillLVL >= SKILL_LEVEL_MASTER || myStats->type != HUMAN )
							{
								// attack anything except if FF is off + friend.
								if ( !(svFlags & SV_FLAG_FRIENDLYFIRE) )
								{
									if ( !checkFriend(target) )
									{
										monsterAcquireAttackTarget(*target, MONSTER_STATE_ATTACK);
										handleNPCInteractDialogue(*myStats, ALLY_EVENT_ATTACK);
									}
									else
									{
										// messagePlayer(0, "Friendly fire is on!");
										handleNPCInteractDialogue(*myStats, ALLY_EVENT_ATTACK_FRIENDLY_FIRE);
									}
								}
								else
								{
									monsterAcquireAttackTarget(*target, MONSTER_STATE_ATTACK);
									handleNPCInteractDialogue(*myStats, ALLY_EVENT_ATTACK);
								}
							}
							else if ( skillLVL >= AllyNPCSkillRequirements[ALLY_CMD_ATTACK_CONFIRM] )
							{
								if ( !checkFriend(target) )
								{
									monsterAcquireAttackTarget(*target, MONSTER_STATE_ATTACK);
									handleNPCInteractDialogue(*myStats, ALLY_EVENT_ATTACK);
								}
								else
								{
									handleNPCInteractDialogue(*myStats, ALLY_EVENT_ATTACK_FRIENDLY_FIRE);
								}
							}
						}
						monsterAllyInteractTarget = 0;
					}
					else
					{
						monsterAcquireAttackTarget(*target, MONSTER_STATE_PATH);
						monsterAllyState = ALLY_STATE_MOVETO;
					}
				}
			}
			break;
		case ALLY_CMD_MOVEASIDE:
			monsterMoveAside(this, players[monsterAllyIndex]->entity);
			handleNPCInteractDialogue(*myStats, ALLY_EVENT_MOVEASIDE);
			break;
		case ALLY_CMD_DEFEND:
			monsterAllyState = ALLY_STATE_DEFEND;
			if ( myStats->type == SENTRYBOT || myStats->type == SPELLBOT )
			{
				monsterSentrybotLookDir = monsterLookDir;
				handleNPCInteractDialogue(*myStats, ALLY_EVENT_WAIT);
			}
			else
			{
				createPathBoundariesNPC(5);
				// stop in your tracks!
				handleNPCInteractDialogue(*myStats, ALLY_EVENT_WAIT);
				monsterState = MONSTER_STATE_WAIT; // wait state
				serverUpdateEntitySkill(this, 0);
			}
			break;
		case ALLY_CMD_MOVETO_SELECT:
			break;
		case ALLY_CMD_FOLLOW:
			monsterAllyState = ALLY_STATE_DEFAULT;
			if ( myStats->type == SENTRYBOT || myStats->type == SPELLBOT )
			{
				monsterSentrybotLookDir = 0.0;
				handleNPCInteractDialogue(*myStats, ALLY_EVENT_FOLLOW);
			}
			else
			{
				monsterAllyFormations.updateOnFollowCommand(getUID(), this);
				handleNPCInteractDialogue(*myStats, ALLY_EVENT_FOLLOW);
			}
			break;
		case ALLY_CMD_CLASS_TOGGLE:
			++monsterAllyClass;
			if ( monsterAllyClass > ALLY_CLASS_RANGED )
			{
				monsterAllyClass = ALLY_CLASS_MIXED;
			}
			myStats->allyClass = monsterAllyClass;
			serverUpdateEntitySkill(this, 46);
			break;
		case ALLY_CMD_PICKUP_TOGGLE:
			++monsterAllyPickupItems;
			if ( monsterAllyPickupItems > ALLY_PICKUP_ALL )
			{
				monsterAllyPickupItems = ALLY_PICKUP_NONPLAYER;
			}
			myStats->allyItemPickup = monsterAllyPickupItems;
			serverUpdateEntitySkill(this, 44);
			break;
		case ALLY_CMD_GYRO_LIGHT_TOGGLE:
			++monsterAllyClass;
			if ( monsterAllyClass >= ALLY_GYRO_LIGHT_END )
			{
				monsterAllyClass = ALLY_GYRO_LIGHT_NONE;
			}
			myStats->allyClass = monsterAllyClass;
			serverUpdateEntitySkill(this, 46);
			break;
		case ALLY_CMD_GYRO_DETECT_TOGGLE:
		{
			++monsterAllyPickupItems;
			bool failQuality = false;
			bool failSkillRequirement = false;
			if ( monsterAllyPickupItems >= ALLY_GYRO_DETECT_END )
			{
				monsterAllyPickupItems = ALLY_GYRO_DETECT_NONE;
			}
			else if ( monsterAllyPickupItems == ALLY_GYRO_DETECT_ITEMS_VALUABLE )
			{
				if ( myStats->LVL < 15 )
				{
					failQuality = true;
				}
				if ( skillLVL < SKILL_LEVEL_MASTER )
				{
					failSkillRequirement = true;
				}
			}
			else if ( monsterAllyPickupItems == ALLY_GYRO_DETECT_MONSTERS )
			{
				if ( myStats->LVL < 10 )
				{
					failQuality = true;
				}
				if ( skillLVL < SKILL_LEVEL_EXPERT )
				{
					failSkillRequirement = true;
				}
			}
			else if ( monsterAllyPickupItems == ALLY_GYRO_DETECT_TRAPS
				|| monsterAllyPickupItems == ALLY_GYRO_DETECT_EXITS )
			{
				if ( myStats->LVL < 5 )
				{
					failQuality = true;
				}
				if ( skillLVL < SKILL_LEVEL_SKILLED )
				{
					failSkillRequirement = true;
				}
			}
			else if ( monsterAllyPickupItems == ALLY_GYRO_DETECT_ITEMS_METAL
				|| monsterAllyPickupItems == ALLY_GYRO_DETECT_ITEMS_MAGIC )
			{
				if ( skillLVL < SKILL_LEVEL_BASIC )
				{
					failSkillRequirement = true;
				}
			}

			if ( failSkillRequirement || failQuality )
			{
				if ( skillLVL < SKILL_LEVEL_BASIC )
				{
					messagePlayerColor(monsterAllyIndex, MESSAGE_STATUS, 0xFFFFFFFF, Language::get(3680));
				}
				else
				{
					messagePlayerColor(monsterAllyIndex, MESSAGE_STATUS, 0xFFFFFFFF, Language::get(3679));
				}
				monsterAllyPickupItems = ALLY_GYRO_DETECT_NONE;
			}
			myStats->allyItemPickup = monsterAllyPickupItems;
			serverUpdateEntitySkill(this, 44);
			break;
		}
		case ALLY_CMD_DROP_EQUIP:
			if ( strcmp(myStats->name, "")
				&& myStats->getAttribute("special_npc") != ""
				&& myStats->type == HUMAN )
			{
				// named humans refuse to drop equipment.
				handleNPCInteractDialogue(*myStats, ALLY_EVENT_DROP_HUMAN_REFUSE);
			}
			else if ( myStats->type == GYROBOT )
			{
				bool droppedSomething = false;
				node_t * nextnode = nullptr;
				for (node_t * node = myStats->inventory.first; node; node = nextnode )
				{
					nextnode = node->next;
					Item* item = (Item*)node->element;
					if ( item )
					{
						if ( itemIsThrowableTinkerTool(item) && item->status > BROKEN )
						{
							int count = item->count;
							this->monsterEquipItem(*item, &myStats->weapon);
							this->attack(0, 0, nullptr);
							if ( count > 1 )
							{
								myStats->weapon = nullptr;
							}
							droppedSomething = true;
							break;
						}
						else
						{
							for ( int c = item->count; c > 0; --c )
							{
								Entity* dropped = dropItemMonster(item, this, myStats, item->count);
								if ( dropped )
								{
									c = 0;
									droppedSomething = true;
								}
							}
						}
					}
				}

				if ( !droppedSomething )
				{
					messagePlayer(monsterAllyIndex, MESSAGE_HINT, Language::get(3868));
				}
			}
			else if ( stats[monsterAllyIndex] )
			{
				Entity* dropped = nullptr;
				bool confirmDropped = false;
				bool dropWeaponOnly = false;
				bool unableToDrop = false;
				uint32_t owner = players[monsterAllyIndex]->entity->getUID();
				if ( skillLVL >= SKILL_LEVEL_MASTER )
				{
					if ( myStats->helmet )
					{
						if ( myStats->helmet->canUnequip(myStats) )
						{
							dropped = dropItemMonster(myStats->helmet, this, myStats);
						}
						else
						{
							unableToDrop = true;
						}
					}
					if ( dropped )
					{
						confirmDropped = true;
						dropped->itemOriginalOwner = owner;
					}
					if ( myStats->breastplate )
					{
						if ( myStats->breastplate->canUnequip(myStats) )
						{
							dropped = dropItemMonster(myStats->breastplate, this, myStats);
						}
						else
						{
							unableToDrop = true;
						}
					}
					if ( dropped )
					{
						confirmDropped = true;
						dropped->itemOriginalOwner = owner;
					}
					if ( myStats->shoes )
					{
						if ( myStats->shoes->canUnequip(myStats) )
						{
							dropped = dropItemMonster(myStats->shoes, this, myStats);
						}
						else
						{
							unableToDrop = true;
						}
					}
					if ( dropped )
					{
						confirmDropped = true;
						dropped->itemOriginalOwner = owner;
					}
					if ( myStats->shield )
					{
						if ( myStats->shield->canUnequip(myStats) )
						{
							dropped = dropItemMonster(myStats->shield, this, myStats, myStats->shield->count);
						}
						else
						{
							unableToDrop = true;
						}
					}
					if ( dropped )
					{
						confirmDropped = true;
						dropped->itemOriginalOwner = owner;
					}
					if ( myStats->cloak )
					{
						if ( myStats->cloak->canUnequip(myStats) )
						{
							dropped = dropItemMonster(myStats->cloak, this, myStats);
						}
						else
						{
							unableToDrop = true;
						}
					}
					if ( dropped )
					{
						confirmDropped = true;
						dropped->itemOriginalOwner = owner;
					}

					if ( skillLVL >= SKILL_LEVEL_LEGENDARY )
					{
						if ( myStats->ring )
						{
							if ( myStats->ring->canUnequip(myStats) )
							{
								dropped = dropItemMonster(myStats->ring, this, myStats);
							}
							else
							{
								unableToDrop = true;
							}
						}
						if ( dropped )
						{
							confirmDropped = true;
							dropped->itemOriginalOwner = owner;
						}
						if ( myStats->amulet )
						{
							if ( myStats->amulet->canUnequip(myStats) )
							{
								dropped = dropItemMonster(myStats->amulet, this, myStats);
							}
							else
							{
								unableToDrop = true;
							}
						}
						if ( dropped )
						{
							confirmDropped = true;
							dropped->itemOriginalOwner = owner;
						}
						if ( confirmDropped )
						{
							handleNPCInteractDialogue(*myStats, ALLY_EVENT_DROP_ALL);
						}
					}
					else
					{
						if ( confirmDropped )
						{
							handleNPCInteractDialogue(*myStats, ALLY_EVENT_DROP_EQUIP);
						}
					}
				}
				else
				{
					if ( myStats->weapon )
					{
						dropWeaponOnly = true;
					}
				}
				if ( myStats->weapon )
				{
					if ( myStats->weapon->canUnequip(myStats) )
					{
						dropped = dropItemMonster(myStats->weapon, this, myStats, myStats->weapon->count);
					}
					else
					{
						unableToDrop = true;
					}
				}
				if ( dropped )
				{
					dropped->itemOriginalOwner = owner;
					if ( dropWeaponOnly )
					{
						handleNPCInteractDialogue(*myStats, ALLY_EVENT_DROP_WEAPON);
					}
				}
				/*if ( unableToDrop )
				{
					handleNPCInteractDialogue(*myStats, ALLY_EVENT_DROP_FAILED);
				}*/
			}
			break;
		case ALLY_CMD_MOVETO_CONFIRM:
		{
			if ( myStats->type == SENTRYBOT || myStats->type == SPELLBOT )
			{
				double floatx = destX * 16 + 8;
				double floaty = destY * 16 + 8;
				double tangent = atan2(floaty - y, floatx - x);
				monsterLookTime = 1;
				monsterMoveTime = local_rng.rand() % 10 + 1;
				monsterLookDir = tangent;
				monsterSentrybotLookDir = monsterLookDir;
				if ( monsterAllyState != ALLY_STATE_DEFEND )
				{
					handleNPCInteractDialogue(*myStats, ALLY_EVENT_WAIT);
				}
				monsterAllyState = ALLY_STATE_DEFEND;
			}
			else if ( monsterSetPathToLocation(destX, destY, 1,
				GeneratePathTypes::GENERATE_PATH_PLAYER_ALLY_MOVETO) )
			{
				monsterState = MONSTER_STATE_HUNT; // hunt state
				monsterAllyState = ALLY_STATE_MOVETO;
				serverUpdateEntitySkill(this, 0);
				handleNPCInteractDialogue(*myStats, ALLY_EVENT_MOVETO_BEGIN);
			}
			else
			{
				//messagePlayer(0, "no path to destination");
				handleNPCInteractDialogue(*myStats, ALLY_EVENT_MOVETO_FAIL);
			}
			break;
		}
		case ALLY_CMD_GYRO_RETURN:
		{
			if ( players[monsterAllyIndex]->entity )
			{
				destX = static_cast<int>(players[monsterAllyIndex]->entity->x) >> 4;
				destY = static_cast<int>(players[monsterAllyIndex]->entity->y) >> 4;

				bool noground = false;
				const int monsterX = static_cast<int>(x) >> 4;
				const int monsterY = static_cast<int>(y) >> 4;
				if ( monsterX >= 0 && monsterX < map.width )
				{
					if ( monsterY >= 0 && monsterY < map.height )
					{
						int index = (monsterY) * MAPLAYERS + (monsterX) * MAPLAYERS * map.height;
						noground = !map.tiles[index];
					}
				}

				if ( entityDist(this, players[monsterAllyIndex]->entity) < STRIKERANGE
					&& !noground )
				{
					monsterSpecialState = GYRO_RETURN_LANDING;
					serverUpdateEntitySkill(this, 33); // for clients to keep track of animation
					playSoundEntity(this, 449, 128);
				}
				else if ( gyrobotSetPathToReturnLocation(destX, destY, 0) )
				{
					monsterState = MONSTER_STATE_HUNT; // hunt state
					monsterAllyState = ALLY_STATE_MOVETO;
					serverUpdateEntitySkill(this, 0);
					handleNPCInteractDialogue(*myStats, ALLY_EVENT_MOVETO_BEGIN);
					monsterSpecialState = GYRO_RETURN_PATHING;
					serverUpdateEntitySkill(this, 33); // for clients to keep track of animation
				}
				else if ( gyrobotSetPathToReturnLocation(destX, destY, 2) ) // expand search
				{
					monsterState = MONSTER_STATE_HUNT; // hunt state
					monsterAllyState = ALLY_STATE_MOVETO;
					serverUpdateEntitySkill(this, 0);
					handleNPCInteractDialogue(*myStats, ALLY_EVENT_MOVETO_BEGIN);
					monsterSpecialState = GYRO_RETURN_PATHING;
					serverUpdateEntitySkill(this, 33); // for clients to keep track of animation
				}
				else
				{
					//messagePlayer(0, "no path to destination");
					messagePlayerMonsterEvent(monsterAllyIndex, 0xFFFFFF, *myStats, Language::get(4317), Language::get(4317), MSG_GENERIC);
				}
			}
			break;
		}
		case ALLY_CMD_SPECIAL:
		{
			if ( monsterAllySpecialCooldown == 0 )
			{
				int duration = TICKS_PER_SECOND * (60);
				if ( myStats->HP < myStats->MAXHP && setEffect(EFF_ASLEEP, true, duration, false) ) // 60 seconds of sleep.
				{
					setEffect(EFF_HP_REGEN, true, duration, false);
					monsterAllySpecial = ALLY_SPECIAL_CMD_REST;
					monsterAllySpecialCooldown = -1; // locked out until next floor.
					serverUpdateEntitySkill(this, 49);
					messagePlayerMonsterEvent(monsterAllyIndex, 0xFFFFFF, *myStats, Language::get(398), Language::get(397), MSG_COMBAT);
					if ( players[monsterAllyIndex] && players[monsterAllyIndex]->entity
						&& myStats->HP < myStats->MAXHP && local_rng.rand() % 3 == 0 )
					{
						players[monsterAllyIndex]->entity->increaseSkill(PRO_LEADERSHIP);
					}
				}
				else
				{
					messagePlayerMonsterEvent(monsterAllyIndex, 0xFFFFFF, *myStats, Language::get(3880), Language::get(3880), MSG_GENERIC);
				}
			}
			break;
		}
		case ALLY_CMD_DUMMYBOT_RETURN:
			if ( myStats->type == DUMMYBOT )
			{
				monsterSpecialState = DUMMYBOT_RETURN_FORM;
				serverUpdateEntitySkill(this, 33);
			}
			else if ( myStats->type == SENTRYBOT || myStats->type == SPELLBOT )
			{
				monsterSpecialState = DUMMYBOT_RETURN_FORM;
				serverUpdateEntitySkill(this, 33);
				playSoundEntity(this, 469 + local_rng.rand() % 3, 92);
			}
			break;
		default:
			break;
	}
	serverUpdateEntitySkill(this, 43);
	//messagePlayer(0, "received: %d", command);
}

/*-------------------------------------------------------------------------------

Creature::teleport

Teleports the given entity to the given (x, y) location on the map,
in map coordinates. Will not teleport if the destination is an obstacle.

-------------------------------------------------------------------------------*/

bool Creature::teleport(int tele_x, int tele_y)
{
	int player = -1;

	if ( behavior == &actPlayer )
	{
		player = skill[2];
		if ( MFLAG_DISABLETELEPORT )
		{
			uint32_t color = makeColorRGB(255, 0, 255);
			// play sound effect
			playSoundEntity(this, 77, 64);
			messagePlayerColor(player, MESSAGE_HINT, color, Language::get(2381));
			return false;
		}
	}

	if ( strstr(map.name, "Minotaur") || checkObstacle((tele_x << 4) + 8, (tele_y << 4) + 8, this, NULL) )
	{
		messagePlayer(player, MESSAGE_HINT, Language::get(707));
		return false;
	}

	// play sound effect
	playSoundEntity(this, 77, 64);
    spawnPoof(x, y, 0, 1.0, true);

	// relocate entity
	double oldx = x;
	double oldy = y;
	x = (tele_x << 4) + 8;
	y = (tele_y << 4) + 8;
	if ( entityInsideSomething(this) && getRace() != LICH_FIRE && getRace() != LICH_ICE )
	{
		x = oldx;
		y = oldy;
		if ( multiplayer == SERVER && player > 0 )
		{
			messagePlayer(player, MESSAGE_HINT, Language::get(707));
		}
		return false;
	}
	updateAchievementBaitAndSwitch(player, true);
	if ( multiplayer != CLIENT )
	{
		TileEntityList.updateEntity(*this);
	}
	if ( player > 0 && multiplayer == SERVER && !players[player]->isLocalPlayer() )
	{
		strcpy((char*)net_packet->data, "TELE");
		net_packet->data[4] = tele_x;
		net_packet->data[5] = tele_y;
		SDLNet_Write16(static_cast<int16_t>(this->yaw * 180 / PI), &net_packet->data[6]);
		net_packet->address.host = net_clients[player - 1].host;
		net_packet->address.port = net_clients[player - 1].port;
		net_packet->len = 8;
		sendPacketSafe(net_sock, -1, net_packet, player - 1);
	}


	if ( behavior == actMonster )
	{
		if ( getRace() != LICH && getRace() != DEVIL && getRace() != LICH_FIRE && getRace() != LICH_ICE )
		{
			//messagePlayer(0, "Resetting monster's path after teleport.");
			monsterState = MONSTER_STATE_PATH;
			/*if ( children.first != nullptr )
			{
				list_RemoveNode(children.first);
			}*/
		}
	}

	// play second sound effect
	playSoundEntity(this, 77, 64);
    const float poofx = x + cosf(yaw) * 4.f;
    const float poofy = y + sinf(yaw) * 4.f;
    spawnPoof(poofx, poofy, 0, 1.0, true);
    bNeedsRenderPositionInit = true;
    for (auto part : bodyparts) {
        part->bNeedsRenderPositionInit = true;
    }
    for (auto node = map.entities->first; node != nullptr; node = node->next) {
        auto entity = (Entity*)node->element;
        if (entity && entity->behavior == &actSpriteNametag) {
            if (entity->parent == uid) {
                entity->bNeedsRenderPositionInit = true;
            }
        }
    }
    if (player == clientnum || (splitscreen && player >= 0)) {
        temporarilyDisableDithering();
    }

	if ( behavior == &actMonster )
	{
		achievementObserver.addEntityAchievementTimer(this, AchievementObserver::BARONY_ACH_TELEFRAG, 50, true, 0);
		achievementObserver.addEntityAchievementTimer(this, AchievementObserver::BARONY_ACH_COWBOY_FROM_HELL, 150, true, 0);
	}

	return true;
}

bool Creature::monsterAllySetInteract()
{
	if ( multiplayer == CLIENT )
	{
		return false;
	}
	if ( monsterAllyInteractTarget == 0 )
	{
		return false;
	}
	Entity* target = uidToEntity(monsterAllyInteractTarget);
	if ( !target )
	{
		monsterAllyState = ALLY_STATE_DEFAULT;
		monsterAllyInteractTarget = 0;
		return false;
	}
	// check distance to interactable.
	double range = pow(y - target->y, 2) + pow(x - target->x, 2);
	if ( range < 576 ) // 24 squared
	{
		if ( getMonsterTypeFromSprite() == GYROBOT
			&& monsterSpecialState != GYRO_INTERACT_LANDING
			&& z < -0.1 )
		{
			// don't set interact yet.
			return true;
		}
		else
		{
			if ( monsterAllyIndex >= 0 )
			{
				FollowerMenu[monsterAllyIndex].entityToInteractWith = target; // set followerInteractedEntity to the mechanism/item/gold etc.
				FollowerMenu[monsterAllyIndex].entityToInteractWith->interactedByMonster = getUID(); // set the remote entity to this monster's uid to lookup later.
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool Creature::monsterSetPathToLocation(int destX, int destY, int adjacentTilesToCheck, int pathingType, bool tryRandomSpot)
{
	int u, v;
	bool foundplace = false;
	int pathToX = destX;
	int pathToY = destY;

	if ( static_cast<int>(x / 16) == destX && static_cast<int>(y / 16) == destY )
	{
		monsterAllyFormations.updateOnPathSucceed(getUID(), this);
		return true; // we're trying to move to the spot we're already at!
	}
	else if ( !checkObstacle((destX << 4) + 8, (destY << 4) + 8, this, nullptr) )
	{
		if ( !tryRandomSpot )
		{
			foundplace = true; // we can path directly to the destination specified.
		}
	}

	std::vector<std::pair<int, std::pair<int, int>>> possibleDestinations; // store distance and the x, y coordinates in each element.

	if ( !foundplace )
	{
		for ( u = destX - adjacentTilesToCheck; u <= destX + adjacentTilesToCheck; u++ )
		{
			for ( v = destY - adjacentTilesToCheck; v <= destY + adjacentTilesToCheck; v++ )
			{
				if ( static_cast<int>(x / 16) == u && static_cast<int>(y / 16) == v )
				{
					// we're trying to move to the spot we're already at!
				}
				else if ( !checkObstacle((u << 4) + 8, (v << 4) + 8, this, nullptr) )
				{
					int distance = pow(x / 16 - u, 2) + pow(y / 16 - v, 2);
					possibleDestinations.push_back(std::make_pair(distance, std::make_pair(u, v)));
				}
			}
		}
	}

	if ( !possibleDestinations.empty() )
	{
		// sort by distance from monster, first result is shortest path.
		std::sort(possibleDestinations.begin(), possibleDestinations.end());
		pathToX = possibleDestinations.at(0).second.first;
		pathToY = possibleDestinations.at(0).second.second;
		foundplace = true;
	}

	path = generatePath(static_cast<int>(floor(x / 16)), static_cast<int>(floor(y / 16)), pathToX, pathToY,
		this, nullptr, (GeneratePathTypes)pathingType);
	if ( children.first != NULL )
	{
		list_RemoveNode(children.first);
	}
	node_t * node = list_AddNodeFirst(&children);
	node->element = path;
	node->deconstructor = &listDeconstructor;

	if ( path == nullptr || !foundplace )
	{
		return false;
	}
	return true;
}

/*-------------------------------------------------------------------------------

Creature::teleportRandom

Teleports the given entity to a random location on the map.

-------------------------------------------------------------------------------*/

bool Creature::teleportRandom()
{
	int numlocations = 0;
	int pickedlocation;
	int player = -1;
	if ( behavior == &actPlayer )
	{
		player = skill[2];
		if ( MFLAG_DISABLETELEPORT )
		{
			uint32_t color = makeColorRGB(255, 0, 255);
			// play sound effect
			playSoundEntity(this, 77, 64);
			messagePlayerColor(player, MESSAGE_HINT, color, Language::get(2381));
			return false;
		}

	}
	for ( int iy = 1; iy < map.height; ++iy )
	{
		for ( int ix = 1; ix < map.width; ++ix )
		{
			if ( !checkObstacle((ix << 4) + 8, (iy << 4) + 8, this, NULL) )
			{
				numlocations++;
			}
		}
	}
	if ( numlocations == 0 )
	{
		messagePlayer(player, MESSAGE_HINT, Language::get(708));
		return false;
	}
	pickedlocation = local_rng.rand() % numlocations;
	numlocations = 0;
	for ( int iy = 1; iy < map.height; iy++ )
	{
		for ( int ix = 1; ix < map.width; ix++ )
		{
			if ( !checkObstacle((ix << 4) + 8, (iy << 4) + 8, this, NULL) )
			{
				if ( numlocations == pickedlocation )
				{
					teleport(ix, iy);
					return true;
				}
				numlocations++;
			}
		}
	}
	return false;
}

bool Creature::gyrobotSetPathToReturnLocation(int destX, int destY, int adjacentTilesToCheck, bool tryRandomSpot)
{
	int u, v;
	bool foundplace = false;
	int pathToX = destX;
	int pathToY = destY;

	if ( static_cast<int>(x / 16) == destX && static_cast<int>(y / 16) == destY )
	{
		return true; // we're trying to move to the spot we're already at!
	}
	else if ( !checkObstacle((destX << 4) + 8, (destY << 4) + 8, this, nullptr) )
	{
		int index = (destY)* MAPLAYERS + (destX)* MAPLAYERS * map.height;
		if ( !tryRandomSpot && map.tiles[index] )
		{
			foundplace = true; // we can path directly to the destination specified.
		}
	}

	std::vector<std::pair<int, std::pair<int, int>>> possibleDestinations; // store distance and the x, y coordinates in each element.

	if ( !foundplace )
	{
		for ( u = destX - adjacentTilesToCheck; u <= destX + adjacentTilesToCheck; u++ )
		{
			for ( v = destY - adjacentTilesToCheck; v <= destY + adjacentTilesToCheck; v++ )
			{
				if ( static_cast<int>(x / 16) == u && static_cast<int>(y / 16) == v )
				{
					// we're trying to move to the spot we're already at!
				}
				else if ( !checkObstacle((u << 4) + 8, (v << 4) + 8, this, nullptr) )
				{
					int index = (v) * MAPLAYERS + (u) * MAPLAYERS * map.height;
					if ( !map.tiles[index] )
					{
						continue; // bad spot to land
					}
					int distance = pow(destX - u, 2) + pow(destY - v, 2);
					possibleDestinations.push_back(std::make_pair(distance, std::make_pair(u, v)));
				}
			}
		}
	}

	if ( !possibleDestinations.empty() )
	{
		// sort by distance from monster, first result is shortest path.
		std::sort(possibleDestinations.begin(), possibleDestinations.end());
		pathToX = possibleDestinations.at(0).second.first;
		pathToY = possibleDestinations.at(0).second.second;
		foundplace = true;
	}

	path = generatePath(static_cast<int>(floor(x / 16)), static_cast<int>(floor(y / 16)), pathToX, pathToY,
		this, nullptr, GeneratePathTypes::GENERATE_PATH_PLAYER_GYRO_RETURN);
	if ( children.first != NULL )
	{
		list_RemoveNode(children.first);
	}
	node_t * node = list_AddNodeFirst(&children);
	node->element = path;
	node->deconstructor = &listDeconstructor;

	if ( path == nullptr || !foundplace )
	{
		return false;
	}
	return true;
}

bool Creature::teleportAroundEntity(Entity* target, int dist, int effectType)
{
    Creature* targetCrtr = (Creature*)(*&target);
	int numlocations = 0;
	int pickedlocation;
	int player = -1;
	if ( !target )
	{
		return false;
	}
	int ty = static_cast<int>(std::floor(target->y)) >> 4;
	int tx = static_cast<int>(std::floor(target->x)) >> 4;
	if ( target->behavior == &::actTeleportShrine )
	{
		tx = static_cast<int>(std::floor(target->x + 32.0 * cos(target->yaw))) >> 4;
		ty = static_cast<int>(std::floor(target->y + 32.0 * sin(target->yaw))) >> 4;

		if ( target->shrineDestXOffset != 0 || target->shrineDestYOffset != 0 )
		{
			// default both to ontop of the shrine
			tx = static_cast<int>(std::floor(target->x)) >> 4;
			ty = static_cast<int>(std::floor(target->y)) >> 4;
		}
		if ( target->shrineDestXOffset != 0 )
		{
			tx = (static_cast<int>(std::floor(target->x)) >> 4) + target->shrineDestXOffset;
		}
		if ( target->shrineDestYOffset != 0 )
		{
			ty = (static_cast<int>(std::floor(target->y)) >> 4) + target->shrineDestYOffset;
		}
	}

	if ( behavior == &actPlayer )
	{
		player = skill[2];
		if ( MFLAG_DISABLETELEPORT )
		{
			uint32_t color = makeColorRGB(255, 0, 255);
			// play sound effect
			playSoundEntity(this, 77, 64);
			messagePlayerColor(player, MESSAGE_HINT, color, Language::get(2381));
			return false;
		}
	}

	struct Coord_t
	{
		bool onHazard = false;
		int x = 0;
		int y = 0;
		Coord_t(const int _x, const int _y, const bool _onHazard)
		{
			x = _x;
			y = _y;
			onHazard = _onHazard;
		};
	};
	std::vector<Coord_t> goodspots;
	std::vector<Coord_t> spotsBehindMonster;
	bool forceSpot = false;
	for ( int iy = std::max(1, ty - dist); !forceSpot && iy <= std::min(ty + dist, static_cast<int>(map.height) - 1); ++iy )
	{
		for ( int ix = std::max(1, tx - dist); !forceSpot && ix <= std::min(tx + dist, static_cast<int>(map.width) - 1); ++ix )
		{
			if ( !checkObstacle((ix << 4) + 8, (iy << 4) + 8, this, NULL) )
			{
				if ( behavior == &actPlayer && targetCrtr && targetCrtr->behavior == &actMonster )
				{
					// check LOS
					Entity* ohit = hit.entity;

					// pretend player has teleported, get the angle needed.
					double tmpx = x;
					double tmpy = y;
					x = (ix << 4) + 8;
					y = (iy << 4) + 8;
					TileEntityList.updateEntity(*this); // important - lineTrace needs the TileEntityListUpdated.

					double tangent = atan2(target->y - this->y, target->x - this->x);
					lineTraceTarget(this, this->x, this->y, tangent, 64 * dist, 0, false, target);
					if ( hit.entity == target && !entityInsideSomething(this) )
					{
						numlocations++;
						double targetYaw = target->yaw;
						while ( targetYaw >= 2 * PI )
						{
							targetYaw -= PI * 2;
						}
						while ( targetYaw < 0 )
						{
							targetYaw += PI * 2;
						}
						double yawDifference = (PI - abs(abs(tangent - targetYaw) - PI)) * 2;
						if ( yawDifference >= 0 && yawDifference <= PI ) // 180 degree arc
						{
							spotsBehindMonster.push_back(Coord_t(ix, iy, teleportCoordHasTrap(ix, iy)));
						}
						else
						{
							goodspots.push_back(Coord_t(ix, iy, teleportCoordHasTrap(ix, iy)));
						}
					}
					// restore coordinates.
					x = tmpx;
					y = tmpy;
					TileEntityList.updateEntity(*this); // important - lineTrace needs the TileEntityListUpdated.
					hit.entity = ohit;
				}
				else
				{
					if ( target->behavior == &actBomb && target->skill[22] == 1 && ix == tx && iy == ty ) // teleport receiver.
					{
						// directly on top, let's go there.
						double tmpx = x;
						double tmpy = y;
						x = (ix << 4) + 8;
						y = (iy << 4) + 8;
						if ( !entityInsideSomething(this) )
						{
							bool onTrap = false;
							if ( !onTrap )
							{
								forceSpot = true;
								goodspots.clear();
								goodspots.push_back(Coord_t(ix, iy, onTrap));
								numlocations = 1;
								// restore coordinates.
								x = tmpx;
								y = tmpy;
								break;
							}
							else
							{
								goodspots.push_back(Coord_t(ix, iy, onTrap));
								numlocations++;
							}
						}
						// restore coordinates.
						x = tmpx;
						y = tmpy;
					}
					else if ( target->behavior == &::actTeleportShrine && ix == tx && iy == ty )
					{
						// directly on top, let's go there.
						double tmpx = x;
						double tmpy = y;
						x = (ix << 4) + 8;
						y = (iy << 4) + 8;
						if ( !entityInsideSomething(this) )
						{
							bool onTrap = teleportCoordHasTrap(ix, iy);
							if ( !onTrap )
							{
								forceSpot = true;
								goodspots.clear();
								goodspots.push_back(Coord_t(ix, iy, onTrap));
								numlocations = 1;
								// restore coordinates.
								x = tmpx;
								y = tmpy;
								break;
							}
							else
							{
								goodspots.push_back(Coord_t(ix, iy, onTrap));
								numlocations++;
							}
						}
						// restore coordinates.
						x = tmpx;
						y = tmpy;
					}
					else
					{
						double tmpx = x;
						double tmpy = y;
						x = (ix << 4) + 8;
						y = (iy << 4) + 8;
						if ( !entityInsideSomething(this) )
						{
							goodspots.push_back(Coord_t(ix, iy, teleportCoordHasTrap(ix, iy)));
							numlocations++;
						}
						// restore coordinates.
						x = tmpx;
						y = tmpy;
					}
				}
			}
		}
	}
	//messagePlayer(0, "locations: %d", numlocations);
	if ( numlocations == 0 )
	{
		messagePlayer(player, MESSAGE_HINT, Language::get(708));
		return false;
	}
	if ( behavior == &actMonster || spotsBehindMonster.empty() )
	{
		std::vector<unsigned int> goodchances;
		std::vector<unsigned int> badchances;
		bool foundGoodSpot = false;
		bool foundBadSpot = false;
		for ( auto& coord : goodspots )
		{
			if ( coord.onHazard )
			{
				foundBadSpot = true;
				badchances.push_back(1);
				goodchances.push_back(0);
			}
			else
			{
				foundGoodSpot = true;
				badchances.push_back(0);
				goodchances.push_back(1);
			}
		}

		if ( foundGoodSpot )
		{
			auto& coord = goodspots[local_rng.discrete(goodchances.data(), goodchances.size())];
			tx = coord.x;
			ty = coord.y;
		}
		else
		{
			auto& coord = goodspots[local_rng.discrete(badchances.data(), badchances.size())];
			tx = coord.x;
			ty = coord.y;
		}
	}
	else
	{
		std::vector<unsigned int> goodchances;
		std::vector<unsigned int> badchances;
		bool foundGoodSpot = false;
		bool foundBadSpot = false;
		for ( auto& coord : spotsBehindMonster )
		{
			if ( coord.onHazard )
			{
				foundBadSpot = true;
				badchances.push_back(1);
				goodchances.push_back(0);
			}
			else
			{
				foundGoodSpot = true;
				badchances.push_back(0);
				goodchances.push_back(1);
			}
		}

		if ( foundGoodSpot )
		{
			auto& coord = spotsBehindMonster[local_rng.discrete(goodchances.data(), goodchances.size())];
			tx = coord.x;
			ty = coord.y;
		}
		else
		{
			auto& coord = spotsBehindMonster[local_rng.discrete(badchances.data(), badchances.size())];
			tx = coord.x;
			ty = coord.y;
		}
	}
	if ( behavior == &actPlayer )
	{
		// pretend player has teleported, get the angle needed.
		double tmpx = x;
		double tmpy = y;
		x = (tx << 4) + 8;
		y = (ty << 4) + 8;
		double tangent = atan2(target->y - this->y, target->x - this->x);
		// restore coordinates.
		x = tmpx;
		y = tmpy;
		this->yaw = tangent;
		if ( targetCrtr && targetCrtr->behavior == &actMonster && targetCrtr->monsterTarget == getUID() )
		{
			targetCrtr->monsterReleaseAttackTarget();
		}
		if ( teleport(tx, ty) )
		{
			return true;
		}
		return false;
	}

	return teleport(tx, ty);
}

void Creature::handleNPCInteractDialogue(Stat& myStats, AllyNPCChatter event)
{
	if ( multiplayer == CLIENT )
	{
		return;
	}
	if ( !isMobile() )
	{
		return;
	}
	if ( monsterAllyIndex < 0 || monsterAllyIndex >= MAXPLAYERS )
	{
		return;
	}
	if ( !stats[monsterAllyIndex] )
	{
		return;
	}

	std::basic_string<char> message;

	if ( myStats.type == HUMAN )
	{
		switch ( event )
		{
			case ALLY_EVENT_MOVEASIDE:
				message = Language::get(535);
				break;
			case ALLY_EVENT_MOVETO_BEGIN:
				if ( local_rng.rand() % 10 == 0 )
				{
					message = Language::get(3079 + local_rng.rand() % 2);
				}
				break;
			case ALLY_EVENT_MOVETO_FAIL:
				message = Language::get(3077 + local_rng.rand() % 2);
				break;
			case ALLY_EVENT_INTERACT_ITEM_CURSED:
				if ( FollowerMenu[monsterAllyIndex].entityToInteractWith && FollowerMenu[monsterAllyIndex].entityToInteractWith->behavior == &actItem )
				{
					Item* item = newItemFromEntity(FollowerMenu[monsterAllyIndex].entityToInteractWith);
					if ( item )
					{
						char fullmsg[256];
						switch ( itemCategory(item) )
						{
							case WEAPON:
							case MAGICSTAFF:
								snprintf(fullmsg, sizeof(fullmsg), Language::get(3071), Language::get(3107));
								break;
							case ARMOR:
							case TOOL:
								switch ( checkEquipType(item) )
								{
									case TYPE_OFFHAND:
										snprintf(fullmsg, sizeof(fullmsg), Language::get(3071), Language::get(3112));
										break;
									case TYPE_HELM:
									case TYPE_HAT:
									case TYPE_BREASTPIECE:
									case TYPE_BOOTS:
									case TYPE_SHIELD:
									case TYPE_GLOVES:
									case TYPE_CLOAK:
										snprintf(fullmsg, sizeof(fullmsg), Language::get(3071), Language::get(3107 + checkEquipType(item)));
										break;
									default:
										snprintf(fullmsg, sizeof(fullmsg), Language::get(3071), Language::get(3117));
										break;
								}
								break;
							case RING:
								snprintf(fullmsg, sizeof(fullmsg), Language::get(3071), Language::get(3107 + TYPE_RING));
								break;
							case AMULET:
								snprintf(fullmsg, sizeof(fullmsg), Language::get(3071), Language::get(3107 + TYPE_AMULET));
								break;
							default:
								break;
						}
						message = fullmsg;
					}
					if ( item )
					{
						free(item);
					}
				}
				break;
			case ALLY_EVENT_INTERACT_ITEM_NOUSE:
				message = Language::get(3074);
				break;
			case ALLY_EVENT_INTERACT_ITEM_FOOD_BAD:
				message = Language::get(3088);
				break;
			case ALLY_EVENT_INTERACT_ITEM_FOOD_GOOD:
				if ( myStats.HUNGER > 800 )
				{
					message = Language::get(3076);
				}
				else
				{
					message = Language::get(3075);
				}
				break;
			case ALLY_EVENT_INTERACT_ITEM_FOOD_ROTTEN:
				message = Language::get(3091);
				break;
			case ALLY_EVENT_INTERACT_ITEM_FOOD_FULL:
				message = Language::get(3089 + local_rng.rand() % 2);
				break;
			case ALLY_EVENT_INTERACT_OTHER:
				break;
			case ALLY_EVENT_ATTACK:
			case ALLY_EVENT_SPOT_ENEMY: {
				auto target = uidToEntity(monsterTarget);
				if (!target) {
					break;
				}
				auto targetstats = target->getStats();
				if (!targetstats) {
					break;
				}
				if ( local_rng.getU8() % 2 == 0 )
				{
					return; // random to not chat
				}
				char fullmsg[256] = "";
				if (local_rng.getU8() % 2) {
					snprintf(fullmsg, sizeof(fullmsg), Language::get(516 + local_rng.uniform(0, 1)),
						Language::get(4234 + local_rng.uniform(0, 16)), getMonsterLocalizedName(targetstats->type).c_str());
				} else {
					snprintf(fullmsg, sizeof(fullmsg), Language::get(518 + local_rng.uniform(0, 1)),
						Language::get(4217 + local_rng.uniform(0, 16)), getMonsterLocalizedName(targetstats->type).c_str());
				}
				message = fullmsg;
				break;
			}
			case ALLY_EVENT_ATTACK_FRIENDLY_FIRE:
				message = Language::get(3084 + local_rng.rand() % 2);
				break;
			case ALLY_EVENT_DROP_HUMAN_REFUSE:
				message = Language::get(3135);
				break;
			case ALLY_EVENT_DROP_WEAPON:
			case ALLY_EVENT_DROP_EQUIP:
			case ALLY_EVENT_DROP_ALL:
				if ( local_rng.rand() % 2 )
				{
					if ( local_rng.rand() % 2 && event == ALLY_EVENT_DROP_ALL )
					{
						message = Language::get(3083);
					}
					else
					{
						message = Language::get(3072 + local_rng.rand() % 2);
					}
				}
				else
				{
					message = Language::get(3081 + local_rng.rand() % 2);
				}
				break;
			case ALLY_EVENT_WAIT:
				message = Language::get(3069 + local_rng.rand() % 2);
				break;
			case ALLY_EVENT_FOLLOW:
				message = Language::get(526 + local_rng.rand() % 3);
				break;
			case ALLY_EVENT_MOVETO_REPATH:
				if ( local_rng.rand() % 20 == 0 )
				{
					message = Language::get(3086 + local_rng.rand() % 2);
				}
				break;
			default:
				break;
		}
		if (!message.empty())
		{
			if ( event == ALLY_EVENT_ATTACK || event == ALLY_EVENT_SPOT_ENEMY )
			{
				for ( int c = 0; c < MAXPLAYERS; ++c )
				{
					players[c]->worldUI.worldTooltipDialogue.createDialogueTooltip(getUID(),
						Player::WorldUI_t::WorldTooltipDialogue_t::DIALOGUE_ATTACK, message.c_str(), stats[monsterAllyIndex]->name);
				}
			}
			else
			{
				players[monsterAllyIndex]->worldUI.worldTooltipDialogue.createDialogueTooltip(getUID(),
					Player::WorldUI_t::WorldTooltipDialogue_t::DIALOGUE_FOLLOWER_CMD, message.c_str(), stats[monsterAllyIndex]->name);
			}
		}
	}
	else
	{
		char genericStr[128] = "";
		char namedStr[128] = "";

		switch ( event )
		{
			case ALLY_EVENT_MOVEASIDE:
				if ( local_rng.getU8() % 8 == 0 )
				{
					players[monsterAllyIndex]->worldUI.worldTooltipDialogue.createDialogueTooltip(getUID(),
						Player::WorldUI_t::WorldTooltipDialogue_t::DIALOGUE_FOLLOWER_CMD, Language::get(4262 + (int)myStats.type), stats[monsterAllyIndex]->name);
					//messagePlayerMonsterEvent(monsterAllyIndex, 0xFFFFFFFF,
					//	myStats, Language::get(3129), Language::get(3130), MSG_COMBAT);
				}
				break;
			case ALLY_EVENT_MOVETO_BEGIN:
				/*if ( local_rng.rand() % 10 == 0 )
				{
					message = Language::get(3079 + local_rng.rand() % 2);
				}*/
				break;
			case ALLY_EVENT_MOVETO_FAIL:
				if ( local_rng.rand() % 2 == 0 )
				{
					messagePlayerMonsterEvent(monsterAllyIndex, 0xFFFFFFFF,
						myStats, Language::get(3131), Language::get(3132), MSG_COMBAT);
				}
				else
				{
					messagePlayerMonsterEvent(monsterAllyIndex, 0xFFFFFFFF,
						myStats, Language::get(3133), Language::get(3134), MSG_COMBAT);
				}
				break;
			case ALLY_EVENT_INTERACT_ITEM_CURSED:
				if ( FollowerMenu[monsterAllyIndex].entityToInteractWith && FollowerMenu[monsterAllyIndex].entityToInteractWith->behavior == &actItem )
				{
					Item* item = newItemFromEntity(FollowerMenu[monsterAllyIndex].entityToInteractWith);
					if ( item )
					{
						char fullmsg[256] = "";
						switch ( itemCategory(item) )
						{
							case WEAPON:
							case MAGICSTAFF:
								snprintf(fullmsg, sizeof(fullmsg), Language::get(3118), Language::get(3107));
								break;
							case ARMOR:
							case TOOL:
								switch ( checkEquipType(item) )
								{
									case TYPE_OFFHAND:
										snprintf(fullmsg, sizeof(fullmsg), Language::get(3118), Language::get(3112));
										break;
									case TYPE_HELM:
									case TYPE_HAT:
									case TYPE_BREASTPIECE:
									case TYPE_BOOTS:
									case TYPE_SHIELD:
									case TYPE_GLOVES:
									case TYPE_CLOAK:
										snprintf(fullmsg, sizeof(fullmsg), Language::get(3118), Language::get(3107 + checkEquipType(item)));
										break;
									default:
										snprintf(fullmsg, sizeof(fullmsg), Language::get(3118), Language::get(3117));
										break;
								}
								break;
							case RING:
								snprintf(fullmsg, sizeof(fullmsg), Language::get(3118), Language::get(3107 + TYPE_RING));
								break;
							case AMULET:
								snprintf(fullmsg, sizeof(fullmsg), Language::get(3118), Language::get(3107 + TYPE_AMULET));
								break;
							default:
								break;
						}
						if ( strcmp(fullmsg, "") )
						{
							char genericStr[128];
							strcpy(genericStr, Language::get(3119));
							strcat(genericStr, fullmsg);
							char namedStr[128];
							strcpy(namedStr, Language::get(3120));
							strcat(namedStr, fullmsg);
							messagePlayerMonsterEvent(monsterAllyIndex, 0xFFFFFFFF,
								myStats, genericStr, namedStr, MSG_COMBAT);
						}
					}
					if ( item )
					{
						free(item);
					}
				}
				break;
			case ALLY_EVENT_INTERACT_ITEM_NOUSE:
			case ALLY_EVENT_INTERACT_ITEM_FOOD_FULL:
			{
				Item* item = newItemFromEntity(FollowerMenu[monsterAllyIndex].entityToInteractWith);
				if ( item )
				{
					char itemString[64] = "";
					snprintf(itemString, 63, Language::get(3123), items[item->type].getUnidentifiedName());
					strcpy(genericStr, Language::get(3121));
					strcat(genericStr, itemString);
					strcpy(namedStr, Language::get(3122));
					strcat(namedStr, itemString);
					messagePlayerMonsterEvent(monsterAllyIndex, 0xFFFFFFFF,
						myStats, genericStr, namedStr, MSG_COMBAT);
					free(item);
				}
				break;
			}
			case ALLY_EVENT_INTERACT_ITEM_FOOD_BAD:
			case ALLY_EVENT_INTERACT_ITEM_FOOD_GOOD:
			case ALLY_EVENT_INTERACT_ITEM_FOOD_ROTTEN:
			{
				Item* item = newItemFromEntity(FollowerMenu[monsterAllyIndex].entityToInteractWith);
				if ( item )
				{
					char itemString[64] = "";
					snprintf(itemString, 63, Language::get(3124), items[item->type].getUnidentifiedName());
					strcpy(genericStr, Language::get(3121));
					strcat(genericStr, itemString);
					strcpy(namedStr, Language::get(3122));
					strcat(namedStr, itemString);
					messagePlayerMonsterEvent(monsterAllyIndex, 0xFFFFFFFF,
						myStats, genericStr, namedStr, MSG_COMBAT);
					free(item);
				}
				break;
			}
			case ALLY_EVENT_INTERACT_OTHER:
				break;
			case ALLY_EVENT_ATTACK:
			case ALLY_EVENT_SPOT_ENEMY:
				break;
			case ALLY_EVENT_ATTACK_FRIENDLY_FIRE:
				break;
			case ALLY_EVENT_DROP_WEAPON:
				strcpy(genericStr, Language::get(3121));
				strcat(genericStr, Language::get(3125));
				strcpy(namedStr, Language::get(3122));
				strcat(namedStr, Language::get(3125));
				messagePlayerMonsterEvent(monsterAllyIndex, 0xFFFFFFFF,
					myStats, genericStr, namedStr, MSG_COMBAT);
				break;
			case ALLY_EVENT_DROP_EQUIP:
				strcpy(genericStr, Language::get(3121));
				strcat(genericStr, Language::get(3126));
				strcpy(namedStr, Language::get(3122));
				strcat(namedStr, Language::get(3126));
				messagePlayerMonsterEvent(monsterAllyIndex, 0xFFFFFFFF,
					myStats, genericStr, namedStr, MSG_COMBAT);
				break;
			case ALLY_EVENT_DROP_ALL:
				strcpy(genericStr, Language::get(3121));
				strcat(genericStr, Language::get(3127));
				strcpy(namedStr, Language::get(3122));
				strcat(namedStr, Language::get(3127));
				messagePlayerMonsterEvent(monsterAllyIndex, 0xFFFFFFFF,
					myStats, genericStr, namedStr, MSG_COMBAT);
				break;
			case ALLY_EVENT_WAIT:
				if ( myStats.type == SENTRYBOT || myStats.type == SPELLBOT )
				{
					messagePlayerColor(monsterAllyIndex, MESSAGE_STATUS, 0xFFFFFFFF,
					    Language::get(3676), monstertypename[myStats.type]);
				}
				else
				{
					messagePlayerMonsterEvent(monsterAllyIndex, 0xFFFFFFFF,
						myStats, Language::get(3105), Language::get(3106), MSG_COMBAT);
				}
				break;
			case ALLY_EVENT_FOLLOW:
				if ( myStats.type == SENTRYBOT || myStats.type == SPELLBOT )
				{
					messagePlayerColor(monsterAllyIndex, MESSAGE_STATUS, 0xFFFFFFFF,
					    Language::get(3677), monstertypename[myStats.type]);
				}
				else
				{
					messagePlayerMonsterEvent(monsterAllyIndex, 0xFFFFFFFF,
						myStats, Language::get(529), Language::get(3128), MSG_COMBAT);
				}
				break;
			case ALLY_EVENT_MOVETO_REPATH:
				/*if ( local_rng.rand() % 20 == 0 )
				{
					message = Language::get(3086 + local_rng.rand() % 2);
				}*/
				break;
			default:
				break;
		}
	}
}

int Creature::shouldMonsterDefend(Stat& myStats, const Entity& target, const Stat& targetStats, int targetDist, bool hasrangedweapon)
{
	if ( behavior != &actMonster )
	{
		return MONSTER_DEFEND_NONE;
	}

	if ( !myStats.shield )
	{
		return MONSTER_DEFEND_NONE;
	}

	if ( itemTypeIsQuiver(myStats.shield->type) )
	{
		return MONSTER_DEFEND_NONE;
	}

	if ( monsterSpecialState > 0 )
	{
		return MONSTER_DEFEND_NONE;
	}

	if ( myStats.type == LICH
		|| myStats.type == DEVIL
		|| myStats.type == LICH_ICE
		|| myStats.type == LICH_FIRE
		|| myStats.type == SHOPKEEPER
		)
	{
		return MONSTER_DEFEND_NONE;
	}

	bool isPlayerAlly = (monsterAllyIndex >= 0 && monsterAllyIndex < MAXPLAYERS);

	if ( !(isPlayerAlly || myStats.type == HUMAN) )
	{
		return MONSTER_DEFEND_NONE;
	}

	int blockChance = 2; // 10%
	bool targetHasRangedWeapon = target.hasRangedWeapon();

	if ( isPlayerAlly )
	{
		if ( stats[monsterAllyIndex] && players[monsterAllyIndex] && players[monsterAllyIndex]->entity )
		{
			int leaderSkill = std::max(players[monsterAllyIndex]->entity->getCHR(), 0) + stats[monsterAllyIndex]->PROFICIENCIES[PRO_LEADERSHIP];
			blockChance += std::max(0, (leaderSkill / 20) * 2); // 0-25% bonus to blockchance.
		}
	}

	if ( myStats.PROFICIENCIES[PRO_SHIELD] > 0 )
	{
		blockChance += std::min(myStats.PROFICIENCIES[PRO_SHIELD] / 10, 5); // 0-25% bonus to blockchance.
	}

	bool retreatBonus = false;
	if ( backupWithRangedWeapon(myStats, targetDist, hasrangedweapon) )
	{
		if ( targetHasRangedWeapon )
		{
			retreatBonus = true;
		}
		else if ( !targetHasRangedWeapon && targetDist < TOUCHRANGE )
		{
			retreatBonus = true;
		}
	}
	if ( shouldRetreat(myStats) )
	{
		retreatBonus = true;
	}

	if ( retreatBonus )
	{
		blockChance += 2; // 10% bonus to blockchance.
	}

	blockChance = std::min(blockChance, 12); // 60% block hard cap.

	if ( targetHasRangedWeapon && targetStats.weapon )
	{
		if ( itemCategory(targetStats.weapon) == MAGICSTAFF )
		{
			if ( myStats.shield->type != MIRROR_SHIELD )
			{
				// no point defending!
				blockChance = 0;
			}
		}
	}

	if ( local_rng.rand() % 20 < blockChance )
	{
		if ( isPlayerAlly || myStats.type == HUMAN )
		{
			if ( local_rng.rand() % 4 == 0 )
			{
				return MONSTER_DEFEND_HOLD;
			}
			else
			{
				return MONSTER_DEFEND_ALLY;
			}
		}
		else
		{
			return MONSTER_DEFEND_HOLD;
		}
	}

	return false;
}

bool Creature::monsterConsumeFoodEntity(Entity* food, Stat* myStats)
{
	if ( !myStats )
	{
		return false;
	}

	if ( !food )
	{
		return false;
	}

	Item* item = newItemFromEntity(food);
	if ( !item || item->type == FOOD_TIN )
	{
		return false;
	}

	if ( !FollowerRadialMenu::allowedInteractFood(myStats->type) )
	{
		handleNPCInteractDialogue(*myStats, ALLY_EVENT_INTERACT_ITEM_NOUSE);
		return false;
	}

	if ( myStats->HUNGER >= 800 )
	{
		handleNPCInteractDialogue(*myStats, ALLY_EVENT_INTERACT_ITEM_FOOD_FULL);
		return false;
	}

	int buffDuration = item->status * TICKS_PER_SECOND * 2; // (2 - 8 seconds)
	bool puking = false;
	int pukeChance = 100;
	// chance of rottenness
	if ( myStats->type == HUMAN )
	{
		switch ( item->status )
		{
			case EXCELLENT:
				pukeChance = 100;
				break;
			case SERVICABLE:
				pukeChance = 25;
				break;
			case WORN:
				pukeChance = 10;
				break;
			case DECREPIT:
				pukeChance = 4;
				break;
			default:
				pukeChance = 100;
				break;
		}
	}

	if ( local_rng.rand() % pukeChance == 0 && pukeChance < 100 )
	{
		buffDuration = 0;
		this->char_gonnavomit = 40 + local_rng.rand() % 10;
		puking = true;
	}
	else
	{
		if ( item->beatitude >= 0 )
		{
			if ( item->status > WORN )
			{
				buffDuration -= local_rng.rand() % ((buffDuration / 2) + 1); // 50-100% duration
			}
			else
			{
				buffDuration -= local_rng.rand() % ((buffDuration / 4) + 1); // 75-100% duration
			}
		}
		else
		{
			buffDuration = 0;
		}
	}

	if ( puking )
	{
		handleNPCInteractDialogue(*myStats, ALLY_EVENT_INTERACT_ITEM_FOOD_ROTTEN);
	}
	else if ( item->status <= WORN )
	{
		handleNPCInteractDialogue(*myStats, ALLY_EVENT_INTERACT_ITEM_FOOD_BAD);
	}
	else
	{
		handleNPCInteractDialogue(*myStats, ALLY_EVENT_INTERACT_ITEM_FOOD_GOOD);
	}

	int heal = 0;
	switch ( item->type )
	{
		case FOOD_APPLE:
			heal = 5 + item->beatitude;
			buffDuration = std::min(buffDuration, 2 * TICKS_PER_SECOND);
			myStats->HUNGER += 200;
			break;
		case FOOD_BREAD:
			heal = 7 + item->beatitude;
			buffDuration = std::min(buffDuration, 6 * TICKS_PER_SECOND);
			myStats->HUNGER += 400;
			break;
		case FOOD_CHEESE:
			heal = 3 + item->beatitude;
			buffDuration = std::min(buffDuration, 2 * TICKS_PER_SECOND);
			myStats->HUNGER += 100;
			break;
		case FOOD_CREAMPIE:
			heal = 7 + item->beatitude;
			buffDuration = std::min(buffDuration, 6 * TICKS_PER_SECOND);
			myStats->HUNGER += 200;
			break;
		case FOOD_FISH:
			heal = 7 + item->beatitude;
			myStats->HUNGER += 500;
			break;
		case FOOD_MEAT:
			heal = 9 + item->beatitude;
			myStats->HUNGER += 600;
			break;
		case FOOD_TOMALLEY:
			heal = 7 + item->beatitude;
			buffDuration = std::min(buffDuration, 4 * TICKS_PER_SECOND);
			myStats->HUNGER += 400;
			break;
		default:
			free(item);
			handleNPCInteractDialogue(*myStats, ALLY_EVENT_INTERACT_ITEM_NOUSE);
			return false;
			break;
	}

	myStats->HUNGER = std::min(myStats->HUNGER, 1500); // range checking max of 1500 hunger points.

	Entity* leader = monsterAllyGetPlayerLeader();
	if ( puking )
	{
		heal -= 5;
		if ( local_rng.rand() % 4 == 0 )
		{
			// angry at owner.
			if ( leader )
			{
				//monsterAcquireAttackTarget(*leader, MONSTER_STATE_ATTACK);
			}
		}
	}
	this->modHP(heal);

	if ( !puking && leader && local_rng.rand() % 2 == 0 )
	{
		leader->increaseSkill(PRO_LEADERSHIP);
	}

	if ( buffDuration > 0 )
	{
		myStats->EFFECTS[EFF_HP_REGEN] = true;
		myStats->EFFECTS_TIMERS[EFF_HP_REGEN] = buffDuration;
	}

	bool foodEntityConsumed = false;

	if ( item->count > 1 )
	{
		--food->skill[13]; // update the entity on ground item count.
	}
	else
	{
		food->removeLightField();
		list_RemoveNode(food->mynode);
		foodEntityConsumed = true;
	}
	free(item);

	// eating sound
	playSoundEntity(this, 50 + local_rng.rand() % 2, 64);

	return foodEntityConsumed;
}

/*-------------------------------------------------------------------------------

Creature::checkFriend

Returns true if my and your are friends, otherwise returns false

-------------------------------------------------------------------------------*/

bool Creature::checkFriend(Entity* your)
{
	bool result = false;

    Creature* yourCrtr = (Creature*)(*&your);
	if ( !your )
	{
		return false;    //Equivalent to if (!myStats || !yourStats)
	}

	Stat* myStats = getStats();
	Stat* yourStats = your->getStats();

	if ( !myStats || !yourStats )
	{
		return false;
	}

	if ( everybodyfriendly || intro )   // friendly monsters mode
	{
		return true;
	}

	if ( ((yourCrtr && yourCrtr->behavior == &actPlayer) || your->behavior == &actPlayerLimb) && (behavior == &actPlayer) )
	{
		return true;
	}

	if ( behavior == &actPlayer && yourCrtr && yourCrtr->behavior == &actMonster && yourStats->monsterForceAllegiance != Stat::MONSTER_FORCE_ALLEGIANCE_NONE )
	{
		if ( yourStats->monsterForceAllegiance == Stat::MONSTER_FORCE_PLAYER_ALLY || yourStats->monsterForceAllegiance == Stat::MONSTER_FORCE_PLAYER_RECRUITABLE )
		{
			return true;
		}
		else if ( yourStats->monsterForceAllegiance == Stat::MONSTER_FORCE_PLAYER_ENEMY )
		{
			return false;
		}
	}
	else if ( yourCrtr && yourCrtr->behavior == &actPlayer && behavior == &actMonster && myStats->monsterForceAllegiance != Stat::MONSTER_FORCE_ALLEGIANCE_NONE )
	{
		if ( myStats->monsterForceAllegiance == Stat::MONSTER_FORCE_PLAYER_ALLY || myStats->monsterForceAllegiance == Stat::MONSTER_FORCE_PLAYER_RECRUITABLE )
		{
			return true;
		}
		else if ( myStats->monsterForceAllegiance == Stat::MONSTER_FORCE_PLAYER_ENEMY )
		{
			return false;
		}
	}


	if ( (myStats->type == SHOPKEEPER && myStats->MISC_FLAGS[STAT_FLAG_MYSTERIOUS_SHOPKEEP] > 0)
		|| (yourStats->type == SHOPKEEPER && yourStats->MISC_FLAGS[STAT_FLAG_MYSTERIOUS_SHOPKEEP] > 0) )
	{
		return false;
	}

	if ( myStats->type == GYROBOT )
	{
		return true;
	}

	if ( (myStats->type == HUMAN || behavior == &actPlayer) && (yourStats->type == AUTOMATON && !strncmp(yourStats->name, "corrupted automaton", 19)) )
	{
		return false;
	}
	else if ( (yourStats->type == HUMAN || (yourCrtr && yourCrtr->behavior == &actPlayer)) && (myStats->type == AUTOMATON && !strncmp(myStats->name, "corrupted automaton", 19)) )
	{
		return false;
	}
	else if ( (yourCrtr && yourCrtr->behavior == &actPlayer) && myStats->type == CREATURE_IMP
		&& (!strncmp(map.name, "Boss", 4) || !strncmp(map.name, "Hell Boss", 9)) )
	{
		if ( this->monsterAllyGetPlayerLeader() )
		{
			return true;
		}
		return false;
	}
	else if ( behavior == &actPlayer && yourStats->type == CREATURE_IMP
		&& (!strncmp(map.name, "Boss", 4) || !strncmp(map.name, "Hell Boss", 9)) )
	{
		if ( your->monsterAllyGetPlayerLeader() )
		{
			return true;
		}
		return false;
	}
	else if ( (yourCrtr && yourCrtr->behavior == &actPlayer) && myStats->type == VAMPIRE && MonsterData_t::nameMatchesSpecialNPCName(*myStats, "bram kindly") )
	{
		return false;
	}
	else if ( behavior == &actPlayer && yourStats->type == VAMPIRE && MonsterData_t::nameMatchesSpecialNPCName(*yourStats, "bram kindly") )
	{
		return false;
	}
	else if ( behavior == &actMonster && myStats->type == INCUBUS && !strncmp(myStats->name, "inner demon", strlen("inner demon")) )
	{
		Entity* parentEntity = uidToEntity(this->parent);
		if ( parentEntity == your )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if ( behavior == &actPlayer && yourCrtr && yourCrtr->behavior == &actMonster && yourStats->type == INCUBUS && !strncmp(yourStats->name, "inner demon", strlen("inner demon")) )
	{
		Entity* parentEntity = uidToEntity(your->parent);
		if ( parentEntity == this )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if ( behavior == &actMonster && yourCrtr && yourCrtr->behavior == &actMonster && yourStats->type == INCUBUS && !strncmp(yourStats->name, "inner demon", strlen("inner demon")) )
	{
		Entity* illusionTauntingThisEntity = uidToEntity(static_cast<uint32_t>(yourCrtr->monsterIllusionTauntingThisUid));
		if ( illusionTauntingThisEntity == this )
		{
			return false;
		}
	}

	// if you have a leader, check whether we are friends instead
	Entity* yourLeader = NULL;
	if ( yourStats->leader_uid )
	{
		yourLeader = uidToEntity(yourStats->leader_uid);
	}
	if ( yourLeader )
	{
		Stat* yourLeaderStats = yourLeader->getStats();
		if ( yourLeaderStats )
		{
			if ( yourLeader == this )
			{
				return true;
			}
			else
			{
				return checkFriend(yourLeader);
			}
		}
	}

	// first find out if I have a leader
	Creature* myLeader = NULL;
	if ( myStats->leader_uid )
	{
		myLeader = uidToCreature(myStats->leader_uid);
	}
	if ( myLeader )
	{
		Stat* myLeaderStats = myLeader->getStats();
		if ( myLeaderStats )
		{
			if ( myLeader == your )
			{
				result = true;
			}
			else
			{
				return myLeader->checkFriend(your);
			}
		}
		else
		{
			// invalid leader, default to allegiance table
			result = monsterally[myStats->type][yourStats->type];
		}
	}
	else
	{
		node_t * t_node;
		bool foundFollower = false;
		for ( t_node = myStats->FOLLOWERS.first; t_node != NULL; t_node = t_node->next )
		{
			uint32_t * uid = (uint32_t *)t_node->element;
			if ( *uid == your->uid )
			{
				foundFollower = true;
				result = true;
				break;
			}
		}
		if ( !foundFollower )
		{
			// no leader, default to allegiance table
			result = monsterally[myStats->type][yourStats->type];

			// player exceptions to table go here.
			if ( myStats->type == SHOPKEEPER && yourCrtr && yourCrtr->behavior == &actPlayer )
			{
				result = !ShopkeeperPlayerHostility.isPlayerEnemy(your->skill[2]);
			}
			else if ( yourStats->type == SHOPKEEPER && behavior == &actPlayer )
			{
				result = !ShopkeeperPlayerHostility.isPlayerEnemy(this->skill[2]);
			}
			else if ( behavior == &actPlayer && myStats->type != HUMAN )
			{
				result = monsterally[HUMAN][yourStats->type];
				if ( (yourStats->type == HUMAN || yourStats->type == SHOPKEEPER) && myStats->type != AUTOMATON )
				{
					result = false;
				}
				else
				{
					result = false;
					switch ( myStats->type )
					{
						case SKELETON:
							if ( yourStats->type == GHOUL )
							{
								result = true;
							}
							break;
						case RAT:
							if ( yourStats->type == RAT )
							{
								result = true;
							}
							break;
						case SPIDER:
							if ( yourStats->type == SPIDER || yourStats->type == SCARAB || yourStats->type == SCORPION )
							{
								result = true;
							}
							break;
						case TROLL:
							if ( yourStats->type == TROLL )
							{
								result = true;
							}
							break;
						case CREATURE_IMP:
							if ( yourStats->type == CREATURE_IMP )
							{
								result = true;
							}
							break;
						case GOBLIN:
							if ( yourStats->type == GOBLIN )
							{
								result = true;
							}
							break;
						case GOATMAN:
							if ( yourStats->type == GOATMAN )
							{
								result = true;
							}
							break;
						case INCUBUS:
						case SUCCUBUS:
							if ( yourStats->type == SUCCUBUS || yourStats->type == INCUBUS )
							{
								result = true;
							}
							break;
						case INSECTOID:
							if ( yourStats->type == SCARAB
								|| yourStats->type == INSECTOID || yourStats->type == SCORPION )
							{
								result = true;
							}
							break;
						case VAMPIRE:
							if ( yourStats->type == VAMPIRE )
							{
								result = true;
							}
							break;
						case AUTOMATON:
							if ( yourStats->type == SHOPKEEPER )
							{
								result = monsterally[SHOPKEEPER][AUTOMATON];
							}
							else if ( yourStats->type == HUMAN )
							{
								result = true;
							}
							break;
						default:
							break;
					}
				}
			}
			else if ( behavior == &actMonster && yourCrtr && yourCrtr->behavior == &actPlayer && yourStats->type != HUMAN )
			{
				result = monsterally[myStats->type][HUMAN];
				if ( (myStats->type == HUMAN || myStats->type == SHOPKEEPER) && yourStats->type != AUTOMATON )
				{
					result = false;
				}
				else
				{
					switch ( yourStats->type )
					{
						case SKELETON:
							if ( myStats->type == GHOUL )
							{
								result = true;
							}
							break;
						case RAT:
							if ( myStats->type == RAT )
							{
								result = true;
							}
							break;
						case SPIDER:
							if ( myStats->type == SPIDER
								|| myStats->type == SCARAB || myStats->type == SCORPION )
							{
								result = true;
							}
							break;
						case TROLL:
							if ( myStats->type == TROLL )
							{
								result = true;
							}
							break;
						case CREATURE_IMP:
							if ( myStats->type == CREATURE_IMP )
							{
								result = true;
							}
							break;
						case GOBLIN:
							if ( myStats->type == GOBLIN )
							{
								result = true;
							}
							break;
						case GOATMAN:
							if ( myStats->type == GOATMAN )
							{
								result = true;
							}
							break;
						case INCUBUS:
						case SUCCUBUS:
							if ( myStats->type == SUCCUBUS || myStats->type == INCUBUS )
							{
								result = true;
							}
							break;
						case INSECTOID:
							if ( myStats->type == SCARAB
								|| myStats->type == INSECTOID || myStats->type == SCORPION )
							{
								result = true;
							}
							break;
						case VAMPIRE:
							if ( myStats->type == VAMPIRE )
							{
								result = true;
							}
							break;
						case AUTOMATON:
							if ( myStats->type == SHOPKEEPER )
							{
								result = monsterally[SHOPKEEPER][AUTOMATON];
							}
							else if ( myStats->type == HUMAN )
							{
								result = true;
							}
							break;
						default:
							break;
					}
				}
			}
		}
	}

	return result;
}

bool Creature::isFollowerFreeToPathToPlayer(Stat* myStats)
{
	Entity* currentTarget = uidToEntity(monsterTarget);
	if ( currentTarget )
	{
		if ( monsterState == MONSTER_STATE_ATTACK )
		{
			// fighting something.
			return false;
		}
		else if ( entityDist(this, currentTarget) < TOUCHRANGE * 2 )
		{
			// in the vicinity of our target
			return false;
		}
	}
	return true;
}

int Creature::getAttackPose() const
{
	Stat *myStats = getStats();
	if ( !myStats )
	{
		return -1;
	}

	int pose = 0;

	if ( myStats->weapon != nullptr )
	{
		if ( myStats->type == LICH_FIRE )
		{
			switch ( monsterLichFireMeleeSeq )
			{
				case LICH_ATK_VERTICAL_SINGLE:
					pose = MONSTER_POSE_MELEE_WINDUP1;
					break;
				case LICH_ATK_HORIZONTAL_SINGLE:
					pose = MONSTER_POSE_MELEE_WINDUP2;
					break;
				case LICH_ATK_RISING_RAIN:
					pose = MONSTER_POSE_SPECIAL_WINDUP1;
					break;
				case LICH_ATK_BASICSPELL_SINGLE:
					pose = MONSTER_POSE_MAGIC_WINDUP1;
					break;
				case LICH_ATK_RISING_SINGLE:
					pose = MONSTER_POSE_MELEE_WINDUP3;
					break;
				case LICH_ATK_VERTICAL_QUICK:
					pose = MONSTER_POSE_MELEE_WINDUP1;
					break;
				case LICH_ATK_HORIZONTAL_RETURN:
					pose = MONSTER_POSE_MELEE_WINDUP2;
					break;
				case LICH_ATK_HORIZONTAL_QUICK:
					pose = MONSTER_POSE_MELEE_WINDUP2;
					break;
				case LICH_ATK_SUMMON:
					pose = MONSTER_POSE_MAGIC_WINDUP3;
					break;
				default:
					break;
			}
		}
		else if ( myStats->type == LICH_ICE )
		{
			switch ( monsterLichIceCastSeq )
			{
				case LICH_ATK_VERTICAL_SINGLE:
					pose = MONSTER_POSE_MELEE_WINDUP1;
					break;
				case LICH_ATK_HORIZONTAL_SINGLE:
					pose = MONSTER_POSE_MELEE_WINDUP2;
					break;
				case LICH_ATK_RISING_RAIN:
					pose = MONSTER_POSE_SPECIAL_WINDUP1;
					break;
				case LICH_ATK_BASICSPELL_SINGLE:
					pose = MONSTER_POSE_MAGIC_WINDUP1;
					break;
				case LICH_ATK_RISING_SINGLE:
					pose = MONSTER_POSE_MELEE_WINDUP1;
					break;
				case LICH_ATK_VERTICAL_QUICK:
					pose = MONSTER_POSE_MELEE_WINDUP1;
					break;
				case LICH_ATK_HORIZONTAL_RETURN:
					pose = MONSTER_POSE_MELEE_WINDUP2;
					break;
				case LICH_ATK_HORIZONTAL_QUICK:
					pose = MONSTER_POSE_MELEE_WINDUP2;
					break;
				case LICH_ATK_CHARGE_AOE:
					pose = MONSTER_POSE_SPECIAL_WINDUP2;
					break;
				case LICH_ATK_FALLING_DIAGONAL:
					pose = MONSTER_POSE_SPECIAL_WINDUP3;
					break;
				case LICH_ATK_SUMMON:
					pose = MONSTER_POSE_MAGIC_WINDUP3;
					break;
				default:
					break;
			}
		}
		else if ( myStats->type == SENTRYBOT )
		{
			pose = MONSTER_POSE_RANGED_WINDUP1;
		}
		else if ( myStats->type == SPELLBOT )
		{
			pose = MONSTER_POSE_MAGIC_WINDUP1;
		}
		else if ( itemCategory(myStats->weapon) == MAGICSTAFF )
		{
			if ( myStats->type == KOBOLD || myStats->type == AUTOMATON
				|| myStats->type == GOATMAN || myStats->type == INSECTOID
				|| myStats->type == INCUBUS || myStats->type == VAMPIRE
				|| myStats->type == HUMAN || myStats->type == GOBLIN
				|| myStats->type == SKELETON || myStats->type == GNOME
				|| myStats->type == SUCCUBUS || myStats->type == SHOPKEEPER
				|| myStats->type == SHADOW )
			{
				pose = MONSTER_POSE_MELEE_WINDUP1;
			}
			else
			{
				pose = 3;  // jab
			}
		}
		else if ( itemCategory(myStats->weapon) == SPELLBOOK )
		{
			if ( myStats->type == INSECTOID && this->monsterSpecialTimer == MONSTER_SPECIAL_COOLDOWN_INSECTOID_ACID )
			{
				pose = MONSTER_POSE_MAGIC_WINDUP3;
			}
			else if ( myStats->type == INCUBUS && this->monsterSpecialTimer == MONSTER_SPECIAL_COOLDOWN_INCUBUS_STEAL )
			{
				pose = MONSTER_POSE_MAGIC_WINDUP3;
			}
			else if ( myStats->type == COCKATRICE && this->monsterSpecialTimer == MONSTER_SPECIAL_COOLDOWN_COCKATRICE_STONE )
			{
				pose = MONSTER_POSE_MAGIC_WINDUP2;
			}
			else if ( myStats->type == VAMPIRE )
			{
				if ( this->monsterSpecialTimer == MONSTER_SPECIAL_COOLDOWN_VAMPIRE_DRAIN )
				{
					pose = MONSTER_POSE_VAMPIRE_DRAIN;
				}
				else if ( this->monsterSpecialTimer == MONSTER_SPECIAL_COOLDOWN_VAMPIRE_AURA )
				{
					pose = MONSTER_POSE_VAMPIRE_AURA_CHARGE;
				}
				else
				{
					pose = MONSTER_POSE_MAGIC_WINDUP1;
				}
			}
			else if ( myStats->type == KOBOLD || myStats->type == AUTOMATON
				|| myStats->type == GOATMAN || myStats->type == INSECTOID
				|| myStats->type == COCKATRICE || myStats->type == INCUBUS
				|| myStats->type == VAMPIRE || myStats->type == HUMAN
				|| myStats->type == GOBLIN || myStats->type == SKELETON
				|| myStats->type == GNOME || myStats->type == SUCCUBUS
				|| myStats->type == SPIDER
				|| myStats->type == CRAB
				|| myStats->type == SHOPKEEPER || myStats->type == SHADOW )
			{
				pose = MONSTER_POSE_MAGIC_WINDUP1;
			}
			else if ( myStats->type == DEMON || myStats->type == CREATURE_IMP || myStats->type == GHOUL )
			{
				pose = MONSTER_POSE_MELEE_WINDUP1;
			}
			else
			{
				pose = 1;  // vertical swing
			}
		}
		else if ( itemCategory(myStats->weapon) == POTION )
		{
			if ( myStats->type == GOATMAN )
			{
				/*if ( this->monsterSpecialTimer == MONSTER_SPECIAL_COOLDOWN_GOATMAN_DRINK )
				{
					pose = MONSTER_POSE_RANGED_WINDUP3;
				}
				else if ( this->monsterSpecialTimer == MONSTER_SPECIAL_COOLDOWN_GOATMAN_THROW )
				{
					pose = MONSTER_POSE_MELEE_WINDUP1;
				}*/
				if ( monsterSpecialState == GOATMAN_POTION )
				{
					pose = MONSTER_POSE_RANGED_WINDUP3;
				}

			}
			else if ( myStats->type == INCUBUS )
			{
				if ( this->monsterSpecialTimer == MONSTER_SPECIAL_COOLDOWN_INCUBUS_CONFUSION )
				{
					pose = MONSTER_POSE_SPECIAL_WINDUP1;
				}
			}
			else
			{
				pose = MONSTER_POSE_MELEE_WINDUP1;
			}
		}
		else if ( this->hasRangedWeapon() )
		{
			if ( myStats->type == KOBOLD || myStats->type == AUTOMATON
				|| myStats->type == GOATMAN || myStats->type == INSECTOID
				|| myStats->type == INCUBUS || myStats->type == VAMPIRE
				|| myStats->type == HUMAN || myStats->type == GOBLIN
				|| myStats->type == SKELETON || myStats->type == GNOME
				|| myStats->type == SUCCUBUS || myStats->type == SHOPKEEPER
				|| myStats->type == SHADOW )
			{
				if ( myStats->weapon->type == CROSSBOW || myStats->weapon->type == HEAVY_CROSSBOW )
				{
					pose = MONSTER_POSE_RANGED_WINDUP1;
				}
				else if ( itemCategory(myStats->weapon) == THROWN )
				{
					if ( myStats->type == INSECTOID )
					{
						if ( this->monsterSpecialTimer == MONSTER_SPECIAL_COOLDOWN_INSECTOID_THROW )
						{
							pose = MONSTER_POSE_RANGED_WINDUP3;
						}
						else
						{
							pose = MONSTER_POSE_MELEE_WINDUP1;
						}
					}
					else
					{
						pose = MONSTER_POSE_MELEE_WINDUP1;
					}
				}
				else
				{
					pose = MONSTER_POSE_RANGED_WINDUP2;
				}
			}
			else
			{
				pose = 0;
			}
		}
		else
		{
			if ( myStats->type == KOBOLD || myStats->type == AUTOMATON
				|| myStats->type == GOATMAN || myStats->type == INSECTOID
				|| myStats->type == INCUBUS || myStats->type == VAMPIRE
				|| myStats->type == HUMAN || myStats->type == GOBLIN
				|| myStats->type == SKELETON || myStats->type == GNOME
				|| myStats->type == SUCCUBUS || myStats->type == SHOPKEEPER
				|| myStats->type == SHADOW )
			{
				if ( getWeaponSkill(myStats->weapon) == PRO_AXE || getWeaponSkill(myStats->weapon) == PRO_MACE
					|| myStats->weapon->type == TOOL_WHIP )
				{
					// axes and maces don't stab
					pose = MONSTER_POSE_MELEE_WINDUP1 + local_rng.rand() % 2;
				}
				else
				{
					pose = MONSTER_POSE_MELEE_WINDUP1 + local_rng.rand() % 3;
				}
			}
			else
			{
				pose = local_rng.rand() % 3 + 1;
			}
		}
	}
	// fists
	else
	{
	    const auto type = myStats->type;
		if (type == KOBOLD || type == AUTOMATON ||
			type == GOATMAN || type == INSECTOID ||
			type == INCUBUS || type == VAMPIRE ||
			type == HUMAN || type == GOBLIN ||
			type == GHOUL || type == SKELETON ||
			type == GNOME || type == DEMON ||
			type == CREATURE_IMP || type == SUCCUBUS ||
			type == SHOPKEEPER || type == MINOTAUR ||
			type == SHADOW || type == RAT || type == SPIDER || type == CRAB ||
			type == SLIME || (type == SCARAB && sprite != 1078 && sprite != 1079))
		{
			pose = MONSTER_POSE_MELEE_WINDUP1;
		}
		else if ( myStats->type == CRYSTALGOLEM )
		{
			if ( this->monsterSpecialTimer == MONSTER_SPECIAL_COOLDOWN_GOLEM )
			{
				pose = MONSTER_POSE_MELEE_WINDUP3;
			}
			else
			{
				pose = MONSTER_POSE_MELEE_WINDUP1 + local_rng.rand() % 2;
			}
		}
		else if ( myStats->type == COCKATRICE )
		{
			if ( this->monsterSpecialTimer == MONSTER_SPECIAL_COOLDOWN_COCKATRICE_ATK )
			{
				pose = MONSTER_POSE_MELEE_WINDUP3;
			}
			else
			{
				pose = MONSTER_POSE_MELEE_WINDUP1 + local_rng.rand() % 2;
			}
		}
		else if ( myStats->type == TROLL )
		{
			pose = MONSTER_POSE_MELEE_WINDUP1;
		}
		else
		{
			pose = 1;
		}
	}

	return pose;
}

void Creature::humanoidAnimateWalk(Entity* limb, node_t * bodypartNode, int bodypart, double walkSpeed, double dist, double distForFootstepSound)
{
	if ( bodypart == LIMB_HUMANOID_RIGHTLEG || bodypart == LIMB_HUMANOID_LEFTARM )
	{
		Entity* rightbody = nullptr;
		// set rightbody to left leg.
		node_t * rightbodyNode = list_Node(&this->children, LIMB_HUMANOID_LEFTLEG);
		if ( rightbodyNode )
		{
			rightbody = (Entity*)rightbodyNode->element;
		}
		else
		{
			return;
		}

		node_t * shieldNode = list_Node(&this->children, 8);
		if ( shieldNode )
		{
			Entity* shield = (Entity*)shieldNode->element;
			if ( dist > 0.1 && (bodypart != LIMB_HUMANOID_LEFTARM || shield->sprite == 0) )
			{
				// walking to destination
				if ( !rightbody->skill[0] )
				{
					limb->pitch -= dist * walkSpeed;
					if ( limb->pitch < -PI / 4.0 )
					{
						limb->pitch = -PI / 4.0;
						if ( bodypart == LIMB_HUMANOID_RIGHTLEG )
						{
							if ( dist > distForFootstepSound )
							{
								if ( limb->skill[0] == 0 ) // fix for waking up on sleep to reduce repeated sound bytes in race condition.
								{
									if ( this->monsterFootstepType == MONSTER_FOOTSTEP_USE_BOOTS )
									{
										node_t * tempNode = list_Node(&this->children, 3);
										if ( tempNode )
										{
											Entity* foot = (Entity*)tempNode->element;
											playSoundEntityLocal(this, getMonsterFootstepSound(this->monsterFootstepType, foot->sprite), 32);
										}
									}
									else
									{
										playSoundEntityLocal(this, getMonsterFootstepSound(this->monsterFootstepType, 0), 32);
									}
									limb->skill[0] = 1;
								}
							}
						}
					}
				}
				else
				{
					limb->pitch += dist * walkSpeed;
					if ( limb->pitch > PI / 4.0 )
					{
						limb->pitch = PI / 4.0;
						if ( bodypart == LIMB_HUMANOID_RIGHTLEG )
						{
							if ( dist > distForFootstepSound )
							{
								if ( limb->skill[0] == 1 ) // fix for waking up on sleep to reduce repeated sound bytes in race condition.
								{
									if ( this->monsterFootstepType == MONSTER_FOOTSTEP_USE_BOOTS )
									{
										node_t * tempNode = list_Node(&this->children, 3);
										if ( tempNode )
										{
											Entity* foot = (Entity*)tempNode->element;
											playSoundEntityLocal(this, getMonsterFootstepSound(this->monsterFootstepType, foot->sprite), 32);
										}
									}
									else
									{
										playSoundEntityLocal(this, getMonsterFootstepSound(this->monsterFootstepType, 0), 32);
									}
									limb->skill[0] = 0;
								}
							}
						}
					}
				}
			}
			else
			{
				// coming to a stop
				if ( limb->pitch < 0 || (limb->pitch > PI && limb->pitch < 2 * PI) )
				{
					limb->pitch += 1 / fmax(dist * .1, 10.0);
					if ( limb->pitch > 0 )
					{
						limb->pitch = 0;
					}
				}
				else if ( limb->pitch > 0 )
				{
					limb->pitch -= 1 / fmax(dist * .1, 10.0);
					if ( limb->pitch < 0 )
					{
						limb->pitch = 0;
					}
				}
			}
		}
	}
	else if ( bodypart == LIMB_HUMANOID_LEFTLEG || bodypart == LIMB_HUMANOID_RIGHTARM || bodypart == LIMB_HUMANOID_CLOAK )
	{
		if ( bodypart != LIMB_HUMANOID_RIGHTARM || (this->monsterAttack == 0 && this->monsterAttackTime == 0) )
		{
			if ( dist > 0.1 )
			{
				double armMoveSpeed = 1.0;
				if ( bodypart == LIMB_HUMANOID_RIGHTARM && this->hasRangedWeapon() && this->monsterState == MONSTER_STATE_ATTACK )
				{
					// don't move ranged weapons so far if ready to attack
					armMoveSpeed = 0.5;
				}

				if ( limb->skill[0] )
				{
					limb->pitch -= dist * walkSpeed * armMoveSpeed;
					if ( limb->pitch < -PI * armMoveSpeed / 4.0 )
					{
						limb->skill[0] = 0;
						limb->pitch = -PI * armMoveSpeed / 4.0;
					}
				}
				else
				{
					limb->pitch += dist * walkSpeed * armMoveSpeed;
					if ( limb->pitch > PI * armMoveSpeed / 4.0 )
					{
						limb->skill[0] = 1;
						limb->pitch = PI * armMoveSpeed / 4.0;
					}
				}
			}
			else
			{
				if ( limb->pitch < 0 )
				{
					limb->pitch += 1 / fmax(dist * .1, 10.0);
					if ( limb->pitch > 0 )
					{
						limb->pitch = 0;
					}
				}
				else if ( limb->pitch > 0 )
				{
					limb->pitch -= 1 / fmax(dist * .1, 10.0);
					if ( limb->pitch < 0 )
					{
						limb->pitch = 0;
					}
				}
			}
		}
	}

	return;
}

void Creature::lookAtEntity(Entity& target)
{
	double tangent = atan2(target.y - y, target.x - x);
	monsterLookTime = 1;
	monsterMoveTime = local_rng.rand() % 10 + 1;
	monsterLookDir = tangent;
}

void Creature::handleEffectsClient()
{
	Stat* myStats = getStats();

	if ( !myStats )
	{
		return;
	}

	if ( myStats->EFFECTS[EFF_MAGICREFLECT] )
	{
		spawnAmbientParticles(80, 579, 10 + local_rng.rand() % 40, 1.0, false);
	}

	if ( myStats->EFFECTS[EFF_FEAR] )
	{
		if ( ticks % 25 == 0 || ticks % 40 == 0 )
		{
			spawnAmbientParticles(1, 864, 20 + local_rng.rand() % 10, 0.5, true);
		}
	}

	if ( myStats->EFFECTS[EFF_TROLLS_BLOOD] )
	{
		spawnAmbientParticles(80, 169, 20 + local_rng.rand() % 10, 0.5, true);
	}

	if ( myStats->EFFECTS[EFF_VAMPIRICAURA] )
	{
		spawnAmbientParticles(30, 600, 20 + local_rng.rand() % 30, 0.5, true);
	}

	if ( myStats->EFFECTS[EFF_PACIFY] )
	{
		spawnAmbientParticles(30, 685, 20 + local_rng.rand() % 30, 0.5, true);
	}

	if ( myStats->EFFECTS[EFF_SHADOW_TAGGED] )
	{
		if ( ticks % 25 == 0 || ticks % 40 == 0 )
		{
			spawnAmbientParticles(1, 871, 20 + local_rng.rand() % 10, 0.5, true);
		}
	}

	if ( myStats->EFFECTS[EFF_POLYMORPH] )
	{
		if ( ticks % 25 == 0 || ticks % 40 == 0 )
		{
			spawnAmbientParticles(1, 593, 20 + local_rng.rand() % 10, 0.5, true);
		}
	}

	if ( myStats->EFFECTS[EFF_INVISIBLE] && getMonsterTypeFromSprite() == SHADOW )
	{
		spawnAmbientParticles(20, 175, 20 + local_rng.rand() % 30, 0.5, true);
	}
}

void Creature::monsterAcquireAttackTarget(const Entity& target, int32_t state, bool monsterWasHit)
{
	Stat* myStats = getStats();
    const Creature& targetCrtr = static_cast<const Creature &>(target);
	if ( !myStats )
	{
		return;
	}

	bool hadOldTarget = (uidToEntity(monsterTarget) != nullptr);
	int32_t oldMonsterState = monsterState;

	if ( target.getRace() == GYROBOT )
	{
		return;
	}
	else if ( myStats->type == GYROBOT )
	{
		if ( state == MONSTER_STATE_ATTACK )
		{
			return;
		}
		else
		{
			if ( targetCrtr.behavior == &actMonster )
			{
				return;
			}
		}
	}
	else if ( monsterIsImmobileTurret(this, myStats) )
	{
		if ( monsterAllyIndex >= 0 && targetCrtr.behavior == &actPlayer )
		{
			return;
		}
		if ( monsterIsImmobileTurret(nullptr, target.getStats()) && state == MONSTER_STATE_PATH )
		{
			return;
		}
		if ( monsterState == MONSTER_STATE_WAIT )
		{
			if ( myStats->LVL >= 10 && monsterHitTime < HITRATE )
			{
				monsterHitTime = HITRATE;
			}
		}
	}

	if ( &target != uidToEntity(monsterTarget) && !monsterReleaseAttackTarget() )
	{
		//messagePlayer(clientnum, "Entity failed to acquire target!");
		return;
	}

	/*if ( &target != uidToEntity(monsterTarget) )
	{
		messagePlayer(clientnum, "Entity acquired new target!");
	}*/

	if ( myStats->type == LICH_ICE ) // make sure automatons don't attack the leader and vice versa...
	{
		Stat* targetStats = target.getStats();
		if ( targetStats )
		{
			if ( targetStats->type == AUTOMATON && !strncmp(targetStats->name, "corrupted automaton", 19) )
			{
				return;
			}
		}
	}
	else if ( myStats->type == AUTOMATON && !strncmp(myStats->name, "corrupted automaton", 19) )
	{
		if ( target.getRace() == LICH_ICE )
		{
			return;
		}
	}
	else if ( myStats->type == INCUBUS && !strncmp(myStats->name, "inner demon", strlen("inner demon")) )
	{
		if ( monsterState == MONSTER_STATE_WAIT )
		{
			return;
		}
	}

	if ( target.getRace() == INCUBUS )
	{
		Stat* targetStats = target.getStats();
		if ( targetStats && !strncmp(targetStats->name, "inner demon", strlen("inner demon")) )
		{
			Entity* illusionTauntingThisEntity = uidToEntity(static_cast<uint32_t>(targetCrtr.monsterIllusionTauntingThisUid));
			if ( illusionTauntingThisEntity != this )
			{
				return;
			}
		}
	}

	if ( myStats->EFFECTS[EFF_DISORIENTED] )
	{
		return;
	}

	if ( monsterState != MONSTER_STATE_ATTACK && !hadOldTarget )
	{
		if ( myStats->type != LICH_FIRE
			&& myStats->type != LICH_ICE
			&& (myStats->type < LICH || myStats->type > DEVIL)
			)
		{
			// check to see if holding ranged weapon, set hittime to be ready to attack.
			// set melee hittime close to max in hardcore mode...
			if ( ((svFlags & SV_FLAG_HARDCORE) || hasRangedWeapon()) && monsterSpecialTimer <= 0 )
			{
				if ( hasRangedWeapon() )
				{
					if ( myStats->weapon && itemCategory(myStats->weapon) == MAGICSTAFF )
					{
						monsterHitTime = HITRATE - 6; // 120 ms reaction time
					}
					else
					{
						monsterHitTime = 2 * HITRATE - 2;
					}
				}
				else if ( svFlags & SV_FLAG_HARDCORE )
				{
					if ( monsterWasHit ) // retaliating to an attack
					{
						monsterHitTime = HITRATE - 12; // 240 ms reaction time
					}
					else // monster find enemy in line of sight
					{
						monsterHitTime = HITRATE - 30; // 600 ms reaction time
					}
				}
			}
		}
	}

	if ( (myStats->type == LICH_FIRE || myStats->type == LICH_ICE)
		&& (monsterState == MONSTER_STATE_LICHFIRE_TELEPORT_STATIONARY
			|| monsterState == MONSTER_STATE_LICHICE_TELEPORT_STATIONARY
			|| monsterState == MONSTER_STATE_LICH_CASTSPELLS
			|| monsterState == MONSTER_STATE_LICH_TELEPORT_ROAMING
			|| monsterState == MONSTER_STATE_LICHFIRE_DIE
			|| monsterState == MONSTER_STATE_LICHICE_DIE) )
	{

	}
	else
	{
		monsterState = state;
	}

	if ( (myStats->type == SHOPKEEPER || myStats->type == HUMAN) && monsterTarget != target.getUID() )
	{
		Stat* targetStats = target.getStats();
		if ( targetStats )
		{
			if ( myStats->type == SHOPKEEPER )
			{
				for (int c = 0; c < MAXPLAYERS; ++c)
				{
					players[c]->worldUI.worldTooltipDialogue.createDialogueTooltip(getUID(),
						Player::WorldUI_t::WorldTooltipDialogue_t::DIALOGUE_ATTACK, Language::get(3243),
						Language::get(4217 + local_rng.uniform(0, 16)), getMonsterLocalizedName(targetStats->type).c_str());
				}
				if (targetCrtr.behavior == &actPlayer && targetStats->type != HUMAN && targetStats->type != AUTOMATON )
				{
					steamAchievementClient(target.skill[2], "BARONY_ACH_RIGHT_TO_REFUSE");
				}
			}
			else
			{
				if ( monsterAllyIndex < 0 || (monsterAllyIndex >= 0 && local_rng.getU8() % 8 == 0) )
				{
					for (int c = 0; c < MAXPLAYERS; ++c)
					{
						if (local_rng.getU8() % 2) {
							players[c]->worldUI.worldTooltipDialogue.createDialogueTooltip(getUID(),
								Player::WorldUI_t::WorldTooltipDialogue_t::DIALOGUE_ATTACK, Language::get(516 + local_rng.uniform(0, 1)),
								Language::get(4234 + local_rng.uniform(0, 16)), getMonsterLocalizedName(targetStats->type).c_str());
						} else {
							players[c]->worldUI.worldTooltipDialogue.createDialogueTooltip(getUID(),
								Player::WorldUI_t::WorldTooltipDialogue_t::DIALOGUE_ATTACK, Language::get(518 + local_rng.uniform(0, 1)),
								Language::get(4217 + local_rng.uniform(0, 16)), getMonsterLocalizedName(targetStats->type).c_str());
						}
					}
				}
			}

			if ( myStats->type == SHOPKEEPER && targetCrtr.behavior == &actPlayer )
			{
				if ( oldMonsterState == MONSTER_STATE_TALK && monsterState != MONSTER_STATE_TALK )
				{
					for ( int i = 0; i < MAXPLAYERS; ++i )
					{
						if ( players[i]->isLocalPlayer() && shopkeeper[i] == getUID() )
						{
							players[i]->closeAllGUIs(CLOSEGUI_ENABLE_SHOOTMODE, CLOSEGUI_CLOSE_ALL);
						}
						else if ( i > 0 && !client_disconnected[i] && multiplayer == SERVER && !players[i]->isLocalPlayer() )
						{
							// inform client of abandonment
							strcpy((char*)net_packet->data, "SHPC");
							SDLNet_Write32(getUID(), &net_packet->data[4]);
							net_packet->address.host = net_clients[i - 1].host;
							net_packet->address.port = net_clients[i - 1].port;
							net_packet->len = 8;
							sendPacketSafe(net_sock, -1, net_packet, i - 1);
						}
					}
				}
			}
		}
	}

	monsterTarget = target.getUID();
	monsterTargetX = target.x;
	monsterTargetY = target.y;

	if ( target.getStats() != nullptr )
	{
		if ( monsterState != MONSTER_STATE_ATTACK && state == MONSTER_STATE_PATH )
		{
			if ( myStats->type != LICH_FIRE && myStats->type != LICH_ICE && myStats->type != LICH && myStats->type != DEVIL )
			{
				double distance = pow(x - target.x, 2) + pow(y - target.y, 2);
				if ( distance < STRIKERANGE * STRIKERANGE )
				{
					monsterState = MONSTER_STATE_ATTACK;
				}
			}
		}
	}


	if ( monsterAllyIndex > 0 && monsterAllyIndex < MAXPLAYERS )
	{
		serverUpdateEntitySkill(this, 1); // update monsterTarget for player leaders.
	}

	if ( !hadOldTarget && myStats->type == SHADOW )
	{
		//messagePlayer(clientnum, "TODO: Shadow got new target.");
		//Activate special ability initially for Shadow.
		monsterSpecialTimer = MONSTER_SPECIAL_COOLDOWN_SHADOW_TELEMIMICINVISI_ATTACK;
		//pose = MONSTER_POSE_MAGIC_WINDUP1;
		monsterShadowInitialMimic = 1; //true!
		attack(MONSTER_POSE_MAGIC_WINDUP3, 0, nullptr);
	}
}

bool Creature::monsterReleaseAttackTarget(bool force)
{
	if ( !monsterTarget )
	{
		return true;
	}

	Stat* myStats = getStats();
	if ( !myStats )
	{
		return false;
	}

	if ( !force && myStats->type == SHADOW && monsterTarget && uidToEntity(monsterTarget) )
	{
		//messagePlayer(clientnum, "Shadow cannot lose target until it's dead!");
		return false; //Shadow cannot lose its target.
	}

	/*if ( myStats->type == SHADOW )
	{
		messagePlayer(0, "DEBUG: Shadow: Creature::monsterReleaseAttackTarget().");
	}*/

	monsterTarget = 0;

	if ( monsterAllyIndex > 0 && monsterAllyIndex < MAXPLAYERS )
	{
		serverUpdateEntitySkill(this, 1); // update monsterTarget for player leaders.
	}

	return true;
}

void Creature::checkGroundForItems()
{
	Stat* myStats = getStats();
	if ( myStats == nullptr )
	{
		return;
	}
	if ( monsterAllyPickupItems == ALLY_PICKUP_NONE && monsterAllyIndex >= 0 )
	{
		return; // set to ignore ground items.
	}

	// Calls the function for a monster to pick up an item, if it's a monster that picks up items, only if they are not Asleep
	if ( myStats->EFFECTS[EFF_ASLEEP] == false )
	{
		switch ( myStats->type )
		{
			case GOBLIN:
			case HUMAN:
				if ( !strcmp(myStats->name, "") )
				{
					//checkBetterEquipment(myStats);
					monsterAddNearbyItemToInventory(myStats, 16, 9);
				}
				break;
			case GOATMAN:
				//Goatman boss picks up items too.
				monsterAddNearbyItemToInventory(myStats, 16, 9); //Replaces checkBetterEquipment(), because more better. Adds items to inventory, and swaps out current equipped with better stuff on the ground.
																 //checkBetterEquipment(myStats);
				break;
			case AUTOMATON:
				monsterAddNearbyItemToInventory(myStats, 16, 5);
				break;
			case SHOPKEEPER:
				if ( myStats->MISC_FLAGS[STAT_FLAG_MYSTERIOUS_SHOPKEEP] > 0 )
				{
					monsterAddNearbyItemToInventory(myStats, 16, 99);
				}
				break;
			default:
				return;
		}
	}
}

bool Creature::canWieldItem(const Item& item) const
{
	Stat* myStats = getStats();
	if ( !myStats )
	{
		return false;
	}

	switch ( myStats->type )
	{
		case GOBLIN:
			return goblinCanWieldItem(item);
		case HUMAN:
			return humanCanWieldItem(item);
		case GOATMAN:
			return goatmanCanWieldItem(item);
		case AUTOMATON:
			return automatonCanWieldItem(item);
		case SHADOW:
			return shadowCanWieldItem(item);
		default:
			return false;
	}
}

bool Creature::monsterAddNearbyItemToInventory(Stat* myStats, int rangeToFind, int maxInventoryItems, Entity* forcePickupItem)
{
	//TODO: Any networking/multiplayer needs?
	if ( !myStats )
	{
		return false; //Can't continue without these.
	}

	if ( list_Size(&myStats->inventory) >= maxInventoryItems + 1 )
	{
		return false;
	}

	list_t * itemsList = nullptr;
	//X and Y in terms of tiles.
	if ( forcePickupItem != nullptr && forcePickupItem->behavior == &actItem )
	{
		for ( int i = 0; i < MAXPLAYERS; ++i )
		{
			if ( FollowerMenu[i].followerToCommand == this )
			{
				if ( !FollowerMenu[i].allowedInteractItems(myStats->type) )
				{
					return false;
				}
				break;
			}
		}

		//If this is the first item found, the list needs to be created.
		if ( !(itemsList) )
		{
			itemsList = (list_t *)malloc(sizeof(list_t));
			(itemsList)->first = NULL;
			(itemsList)->last = NULL;
		}

		//Add the current entity to it.
		node_t * node2 = list_AddNodeLast(itemsList);
		node2->element = forcePickupItem;
		node2->deconstructor = &emptyDeconstructor;
	}
	else
	{
		int tx = x / 16;
		int ty = y / 16;
		getItemsOnTile(tx, ty, &itemsList); //Check the tile the monster is on for items.
		getItemsOnTile(tx - 1, ty, &itemsList); //Check tile to the left.
		getItemsOnTile(tx + 1, ty, &itemsList); //Check tile to the right.
		getItemsOnTile(tx, ty - 1, &itemsList); //Check tile up.
		getItemsOnTile(tx, ty + 1, &itemsList); //Check tile down.
		getItemsOnTile(tx - 1, ty - 1, &itemsList); //Check tile diagonal up left.
		getItemsOnTile(tx + 1, ty - 1, &itemsList); //Check tile diagonal up right.
		getItemsOnTile(tx - 1, ty + 1, &itemsList); //Check tile diagonal down left.
		getItemsOnTile(tx + 1, ty + 1, &itemsList); //Check tile diagonal down right.
	}
	node_t * node = nullptr;
	bool pickedUpItemReturnValue = false;

	if ( itemsList )
	{
		/*
		* Rundown of the function:
		* Loop through all items.
		* Add item to inventory.
		*/

		for ( node = itemsList->first; node != nullptr; node = node->next )
		{
			//Turn the entity into an item.
			if ( node->element )
			{
				if ( list_Size(&myStats->inventory) >= maxInventoryItems + 1 )
				{
					break;
				}

				Entity* entity = (Entity*)node->element;
				if ( entity->flags[INVISIBLE] )
				{
					continue; // ignore invisible items like Sokoban gloves or other scripted events.
				}

				Item* item = nullptr;
				if ( entity != nullptr )
				{
					item = newItemFromEntity(entity);
					if ( forcePickupItem )
					{
						item->forcedPickupByPlayer = true;
					}
				}
				if ( !item )
				{
					continue;
				}

				double dist = sqrt(pow(this->x - entity->x, 2) + pow(this->y - entity->y, 2));
				if ( std::floor(dist) > rangeToFind )
				{
					// item was too far away, continue.
					if ( item != nullptr )
					{
						free(item);
					}
					continue;
				}

				if ( !entity->itemNotMoving && entity->parent && entity->parent != uid )
				{
					if ( itemCategory(item) == THROWN && entity->parent && entity->parent == uid )
					{
						//It's good. Can pick this one up, it's your THROWN now.
					}
					else
					{
						//Don't pick up non-THROWN items that are moving, or owned THROWN that are moving.
						if ( item != nullptr )
						{
							free(item);
						}
						continue; //Item still in motion, don't pick it up.
					}
				}

				Item** shouldWield = nullptr;
				node_t * replaceInventoryItem = nullptr;
				if ( !monsterWantsItem(*item, shouldWield, replaceInventoryItem) )
				{
					if ( item && item->interactNPCUid == getUID() && forcePickupItem )
					{
						// I don't want this.
						handleNPCInteractDialogue(*myStats, ALLY_EVENT_INTERACT_ITEM_NOUSE);
					}
					if ( item != nullptr )
					{
						free(item);
					}
					continue;
				}

				int playerOwned = -1;
				if ( entity->itemOriginalOwner != 0 )
				{
					for ( int c = 0; c < MAXPLAYERS; ++c )
					{
						if ( players[c] && players[c]->entity )
						{
							if ( players[c]->entity->getUID() == entity->itemOriginalOwner )
							{
								if ( players[c]->entity->checkFriend(this) )
								{
									// player owned.
									playerOwned = c;
								}
								break;
							}
						}
					}
					if ( item->interactNPCUid == getUID() )
					{
						// item being interacted with, can interact with item.
					}
					else if ( (playerOwned >= 0
						&& (entity->ticks < 5 * TICKS_PER_SECOND
							|| (monsterAllyPickupItems != ALLY_PICKUP_ALL && monsterAllyIndex >= 0))
							)
						)
					{
						// player item too new on ground, or monster is set to not pickup player items.
						if ( item != nullptr )
						{
							free(item);
						}
						continue;
					}
				}
				if ( entity->itemDelayMonsterPickingUp > 0 && entity->ticks < entity->itemDelayMonsterPickingUp )
				{
					// dropped from a disarm skill, don't pick up item until timer is up.
					if ( item != nullptr )
					{
						free(item);
					}
					continue;
				}

				if ( myStats->type == SHOPKEEPER && myStats->MISC_FLAGS[STAT_FLAG_MYSTERIOUS_SHOPKEEP] > 0 )
				{
					// pickup the item always.
					Creature* owner = uidToCreature(item->ownerUid);
					if ( owner && owner->behavior == &actPlayer )
					{
						for ( int c = 0; c < MAXPLAYERS; ++c )
						{
							switch ( item->type )
							{
								case ARTIFACT_ORB_GREEN:
									//messagePlayer(owner->skill[2], MESSAGE_WORLD, Language::get(3888), myStats->name);
									players[c]->worldUI.worldTooltipDialogue.createDialogueTooltip(getUID(),
										Player::WorldUI_t::WorldTooltipDialogue_t::DIALOGUE_BROADCAST, Language::get(3888));
									break;
								case ARTIFACT_ORB_BLUE:
									//messagePlayer(owner->skill[2], MESSAGE_WORLD, Language::get(3889), myStats->name);
									players[c]->worldUI.worldTooltipDialogue.createDialogueTooltip(getUID(),
										Player::WorldUI_t::WorldTooltipDialogue_t::DIALOGUE_BROADCAST, Language::get(3889));
									break;
								case ARTIFACT_ORB_RED:
									//messagePlayer(owner->skill[2], MESSAGE_WORLD, Language::get(3890), myStats->name);
									players[c]->worldUI.worldTooltipDialogue.createDialogueTooltip(getUID(),
										Player::WorldUI_t::WorldTooltipDialogue_t::DIALOGUE_BROADCAST, Language::get(3890));
									break;
								default:
									break;
							}
						}
					}
					playSoundEntity(this, 35 + local_rng.rand() % 3, 64);
					addItemToMonsterInventory(item);
					item = nullptr;
					list_RemoveNode(entity->mynode);
					pickedUpItemReturnValue = true;
				}
				else if ( myStats->type == SLIME )
				{
					if ( item->identified )
					{
						messagePlayer(monsterAllyIndex, MESSAGE_WORLD, Language::get(3145), items[item->type].getIdentifiedName());
					}
					else
					{
						messagePlayer(monsterAllyIndex, MESSAGE_WORLD, Language::get(3145), items[item->type].getUnidentifiedName());
					}
					list_RemoveNode(entity->mynode); // slimes eat the item up.
					pickedUpItemReturnValue = true;
				}
				else if ( shouldWield )
				{
					if ( (*shouldWield) && (*shouldWield)->beatitude < 0 && myStats->type != AUTOMATON )
					{
						if ( item && item->interactNPCUid == getUID() && forcePickupItem )
						{
							// held item is cursed!
							handleNPCInteractDialogue(*myStats, ALLY_EVENT_INTERACT_ITEM_CURSED);
						}
						if ( item != nullptr )
						{
							free(item);
						}
						continue;
					}

					if ( myStats->type == AUTOMATON && list_Size(&myStats->inventory) < maxInventoryItems
						&& !(item->interactNPCUid == getUID() && forcePickupItem) )
					{
						addItemToMonsterInventory(*shouldWield); // Automatons are hoarders, except if commanded.
					}
					else
					{
						Entity* dropped = dropItemMonster((*shouldWield), this, myStats); //And I threw it on the ground!
						if ( dropped && item && item->interactNPCUid == getUID() )
						{
							if ( monsterAllyIndex >= 0 && monsterAllyIndex < MAXPLAYERS )
							{
								if ( players[monsterAllyIndex] && players[monsterAllyIndex]->entity )
								{
									dropped->itemOriginalOwner = players[monsterAllyIndex]->entity->getUID();
								}
							}
						}
					}

					if ( playerOwned >= 0 && checkFriend(players[playerOwned]->entity)
						&& (item->type >= ARTIFACT_SWORD && item->type <= ARTIFACT_GLOVES) )
					{
						steamAchievementClient(playerOwned, "BARONY_ACH_EARN_THIS");
					}

					if ( item && item->interactNPCUid == getUID() && forcePickupItem )
					{
						if ( itemCategory(item) == AMULET || itemCategory(item) == RING )
						{
							playSoundEntity(this, 33 + local_rng.rand() % 2, 64);
						}
						else if ( itemCategory(item) == WEAPON || itemCategory(item) == THROWN )
						{
							playSoundEntity(this, 40 + local_rng.rand() % 4, 64);
						}
						else if ( itemCategory(item) == ARMOR )
						{
							playSoundEntity(this, 44 + local_rng.rand() % 3, 64);
						}
						else if ( item->type == TOOL_TORCH || item->type == TOOL_LANTERN || item->type == TOOL_CRYSTALSHARD )
						{
							playSoundEntity(this, 134, 64);
						}
					}

					(*shouldWield) = item;
					item = nullptr;
					list_RemoveNode(entity->mynode);
					pickedUpItemReturnValue = true;
				}
				else if ( replaceInventoryItem )
				{
					//Drop that item out of the monster's inventory, and add this item to the monster's inventory.
					Item* itemToDrop = static_cast<Item*>(replaceInventoryItem->element);
					if ( itemToDrop )
					{
						if ( !(myStats->type == AUTOMATON && list_Size(&myStats->inventory) < maxInventoryItems) )
						{
							// Automatons are hoarders when swapping. Everything else will drop the weapon.
							dropItemMonster(itemToDrop, this, myStats, itemToDrop->count);
						}
						//list_RemoveNode(replaceInventoryItem);
					}

					if ( list_Size(&myStats->inventory) < maxInventoryItems )
					{
						addItemToMonsterInventory(item);
					}
					item = nullptr;
					list_RemoveNode(entity->mynode);
					pickedUpItemReturnValue = true;
				}
				else if ( list_Size(&myStats->inventory) < maxInventoryItems )
				{
					bool addItem = true;
					if ( myStats->type == GYROBOT && list_Size(&myStats->inventory) >= 1 )
					{
						node_t * inv = myStats->inventory.first;
						Item* toStack = (Item*)inv->element;
						if ( toStack )
						{
							if ( toStack->type >= TOOL_BOMB && toStack->type <= TOOL_TELEPORT_BOMB )
							{
								if ( !itemCompare(toStack, item, false) )
								{
									// stack the items.
									toStack->count += item->count;
									item = nullptr;
									list_RemoveNode(entity->mynode);
									pickedUpItemReturnValue = true;
									addItem = false;
								}
							}
						}
					}

					if ( addItem )
					{
						addItemToMonsterInventory(item);
						item = nullptr;
						list_RemoveNode(entity->mynode);
						pickedUpItemReturnValue = true;
					}
				}

				if ( item != nullptr )
				{
					free(item);
				}
			}
		}
		list_FreeAll(itemsList);
		free(itemsList);
	}

	return pickedUpItemReturnValue;
}

bool Creature::shouldMonsterEquipThisWeapon(const Item& itemToEquip) const
{
	Stat* myStats = getStats();
	if ( !myStats )
	{
		return false;
	}

	if ( myStats->weapon == nullptr )
	{
		return true; //Something is better than nothing.
	}

	if ( itemToEquip.interactNPCUid == getUID() )
	{
		return true;
	}

	if ( myStats->weapon->beatitude < 0 )
	{
		//If monster already holding an item, can't swap it out if it's cursed.
		return false;
	}

	if ( myStats->weapon->forcedPickupByPlayer == true )
	{
		return false;
	}

	//Monster is already holding a weapon.
	if ( monsterAllyIndex >= 0 )
	{
		if ( monsterAllyClass == ALLY_CLASS_RANGED && isRangedWeapon(itemToEquip)
			&& !isRangedWeapon(*(myStats->weapon)) )
		{
			// drop what you're holding and pickup that new bow!
			return true;
		}
		else if ( monsterAllyClass == ALLY_CLASS_MELEE && !isRangedWeapon(itemToEquip)
			&& isRangedWeapon(*(myStats->weapon)) )
		{
			// drop what you're holding and pickup that new non-bow!
			return true;
		}
	}

	if ( !Item::isThisABetterWeapon(itemToEquip, myStats->weapon) )
	{
		return false; //Don't want junk.
	}

	if ( itemCategory(myStats->weapon) == MAGICSTAFF || itemCategory(myStats->weapon) == POTION || itemCategory(myStats->weapon) == THROWN || itemCategory(myStats->weapon) == GEM )
	{
		//If current hand item is not cursed, but it's a certain item, don't want to equip this new one.
		return false;
	}

	if ( !isRangedWeapon(itemToEquip) && isRangedWeapon(*(myStats->weapon)) && rangedWeaponUseQuiverOnAttack(myStats) )
	{
		// have ranged weapon and quiver, don't pickup non-ranged weapon.
		return false;
	}

	return true;
}

bool Creature::monsterWantsItem(const Item& item, Item**& shouldEquip, node_t *& replaceInventoryItem) const
{
	Stat* myStats = getStats();
	if ( !myStats )
	{
		return false;
	}

	if ( myStats->type == GYROBOT && item.interactNPCUid == getUID() )
	{
		return true;
	}
	if ( item.status == BROKEN )
	{
		return false; // no want broken.
	}

	switch ( myStats->type )
	{
		case GOBLIN:
			if ( !goblinCanWieldItem(item) )
			{
				return false;
			}
			break;
		case HUMAN:
			if ( !humanCanWieldItem(item) )
			{
				return false;
			}
			break;
		case GOATMAN:
			if ( !goatmanCanWieldItem(item) )
			{
				return false;
			}
			break;
		case AUTOMATON:
			if ( !automatonCanWieldItem(item) )
			{
				if ( item.interactNPCUid == getUID() )
				{
					// item is being interacted with but we won't auto pick up on interact.
					return false;
				}
				else
				{
					if ( item.type == ARTIFACT_ORB_BLUE
						|| item.type == ARTIFACT_ORB_GREEN
						|| item.type == ARTIFACT_ORB_RED
						|| item.type == ARTIFACT_ORB_PURPLE )
					{
						return false;
					}
					return true; //Can pick up all items automaton can't equip, because recycler.
				}
			}
			break;
		case GNOME:
		case KOBOLD:
		case INCUBUS:
		case INSECTOID:
		case SKELETON:
		case VAMPIRE:
			if ( !monsterAllyEquipmentInClass(item) )
			{
				return false;
			}
			break;
		case SLIME:
			if ( item.type == ARTIFACT_ORB_BLUE
				|| item.type == ARTIFACT_ORB_GREEN
				|| item.type == ARTIFACT_ORB_RED
				|| item.type == ARTIFACT_ORB_PURPLE )
			{
				return false;
			}
			return true; // noms on all items.
			break;
		case SHOPKEEPER:
			if ( myStats->MISC_FLAGS[STAT_FLAG_MYSTERIOUS_SHOPKEEP] > 0 )
			{
				if ( item.type == ARTIFACT_ORB_BLUE
					|| item.type == ARTIFACT_ORB_GREEN
					|| item.type == ARTIFACT_ORB_RED )
				{
					return true;
				}
			}
			return false;
			break;
		default:
			return false;
			break;
	}

	switch ( itemCategory(&item) )
	{
		case WEAPON:
			if ( !myStats->weapon )
			{
				shouldEquip = &myStats->weapon;
			}

			if ( myStats->weapon && itemCategory(myStats->weapon) == WEAPON && shouldMonsterEquipThisWeapon(item) )
			{
				shouldEquip = &myStats->weapon;
				return true;
			}
			else
			{
				if ( myStats->weapon && itemCategory(myStats->weapon) == WEAPON )
				{
					//Weapon ain't better than weapon already holding. Don't want it.
					if ( myStats->type == AUTOMATON ) // Automatons are hoarders.
					{
						return true;
					}
					return false;
				}

				if ( monsterAllyIndex >= 0 && monsterAllyIndex < MAXPLAYERS )
				{
					if ( myStats->weapon && item.interactNPCUid == getUID() )
					{
						shouldEquip = &myStats->weapon;
						return true;
					}
					if ( myStats->weapon && myStats->weapon->forcedPickupByPlayer )
					{
						return false;
					}
				}

				//Not holding a weapon. Make sure don't already have a weapon in the inventory. If doesn't have a weapon at all, then add it into the inventory since something is better than nothing.
				node_t * weaponNode = itemNodeInInventory(myStats, -1, WEAPON);
				if ( !weaponNode )
				{
					//If no weapons found in inventory, then yes, the goatman wants it, and it should be added to the inventory.
					return true; //Want this item.
				}

				//Search inventory and replace weapon if this one is better.
				if ( Item::isThisABetterWeapon(item, static_cast<Item*>(weaponNode->element)) )
				{
					replaceInventoryItem = weaponNode;
					return true;
				}
				return false; //Don't want your junk.
			}
		case ARMOR:
			if ( myStats->type == AUTOMATON ) // Automatons are hoarders.
			{
				shouldEquip = shouldMonsterEquipThisArmor(item);
				return true;
			}

			return (shouldEquip = shouldMonsterEquipThisArmor(item));
		case THROWN:
			if ( myStats->weapon == nullptr )
			{
				shouldEquip = &myStats->weapon;
				return true;
			}
			else
			{
				return true; //Store in inventory.
			}
		case MAGICSTAFF:
			if ( item.interactNPCUid == getUID() )
			{
				shouldEquip = &myStats->weapon;
			}
			return true;
			break;
		case RING:
			if ( item.interactNPCUid == getUID() )
			{
				shouldEquip = &myStats->ring;
			}
			return true;
			break;
		case AMULET:
			if ( item.interactNPCUid == getUID() )
			{
				shouldEquip = &myStats->amulet;
			}
			return true;
			break;
		case TOOL:
			if ( itemTypeIsQuiver(item.type) )
			{
				return (shouldEquip = shouldMonsterEquipThisArmor(item));
			}
			if ( item.interactNPCUid == getUID() )
			{
				if ( item.type == TOOL_TORCH || item.type == TOOL_LANTERN || item.type == TOOL_CRYSTALSHARD )
				{
					shouldEquip = &myStats->shield;
					return true;
				}
			}
			break;
		default:
			return true; //Already checked if monster likes this specific item in the racial calls.
	}

	return false;
}

double Creature::monsterRotate()
{
	double dir = yaw - monsterLookDir;
	while ( dir >= PI )
	{
		dir -= PI * 2;
	}
	while ( dir < -PI )
	{
		dir += PI * 2;
	}
	int race = getMonsterTypeFromSprite();
	if ( race == SENTRYBOT || race == SPELLBOT )
	{
		Stat* myStats = getStats();
		int ratio = 64;
		if ( myStats )
		{
			if ( myStats->LVL >= 15 )
			{
				ratio = 2;
			}
			else if ( myStats->LVL >= 10 )
			{
				ratio = 4;
			}
			else if ( myStats->LVL >= 5 )
			{
				ratio = 16;
			}
			else if ( myStats->LVL >= 3 )
			{
				ratio = 64;
			}
		}
		yaw -= dir / ratio;
	}
	else if ( race == DUMMYBOT )
	{
		yaw -= dir / 4;
	}
	else
	{
		yaw -= dir / 2;
	}
	while ( yaw < 0 )
	{
		yaw += 2 * PI;
	}
	while ( yaw >= 2 * PI )
	{
		yaw -= 2 * PI;
	}

	return dir;
}

void Creature::degradeArmor(Stat& hitstats, Item& armor, int armornum)
{
	if ( hitstats.type == SHADOW )
	{
		return; //Shadows' armor and shields don't break.
	}

	if ( hitstats.type == SKELETON && behavior == &actMonster && monsterAllySummonRank > 0 )
	{
		return; // conjured skeleton armor doesn't break.
	}

	if ( armor.type == ARTIFACT_BOOTS
		|| armor.type == ARTIFACT_HELM
		|| armor.type == ARTIFACT_CLOAK
		|| armor.type == ARTIFACT_GLOVES
		|| armor.type == ARTIFACT_BREASTPIECE )
	{
		return;
	}

	if ( itemTypeIsQuiver(armor.type) )
	{
		// quivers don't break.
		return;
	}

	int playerhit = -1;

	if ( this->behavior == &actPlayer )
	{
		playerhit = this->skill[2];
	}

	if ( armor.type == TOOL_TORCH && armor.count > 1 && playerhit >= 0 && &armor == stats[playerhit]->shield )
	{
		std::basic_string<char> itemName = armor.getName();
		ItemType itemType = armor.type;
		Status itemStatus = armor.status;

		playSoundEntity(this, 76, 64);
		messagePlayer(playerhit, MESSAGE_EQUIPMENT, Language::get(682), armor.getName()); // torch is destroyed

		int qty = std::max(0, armor.count - 1);
		Item* item = stats[playerhit]->shield;
		consumeItem(item, playerhit);
		if ( qty > 0 && item )
		{
			messagePlayer(playerhit, MESSAGE_EQUIPMENT, Language::get(4101), itemName.c_str()); // you reignite another torch
			playSoundEntity(this, 134, 64); // ignite
			if ( playerhit >= 0 && players[playerhit]->isLocalPlayer() )
			{
				players[playerhit]->hud.shieldSwitch = true;
			}
		}
		if ( !item )
		{
			stats[playerhit]->shield = nullptr;
		}

		if ( multiplayer == SERVER && playerhit > 0 && !players[playerhit]->isLocalPlayer() )
		{
			strcpy((char*)net_packet->data, "TORC");
			SDLNet_Write16((int16_t)itemType, &net_packet->data[4]);
			net_packet->data[6] = itemStatus;
			net_packet->data[7] = qty;
			net_packet->address.host = net_clients[playerhit - 1].host;
			net_packet->address.port = net_clients[playerhit - 1].port;
			net_packet->len = 8;
			sendPacketSafe(net_sock, -1, net_packet, playerhit - 1);
		}

		return;
	}

	if ( (playerhit >= 0 && players[playerhit]->isLocalPlayer()) || playerhit < 0 )
	{
		if ( armor.count > 1 )
		{
			newItem(armor.type, armor.status, armor.beatitude, armor.count - 1, armor.appearance, armor.identified, &hitstats.inventory);
		}
	}
	armor.count = 1;
	armor.status = static_cast<Status>(std::max(static_cast<int>(BROKEN), armor.status - 1));
	if ( armor.status > BROKEN )
	{
		if ( armor.type == TOOL_CRYSTALSHARD )
		{
			messagePlayer(playerhit, MESSAGE_EQUIPMENT, Language::get(2350), armor.getName());
		}
		else
		{
			messagePlayer(playerhit, MESSAGE_EQUIPMENT, Language::get(681), armor.getName());
		}
	}
	else
	{
		if ( armor.type == TOOL_CRYSTALSHARD )
		{
			playSoundEntity(this, 162, 64);
			messagePlayer(playerhit, MESSAGE_EQUIPMENT, Language::get(2351), armor.getName());
		}
		else if ( itemCategory(&armor) == SPELLBOOK )
		{
			playSoundEntity(this, 414, 64);
			messagePlayer(playerhit, MESSAGE_EQUIPMENT, Language::get(3459), armor.getName());
		}
		else
		{
			playSoundEntity(this, 76, 64);
			messagePlayer(playerhit, MESSAGE_EQUIPMENT, Language::get(682), armor.getName());
		}
	}
	if ( playerhit > 0 && multiplayer == SERVER && !players[playerhit]->isLocalPlayer() )
	{
		strcpy((char*)net_packet->data, "ARMR");
		net_packet->data[4] = armornum;
		net_packet->data[5] = armor.status;
		net_packet->address.host = net_clients[playerhit - 1].host;
		net_packet->address.port = net_clients[playerhit - 1].port;
		net_packet->len = 6;
		sendPacketSafe(net_sock, -1, net_packet, playerhit - 1);
	}
}

bool Creature::shouldRetreat(Stat& myStats)
{
	// monsters that retreat based on CHR
	// gnomes, spiders, humans (50%)
	// kobolds, scarabs, suc/incubi, goatmen, rats

	// excluded golems, shadows, cockatrice, skeletons, demons, imps
	// scorpions, slimes, ghouls, vampires, shopkeeps

	// retreating monsters will not try path when losing sight of target

	if ( myStats.EFFECTS[EFF_PACIFY] || myStats.EFFECTS[EFF_FEAR] )
	{
		return true;
	}
	if ( myStats.EFFECTS[EFF_KNOCKBACK] )
	{
		return true;
	}
	if ( (myStats.EFFECTS[EFF_DASH] || (myStats.weapon && myStats.weapon->type == SPELLBOOK_DASH)) && behavior == &actMonster )
	{
		return false;
	}
	if ( myStats.type == VAMPIRE )
	{
		return false;
	}
	else if ( myStats.type == SHADOW )
	{
		return false;
	}
	else if ( myStats.type == SHOPKEEPER )
	{
		return false;
	}
	else if ( myStats.type == LICH_FIRE )
	{
		if ( monsterLichFireMeleeSeq == LICH_ATK_BASICSPELL_SINGLE )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if ( monsterIsImmobileTurret(this, &myStats) )
	{
		return false;
	}
	else if ( myStats.type == SPIDER )
	{
		if ( monsterTarget != 0 )
		{
			if ( Entity* target = uidToEntity(monsterTarget) )
			{
				if ( Stat* targetStats = target->getStats() )
				{
					if ( targetStats->EFFECTS[EFF_WEBBED] )
					{
						return false;
					}
				}
			}
		}
	}
	if ( monsterAllySummonRank != 0 )
	{
		return false;
	}
	if ( myStats.type == LICH_ICE )
	{
		return false;
	}
	if ( myStats.type == INCUBUS && !strncmp(myStats.name, "inner demon", strlen("inner demon")) )
	{
		return false;
	}
	if ( monsterTarget != 0 && myStats.monsterDemonHasBeenExorcised != 0 )
	{
		Entity* target = uidToEntity(monsterTarget);
		if ( target )
		{
			Stat* targetStats = target->getStats();
			if ( targetStats && targetStats->type == INCUBUS && !strncmp(targetStats->name, "inner demon", strlen("inner demon")) )
			{
				return false;
			}
		}
	}

	Entity* leader = monsterAllyGetPlayerLeader();
	if ( leader )
	{
		// do not retreat for brave leader!
		return false;
	}

	if ( myStats.MAXHP >= 100 )
	{
		if ( myStats.HP <= myStats.MAXHP / 8 && this->getCHR() >= -2 )
		{
			return true;
		}
	}
	else if ( myStats.HP <= myStats.MAXHP / 4 && this->getCHR() >= -2 )
	{
		return true;
	}

	return false;
}

bool Creature::backupWithRangedWeapon(Stat& myStats, int dist, int hasrangedweapon)
{
	int distanceLimit = 100;
	if ( hasrangedweapon && myStats.weapon )
	{
		if ( distanceLimit >= getMonsterEffectiveDistanceOfRangedWeapon(myStats.weapon) )
		{
			distanceLimit = getMonsterEffectiveDistanceOfRangedWeapon(myStats.weapon) - 20;
		}
	}
	if ( dist >= distanceLimit || !hasrangedweapon )
	{
		return false;
	}

	if ( (myStats.EFFECTS[EFF_DASH] || (myStats.weapon && myStats.weapon->type == SPELLBOOK_DASH)) && behavior == &actMonster )
	{
		return false;
	}
	if ( myStats.type == INSECTOID && monsterSpecialState > 0 )
	{
		return false;
	}
	if ( myStats.type == SPIDER && monsterSpecialState > 0 )
	{
		return false;
	}
	if ( monsterIsImmobileTurret(this, &myStats) )
	{
		return false;
	}
	if ( myStats.type == VAMPIRE && (monsterSpecialState > 0 || MonsterData_t::nameMatchesSpecialNPCName(myStats, "bram kindly")) )
	{
		return false;
	}
	Entity* leader = monsterAllyGetPlayerLeader();
	if ( leader )
	{
		if ( monsterTarget != 0 )
		{
			if ( Creature* target = uidToCreature(monsterTarget) )
			{
				if ( target->behavior == &actMonster && target->monsterTarget == getUID() ) // my target is attacking me
				{
					if ( entityDist(this, target) < TOUCHRANGE * 2 )
					{
						return true;
					}
				}
			}
			return false;
		}
	}

	return true;
}

void Creature::createPathBoundariesNPC(int maxTileDistance)
{
	Stat* myStats = this->getStats();

	if ( !myStats )
	{
		return;
	}

	if ( myStats->MISC_FLAGS[STAT_FLAG_NPC] != 0
		|| myStats->type == SHOPKEEPER
		|| monsterAllyState == ALLY_STATE_DEFEND )
	{
		// is NPC, find the bounds which movement is restricted to by finding the "box" it spawned in.
		int i, j;
		int numTiles = 0;
		monsterPathBoundaryXStart = x / 16;
		monsterPathBoundaryXEnd = x / 16;
		monsterPathBoundaryYStart = y / 16;
		monsterPathBoundaryYEnd = y / 16;
		for ( i = x; i >= 0; i -= 16 )
		{
			if ( !checkObstacle(i, y, this, nullptr) )
			{
				monsterPathBoundaryXStart = i;
			}
			else
			{
				if ( monsterAllyState == ALLY_STATE_DEFEND )
				{
					// don't use players to block boundaries.
					bool foundplayer = false;
					for ( int player = 0; player < MAXPLAYERS; ++player )
					{
						if ( players[player] && players[player]->entity )
						{
							int playerx = static_cast<int>(players[player]->entity->x);
							int playery = static_cast<int>(players[player]->entity->y);
							if ( playerx == i && playery == y )
							{
								monsterPathBoundaryXStart = i;
								foundplayer = true;
							}
						}
					}
					if ( !foundplayer )
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
			if ( maxTileDistance > 0 )
			{
				++numTiles;
				if ( numTiles > maxTileDistance )
				{
					break;
				}
			}
		}
		numTiles = 0;
		for ( i = x; i < map.width << 4; i += 16 )
		{
			if ( !checkObstacle(i, y, this, nullptr) )
			{
				monsterPathBoundaryXEnd = i;
			}
			else
			{
				if ( monsterAllyState == ALLY_STATE_DEFEND )
				{
					// don't use players to block boundaries.
					bool foundplayer = false;
					for ( int player = 0; player < MAXPLAYERS; ++player )
					{
						if ( players[player] && players[player]->entity )
						{
							int playerx = static_cast<int>(players[player]->entity->x);
							int playery = static_cast<int>(players[player]->entity->y);
							if ( playerx == i && playery == y )
							{
								monsterPathBoundaryXEnd = i;
								foundplayer = true;
							}
						}
					}
					if ( !foundplayer )
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
			if ( maxTileDistance > 0 )
			{
				++numTiles;
				if ( numTiles > maxTileDistance )
				{
					break;
				}
			}
		}
		numTiles = 0;
		for ( j = y; j >= 0; j -= 16 )
		{
			if ( !checkObstacle(x, j, this, nullptr) )
			{
				monsterPathBoundaryYStart = j;
			}
			else
			{
				if ( monsterAllyState == ALLY_STATE_DEFEND )
				{
					// don't use players to block boundaries.
					bool foundplayer = false;
					for ( int player = 0; player < MAXPLAYERS; ++player )
					{
						if ( players[player] && players[player]->entity )
						{
							int playerx = static_cast<int>(players[player]->entity->x);
							int playery = static_cast<int>(players[player]->entity->y);
							if ( playerx == x && playery == j )
							{
								monsterPathBoundaryYStart = j;
								foundplayer = true;
							}
						}
					}
					if ( !foundplayer )
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
			if ( maxTileDistance > 0 )
			{
				++numTiles;
				if ( numTiles > maxTileDistance )
				{
					break;
				}
			}
		}
		numTiles = 0;
		for ( j = y; j < map.height << 4; j += 16 )
		{
			if ( !checkObstacle(x, j, this, nullptr) )
			{
				monsterPathBoundaryYEnd = j;
			}
			else
			{
				if ( monsterAllyState == ALLY_STATE_DEFEND )
				{
					// don't use players to block boundaries.
					bool foundplayer = false;
					for ( int player = 0; player < MAXPLAYERS; ++player )
					{
						if ( players[player] && players[player]->entity )
						{
							int playerx = static_cast<int>(players[player]->entity->x);
							int playery = static_cast<int>(players[player]->entity->y);
							if ( playerx == x && playery == j )
							{
								monsterPathBoundaryYEnd = j;
								foundplayer = true;
							}
						}
					}
					if ( !foundplayer )
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
			if ( maxTileDistance > 0 )
			{
				++numTiles;
				if ( numTiles > maxTileDistance )
				{
					break;
				}
			}
		}
		numTiles = 0;
		//messagePlayer(0, "restricted to (%d, %d), (%d, %d)", monsterPathBoundaryXStart >> 4, monsterPathBoundaryYStart >> 4, monsterPathBoundaryXEnd >> 4, monsterPathBoundaryYEnd >> 4);
	}
}

node_t * Creature::chooseAttackSpellbookFromInventory()
{
	Stat* myStats = getStats();
	if (!myStats )
	{
		return nullptr;
	}

	node_t * spellbook = nullptr;
	std::vector<int> spellbooks;

	//Ok, first, compile a list of all spells it has on it.
	//Then choose one and return it.
	for ( int i = 1; i < NUM_SPELLS; ++i ) //Skip 0, which = SPELL_NONE.
	{
		if ( monsterHasSpellbook(i) )
		{
			if ( myStats->type == SHADOW ) //TODO: Replace this if-else block with an "isAttackSpell() && monsterCanUseSpell()"
			{
				if ( shadowCanMimickSpell(i) )
				{
					//messagePlayer(clientnum, "I can mimic spell %d!", i);
					spellbooks.push_back(i);
				}
				else
				{
					//messagePlayer(clientnum, "I no can does spell %d", i);
				}
			}
			else
			{
				//messagePlayer(clientnum, "TODO: Only shadow has CanCastSpell() checking implemented! Need to update other relevant monsters.");
			}
		}
	}

	if ( spellbooks.size() == 0 )
	{
		//messagePlayer(clientnum, "[DEBUG:Creature::chooseAttackSpellbookFromInventory()] No applicable spellbooks on me!");
		return nullptr;
	}

	spellbook = spellbookNodeInInventory(myStats, spellbooks[local_rng.rand()%spellbooks.size()]); //Choose a random spell and return it.
	if (!spellbook )
	{
		//messagePlayer(clientnum, "[DEBUG:Creature::chooseAttackSpellbookFromInventory()] Error: Failed to choose a spellbook!");
	}
	return spellbook;
}

/*-------------------------------------------------------------------------------

setHelmetLimbOffset
Adjusts helmet offsets for all monsters, depending on the type of headwear.

-------------------------------------------------------------------------------*/

void Creature::setHelmetLimbOffset(Entity* helm)
{
	helm->scalex = 1.01;
	helm->scaley = 1.01;
	helm->scalez = 1.01;
	// for non-armor helmets, they are rotated so focaly acts as up/down postion.
	int monster = getMonsterTypeFromSprite();
	if ( helm->sprite == items[HAT_PHRYGIAN].index )
	{
		switch ( monster )
		{
			case AUTOMATON:
			case SKELETON:
				helm->focalx = limbs[monster][9][0] - .5;
				helm->focaly = limbs[monster][9][1] - 3.25;
				helm->focalz = limbs[monster][9][2] + 2.25;
				break;
			case HUMAN:
			case SHOPKEEPER:
			case VAMPIRE:
				helm->focalx = limbs[monster][9][0] - .5;
				helm->focaly = limbs[monster][9][1] - 3.25;
				helm->focalz = limbs[monster][9][2] + 2.25;
				break;
			case INSECTOID:
				helm->focalx = limbs[monster][9][0] - .5;
				helm->focaly = limbs[monster][9][1] - 3.05;
				helm->focalz = limbs[monster][9][2] + 2.25;
				break;
			case GOBLIN:
			case SHADOW:
				helm->focalx = limbs[monster][9][0] - .5;
				helm->focaly = limbs[monster][9][1] - 3.55;
				helm->focalz = limbs[monster][9][2] + 2.5;
				break;
			case GOATMAN:
				helm->focalx = limbs[monster][9][0] - .5;
				helm->focaly = limbs[monster][9][1] - 3.55;
				helm->focalz = limbs[monster][9][2] + 2.75;
				break;
			case INCUBUS:
			case SUCCUBUS:
				helm->focalx = limbs[monster][9][0] - .5;
				helm->focaly = limbs[monster][9][1] - 3.2;
				helm->focalz = limbs[monster][9][2] + 2.5;
				break;
			default:
				break;
		}
		helm->roll = PI / 2;
	}
	else if ( (helm->sprite >= items[HAT_HOOD].index && helm->sprite < items[HAT_HOOD].index + items[HAT_HOOD].variations)
		|| helm->sprite == items[HAT_HOOD_RED].index || helm->sprite == items[HAT_HOOD_SILVER].index
		|| helm->sprite == items[PUNISHER_HOOD].index )
	{
		switch ( monster )
		{
			case AUTOMATON:
			case SKELETON:
				helm->focalx = limbs[monster][9][0] - .5;
				helm->focaly = limbs[monster][9][1] - 2.5;
				helm->focalz = limbs[monster][9][2] + 2.25;
				if ( helm->sprite == (items[HAT_HOOD].index + 2) )
				{
					helm->focaly += 0.5; // black hood
				}
				else if ( helm->sprite == (items[HAT_HOOD].index + 3) )
				{
					helm->focaly -= 0.5; // purple hood
				}
				else if ( helm->sprite == items[PUNISHER_HOOD].index )
				{
					helm->focalx += 0.25;
					helm->focaly += 0.5;
				}
				break;
			case INCUBUS:
			case SUCCUBUS:
				helm->focalx = limbs[monster][9][0] - .5;
				helm->focaly = limbs[monster][9][1] - 2.5;
				helm->focalz = limbs[monster][9][2] + 2.5;
				if ( helm->sprite == (items[HAT_HOOD].index + 3) )
				{
					helm->focaly -= 0.5; // purple hood
				}
				else if ( helm->sprite == items[PUNISHER_HOOD].index )
				{
					if ( monster == INCUBUS )
					{
						helm->focalx += 0.25;
						helm->focaly += 0.25;
					}
				}
				break;
			case VAMPIRE:
			case SHOPKEEPER:
			case HUMAN:
				helm->focalx = limbs[monster][9][0] - .5;
				helm->focaly = limbs[monster][9][1] - 2.5;
				helm->focalz = limbs[monster][9][2] + 2.25;
				if ( helm->sprite == items[PUNISHER_HOOD].index )
				{
					helm->focaly += 0.25;
				}
				break;
			case GOATMAN:
				helm->focalx = limbs[monster][9][0] - .5;
				helm->focaly = limbs[monster][9][1] - 2.75;
				helm->focalz = limbs[monster][9][2] + 2.75;
				if ( helm->sprite == (items[HAT_HOOD].index + 2) )
				{
					helm->focaly -= 0.25; // black hood
				}
				else if ( helm->sprite == (items[HAT_HOOD].index + 3) )
				{
					helm->focaly -= 0.5; // purple hood
				}
				break;
			case INSECTOID:
				helm->focalx = limbs[monster][9][0] - .5;
				helm->focaly = limbs[monster][9][1] - 2.15;
				helm->focalz = limbs[monster][9][2] + 2.25;
				if ( helm->sprite == (items[HAT_HOOD].index + 2) )
				{
					helm->focaly += 0.25; // black hood
				}
				else if ( helm->sprite == (items[HAT_HOOD].index + 3) )
				{
					helm->focaly -= 0.5; // purple hood
				}
				else if ( helm->sprite == items[PUNISHER_HOOD].index )
				{
					helm->focalx += 0.5;
					helm->focaly += 0.15;
				}
				break;
			case GOBLIN:
			case SHADOW:
				helm->focalx = limbs[monster][9][0] - .5;
				helm->focaly = limbs[monster][9][1] - 2.75;
				helm->focalz = limbs[monster][9][2] + 2.5;
				if ( monster == GOBLIN && (this->sprite == 752 || this->sprite == 1039) ) // special female offset.
				{
					if ( helm->sprite == (items[HAT_HOOD].index + 3) )
					{
						helm->focaly -= 0.5; // purple hood
					}
				}
				if ( helm->sprite == items[PUNISHER_HOOD].index )
				{
					helm->focaly += 0.25;
				}
				break;
			default:
				break;
		}
		/*helm->focalx += limbs[HUMAN][12][0];
		helm->focaly += limbs[HUMAN][12][1];
		helm->focalz += limbs[HUMAN][12][2];*/
		helm->roll = PI / 2;
	}
	else if ( helm->sprite == items[HAT_WIZARD].index || helm->sprite == items[HAT_JESTER].index )
	{
		switch ( monster )
		{
			case AUTOMATON:
			case SKELETON:
				helm->focalx = limbs[monster][9][0];
				helm->focaly = limbs[monster][9][1] - 4.5;
				helm->focalz = limbs[monster][9][2] + 2.25;
				break;
			case INCUBUS:
			case SUCCUBUS:
				helm->focalx = limbs[monster][9][0];
				helm->focaly = limbs[monster][9][1] - 4.75;
				helm->focalz = limbs[monster][9][2] + 2.5;
				break;
			case VAMPIRE:
			case SHOPKEEPER:
			case HUMAN:
				helm->focalx = limbs[monster][9][0];
				helm->focaly = limbs[monster][9][1] - 4.75;
				helm->focalz = limbs[monster][9][2] + 2.25;
				break;
			case GOATMAN:
				helm->focalx = limbs[monster][9][0];
				helm->focaly = limbs[monster][9][1] - 5.1;
				helm->focalz = limbs[monster][9][2] + 2.75;
				break;
			case INSECTOID:
				helm->focalx = limbs[monster][9][0];
				helm->focaly = limbs[monster][9][1] - 4.75;
				helm->focalz = limbs[monster][9][2] + 2.25;
				break;
			case GOBLIN:
			case SHADOW:
				helm->focalx = limbs[monster][9][0];
				helm->focaly = limbs[monster][9][1] - 5;
				helm->focalz = limbs[monster][9][2] + 2.5;
				break;
			default:
				break;
		}
		helm->roll = PI / 2;
	}
	else if ( helm->sprite == items[HAT_FEZ].index )
	{
		switch ( monster )
		{
			case AUTOMATON:
			case SKELETON:
				helm->focalx = limbs[monster][9][0];
				helm->focaly = limbs[monster][9][1] - 4.f;
				helm->focalz = limbs[monster][9][2] + 2.25;
				break;
			case INCUBUS:
			case SUCCUBUS:
				helm->focalx = limbs[monster][9][0];
				helm->focaly = limbs[monster][9][1] - 4.0;
				helm->focalz = limbs[monster][9][2] + 2.5;
				break;
			case VAMPIRE:
			case SHOPKEEPER:
			case HUMAN:
				helm->focalx = limbs[monster][9][0];
				helm->focaly = limbs[monster][9][1] - 4.35;
				helm->focalz = limbs[monster][9][2] + 2.25;
				break;
			case GOATMAN:
				helm->focalx = limbs[monster][9][0];
				helm->focaly = limbs[monster][9][1] - 4.5;
				helm->focalz = limbs[monster][9][2] + 2.75;
				break;
			case INSECTOID:
				helm->focalx = limbs[monster][9][0];
				helm->focaly = limbs[monster][9][1] - 4;
				helm->focalz = limbs[monster][9][2] + 2.25;
				break;
			case GOBLIN:
			case SHADOW:
				helm->focalx = limbs[monster][9][0];
				helm->focaly = limbs[monster][9][1] - 4.5;
				helm->focalz = limbs[monster][9][2] + 2.5;
				if ( monster == GOBLIN && (this->sprite == 752 || this->sprite == 1039) ) // special female offset.
				{
					helm->focaly -= 0.25;
				}
				break;
			default:
				break;
		}
		helm->roll = PI / 2;
	}
	else if ( helm->sprite == items[MASK_SHAMAN].index )
	{
		switch ( monster )
		{
			case AUTOMATON:
				helm->focalx = limbs[monster][10][0] + 1.f;
				helm->focaly = limbs[monster][10][1] - 0.5;
				helm->focalz = limbs[monster][10][2] - 1.5;
				break;
			case SKELETON:
				helm->focalx = limbs[monster][10][0] + 0.5;
				helm->focaly = limbs[monster][10][1] - 0.5;
				helm->focalz = limbs[monster][10][2] - 1.7;
				break;
			case INCUBUS:
				helm->focalx = limbs[monster][10][0] + 0.5;
				helm->focaly = limbs[monster][10][1] - 0.25;
				helm->focalz = limbs[monster][10][2] - 2;
				break;
			case SUCCUBUS:
				helm->focalx = limbs[monster][10][0] + 0.5;
				helm->focaly = limbs[monster][10][1] - 0;
				helm->focalz = limbs[monster][10][2] - 2.25;
				break;
			case VAMPIRE:
			case SHOPKEEPER:
			case HUMAN:
				helm->focalx = limbs[monster][10][0] + 0.75;
				helm->focaly = limbs[monster][10][1] - 0;
				helm->focalz = limbs[monster][10][2] - 2;
				break;
			case GOATMAN:
				helm->focalx = limbs[monster][10][0] + 0.7;
				helm->focaly = limbs[monster][10][1] + 0.25;
				helm->focalz = limbs[monster][10][2] - 2.55;
				break;
			case INSECTOID:
				helm->focalx = limbs[monster][10][0] + 1.03;
				helm->focaly = limbs[monster][10][1] - 0.25;
				helm->focalz = limbs[monster][10][2] - 1.5;
				break;
			case GOBLIN:
				helm->focalx = limbs[monster][10][0] + 0.7;
				helm->focaly = limbs[monster][10][1] + 0;
				helm->focalz = limbs[monster][10][2] - 2.25;
				//if ( monster == GOBLIN && (this->sprite == 752 || this->sprite == 1039) ) // special female offset.
				//{
				//	helm->focaly -= 0.25;
				//}
				break;
			case SHADOW:
			default:
				break;
		}
	}
	else
	{
		if ( monster == GOBLIN && (this->sprite == 752 || this->sprite == 1039) ) // special female offset.
		{
			helm->focalz = limbs[monster][9][2] - 0.25; // all non-hat helms
		}
	}
}

void Creature::setHumanoidLimbOffset(Entity* limb, Monster race, int limbType)
{
	if ( !limb )
	{
		return;
	}
	if ( limbType == LIMB_HUMANOID_TORSO )
	{
		limb->scalez = 1.f; // reset this scale incase something modifies this.
	}
	switch ( race )
	{
		case CREATURE_IMP:
			if ( limbType == LIMB_HUMANOID_TORSO )
			{
				limb->x -= 2 * cos(this->yaw);
				limb->y -= 2 * sin(this->yaw);
				limb->z += 2.75;
				limb->focalz -= 0.25;
			}
			else if ( limbType == LIMB_HUMANOID_RIGHTLEG )
			{
				limb->x += 1 * cos(this->yaw + PI / 2);
				limb->y += 1 * sin(this->yaw + PI / 2);
				limb->z += 6;
			}
			else if ( limbType == LIMB_HUMANOID_LEFTLEG )
			{
				limb->x -= 1 * cos(this->yaw + PI / 2);
				limb->y -= 1 * sin(this->yaw + PI / 2);
				limb->z += 6;
			}
			else if ( limbType == LIMB_HUMANOID_RIGHTARM )
			{
				limb->x += 3 * cos(this->yaw + PI / 2) - 1 * cos(this->yaw);
				limb->y += 3 * sin(this->yaw + PI / 2) - 1 * sin(this->yaw);
				limb->z += 1;
			}
			else if ( limbType == LIMB_HUMANOID_LEFTARM )
			{
				limb->x -= 3 * cos(this->yaw + PI / 2) + 1 * cos(this->yaw);
				limb->y -= 3 * sin(this->yaw + PI / 2) + 1 * sin(this->yaw);
				limb->z += 1;
			}
			break;
		case HUMAN:
		case VAMPIRE:
			if ( limbType == LIMB_HUMANOID_TORSO )
			{
				limb->x -= .25 * cos(this->yaw);
				limb->y -= .25 * sin(this->yaw);
				limb->z += 2.5;
			}
			else if ( limbType == LIMB_HUMANOID_RIGHTLEG )
			{
				limb->x += 1 * cos(this->yaw + PI / 2) + .25 * cos(this->yaw);
				limb->y += 1 * sin(this->yaw + PI / 2) + .25 * sin(this->yaw);
				limb->z += 5;
				if ( this->z >= 1.4 && this->z <= 1.6 )
				{
					limb->yaw += PI / 8;
					limb->pitch = -PI / 2;
				}
			}
			else if ( limbType == LIMB_HUMANOID_LEFTLEG )
			{
				limb->x -= 1 * cos(this->yaw + PI / 2) - .25 * cos(this->yaw);
				limb->y -= 1 * sin(this->yaw + PI / 2) - .25 * sin(this->yaw);
				limb->z += 5;
				if ( this->z >= 1.4 && this->z <= 1.6 )
				{
					limb->yaw -= PI / 8;
					limb->pitch = -PI / 2;
				}
			}
			else if ( limbType == LIMB_HUMANOID_RIGHTARM )
			{
				limb->x += 2.5 * cos(this->yaw + PI / 2) - .20 * cos(this->yaw);
				limb->y += 2.5 * sin(this->yaw + PI / 2) - .20 * sin(this->yaw);
				limb->z += 1.5;
				if ( this->z >= 1.4 && this->z <= 1.6 )
				{
					limb->pitch = 0;
				}
			}
			else if ( limbType == LIMB_HUMANOID_LEFTARM )
			{
				limb->x -= 2.5 * cos(this->yaw + PI / 2) + .20 * cos(this->yaw);
				limb->y -= 2.5 * sin(this->yaw + PI / 2) + .20 * sin(this->yaw);
				limb->z += 1.5;
				if ( this->z >= 1.4 && this->z <= 1.6 )
				{
					limb->pitch = 0;
				}
			}
			break;
		case TROLL:
			if ( limbType == LIMB_HUMANOID_TORSO )
			{
				limb->x -= .5 * cos(this->yaw);
				limb->y -= .5 * sin(this->yaw);
				limb->z += 2.25;
			}
			else if ( limbType == LIMB_HUMANOID_RIGHTLEG )
			{
				limb->x += 2 * cos(this->yaw + PI / 2) - 0.75 * cos(this->yaw);
				limb->y += 2 * sin(this->yaw + PI / 2) - 0.75 * sin(this->yaw);
				limb->z += 5;
				if ( this->z >= 1.4 && this->z <= 1.6 )
				{
					limb->yaw += PI / 8;
					limb->pitch = -PI / 2;
				}
				else if ( limb->pitch <= -PI / 3 )
				{
					limb->pitch = 0;
				}
			}
			else if ( limbType == LIMB_HUMANOID_LEFTLEG )
			{
				limb->x -= 2 * cos(this->yaw + PI / 2) + 0.75 * cos(this->yaw);
				limb->y -= 2 * sin(this->yaw + PI / 2) + 0.75 * sin(this->yaw);
				limb->z += 5;
				if ( this->z >= 1.4 && this->z <= 1.6 )
				{
					limb->yaw -= PI / 8;
					limb->pitch = -PI / 2;
				}
				else if ( limb->pitch <= -PI / 3 )
				{
					limb->pitch = 0;
				}
			}
			else if ( limbType == LIMB_HUMANOID_RIGHTARM )
			{
				limb->x += 3.5 * cos(this->yaw + PI / 2) - 1 * cos(this->yaw);
				limb->y += 3.5 * sin(this->yaw + PI / 2) - 1 * sin(this->yaw);
				limb->z += .1;
				//limb->yaw += MONSTER_WEAPONYAW;
				if ( this->z >= 1.4 && this->z <= 1.6 )
				{
					limb->pitch = 0;
				}
			}
			else if ( limbType == LIMB_HUMANOID_LEFTARM )
			{
				limb->x -= 3.5 * cos(this->yaw + PI / 2) + 1 * cos(this->yaw);
				limb->y -= 3.5 * sin(this->yaw + PI / 2) + 1 * sin(this->yaw);
				limb->z += .1;
				if ( this->z >= 1.4 && this->z <= 1.6 )
				{
					limb->pitch = 0;
				}
			}
			break;
		case SKELETON:
		case AUTOMATON:
			if ( limbType == LIMB_HUMANOID_TORSO )
			{
				limb->x -= .25 * cos(this->yaw);
				limb->y -= .25 * sin(this->yaw);
				limb->z += 2;
				if ( limb->sprite == items[WIZARD_DOUBLET].index
					|| limb->sprite == items[HEALER_DOUBLET].index
					|| limb->sprite == items[TUNIC].index
					|| limb->sprite == items[TUNIC].index + 1 )
				{
					limb->z += 0.15;
					limb->scalez = 0.9;
					limb->x += .1 * cos(this->yaw);
					limb->y += .1 * sin(this->yaw);
				}
				else
				{
					limb->scalez = 1.f;
				}
			}
			else if ( limbType == LIMB_HUMANOID_RIGHTLEG )
			{
				limb->x += 1 * cos(this->yaw + PI / 2) + .25 * cos(this->yaw);
				limb->y += 1 * sin(this->yaw + PI / 2) + .25 * sin(this->yaw);
				limb->z += 4;
				if ( this->z >= 1.9 && this->z <= 2.1 )
				{
					limb->yaw += PI / 8;
					limb->pitch = -PI / 2;
				}
				else if ( limb->pitch <= -PI / 3 )
				{
					limb->pitch = 0;
				}
			}
			else if ( limbType == LIMB_HUMANOID_LEFTLEG )
			{
				limb->x -= 1 * cos(this->yaw + PI / 2) - .25 * cos(this->yaw);
				limb->y -= 1 * sin(this->yaw + PI / 2) - .25 * sin(this->yaw);
				limb->z += 4;
				if ( this->z >= 1.9 && this->z <= 2.1 )
				{
					limb->yaw -= PI / 8;
					limb->pitch = -PI / 2;
				}
				else if ( limb->pitch <= -PI / 3 )
				{
					limb->pitch = 0;
				}
			}
			else if ( limbType == LIMB_HUMANOID_RIGHTARM )
			{
				if ( limb->sprite != 689 && limb->sprite != 691
				    && limb->sprite != 1046 && limb->sprite != 1048
					&& limb->sprite != 233 && limb->sprite != 234
					&& limb->sprite != 745 && limb->sprite != 747
					&& limb->sprite != 471 && limb->sprite != 472 )
				{
					// wearing gloves (not default arms), position tighter to body.
					limb->x += 1.75 * cos(this->yaw + PI / 2) - .20 * cos(this->yaw);
					limb->y += 1.75 * sin(this->yaw + PI / 2) - .20 * sin(this->yaw);
				}
				else
				{
					limb->x += 2.f * cos(this->yaw + PI / 2) - .20 * cos(this->yaw);
					limb->y += 2.f * sin(this->yaw + PI / 2) - .20 * sin(this->yaw);
				}
				limb->z += .6;
				if ( this->z >= 1.9 && this->z <= 2.1 )
				{
					limb->pitch = 0;
				}
			}
			else if ( limbType == LIMB_HUMANOID_LEFTARM )
			{
				if ( limb->sprite != 688 && limb->sprite != 690
				    && limb->sprite != 1045 && limb->sprite != 1047
					&& limb->sprite != 231 && limb->sprite != 232
					&& limb->sprite != 744 && limb->sprite != 746
					&& limb->sprite != 469 && limb->sprite != 470 )
				{
					// wearing gloves (not default arms), position tighter to body.
					limb->x -= 1.75 * cos(this->yaw + PI / 2) + .20 * cos(this->yaw);
					limb->y -= 1.75 * sin(this->yaw + PI / 2) + .20 * sin(this->yaw);
				}
				else
				{
					limb->x -= 2.f * cos(this->yaw + PI / 2) + .20 * cos(this->yaw);
					limb->y -= 2.f * sin(this->yaw + PI / 2) + .20 * sin(this->yaw);
				}
				limb->z += .6;
				if ( this->z >= 1.9 && this->z <= 2.1 )
				{
					limb->pitch = 0;
				}
			}
			break;
		case GOBLIN:
		case GOATMAN:
		case INSECTOID:
			if ( limbType == LIMB_HUMANOID_TORSO )
			{
				limb->x -= .25 * cos(this->yaw);
				limb->y -= .25 * sin(this->yaw);
				limb->z += 2;
				if ( race == INSECTOID )
				{
					if ( limb->sprite != 727 && limb->sprite != 458
						&& limb->sprite != 761
						&& limb->sprite != 1060 )
					{
						// wearing armor, offset by 1.
						limb->z -= 1;
					}
				}

				/*if ( limb->sprite == items[WIZARD_DOUBLET].index
					|| limb->sprite == items[HEALER_DOUBLET].index
					|| limb->sprite == items[TUNIC].index
					|| limb->sprite == items[TUNIC].index + 1 )
				{
					limb->z += 0.25;
				}*/
			}
			else if ( limbType == LIMB_HUMANOID_RIGHTLEG )
			{
				limb->x += 1 * cos(this->yaw + PI / 2) + .25 * cos(this->yaw);
				limb->y += 1 * sin(this->yaw + PI / 2) + .25 * sin(this->yaw);
				limb->z += 4;
				if ( this->z >= 2.4 && this->z <= 2.6 )
				{
					limb->yaw += PI / 8;
					limb->pitch = -PI / 2;
				}
				else if ( limb->pitch <= -PI / 3 )
				{
					limb->pitch = 0;
				}
			}
			else if ( limbType == LIMB_HUMANOID_LEFTLEG )
			{
				limb->x -= 1 * cos(this->yaw + PI / 2) - .25 * cos(this->yaw);
				limb->y -= 1 * sin(this->yaw + PI / 2) - .25 * sin(this->yaw);
				limb->z += 4;
				if ( this->z >= 2.4 && this->z <= 2.6 )
				{
					limb->yaw -= PI / 8;
					limb->pitch = -PI / 2;
				}
				else if ( limb->pitch <= -PI / 3 )
				{
					limb->pitch = 0;
				}
			}
			else if ( limbType == LIMB_HUMANOID_RIGHTARM )
			{
				limb->x += 2.5 * cos(this->yaw + PI / 2) - .20 * cos(this->yaw);
				limb->y += 2.5 * sin(this->yaw + PI / 2) - .20 * sin(this->yaw);
				limb->z += .5;
				if ( this->z >= 2.4 && this->z <= 2.6 )
				{
					limb->pitch = 0;
				}
			}
			else if ( limbType == LIMB_HUMANOID_LEFTARM )
			{
				limb->x -= 2.5 * cos(this->yaw + PI / 2) + .20 * cos(this->yaw);
				limb->y -= 2.5 * sin(this->yaw + PI / 2) + .20 * sin(this->yaw);
				limb->z += .5;
				if ( this->z >= 2.4 && this->z <= 2.6 )
				{
					limb->pitch = 0;
				}
			}
			break;
		case INCUBUS:
		case SUCCUBUS:
			if ( limbType == LIMB_HUMANOID_TORSO )
			{
				limb->x -= .5 * cos(this->yaw);
				limb->y -= .5 * sin(this->yaw);
				limb->z += 2.5;

				if ( limb->sprite == items[WIZARD_DOUBLET].index
					|| limb->sprite == items[HEALER_DOUBLET].index
					|| limb->sprite == items[TUNIC].index
					|| limb->sprite == items[TUNIC].index + 1 )
				{
					limb->z += 0.5;
				}
			}
			else if ( limbType == LIMB_HUMANOID_RIGHTLEG )
			{
				limb->x += 1 * cos(this->yaw + PI / 2) - .75 * cos(this->yaw);
				limb->y += 1 * sin(this->yaw + PI / 2) - .75 * sin(this->yaw);
				limb->z += 5;
				if ( this->z >= 1.4 && this->z <= 1.6 )
				{
					limb->yaw += PI / 8;
					limb->pitch = -PI / 2;
				}
				else if ( limb->pitch <= -PI / 3 )
				{
					limb->pitch = 0;
				}
			}
			else if ( limbType == LIMB_HUMANOID_LEFTLEG )
			{
				limb->x -= 1 * cos(this->yaw + PI / 2) + .75 * cos(this->yaw);
				limb->y -= 1 * sin(this->yaw + PI / 2) + .75 * sin(this->yaw);
				limb->z += 5;
				if ( this->z >= 1.4 && this->z <= 1.6 )
				{
					limb->yaw -= PI / 8;
					limb->pitch = -PI / 2;
				}
				else if ( limb->pitch <= -PI / 3 )
				{
					limb->pitch = 0;
				}
			}
			else if ( limbType == LIMB_HUMANOID_RIGHTARM )
			{
				limb->x += 2.5 * cos(this->yaw + PI / 2) - .20 * cos(this->yaw);
				limb->y += 2.5 * sin(this->yaw + PI / 2) - .20 * sin(this->yaw);
				limb->z += .5;
				if ( this->z >= 1.4 && this->z <= 1.6 )
				{
					limb->pitch = 0;
				}
			}
			else if ( limbType == LIMB_HUMANOID_LEFTARM )
			{
				limb->x -= 2.5 * cos(this->yaw + PI / 2) + .20 * cos(this->yaw);
				limb->y -= 2.5 * sin(this->yaw + PI / 2) + .20 * sin(this->yaw);
				limb->z += .5;
				if ( this->z >= 1.4 && this->z <= 1.6 )
				{
					limb->pitch = 0;
				}
			}
			break;
		default:
			break;
	}
}

void Creature::handleHumanoidShieldLimb(Entity* shieldLimb, Entity* shieldArmLimb)
{
	if ( !shieldLimb || !shieldArmLimb )
	{
		return;
	}

	int race = this->getMonsterTypeFromSprite();
	int player = -1;
	if ( this->behavior == &actPlayer )
	{
		player = this->skill[2];
	}
	Entity* flameEntity = nullptr;

	shieldLimb->focalx = limbs[race][7][0];
	shieldLimb->focaly = limbs[race][7][1];
	shieldLimb->focalz = limbs[race][7][2];

	shieldLimb->scalex = 1.f;
	shieldLimb->scaley = 1.f;
	shieldLimb->scalez = 1.f;

	switch ( race )
	{
		case CREATURE_IMP:
			shieldLimb->focalx = limbs[race][8][0];
			shieldLimb->focaly = limbs[race][8][1];
			shieldLimb->focalz = limbs[race][8][2];

			shieldLimb->x -= 2.5 * cos(this->yaw + PI / 2) + .20 * cos(this->yaw);
			shieldLimb->y -= 2.5 * sin(this->yaw + PI / 2) + .20 * sin(this->yaw);
			shieldLimb->z += 2.5;
			shieldLimb->yaw = shieldArmLimb->yaw;
			shieldLimb->roll = 0;
			shieldLimb->pitch = 0;

			if ( shieldLimb->sprite >= items[SPELLBOOK_LIGHT].index
				&& shieldLimb->sprite < (items[SPELLBOOK_LIGHT].index + items[SPELLBOOK_LIGHT].variations) )
			{
				shieldLimb->pitch = shieldArmLimb->pitch - .35 + 3 * PI / 2;
				shieldLimb->yaw += PI / 6;
				shieldLimb->focalx -= 4;
				shieldLimb->focalz += .5;
				shieldLimb->x += 0.5 * cos(this->yaw + PI / 2) - 1 * cos(this->yaw);
				shieldLimb->y += 0.5 * sin(this->yaw + PI / 2) - 1 * sin(this->yaw);
				shieldLimb->z -= 0.5;
				shieldLimb->scalex = 0.8;
				shieldLimb->scaley = 0.8;
				shieldLimb->scalez = 0.8;
			}
			break;
		case HUMAN:
		case VAMPIRE:
			shieldLimb->x -= 2.5 * cos(this->yaw + PI / 2) + .20 * cos(this->yaw);
			shieldLimb->y -= 2.5 * sin(this->yaw + PI / 2) + .20 * sin(this->yaw);
			shieldLimb->z += 2.5;
			shieldLimb->yaw = shieldArmLimb->yaw;
			shieldLimb->roll = 0;
			shieldLimb->pitch = 0;

			if ( shieldLimb->sprite == items[TOOL_LANTERN].index )
			{
			    shieldLimb->z += 2;
			}
	        if ( flickerLights || ticks % TICKS_PER_SECOND == 1 )
	        {
			    if ( shieldLimb->sprite == items[TOOL_TORCH].index )
			    {
					if ( flameEntity = spawnFlame(shieldLimb, SPRITE_FLAME) )
					{
						flameEntity->x += 2 * cos(shieldArmLimb->yaw);
						flameEntity->y += 2 * sin(shieldArmLimb->yaw);
						flameEntity->z -= 2;
					}
			    }
			    else if ( shieldLimb->sprite == items[TOOL_CRYSTALSHARD].index )
			    {
				    /*flameEntity = spawnFlame(shieldLimb, SPRITE_CRYSTALFLAME);
				    flameEntity->x += 2 * cos(shieldArmLimb->yaw);
				    flameEntity->y += 2 * sin(shieldArmLimb->yaw);
				    flameEntity->z -= 2;*/
			    }
			    else if ( shieldLimb->sprite == items[TOOL_LANTERN].index )
			    {
					if ( flameEntity = spawnFlame(shieldLimb, SPRITE_FLAME) )
					{
						flameEntity->x += 2 * cos(shieldArmLimb->yaw);
						flameEntity->y += 2 * sin(shieldArmLimb->yaw);
						flameEntity->z += 1;
					}
			    }
			}
			if ( itemSpriteIsQuiverThirdPersonModel(shieldLimb->sprite) )
			{
				shieldLimb->focalz += 3;
				shieldLimb->scalex = 1.05;
				shieldLimb->x -= -0.25 * cos(this->yaw + PI / 2) + 1.25 * cos(this->yaw);
				shieldLimb->y -= -0.25 * sin(this->yaw + PI / 2) + 1.25 * sin(this->yaw);
				shieldLimb->z += -1.28;
			}
			else if ( shieldLimb->sprite >= items[SPELLBOOK_LIGHT].index
				&& shieldLimb->sprite < (items[SPELLBOOK_LIGHT].index + items[SPELLBOOK_LIGHT].variations) )
			{
				shieldLimb->pitch = shieldArmLimb->pitch - .25 + 3 * PI / 2;
				shieldLimb->yaw += PI / 6;
				shieldLimb->focalx -= 4;
				shieldLimb->focalz += .5;
				shieldLimb->x += 0.5 * cos(this->yaw + PI / 2) + .5 * cos(this->yaw);
				shieldLimb->y += 0.5 * sin(this->yaw + PI / 2) + .5 * sin(this->yaw);
				shieldLimb->z -= 1;
				shieldLimb->scalex = 0.8;
				shieldLimb->scaley = 0.8;
				shieldLimb->scalez = 0.8;
			}

			if ( this->fskill[8] > PI / 32 ) //MONSTER_SHIELDYAW and PLAYER_SHIELDYAW defending animation
			{
				if ( shieldLimb->sprite != items[TOOL_TORCH].index
					&& shieldLimb->sprite != items[TOOL_LANTERN].index
					&& shieldLimb->sprite != items[TOOL_CRYSTALSHARD].index )
				{
					// shield, so rotate a little.
					shieldLimb->roll += PI / 64;
				}
				else
				{
					shieldLimb->x += 0.25 * cos(this->yaw);
					shieldLimb->y += 0.25 * sin(this->yaw);
					shieldLimb->pitch += PI / 16;
					if ( flameEntity )
					{
						flameEntity->x += 0.75 * cos(shieldArmLimb->yaw);
						flameEntity->y += 0.75 * sin(shieldArmLimb->yaw);
					}
				}
			}
			break;
		case SKELETON:
		case AUTOMATON:
			shieldLimb->x -= 3.f * cos(this->yaw + PI / 2) + .20 * cos(this->yaw);
			shieldLimb->y -= 3.f * sin(this->yaw + PI / 2) + .20 * sin(this->yaw);
			shieldLimb->z += 2.5;
			shieldLimb->yaw = shieldArmLimb->yaw;
			shieldLimb->roll = 0;
			shieldLimb->pitch = 0;

			if ( shieldLimb->sprite != items[TOOL_TORCH].index && shieldLimb->sprite != items[TOOL_LANTERN].index && shieldLimb->sprite != items[TOOL_CRYSTALSHARD].index )
			{
				shieldLimb->focalx = limbs[race][7][0] - 0.65;
				shieldLimb->focaly = limbs[race][7][1];
				shieldLimb->focalz = limbs[race][7][2];
			}
			if ( itemSpriteIsQuiverThirdPersonModel(shieldLimb->sprite) )
			{
				shieldLimb->focalz += 3;
				shieldLimb->z += -1.78;
				shieldLimb->scalex = 1.05;
				if ( race == SKELETON )
				{
					shieldLimb->x -= -0.2 * cos(this->yaw + PI / 2) + 1.6 * cos(this->yaw);
					shieldLimb->y -= -0.2 * sin(this->yaw + PI / 2) + 1.6 * sin(this->yaw);
					if ( behavior == &actMonster )
					{
						// additional offsets for skellie monsters since the player offsets slightly different
						shieldLimb->x += -0.25 * cos(this->yaw);
						shieldLimb->y += -0.25 * sin(this->yaw);
						shieldLimb->z += -0.25;
					}
				}
				else if ( race == AUTOMATON )
				{
					shieldLimb->x -= 0.1 * cos(this->yaw + PI / 2) + 2.1 * cos(this->yaw);
					shieldLimb->y -= 0.1 * sin(this->yaw + PI / 2) + 2.1 * sin(this->yaw);
					shieldLimb->z += 0.1;

					// originally shieldLimb->x/y was -1.1 * cos or sin but it fell outside the player bounding box and was visible in hud
					// so shieldLimb->x/y is -0.1 and -1 is added to focaly.
					shieldLimb->focaly -= 1;
				}
			}

            if ( shieldLimb->sprite == items[TOOL_LANTERN].index )
			{
				shieldLimb->z += 2;
			}
	        if ( flickerLights || ticks % TICKS_PER_SECOND == 1 )
	        {
			    if ( shieldLimb->sprite == items[TOOL_TORCH].index )
			    {
					if ( flameEntity = spawnFlame(shieldLimb, SPRITE_FLAME) )
					{
						flameEntity->x += 2.5 * cos(shieldLimb->yaw + PI / 16);
						flameEntity->y += 2.5 * sin(shieldLimb->yaw + PI / 16);
						flameEntity->z -= 2;
					}
			    }
			    else if ( shieldLimb->sprite == items[TOOL_CRYSTALSHARD].index )
			    {
				    /*flameEntity = spawnFlame(shieldLimb, SPRITE_CRYSTALFLAME);
				    flameEntity->x += 2.5 * cos(shieldLimb->yaw + PI / 16);
				    flameEntity->y += 2.5 * sin(shieldLimb->yaw + PI / 16);
				    flameEntity->z -= 2;*/
			    }
			    else if ( shieldLimb->sprite == items[TOOL_LANTERN].index )
			    {
					if ( flameEntity = spawnFlame(shieldLimb, SPRITE_FLAME) )
					{
						flameEntity->x += 2.5 * cos(shieldLimb->yaw);
						flameEntity->y += 2.5 * sin(shieldLimb->yaw);
						flameEntity->z += 1;
					}
			    }
			}
			if ( shieldLimb->sprite >= items[SPELLBOOK_LIGHT].index
				&& shieldLimb->sprite < (items[SPELLBOOK_LIGHT].index + items[SPELLBOOK_LIGHT].variations) )
			{
				shieldLimb->pitch = shieldArmLimb->pitch - .25 + 3 * PI / 2;
				shieldLimb->yaw += PI / 16;
				shieldLimb->focalx -= 4;
				if ( race == SKELETON )
				{
					shieldLimb->focaly += .5;
				}
				else if ( race == AUTOMATON )
				{
					shieldLimb->focaly += .5;
					shieldLimb->focalz -= .5;
				}
				shieldLimb->focalz += .5;
				shieldLimb->x += 0.5 * cos(this->yaw + PI / 2) + .5 * cos(this->yaw);
				shieldLimb->y += 0.5 * sin(this->yaw + PI / 2) + .5 * sin(this->yaw);
				shieldLimb->z -= 1;
				shieldLimb->scalex = 0.8;
				shieldLimb->scaley = 0.8;
				shieldLimb->scalez = 0.8;
			}
			else
			{
				// automaton arm clips through shield. extend shield out more.
				if ( race == AUTOMATON )
				{
					if ( shieldLimb->sprite != items[TOOL_TINKERING_KIT].index
						&& shieldLimb->sprite != items[TOOL_TORCH].index
						&& shieldLimb->sprite != items[TOOL_CRYSTALSHARD].index
						&& shieldLimb->sprite != items[TOOL_LANTERN].index )
					{
						shieldLimb->focalx += 0.75;
						shieldLimb->focaly += 1.25;
					}
				}
			}

			if ( this->fskill[8] > PI / 32 ) //MONSTER_SHIELDYAW and PLAYER_SHIELDYAW defending animation
			{
				if ( shieldLimb->sprite != items[TOOL_TORCH].index
					&& shieldLimb->sprite != items[TOOL_LANTERN].index
					&& shieldLimb->sprite != items[TOOL_CRYSTALSHARD].index )
				{
					// shield, so rotate a little.
					shieldLimb->roll += PI / 64;
				}
				else
				{
					shieldLimb->x += 0.25 * cos(this->yaw);
					shieldLimb->y += 0.25 * sin(this->yaw);
					shieldLimb->pitch += PI / 16;
					if ( flameEntity )
					{
						flameEntity->x += 0.75 * cos(shieldArmLimb->yaw);
						flameEntity->y += 0.75 * sin(shieldArmLimb->yaw);
					}
				}
			}
			else
			{
				if ( !shieldLimb->flags[INVISIBLE] )
				{
					if ( !itemSpriteIsQuiverThirdPersonModel(shieldLimb->sprite) )
					{
						shieldArmLimb->yaw -= PI / 8;
					}
				}
			}
			break;
		case GOBLIN:
		case GOATMAN:
		case INSECTOID:
		case INCUBUS:
		case SUCCUBUS:
			shieldLimb->x -= 2.5 * cos(this->yaw + PI / 2) + .20 * cos(this->yaw);
			shieldLimb->y -= 2.5 * sin(this->yaw + PI / 2) + .20 * sin(this->yaw);
			shieldLimb->z += 2.5;
			shieldLimb->yaw = shieldArmLimb->yaw;
			shieldLimb->roll = 0;
			shieldLimb->pitch = 0;

			/*if ( shieldLimb->sprite != items[TOOL_TORCH].index && shieldLimb->sprite != items[TOOL_LANTERN].index && shieldLimb->sprite != items[TOOL_CRYSTALSHARD].index )
			{
				shieldLimb->focalx = limbs[race][7][0];
				shieldLimb->focaly = limbs[race][7][1];
				shieldLimb->focalz = limbs[race][7][2];
			}
			else
			{
				shieldLimb->focalx = limbs[race][7][0] - 0.5;
				shieldLimb->focaly = limbs[race][7][1] - 1;
				shieldLimb->focalz = limbs[race][7][2];
			}*/

            if ( shieldLimb->sprite == items[TOOL_LANTERN].index )
			{
			    shieldLimb->z += 2;
			}
	        if ( flickerLights || ticks % TICKS_PER_SECOND == 1 )
	        {
		        if ( shieldLimb->sprite == items[TOOL_TORCH].index )
			    {
					if ( flameEntity = spawnFlame(shieldLimb, SPRITE_FLAME) )
					{
						flameEntity->x += 2 * cos(shieldLimb->yaw);
						flameEntity->y += 2 * sin(shieldLimb->yaw);
						flameEntity->z -= 2;
					}
			    }
			    else if ( shieldLimb->sprite == items[TOOL_CRYSTALSHARD].index )
			    {
				    /*flameEntity = spawnFlame(shieldLimb, SPRITE_CRYSTALFLAME);
				    flameEntity->x += 2 * cos(shieldLimb->yaw);
				    flameEntity->y += 2 * sin(shieldLimb->yaw);
				    flameEntity->z -= 2;*/
			    }
			    else if ( shieldLimb->sprite == items[TOOL_LANTERN].index )
			    {
					if ( flameEntity = spawnFlame(shieldLimb, SPRITE_FLAME) )
					{
						flameEntity->x += 2 * cos(shieldLimb->yaw);
						flameEntity->y += 2 * sin(shieldLimb->yaw);
						flameEntity->z += 1;
					}
			    }
			}
			if ( shieldLimb->sprite >= items[SPELLBOOK_LIGHT].index
				&& shieldLimb->sprite < (items[SPELLBOOK_LIGHT].index + items[SPELLBOOK_LIGHT].variations) )
			{
				shieldLimb->pitch = shieldArmLimb->pitch - .25 + 3 * PI / 2;
				shieldLimb->yaw += PI / 6;
				shieldLimb->focalx -= 4;
				shieldLimb->focalz += .5;
				if ( race == INCUBUS || race == SUCCUBUS )
				{
					shieldLimb->focalz -= 1.5;
				}
				shieldLimb->x += 0.5 * cos(this->yaw + PI / 2) + .5 * cos(this->yaw);
				shieldLimb->y += 0.5 * sin(this->yaw + PI / 2) + .5 * sin(this->yaw);
				shieldLimb->z -= 1;
				shieldLimb->scalex = 0.8;
				shieldLimb->scaley = 0.8;
				shieldLimb->scalez = 0.8;
			}
			else if ( itemSpriteIsQuiverThirdPersonModel(shieldLimb->sprite) )
			{
				shieldLimb->scalex = 1.05;
				//shieldLimb->scalez = 1.05;
				shieldLimb->focalz += 3;
				if ( race == INCUBUS || race == SUCCUBUS )
				{
					if ( race == SUCCUBUS )
					{
						shieldLimb->x -= 0.05 * cos(this->yaw + PI / 2) + (1.75) * cos(this->yaw);
						shieldLimb->y -= 0.05 * sin(this->yaw + PI / 2) + (1.75) * sin(this->yaw);
					}
					else
					{
						shieldLimb->x -= 0.05 * cos(this->yaw + PI / 2) + (1.5) * cos(this->yaw);
						shieldLimb->y -= 0.05 * sin(this->yaw + PI / 2) + (1.5) * sin(this->yaw);
					}
					shieldLimb->z += -2.28;
				}
				else
				{
					if ( race == GOATMAN )
					{
						shieldLimb->x -= -0.25 * cos(this->yaw + PI / 2) + 1.5 * cos(this->yaw);
						shieldLimb->y -= -0.25 * sin(this->yaw + PI / 2) + 1.5 * sin(this->yaw);
					}
					else if ( race == GOBLIN )
					{
						shieldLimb->x -= -0.25 * cos(this->yaw + PI / 2) + 1.25 * cos(this->yaw);
						shieldLimb->y -= -0.25 * sin(this->yaw + PI / 2) + 1.25 * sin(this->yaw);
					}
					else
					{
						shieldLimb->x -= -0.25 * cos(this->yaw + PI / 2) + 1.0 * cos(this->yaw);
						shieldLimb->y -= -0.25 * sin(this->yaw + PI / 2) + 1.0 * sin(this->yaw);
					}
					shieldLimb->z += -1.78;
				}
			}

			if ( this->fskill[8] > PI / 32 ) //MONSTER_SHIELDYAW and PLAYER_SHIELDYAW defending animation
			{
				if ( shieldLimb->sprite != items[TOOL_TORCH].index
					&& shieldLimb->sprite != items[TOOL_LANTERN].index
					&& shieldLimb->sprite != items[TOOL_CRYSTALSHARD].index )
				{
					// shield, so rotate a little.
					shieldLimb->roll += PI / 64;
				}
				else
				{
					shieldLimb->x += 0.25 * cos(this->yaw);
					shieldLimb->y += 0.25 * sin(this->yaw);
					shieldLimb->pitch += PI / 16;
					if ( flameEntity )
					{
						flameEntity->x += 0.75 * cos(shieldArmLimb->yaw);
						flameEntity->y += 0.75 * sin(shieldArmLimb->yaw);
					}
				}
			}
			break;
		default:
			break;
	}

	if ( shieldLimb->sprite == items[TOOL_TINKERING_KIT].index )
	{
		//shieldLimb->pitch = 0;
		shieldLimb->yaw += PI / 6;
		shieldLimb->focalx -= .5;
		shieldLimb->focaly += .25;
		shieldLimb->focalz += 2.25;
		if ( race == INCUBUS || race == SUCCUBUS )
		{
			shieldLimb->focalx -= .5;
			shieldLimb->focaly -= 0.7;
		}
		else if ( race == GOATMAN || race == GOBLIN || race == INSECTOID )
		{
			shieldLimb->focalz -= 0.5;
		}
		else if ( race == AUTOMATON || race == SKELETON )
		{
			shieldLimb->focalz -= 1;
			if ( race == SKELETON )
			{
				shieldLimb->focalx -= 0.5;
			}
			if ( this->fskill[8] > PI / 32 ) //MONSTER_SHIELDYAW and PLAYER_SHIELDYAW defending animation
			{
				shieldLimb->focalx += 2;
			}
			else
			{
				shieldLimb->focalx += 1;
				shieldLimb->focaly -= 0.8;
			}
		}
	}
	else if ( itemSpriteIsQuiverThirdPersonModel(shieldLimb->sprite) )
	{
		shieldLimb->yaw += PI / 6;
	}

	if ( flameEntity && player >= 0 )
	{
		if ( players[player]->isLocalPlayer() )
		{
			flameEntity->flags[GENIUS] = true;
			flameEntity->setUID(-4);
		}
		else
		{
			flameEntity->setUID(-3);
		}
	}
}

void Creature::handleKnockbackDamage(Stat& myStats, Entity* knockedInto)
{
	if ( knockedInto != NULL && myStats.EFFECTS[EFF_KNOCKBACK] && myStats.HP > 0 )
	{
		int damageOnHit = 5 + local_rng.rand() % 6;
		if ( knockedInto->behavior == &actDoor )
		{
			playSoundEntity(this, 28, 64);
			this->modHP(-damageOnHit);
			Entity* whoKnockedMe = uidToEntity(this->monsterKnockbackUID);
            Creature* whoKnockedMeCrtr = (Creature*)(*&whoKnockedMe);
			if ( myStats.HP <= 0 )
			{
				if ( whoKnockedMe )
				{
					whoKnockedMe->awardXP(this, true, true);
				}
			}
			if ( whoKnockedMeCrtr && whoKnockedMeCrtr->behavior == &actPlayer )
			{
				steamStatisticUpdateClient(whoKnockedMe->skill[2], STEAM_STAT_TAKE_THIS_OUTSIDE, STEAM_STAT_INT, 1);
			}
			knockedInto->doorHealth = 0;    // smash doors instantly
			playSoundEntity(knockedInto, 28, 64);
			if ( knockedInto->doorHealth <= 0 )
			{
				// set direction of splinters
				if ( !knockedInto->doorDir )
				{
					knockedInto->doorSmacked = (this->x > knockedInto->x);
				}
				else
				{
					knockedInto->doorSmacked = (this->y < knockedInto->y);
				}
			}
		}
		else if ( knockedInto->behavior == &::actFurniture )
		{
			// break it down!
			playSoundEntity(this, 28, 64);
			this->modHP(-damageOnHit);
			if ( myStats.HP <= 0 )
			{
				Entity* whoKnockedMe = uidToEntity(this->monsterKnockbackUID);
				if ( whoKnockedMe )
				{
					whoKnockedMe->awardXP(this, true, true);
				}
			}
			knockedInto->furnitureHealth = 0;    // smash furniture instantly
			playSoundEntity(knockedInto, 28, 64);
		}
	}
}

int32_t Creature::playerInsectoidExpectedManaFromHunger(Stat& myStats)
{
	double manaPercentFromHunger = myStats.HUNGER / 1000.f;
	double expectedManaValue = std::floor(myStats.MAXMP * manaPercentFromHunger);
	if ( myStats.HUNGER > 0 )
	{
		// add extra expected mana point here.
		// i.e 950 hunger is still full mana to avoid always having 1 short.
		// skip 0 hunger as it will be 0 expected.
		return (static_cast<int32_t>(expectedManaValue) + 1);
	}
	return static_cast<int32_t>(expectedManaValue);
}

void Creature::alertAlliesOnBeingHit(Creature *attacker, std::unordered_set<Entity*>* skipEntitiesToAlert)
{
	if ( !attacker )
	{
		return;
	}
	Stat* hitstats = getStats();
	if ( !hitstats )
	{
		return;
	}

	bool infightingStop = false;
	static ConsoleVariable<bool> cvar_infightingprotect("/infighting_protect", true);
	static ConsoleVariable<bool> cvar_infightingprotectplayerallies("/infighting_protect_player_allies", true);
	if ( (behavior == &actMonster && attacker->behavior == &actPlayer)
		|| (behavior == &actPlayer && attacker->behavior == &actMonster) )
	{
		infightingStop = *cvar_infightingprotectplayerallies;
	}
	else if ( behavior == &actMonster && attacker->behavior == &actMonster )
	{
		if ( !monsterAllyGetPlayerLeader() && !attacker->monsterAllyGetPlayerLeader() )
		{
			infightingStop = *cvar_infightingprotect;
		}
	}

	// alert other monsters too
	Entity* ohitentity = hit.entity;
	for (node_t * node = map.creatures->first; node != nullptr; node = node->next ) //Only searching for monsters, so don't iterate full map.entities.
	{
		Creature* entity = (Creature*)node->element;
		if ( !entity ) { continue; }
		if ( skipEntitiesToAlert && (skipEntitiesToAlert->find(entity) != skipEntitiesToAlert->end()) )
		{
			continue;
		}
		if ( entity->behavior == &actMonster && entity != this )
		{
			Stat* buddystats = entity->getStats();
			if ( buddystats != nullptr )
			{
				if ( buddystats->type == SHOPKEEPER && hitstats->type != SHOPKEEPER )
				{
					continue; // shopkeepers don't care about hitting humans/robots etc.
				}
				if ( hitstats->type == SHOPKEEPER && entity->monsterAllyGetPlayerLeader() )
				{
					continue; // hitting a shopkeeper, player followers won't retaliate against player
				}
				if ( entity->checkFriend(this) )
				{
					if ( entity->monsterState == MONSTER_STATE_WAIT )
					{
						if ( infightingStop && entity->checkFriend(attacker) )
						{
							if ( attacker->behavior == &actPlayer )
							{
								if ( (monsterAllyGetPlayerLeader() == nullptr)
									!= (entity->monsterAllyGetPlayerLeader() == nullptr) )
								{
									// if the fight is between player allies, outside mobs do not interfere
									//messagePlayer(0, MESSAGE_DEBUG, "Stopped an ally infight 1.");
									continue;
								}
							}
							else if ( behavior == &actPlayer )
							{
								if ( (attacker->monsterAllyGetPlayerLeader() == nullptr)
									!= (entity->monsterAllyGetPlayerLeader() == nullptr) )
								{
									// if the fight is between player allies, outside mobs do not interfere
									//messagePlayer(0, MESSAGE_DEBUG, "Stopped an ally infight 2.");
									continue;
								}
							}
							else
							{
								//messagePlayer(0, MESSAGE_DEBUG, "Stopped an infight.");
								continue;
							}
						}
						double tangent = atan2(entity->y - this->y, entity->x - this->x);
						lineTrace(this, this->x, this->y, tangent, 1024, 0, false);
						if ( hit.entity == entity )
						{
							entity->monsterAcquireAttackTarget(*attacker, MONSTER_STATE_PATH);
						}
					}
				}
			}
		}
	}
	hit.entity = ohitentity;
}

void Creature::increaseSkill(int skill, bool notify)
{
    Stat* myStats = this->getStats();
    int player = -1;

    if ( myStats == NULL )
    {
        return;
    }
    if ( this->behavior == &actPlayer )
    {
        player = this->skill[2];
    }
    Entity::increaseSkill(skill, notify);
}

int Creature::getHealthRegenInterval(Entity* my, Stat& myStats, bool isPlayer)
{
    Creature* myCrtr = dynamic_cast<Creature*>(my);
	if ( !(svFlags & SV_FLAG_HUNGER) )
	{
		if ( isPlayer )
		{
			return -1;
		}
	}
	if ( myStats.EFFECTS[EFF_VAMPIRICAURA] )
	{
		if ( isPlayer && myStats.EFFECTS_TIMERS[EFF_VAMPIRICAURA] > 0 )
		{
			return -1;
		}
	}
	if ( myStats.HP <= 0 )
	{
		return -1;
	}

	if ( myStats.breastplate && myStats.breastplate->type == VAMPIRE_DOUBLET )
	{
		return -1;
	}
	double healring = 0;
	if ( isPlayer && myStats.type != HUMAN )
	{
		if ( myStats.type == SKELETON )
		{
			healring = -1; // 0.25x regen speed.
		}
	}

	double bonusHealring = 0.0;
	bonusHealring += getHealringFromEquipment(my, myStats, isPlayer);
	bonusHealring += getHealringFromEffects(myCrtr, myStats);
	healring += bonusHealring;

	if ( myCrtr && bonusHealring >= 2.0 && ::ticks % TICKS_PER_SECOND == 0 )
	{
		bool oldRegen = myStats.EFFECTS[EFF_HP_REGEN];
		myStats.EFFECTS[EFF_HP_REGEN] = false;
		int bonusHealringNoRegen = getHealringFromEquipment(myCrtr, myStats, isPlayer) + getHealringFromEffects(myCrtr, myStats);
		if ( bonusHealringNoRegen >= 2 )
		{
			steamAchievementEntity(myCrtr, "BARONY_ACH_TROLLS_BLOOD");
		}
		myStats.EFFECTS[EFF_HP_REGEN] = oldRegen;
	}

	if ( healring > 3 )
	{
		healring = 3;
	}

	if ( !strncmp(map.name, "Mages Guild", 11) && myStats.type == SHOPKEEPER )
	{
		healring = 25; // these guys like regenerating
	}

	if ( healring > 0 )
	{
		return (HEAL_TIME / (healring * 6)); // 1 HP each 12 sec base
	}
	else if ( healring < 0 )
	{
		return (abs(healring) * HEAL_TIME * 4); // 1 HP each 48 sec if negative regen
	}
	else if ( healring == 0 )
	{
		return HEAL_TIME;
	}
	return HEAL_TIME;
}

Item** Creature::shouldMonsterEquipThisArmor(const Item& item) const
{
	Stat* myStats = getStats();
	if ( !myStats )
	{
		return nullptr;
	}

	switch ( checkEquipType(&item) )
	{
		case TYPE_HAT:
			if ( item.interactNPCUid == getUID() && myStats->helmet )
			{
				return &myStats->helmet;
			}
			if ( myStats->helmet && (myStats->helmet->beatitude < 0 || myStats->helmet->forcedPickupByPlayer == true) )
			{
				return nullptr; //No can has hats : (
			}

			return Item::isThisABetterArmor(item, myStats->helmet) ? &myStats->helmet : nullptr;
		case TYPE_HELM:
			if ( item.interactNPCUid == getUID() && myStats->helmet )
			{
				return &myStats->helmet;
			}

			if ( myStats->helmet && (myStats->helmet->beatitude < 0 || myStats->helmet->forcedPickupByPlayer == true) )
			{
				return nullptr; //Can't swap out armor piece if current one is cursed!
			}

			if ( myStats->type == GOBLIN && myStats->helmet && checkEquipType(myStats->helmet) == TYPE_HAT )
			{
				return nullptr; //Goblins love hats.
			}

			return Item::isThisABetterArmor(item, myStats->helmet) ? &myStats->helmet : nullptr;
			break;
		case TYPE_SHIELD:
		case TYPE_OFFHAND:
			if ( item.interactNPCUid == getUID() && myStats->shield )
			{
				return &myStats->shield;
			}

			if ( myStats->shield && (myStats->shield->beatitude < 0 || myStats->shield->forcedPickupByPlayer == true) )
			{
				return nullptr; //Can't swap out armor piece if current one is cursed!
			}

			return Item::isThisABetterArmor(item, myStats->shield) ? &myStats->shield : nullptr;
		case TYPE_BREASTPIECE:
			if ( item.interactNPCUid == getUID() && myStats->breastplate )
			{
				return &myStats->breastplate;
			}

			if ( myStats->breastplate && (myStats->breastplate->beatitude < 0 || myStats->breastplate->forcedPickupByPlayer == true) )
			{
				return nullptr; //Can't swap out armor piece if current one is cursed!
			}

			return Item::isThisABetterArmor(item, myStats->breastplate) ? &myStats->breastplate : nullptr;
		case TYPE_CLOAK:
			if ( item.interactNPCUid == getUID() && myStats->cloak )
			{
				return &myStats->cloak;
			}

			if ( myStats->cloak && (myStats->cloak->beatitude < 0 || myStats->cloak->forcedPickupByPlayer == true) )
			{
				return nullptr; //Can't swap out armor piece if current one is cursed!
			}

			return Item::isThisABetterArmor(item, myStats->cloak) ? &myStats->cloak : nullptr;
		case TYPE_BOOTS:
			if ( item.interactNPCUid == getUID() && myStats->shoes )
			{
				return &myStats->shoes;
			}

			if ( myStats->shoes && (myStats->shoes->beatitude < 0 || myStats->shoes->forcedPickupByPlayer == true) )
			{
				return nullptr; //Can't swap out armor piece if current one is cursed!
			}

			return Item::isThisABetterArmor(item, myStats->shoes) ? &myStats->shoes : nullptr;
		case TYPE_GLOVES:
			if ( item.interactNPCUid == getUID() && myStats->gloves )
			{
				return &myStats->gloves;
			}

			if ( myStats->gloves && (myStats->gloves->beatitude < 0 || myStats->gloves->forcedPickupByPlayer == true) )
			{
				return nullptr; //Can't swap out armor piece if current one is cursed!
			}

			return Item::isThisABetterArmor(item, myStats->gloves) ? &myStats->gloves : nullptr;
			break;
		default:
			return nullptr;
	}
}