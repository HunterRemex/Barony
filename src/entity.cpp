/*-------------------------------------------------------------------------------

BARONY
File: entity.cpp
Desc: implements entity code

Copyright 2013-2016 (c) Turning Wheel LLC, all rights reserved.
See LICENSE for details.

-------------------------------------------------------------------------------*/

#include "main.hpp"
#include "game.hpp"
#include "stat.hpp"
#include "entity.hpp"
#include "items.hpp"
#include "monster.hpp"
#include "engine/audio/sound.hpp"
#include "magic/magic.hpp"
#include "interface/interface.hpp"
#include "net.hpp"
#include "collision.hpp"
#include "paths.hpp"
#include "book.hpp"
#include "shops.hpp"
#ifdef STEAMWORKS
#include <steam/steam_api.h>
#endif
#include "player.hpp"
#include "scores.hpp"
#include "menu.hpp"
#include "mod_tools.hpp"
#ifdef __ARM_NEON__
#include <arm_neon.h>
#endif
#include "ui/MainMenu.hpp"
#include "ui/GameUI.hpp"
#include "creature.h"

/*-------------------------------------------------------------------------------

Entity::Entity)

Construct an Entity

-------------------------------------------------------------------------------*/

Entity::Entity(Sint32 in_sprite, Uint32 pos, list_t* entlist, list_t* creaturelist) :
        Entity_Base(skill[47],fskill[29],fskill[28],fskill[27]),
        lightBonus(0.f),
        char_gonnavomit(skill[26]),
        char_heal(skill[22]),
        char_energize(skill[23]),
        char_torchtime(skill[25]),
        char_poison(skill[21]),
        char_fire(skill[36]),
        chanceToPutOutFire(skill[37]),
        circuit_status(skill[28]),
        switch_power(skill[0]),
        chestInit(skill[0]),
        chestStatus(skill[1]),
        chestHealth(skill[3]),
        chestLocked(skill[4]),
        chestOpener(skill[5]),
        chestLidClicked(skill[6]),
        chestAmbience(skill[7]),
        chestMaxHealth(skill[8]),
        chestType(skill[9]),
        chestPreventLockpickCapstoneExploit(skill[10]),
        chestHasVampireBook(skill[11]),
        chestLockpickHealth(skill[12]),
        chestOldHealth(skill[15]),
        crystalInitialised(skill[1]),
        crystalTurning(skill[3]),
        crystalTurnStartDir(skill[4]),
        crystalGeneratedElectricityNodes(skill[5]),
        crystalNumElectricityNodes(skill[6]),
        crystalHoverDirection(skill[7]),
        crystalHoverWaitTimer(skill[8]),
        crystalTurnReverse(skill[9]),
        crystalSpellToActivate(skill[10]),
        crystalStartZ(fskill[0]),
        crystalMaxZVelocity(fskill[1]),
        crystalMinZVelocity(fskill[2]),
        crystalTurnVelocity(fskill[3]),
        creatureWebbedSlowCount(skill[52]),
        particleDuration(skill[0]),
        particleShrink(skill[1]),
        itemNotMoving(skill[18]),
        itemNotMovingClient(skill[19]),
        itemSokobanReward(skill[20]),
        itemOriginalOwner(skill[21]),
        itemStolen(skill[22]),
        itemShowOnMap(skill[23]),
        itemDelayMonsterPickingUp(skill[24]),
        itemReceivedDetailsFromServer(skill[25]),
        itemAutoSalvageByPlayer(skill[26]),
        itemSplooshed(skill[27]),
        itemWaterBob(fskill[2]),
        gateInit(skill[1]),
        gateStatus(skill[3]),
        gateRattle(skill[4]),
        gateStartHeight(fskill[0]),
        gateVelZ(vel_z),
        gateInverted(skill[5]),
        gateDisableOpening(skill[6]),
        leverStatus(skill[1]),
        leverTimerTicks(skill[3]),
        boulderTrapRefireAmount(skill[1]),
        boulderTrapRefireDelay(skill[3]),
        boulderTrapAmbience(skill[6]),
        boulderTrapFired(skill[0]),
        boulderTrapRefireCounter(skill[4]),
        boulderTrapPreDelay(skill[5]),
        boulderTrapRocksToSpawn(skill[7]),
        doorDir(skill[0]),
        doorInit(skill[1]),
        doorStatus(skill[3]),
        doorHealth(skill[4]),
        doorLocked(skill[5]),
        doorSmacked(skill[6]),
        doorTimer(skill[7]),
        doorOldStatus(skill[8]),
        doorMaxHealth(skill[9]),
        doorStartAng(fskill[0]),
        doorPreventLockpickExploit(skill[10]),
        doorForceLockedUnlocked(skill[11]),
        doorDisableLockpicks(skill[12]),
        doorDisableOpening(skill[13]),
        doorLockpickHealth(skill[14]),
        doorOldHealth(skill[15]),
        particleTimerDuration(skill[0]),
        particleTimerEndAction(skill[1]),
        particleTimerEndSprite(skill[3]),
        particleTimerCountdownAction(skill[4]),
        particleTimerCountdownSprite(skill[5]),
        particleTimerTarget(skill[6]),
        particleTimerPreDelay(skill[7]),
        particleTimerVariable1(skill[8]),
        particleTimerVariable2(skill[9]),
        pedestalHasOrb(skill[0]),
        pedestalOrbType(skill[1]),
        pedestalInvertedPower(skill[3]),
        pedestalInGround(skill[4]),
        pedestalInit(skill[5]),
        pedestalAmbience(skill[6]),
        pedestalLockOrb(skill[7]),
        pedestalPowerStatus(skill[8]),
        orbInitialised(skill[1]),
        orbHoverDirection(skill[7]),
        orbHoverWaitTimer(skill[8]),
        orbStartZ(fskill[0]),
        orbMaxZVelocity(fskill[1]),
        orbMinZVelocity(fskill[2]),
        orbTurnVelocity(fskill[3]),
        portalAmbience(skill[0]),
        portalInit(skill[1]),
        portalNotSecret(skill[3]),
        portalVictoryType(skill[4]),
        portalFireAnimation(skill[5]),
        portalCustomLevelsToJump(skill[6]),
        portalCustomRequiresPower(skill[7]),
        portalCustomSprite(skill[8]),
        portalCustomSpriteAnimationFrames(skill[9]),
        portalCustomZOffset(skill[10]),
        portalCustomLevelText1(skill[11]),
        portalCustomLevelText2(skill[12]),
        portalCustomLevelText3(skill[13]),
        portalCustomLevelText4(skill[14]),
        portalCustomLevelText5(skill[15]),
        portalCustomLevelText6(skill[16]),
        portalCustomLevelText7(skill[17]),
        portalCustomLevelText8(skill[18]),
        teleporterX(skill[0]),
        teleporterY(skill[1]),
        teleporterType(skill[3]),
        teleporterAmbience(skill[4]),
        spellTrapType(skill[0]),
        spellTrapRefire(skill[1]),
        spellTrapLatchPower(skill[3]),
        spellTrapFloorTile(skill[4]),
        spellTrapRefireRate(skill[5]),
        spellTrapAmbience(skill[6]),
        spellTrapInit(skill[7]),
        spellTrapCounter(skill[8]),
        spellTrapReset(skill[9]),
        shrineSpellEffect(skill[0]),
        shrineRefire1(skill[1]),
        shrineRefire2(skill[3]),
        shrineDir(skill[4]),
        shrineAmbience(skill[5]),
        shrineInit(skill[6]),
        shrineActivateDelay(skill[7]),
        shrineZ(skill[8]),
        shrineDestXOffset(skill[9]),
        shrineDestYOffset(skill[10]),
        ceilingTileModel(skill[0]),
        ceilingTileDir(skill[1]),
        ceilingTileAllowTrap(skill[3]),
        ceilingTileBreakable(skill[4]),
        floorDecorationModel(skill[0]),
        floorDecorationRotation(skill[1]),
        floorDecorationHeightOffset(skill[3]),
        floorDecorationXOffset(skill[4]),
        floorDecorationYOffset(skill[5]),
        floorDecorationInteractText1(skill[8]),
        floorDecorationInteractText2(skill[9]),
        floorDecorationInteractText3(skill[10]),
        floorDecorationInteractText4(skill[11]),
        floorDecorationInteractText5(skill[12]),
        floorDecorationInteractText6(skill[13]),
        floorDecorationInteractText7(skill[14]),
        floorDecorationInteractText8(skill[15]),
        colliderDecorationModel(skill[0]),
        colliderDecorationRotation(skill[1]),
        colliderDecorationHeightOffset(skill[3]),
        colliderDecorationXOffset(skill[4]),
        colliderDecorationYOffset(skill[5]),
        colliderHasCollision(skill[6]),
        colliderSizeX(skill[7]),
        colliderSizeY(skill[8]),
        colliderMaxHP(skill[9]),
        colliderDiggable(skill[10]),
        colliderDamageTypes(skill[11]),
        colliderCurrentHP(skill[12]),
        colliderOldHP(skill[13]),
        colliderInit(skill[14]),
        furnitureType(skill[0]),
        furnitureInit(skill[1]),
        furnitureDir(skill[3]),
        furnitureHealth(skill[4]),
        furnitureMaxHealth(skill[9]),
        furnitureTableRandomItemChance(skill[10]),
        furnitureTableSpawnChairs(skill[11]),
        furnitureOldHealth(skill[15]),
        pistonCamDir(skill[0]),
        pistonCamTimer(skill[1]),
        pistonCamRotateSpeed(fskill[0]),
        arrowPower(skill[3]),
        arrowPoisonTime(skill[4]),
        arrowArmorPierce(skill[5]),
        arrowSpeed(fskill[4]),
        arrowFallSpeed(fskill[5]),
        arrowBoltDropOffRange(skill[6]),
        arrowShotByWeapon(skill[7]),
        arrowQuiverType(skill[8]),
        arrowShotByParent(skill[9]),
        actmagicIsVertical(skill[6]),
        actmagicIsOrbiting(skill[7]),
        actmagicOrbitDist(skill[8]),
        actmagicOrbitVerticalDirection(skill[9]),
        actmagicOrbitLifetime(skill[10]),
        actmagicMirrorReflected(skill[24]),
        actmagicMirrorReflectedCaster(skill[12]),
        actmagicCastByMagicstaff(skill[13]),
        actmagicOrbitVerticalSpeed(fskill[2]),
        actmagicOrbitStartZ(fskill[3]),
        actmagicOrbitStationaryX(fskill[4]),
        actmagicOrbitStationaryY(fskill[5]),
        actmagicOrbitStationaryCurrentDist(fskill[6]),
        actmagicOrbitStationaryHitTarget(skill[14]),
        actmagicOrbitHitTargetUID1(skill[15]),
        actmagicOrbitHitTargetUID2(skill[16]),
        actmagicOrbitHitTargetUID3(skill[17]),
        actmagicOrbitHitTargetUID4(skill[18]),
        actmagicProjectileArc(skill[19]),
        actmagicOrbitCastFromSpell(skill[20]),
        actmagicSpellbookBonus(skill[21]),
        actmagicCastByTinkerTrap(skill[22]),
        actmagicTinkerTrapFriendlyFire(skill[23]),
        actmagicReflectionCount(skill[25]),
        goldAmount(skill[0]),
        goldAmbience(skill[1]),
        goldSokoban(skill[2]),
        soundSourceFired(skill[0]),
        soundSourceToPlay(skill[1]),
        soundSourceVolume(skill[2]),
        soundSourceLatchOn(skill[3]),
        soundSourceDelay(skill[4]),
        soundSourceDelayCounter(skill[5]),
        soundSourceOrigin(skill[6]),
        lightSourceBrightness(skill[0]),
        lightSourceAlwaysOn(skill[1]),
        lightSourceInvertPower(skill[2]),
        lightSourceLatchOn(skill[3]),
        lightSourceRadius(skill[4]),
        lightSourceFlicker(skill[5]),
        lightSourceDelay(skill[6]),
        lightSourceDelayCounter(skill[7]),
        textSourceColorRGB(skill[0]),
        textSourceVariables4W(skill[1]),
        textSourceDelay(skill[2]),
        textSourceIsScript(skill[3]),
        textSourceBegin(skill[4]),
        signalActivateDelay(skill[1]),
        signalTimerInterval(skill[2]),
        signalTimerRepeatCount(skill[3]),
        signalTimerLatchInput(skill[4]),
        signalInputDirection(skill[5]),
        effectPolymorph(skill[50]),
        effectShapeshift(skill[53]),
        entityShowOnMap(skill[59]),
        thrownProjectilePower(skill[19]),
        thrownProjectileCharge(skill[20]),
        worldTooltipAlpha(fskill[0]),
        worldTooltipZ(fskill[1]),
        worldTooltipActive(skill[0]),
        worldTooltipPlayer(skill[1]),
        worldTooltipInit(skill[3]),
        worldTooltipFadeDelay(skill[4]),
        worldTooltipIgnoreDrawing(skill[5]),
        worldTooltipRequiresButtonHeld(skill[6]),
        monsterAnimationLimbOvershoot(skill[30]),
        monsterEntityRenderAsTelepath(skill[41]),
        monsterSpellAnimation(skill[31]),
        monsterAllyIndex(skill[42]),
        monsterAttack(skill[8]),
        monsterAttackTime(skill[9]),
        monsterAnimationLimbDirection(skill[20]),
        monsterArmbended(skill[10]),
        monsterWeaponYaw(fskill[5]),
        statueInit(skill[0]),
        statueDir(skill[1]),
        creatureShadowTaggedThisUid(skill[54]),
        statueId(skill[3])
{
	int c;
	// add the entity to the entity list
	if ( !pos )
	{
		mynode = list_AddNodeFirst(entlist);
	}
	else
	{
		mynode = list_AddNodeLast(entlist);
	}
	mynode->element = this;
	mynode->deconstructor = &entityDeconstructor;
	mynode->size = sizeof(Entity);

	myCreatureListNode = nullptr;
	if ( creaturelist )
	{
		addToCreatureList(creaturelist);
	}
	myWorldUIListNode = nullptr;
	myTileListNode = nullptr;

	// now reset all of my data elements
	lastupdate = 0;
	lastupdateserver = 0;
	ticks = 0;
	x = 0;
	y = 0;
	z = 0;
	new_x = 0;
	new_y = 0;
	new_z = 0;
	focalx = 0;
	focaly = 0;
	focalz = 0;
	scalex = 1;
	scaley = 1;
	scalez = 1;
	vel_x = 0;
	vel_y = 0;
	vel_z = 0;
	sizex = 0;
	sizey = 0;
	yaw = 0;
	pitch = 0;
	roll = 0;
	new_yaw = 0;
	new_pitch = 0;
	new_roll = 0;
	lerpCurrentState.resetMovement();
	lerpCurrentState.resetPosition();
	lerpPreviousState.resetMovement();
	lerpPreviousState.resetPosition();
	lerpRenderState.resetMovement();
	lerpRenderState.resetPosition();
	bNeedsRenderPositionInit = true;
	bUseRenderInterpolation = false;
	mapGenerationRoomX = 0;
	mapGenerationRoomY = 0;
	lerp_ox = 0.0;
	lerp_oy = 0.0;
	sprite = in_sprite;
	light = nullptr;
	string = nullptr;
	children.first = nullptr;
	children.last = nullptr;
	//this->magic_effects = (list_t *) malloc(sizeof(list_t));
	//this->magic_effects->first = NULL; this->magic_effects->last = NULL;
	for ( c = 0; c < NUMENTITYSKILLS; ++c )
	{
		skill[c] = 0;
	}
	for ( c = 0; c < NUMENTITYFSKILLS; ++c )
	{
		fskill[c] = 0;
	}
	skill[2] = -1;
	for ( c = 0; c < 16; c++ )
	{
		flags[c] = false;
	}
	if ( entlist == map.entities )
	{
		if ( multiplayer != CLIENT || loading )
		{
			uid = entity_uids;
			entity_uids++;
			map.entities_map.insert({ uid, mynode });
		}
		else
		{
			uid = -2;
		}
	}
	else
	{
		uid = -2;
	}
	behavior = nullptr;
	ranbehavior = false;
	parent = 0;
	path = nullptr;
	monsterAllyIndex = -1; // set to -1 to not reference player indices 0-3.
	/*if ( checkSpriteType(this->sprite) > 1 )
	{
		setSpriteAttributes(this, nullptr, nullptr);
	}*/

	clientStats = nullptr;
	clientsHaveItsStats = false;
}

void Entity::setUID(Uint32 new_uid)
{
	if ( mynode->list == map.entities )
	{
		map.entities_map.erase(uid);
		map.entities_map.insert({ new_uid, mynode });
	}
	uid = new_uid;
}

/*-------------------------------------------------------------------------------

Entity::~Entity)

Deconstruct an Entity

-------------------------------------------------------------------------------*/

Entity::~Entity()
{
	node_t* node;
	//node_t *node2;
	int i;
	//deleteent_t *deleteent;

	// remove any remaining "parent" references
	/*if( entity->mynode != NULL ) {
	if( entity->mynode->list != NULL ) {
	for( node2=entity->mynode->list->first; node2!=NULL; node2=node2->next ) {
	Entity *entity2 = (Entity *)node2->element;
	if( entity2 != entity && entity2 != NULL )
	if( entity2->parent == entity )
	entity2->parent = NULL;
	}
	}
	}*/

	//Remove me from the
	if ( myCreatureListNode )
	{
		list_RemoveNode(myCreatureListNode);
		myCreatureListNode = nullptr;
	}
	if ( myWorldUIListNode )
	{
		list_RemoveNode(myWorldUIListNode);
		myWorldUIListNode = nullptr;
	}
	if ( myTileListNode )
	{
		list_RemoveNode(myTileListNode);
		myTileListNode = nullptr;
	}

	// alert clients of the entity's deletion
	if ( multiplayer == SERVER && !loading )
	{
		if ( mynode->list == map.entities && uid != 0 && flags[NOUPDATE] == false )
		{
			for ( i = 1; i < MAXPLAYERS; ++i )
			{
				if ( client_disconnected[i] == true || players[i]->isLocalPlayer() )
				{
					continue;
				}

				// create a reminder for the server to continue informing the client of the deletion
				/*deleteent = (deleteent_t *) malloc(sizeof(deleteent_t)); //TODO: C++-PORT: Replace with new + class.
				deleteent->uid = uid;
				deleteent->tries = 0;
				node = list_AddNodeLast(&entitiesToDelete[i]);
				node->element = deleteent;
				node->deconstructor = &defaultDeconstructor;*/

				// send the delete entity command to the client
				strcpy((char*)net_packet->data, "ENTD");
				SDLNet_Write32((Uint32)uid, &net_packet->data[4]);
				net_packet->address.host = net_clients[i - 1].host;
				net_packet->address.port = net_clients[i - 1].port;
				net_packet->len = 8;
				/*if ( directConnect ) {
				SDLNet_UDP_Send(net_sock,-1,net_packet);
				} else {
				#ifdef STEAMWORKS
				SteamNetworking()->SendP2PPacket(*static_cast<CSteamID* >(steamIDRemote[i - 1]), net_packet->data, net_packet->len, k_EP2PSendUnreliable, 0);
				#endif
				}*/
				sendPacketSafe(net_sock, -1, net_packet, i - 1);
			}
		}
	}

	// set appropriate player pointer to NULL
	for ( i = 0; i < MAXPLAYERS; ++i )
	{
		if ( this == players[i]->entity )
		{
			players[i]->entity = nullptr;    //TODO: PLAYERSWAP VERIFY. Should this do anything to the player itself?
			players[i]->cleanUpOnEntityRemoval();
		}
	}
	// destroy my children
	list_FreeAll(&this->children);

	node = list_AddNodeLast(&entitiesdeleted);
	node->element = this;
	node->deconstructor = &emptyDeconstructor;

	if ( clientStats )
	{
		delete clientStats;
	}

	printlog("ENTITY DECON: %i", uid);

}

/*-------------------------------------------------------------------------------

Entity::setObituary

Sets the obituary text on an entity.

-------------------------------------------------------------------------------*/

void Entity::setObituary(const char* obituary)
{
	Stat* tempStats = this->getStats();
	if ( !tempStats )
	{
		return;
	}
	strncpy(tempStats->obituary, obituary, 127);
}

/*-------------------------------------------------------------------------------

Entity::killedByMonsterObituary

Sets the obituary to that of a mon

-------------------------------------------------------------------------------*/

void Entity::killedByMonsterObituary(Entity* victim)
{
    Creature* victimCrtr = (Creature*)victim;
	if ( !victim )
	{
		return;
	}

	Stat* hitstats = victim->getStats();
	if ( !hitstats )
	{
	    return;
	}

	if ( behavior == &actMagicTrap )
	{
	    hitstats->killer = KilledBy::TRAP_MAGIC;
	    victim->setObituary(Language::get(1501));
	    return;
	}
	if ( behavior == &::actMagicTrapCeiling )
	{
	    hitstats->killer = KilledBy::TRAP_MAGIC;
	    victim->setObituary(Language::get(1502));
	    return;
	}
	if ( behavior == &actBoulder )
	{
	    hitstats->killer = KilledBy::BOULDER;
		if ( sprite == 989 )
		{
			victim->setObituary(Language::get(3898));
		}
		else if ( sprite == 990 )
		{
			victim->setObituary(Language::get(3899));
		}
		return;
	}

	Stat* myStats = this->getStats();
	if ( !myStats )
	{
		return;
	}

	if ( myStats->type == hitstats->type )
	{
		hitstats->killer = KilledBy::ALLY_BETRAYAL;
		if ( hitstats->sex == MALE )
		{
			snprintf(tempstr, 256, Language::get(1509), getMonsterLocalizedName(hitstats->type).c_str());
		}
		else
		{
			snprintf(tempstr, 256, Language::get(1510), getMonsterLocalizedName(hitstats->type).c_str());
		}
		victim->setObituary(tempstr);
	}
	else
	{
		hitstats->killer = KilledBy::MONSTER;
		switch ( myStats->type )
		{
			case HUMAN:
				victim->setObituary(Language::get(1511));
				break;
			case RAT:
				victim->setObituary(Language::get(1512));
				break;
			case GOBLIN:
				victim->setObituary(Language::get(1513));
				break;
			case SLIME:
				victim->setObituary(Language::get(1514));
				break;
			case TROLL:
				victim->setObituary(Language::get(1515));
				break;
			case SPIDER:
			    if (arachnophobia_filter) {
				    victim->setObituary(Language::get(4090));
			    } else {
				    victim->setObituary(Language::get(1516));
			    }
				break;
			case GHOUL:
				victim->setObituary(Language::get(1517));
				break;
			case SKELETON:
				victim->setObituary(Language::get(1518));
				break;
			case SCORPION:
				victim->setObituary(Language::get(1519));
				break;
			case CREATURE_IMP:
				victim->setObituary(Language::get(1520));
				break;
			case GNOME:
				victim->setObituary(Language::get(1521));
				break;
			case DEMON:
				victim->setObituary(Language::get(1522));
				break;
			case SUCCUBUS:
				victim->setObituary(Language::get(1523));
				break;
			case LICH:
				victim->setObituary(Language::get(1524));
				break;
			case MINOTAUR:
				victim->setObituary(Language::get(1525));
				break;
			case DEVIL:
				victim->setObituary(Language::get(1526));
				break;
			case SHOPKEEPER:
				if ( victimCrtr && victimCrtr->behavior == &actPlayer )
				{
					if ( hitstats->type != HUMAN )
					{
						snprintf(hitstats->obituary, 127, Language::get(3244), getMonsterLocalizedPlural(hitstats->type).c_str(), myStats->name);
						hitstats->killer = KilledBy::TRESPASSING;
					}
					else
					{
						victim->setObituary(Language::get(1527)); // attempts a robbery.
						hitstats->killer = KilledBy::ATTEMPTED_ROBBERY;
					}
				}
				else
				{
					victim->setObituary(Language::get(1527)); // attempts a robbery.
					hitstats->killer = KilledBy::ATTEMPTED_ROBBERY;
				}
				break;
			case KOBOLD:
				victim->setObituary(Language::get(2150));
				break;
			case SCARAB:
				victim->setObituary(Language::get(2151));
				break;
			case CRYSTALGOLEM:
				victim->setObituary(Language::get(2152));
				break;
			case INCUBUS:
				victim->setObituary(Language::get(2153));
				break;
			case VAMPIRE:
				victim->setObituary(Language::get(2154));
				break;
			case SHADOW:
				victim->setObituary(Language::get(2155));
				break;
			case COCKATRICE:
				victim->setObituary(Language::get(2156));
				break;
			case INSECTOID:
				victim->setObituary(Language::get(2157));
				break;
			case GOATMAN:
				victim->setObituary(Language::get(2158));
				break;
			case AUTOMATON:
				victim->setObituary(Language::get(2159));
				break;
			case LICH_ICE:
				victim->setObituary(Language::get(2160));
				break;
			case LICH_FIRE:
				victim->setObituary(Language::get(2161));
				break;
			default:
				victim->setObituary(Language::get(1500));
				break;
		}
	}
	hitstats->killer_monster = myStats->type;
	hitstats->killer_name = myStats->name;
}

/*-------------------------------------------------------------------------------

Entity::light

Returns the illumination of the given entity

-------------------------------------------------------------------------------*/

int Entity::entityLight()
{
	if ( this->x < 0 || this->y < 0 || this->x >= map.width << 4 || this->y >= map.height << 4 )
	{
		return 255;
	}
	int light_x = (int)this->x / 16;
	int light_y = (int)this->y / 16;
    const auto& light = lightmaps[0][light_y + light_x * map.height];
    //return (light.x + light.y + light.z) / 3.f;
    return std::min(std::max(0, (int)((light.x + light.y + light.z) / 3.f)), 255);
	//return std::min(std::max(0, (int)((light.x + light.y + light.z) / 3.f * 255.f)), 255);
}

/*-------------------------------------------------------------------------------

Entity::entityLightAfterReductions

Returns new entities' illumination,  
after reductions depending on the entity stats and another entity observing

-------------------------------------------------------------------------------*/

int Entity::entityLightAfterReductions(Stat& myStats, Entity* observer)
{
	int player = -1;
	const int minLight = (int)(TOUCHRANGE * 1.5);
	int light = std::max(minLight, entityLight()); // max 255 light to start with.
	bool invis = isInvisible();
	if ( !invis )
	{
		bool sneaking = false;
        auto crtrBhvr = ((void(*)(Creature *))behavior);
		if ( crtrBhvr && crtrBhvr == &actPlayer )
		{
			player = skill[2];
			if ( player > -1 && stats[player] )
			{
				if ( stats[player]->sneaking == 1 && !stats[player]->defending )
				{
					sneaking = true;
				}
			}
		}

		if ( observer )
		{
			light += observer->getPER() * 4; // add light level for PER x 4
			if ( sneaking )
			{
				light /= 2; // halve for sneaking
			}
			light -= (light - TOUCHRANGE) * (1.0 * (myStats.PROFICIENCIES[PRO_STEALTH] / 100.0)); // reduce to 32 as sneak approaches 100
			Stat* observerStats = observer->getStats();
			if ( observerStats && observerStats->EFFECTS[EFF_BLIND] )
			{
				light = TOUCHRANGE;
			}
			if ( Creature* crtrObserver = (Creature*)observer;
                crtrObserver && crtrObserver->behavior == &actMonster
                && crtrObserver->monsterLastDistractedByNoisemaker > 0
                && uidToEntity(crtrObserver->monsterLastDistractedByNoisemaker) )
			{
				if (crtrObserver->monsterTarget == crtrObserver->monsterLastDistractedByNoisemaker
                    || myStats.EFFECTS[EFF_DISORIENTED] )
				{
					// currently hunting noisemaker.
					light = 16;
				}
			}
		}
		else
		{
			if ( sneaking )
			{
				light /= 2; // halve for sneaking
			}
			light -= (light - TOUCHRANGE) * (1.0 * (myStats.PROFICIENCIES[PRO_STEALTH] / 100.0)); // reduce to 32 as sneak approaches 100
		}
	}
	
	if ( invis )
	{
		light = std::min(light, TOUCHRANGE);
	}

	light = std::max(light, 0);
	if ( myStats.type == DUMMYBOT )
	{
		light = std::max(light, 256); // dummybots can always be seen at least 16 tiles away.
	}
	return light;
}

/*-------------------------------------------------------------------------------

Entity::increaseSkill

Increases the given skill of the given entity by 1.

-------------------------------------------------------------------------------*/

void Entity::increaseSkill(int skill, bool notify)
{
	Stat* myStats = this->getStats();
	int player = -1;

	if ( myStats == NULL )
	{
		return;
	}

	Uint32 color = makeColorRGB(255, 255, 0);
	if ( myStats->PROFICIENCIES[skill] < 100 )
	{
		myStats->PROFICIENCIES[skill]++;
		if ( notify )
		{
			if ( player >= 0 )
			{
				messagePlayerColor(player, MESSAGE_SPAM_MISC, color, Language::get(615), getSkillLangEntry(skill));
				if ( players[player]->isLocalPlayer() )
				{
					skillUpAnimation[player].addSkillUp(skill, myStats->PROFICIENCIES[skill] - 1, 1);
				}
			}
		}
		switch ( myStats->PROFICIENCIES[skill] )
		{
			case 20:
				messagePlayerColor(player, MESSAGE_PROGRESSION, color, Language::get(616), getSkillLangEntry(skill));
				break;
			case 40:
				messagePlayerColor(player, MESSAGE_PROGRESSION, color, Language::get(617), getSkillLangEntry(skill));
				break;
			case 60:
				messagePlayerColor(player, MESSAGE_PROGRESSION, color, Language::get(618), getSkillLangEntry(skill));
				break;
			case 80:
				messagePlayerColor(player, MESSAGE_PROGRESSION, color, Language::get(619), getSkillLangEntry(skill));
				break;
			case 100:
				messagePlayerColor(player, MESSAGE_PROGRESSION, color, Language::get(620), getSkillLangEntry(skill));
				break;
			default:
				break;
		}

		if ( skill == PRO_SPELLCASTING && skillCapstoneUnlockedEntity(PRO_SPELLCASTING) )
		{
			//Spellcasting capstone = free casting of Forcebolt.
			//Give the player the spell if they haven't learned it yet.
			if ( player > 0 && multiplayer == SERVER && !players[player]->isLocalPlayer() )
			{
				strcpy((char*)net_packet->data, "ASPL");
				net_packet->data[4] = clientnum;
				net_packet->data[5] = SPELL_FORCEBOLT;
				net_packet->address.host = net_clients[player - 1].host;
				net_packet->address.port = net_clients[player - 1].port;
				net_packet->len = 6;
				sendPacketSafe(net_sock, -1, net_packet, player - 1);
			}
			else if ( player >= 0 )
			{
				addSpell(SPELL_FORCEBOLT, player, true);
			}
		}

		if ( skill == PRO_STEALTH && myStats->PROFICIENCIES[skill] == 100 )
		{
			if ( player >= 0 && client_classes[player] == CLASS_ACCURSED )
			{
				steamAchievementClient(player, "BARONY_ACH_BLOOD_RUNS_CLEAR");
			}
		}

		if ( player >= 0 && stats[player]->playerRace == RACE_GOBLIN && stats[player]->appearance == 0
			&& myStats->PROFICIENCIES[skill] == 100 )
		{
			switch ( skill )
			{
				case PRO_SWORD:
				case PRO_POLEARM:
				case PRO_AXE:
				case PRO_MACE:
				case PRO_UNARMED:
					steamAchievementClient(player, "BARONY_ACH_SAVAGE");
					break;
				default:
					break;
			}
		}

		if ( skill == PRO_ALCHEMY )
		{
			if ( player >= 0 && players[player]->isLocalPlayer() )
			{
				GenericGUI[player].alchemyLearnRecipeOnLevelUp(myStats->PROFICIENCIES[skill]);
			}
		}

		if ( skill == PRO_SWIMMING && !(svFlags & SV_FLAG_HUNGER) )
		{
			// hunger off and swimming is raised.
			serverUpdatePlayerGameplayStats(player, STATISTICS_HOT_TUB_TIME_MACHINE, 1);
		}

		if ( skill == PRO_MAGIC && skillCapstoneUnlockedEntity(PRO_MAGIC) )
		{
			//magic capstone = bonus spell: Dominate.
			if ( player > 0 && multiplayer == SERVER && !players[player]->isLocalPlayer() )
			{
				strcpy((char*)net_packet->data, "ASPL");
				net_packet->data[4] = clientnum;
				net_packet->data[5] = SPELL_DOMINATE;
				net_packet->address.host = net_clients[player - 1].host;
				net_packet->address.port = net_clients[player - 1].port;
				net_packet->len = 6;
				sendPacketSafe(net_sock, -1, net_packet, player - 1);
			}
			else if ( player >= 0 )
			{
				addSpell(SPELL_DOMINATE, player, true);
			}
		}
		myStats->EXP += 2;
	}

	int statBonusSkill = getStatForProficiency(skill);

	if ( statBonusSkill >= STAT_STR )
	{
		// stat has chance for bonus point if the relevant proficiency has been trained.
		// write the last proficiency that effected the skill.
		myStats->PLAYER_LVL_STAT_BONUS[statBonusSkill] = skill;
	}



	if ( player > 0 && multiplayer == SERVER && !players[player]->isLocalPlayer() )
	{
		// update SKILL
		strcpy((char*)net_packet->data, "SKIL");
		net_packet->data[4] = clientnum;
		net_packet->data[5] = skill;
		net_packet->data[6] = myStats->PROFICIENCIES[skill];
		if ( notify )
		{
			net_packet->data[6] |= (1 << 7);
		}
		net_packet->address.host = net_clients[player - 1].host;
		net_packet->address.port = net_clients[player - 1].port;
		net_packet->len = 7;
		sendPacketSafe(net_sock, -1, net_packet, player - 1);

		// update EXP
		strcpy((char*)net_packet->data, "ATTR");
		net_packet->data[4] = clientnum;
		net_packet->data[5] = (Sint8)myStats->STR;
		net_packet->data[6] = (Sint8)myStats->DEX;
		net_packet->data[7] = (Sint8)myStats->CON;
		net_packet->data[8] = (Sint8)myStats->INT;
		net_packet->data[9] = (Sint8)myStats->PER;
		net_packet->data[10] = (Sint8)myStats->CHR;
		net_packet->data[11] = (Sint8)myStats->EXP;
		net_packet->data[12] = (Sint8)myStats->LVL;
		SDLNet_Write16((Sint16)myStats->HP, &net_packet->data[13]);
		SDLNet_Write16((Sint16)myStats->MAXHP, &net_packet->data[15]);
		SDLNet_Write16((Sint16)myStats->MP, &net_packet->data[17]);
		SDLNet_Write16((Sint16)myStats->MAXMP, &net_packet->data[19]);
		net_packet->address.host = net_clients[player - 1].host;
		net_packet->address.port = net_clients[player - 1].port;
		net_packet->len = 21;
		sendPacketSafe(net_sock, -1, net_packet, player - 1);
	}
}

/*-------------------------------------------------------------------------------

Entity::stats

Returns a pointer to a Stat instance given a pointer to an entity

-------------------------------------------------------------------------------*/

Stat* Entity::getStats() const
{
	if ( this->behavior == &actPlayerLimb ) // player bodyparts
	{
		return stats[this->skill[2]];
	}

	return nullptr;
}

/*-------------------------------------------------------------------------------

uidToEntity

Returns an entity pointer from the given entity UID, provided one exists.
Otherwise returns NULL

-------------------------------------------------------------------------------*/

Entity* uidToEntity(Sint32 uidnum)
{
	node_t* node;
	Entity* entity;

	auto it = map.entities_map.find(uidnum);
	if ( it != map.entities_map.end() )
		return (Entity*)it->second->element;

	return NULL;
}

/*-------------------------------------------------------------------------------

uidToCreature

Returns a creature pointer from the given entity UID, provided one exists.
Otherwise returns NULL

-------------------------------------------------------------------------------*/

Creature* uidToCreature(Sint32 uidnum)
{
	node_t* node;
	Entity* entity;

	auto it = map.entities_map.find(uidnum);
	if ( it != map.entities_map.end() )
		return (Creature*)it->second->element;

	return nullptr;
}

int Entity::getMP()
{
	Stat* myStats = getStats();

	if ( !myStats )
	{
		return 0;
	}

	return myStats->MP;
}

int Entity::getHP()
{
	Stat* myStats = getStats();

	if ( !myStats )
	{
		return 0;
	}

	return myStats->HP;
}

int Entity::getHungerTickRate(Stat* myStats, bool isPlayer, bool checkItemsEffects)
{
	int hungerTickRate = 30; // how many ticks to reduce hunger by a point.
	if ( !myStats )
	{
		return hungerTickRate;
	}
	int hungerring = 0;
	if ( checkItemsEffects )
	{
		if ( myStats->ring != NULL )
		{
			if ( myStats->ring->type == RING_SLOWDIGESTION )
			{
				if ( myStats->ring->beatitude >= 0 )
				{
					hungerring = 1;
				}
				else
				{
					if ( isPlayer && shouldInvertEquipmentBeatitude(myStats) )
					{
						hungerring = 1;
					}
					else
					{
						hungerring = -1;
					}
				}
			}
		}
	}

	int vampiricHunger = 0;
	if ( checkItemsEffects )
	{
		if ( myStats->EFFECTS[EFF_VAMPIRICAURA] )
		{
			if ( myStats->EFFECTS_TIMERS[EFF_VAMPIRICAURA] == -2 )
			{
				vampiricHunger = 2;
			}
			else
			{
				vampiricHunger = 1;
			}
		}
	}

	if ( !strncmp(map.name, "Sanctum", 7)
		|| !strncmp(map.name, "Boss", 4)
		|| !strncmp(map.name, "Hell Boss", 9)
		|| !strncmp(map.name, "Mages Guild", 11)
		|| strstr(map.name, " Transition") )
	{
		hungerring = 1; // slow down hunger on boss stages.
		if ( vampiricHunger > 0 )
		{
			vampiricHunger *= 8;
		}
	}

	if ( vampiricHunger > 0 )
	{
		hungerTickRate = 5 * vampiricHunger;
	}
	else if ( hungerring > 0 )
	{
		hungerTickRate = 120;
	}
	else if ( hungerring < 0 )
	{
		hungerTickRate = 15;
	}

	int playerCount = 0;
	for ( int i = 0; i < MAXPLAYERS; ++i )
	{
		if ( !client_disconnected[i] )
		{
			++playerCount;
		}
	}

	if ( playerCount == 3 )
	{
		hungerTickRate *= 1.25;
	}
	else if ( playerCount == 4 )
	{
		hungerTickRate *= 1.5;
	}
	if ( myStats->type == INSECTOID )
	{
		hungerTickRate *= 1.5;
	}

	bool playerAutomaton = (myStats->type == AUTOMATON && isPlayer);
	if ( playerAutomaton )
	{
		// give a little extra hunger duration.
		if ( playerCount == 3 )
		{
			hungerTickRate *= 1.25; // 1.55x (1.25 x 1.25)
		}
		else if ( playerCount == 4 )
		{
			hungerTickRate *= 1.5; // 2.55x (1.5 x 1.5)
		}

		if ( myStats->HUNGER > 1000 && hungerTickRate > 30 )
		{
			hungerTickRate = 30; // don't slow down during superheated.
		}
	}
	return hungerTickRate;
}


real_t Entity::getACEffectiveness(Entity* my, Stat* myStats, bool isPlayer, Entity* attacker, Stat* attackerStats)
{
	if ( !myStats || !my )
	{
		return 1.0;
	}

	if ( myStats->defending )
	{
		return 1.0;
	}

	int blessings = 0;
	bool cursedItemIsBuff = shouldInvertEquipmentBeatitude(myStats);

	if ( myStats->helmet && myStats->helmet->doesItemProvideBeatitudeAC() )
	{
		blessings += cursedItemIsBuff ? abs(myStats->helmet->beatitude) : myStats->helmet->beatitude;
	}
	if ( myStats->breastplate && myStats->breastplate->doesItemProvideBeatitudeAC() )
	{
		blessings += cursedItemIsBuff ? abs(myStats->breastplate->beatitude) : myStats->breastplate->beatitude;
	}
	if ( myStats->gloves && myStats->gloves->doesItemProvideBeatitudeAC() )
	{
		blessings += cursedItemIsBuff ? abs(myStats->gloves->beatitude) : myStats->gloves->beatitude;
	}
	if ( myStats->shoes && myStats->shoes->doesItemProvideBeatitudeAC() )
	{
		blessings += cursedItemIsBuff ? abs(myStats->shoes->beatitude) : myStats->shoes->beatitude;
	}
	if ( myStats->shield && myStats->shield->doesItemProvideBeatitudeAC() )
	{
		blessings += cursedItemIsBuff ? abs(myStats->shield->beatitude) : myStats->shield->beatitude;
	}
	if ( myStats->cloak && myStats->cloak->doesItemProvideBeatitudeAC() )
	{
		blessings += cursedItemIsBuff ? abs(myStats->cloak->beatitude) : myStats->cloak->beatitude;
	}
	if ( myStats->ring && myStats->ring->doesItemProvideBeatitudeAC() )
	{
		blessings += cursedItemIsBuff ? abs(myStats->ring->beatitude) : myStats->ring->beatitude;
	}
	if ( myStats->mask && myStats->mask->doesItemProvideBeatitudeAC() )
	{
		blessings += cursedItemIsBuff ? abs(myStats->mask->beatitude) : myStats->mask->beatitude;
	}
	if ( myStats->amulet && myStats->amulet->doesItemProvideBeatitudeAC() )
	{
		blessings += cursedItemIsBuff ? abs(myStats->amulet->beatitude) : myStats->amulet->beatitude;
	}

	return std::max(0.0, std::min(1.0, .75 + 0.025 * blessings));
}

/*-------------------------------------------------------------------------------

Entity::getAttack

returns the attack power of an entity based on strength, weapon, and a
base number

-------------------------------------------------------------------------------*/

Sint32 Entity::getAttack(Entity* my, Stat* myStats, bool isPlayer)
{
	Sint32 attack = 0;
    Creature* myCrtr = (Creature*)my;

	if ( !myStats )
	{
		return 0;
	}

	attack = BASE_MELEE_DAMAGE; // base attack strength
	bool shapeshifted = (myCrtr && myCrtr->behavior == &actPlayer && myCrtr->effectShapeshift != NOTHING);
	if ( myStats->weapon == nullptr || shapeshifted )
	{
		// bare handed.
		if ( isPlayer )
		{
			attack = BASE_PLAYER_UNARMED_DAMAGE;
			attack += (myStats->PROFICIENCIES[PRO_UNARMED] / 20); // 0, 1, 2, 3, 4, 5 damage from total
		}
		if ( myStats->gloves && !shapeshifted )
		{
			int beatitude = myStats->gloves->beatitude;
			if ( myStats->gloves->type == BRASS_KNUCKLES )
			{
				attack += 1 + (shouldInvertEquipmentBeatitude(myStats) ? abs(beatitude) : beatitude);
			}
			else if ( myStats->gloves->type == IRON_KNUCKLES )
			{
				attack += 2 + (shouldInvertEquipmentBeatitude(myStats) ? abs(beatitude) : beatitude);
			}
			else if ( myStats->gloves->type == SPIKED_GAUNTLETS )
			{
				attack += 3 + (shouldInvertEquipmentBeatitude(myStats) ? abs(beatitude) : beatitude);
			}
		}
		if ( myStats->ring )
		{
			int beatitude = myStats->ring->beatitude;
			attack += 1 + (shouldInvertEquipmentBeatitude(myStats) ? abs(beatitude) : beatitude);
		}
	}
	else if ( myStats->weapon != nullptr )
	{
		attack += myStats->weapon->weaponGetAttack(myStats);
	}

	if ( !shapeshifted && myStats->weapon && myStats->weapon->type == TOOL_WHIP )
	{
		int atk = statGetSTR(myStats, my) + statGetDEX(myStats, my);
		atk = std::min(atk / 2, atk);
		attack += atk;
	}
	else
	{
		attack += statGetSTR(myStats, my);
	}

	return attack;
}

/*-------------------------------------------------------------------------------

Entity::getRangedAttack

returns the ranged attack power of an entity based on dex, ranged weapon, and a
base number

-------------------------------------------------------------------------------*/

Sint32 Entity::getRangedAttack()
{
	Stat* entitystats;
	int attack = BASE_RANGED_DAMAGE; // base ranged attack strength

	if ( (entitystats = this->getStats()) == nullptr )
	{
		return 0;
	}

	if ( entitystats->weapon )
	{
		attack += entitystats->weapon->weaponGetAttack(entitystats);
		attack += getDEX();
		if ( ((Creature*)this) && ((Creature*)this)->behavior == &actMonster )
		{
			attack += getPER(); // monsters take PER into their ranged attacks to avoid having to increase their speed.
			attack += entitystats->PROFICIENCIES[PRO_RANGED] / 20; // 0 to 5 bonus attack for monsters
		}
	}
	else
	{
		return 0;
	}
	return attack;
}

/*-------------------------------------------------------------------------------

Entity::getThrownAttack

returns the thrown attack power of an entity based on dex, thrown weapon, and a
base number. For tooltip only.

-------------------------------------------------------------------------------*/

Sint32 Entity::getThrownAttack()
{
	Stat* entitystats;
	int attack = BASE_THROWN_DAMAGE; // base thrown attack strength

	if ( (entitystats = this->getStats()) == nullptr )
	{
		return attack;
	}

	int skillLVL = entitystats->PROFICIENCIES[PRO_RANGED] / 20;

	if ( entitystats->weapon )
	{
		if ( itemCategory(entitystats->weapon) == THROWN )
		{
			int dex = getDEX() / 4;
			attack += dex;
			attack += entitystats->weapon->weaponGetAttack(entitystats);
			attack *= thrownDamageSkillMultipliers[std::min(skillLVL, 5)];
		}
		else if ( itemCategory(entitystats->weapon) == POTION )
		{
			int skillLVL = entitystats->PROFICIENCIES[PRO_ALCHEMY] / 20;
			attack += entitystats->weapon->weaponGetAttack(entitystats);
			/*int dex = getDEX() / 4;
			attack += dex;*/
			attack *= potionDamageSkillMultipliers[std::min(skillLVL, 5)];
		}
		else
		{
			int dex = getDEX() / 4;
			attack += dex;
			attack += entitystats->weapon->weaponGetAttack(entitystats);
			attack += entitystats->PROFICIENCIES[PRO_RANGED] / 10; // 0 to 10 bonus attack.
		}
	}
	else
	{
		return 0;
	}
	return attack;
}

/*-------------------------------------------------------------------------------

Entity::getBonusAttackOnTarget

returns the attack power depending on targets attributes, status effects and race

-------------------------------------------------------------------------------*/

Sint32 Entity::getBonusAttackOnTarget(Stat& hitstats)
{
	Stat* entitystats;
	Sint32 bonusAttack = 0;

	if ( (entitystats = this->getStats()) == nullptr )
	{
		return 0;
	}

	if ( entitystats->weapon )
	{
		if ( hitstats.EFFECTS[EFF_VAMPIRICAURA] )
		{
			// blessed weapons deal more damage under this effect.
			bonusAttack += entitystats->weapon->beatitude;
		}
	}

	return bonusAttack;
}

/*-------------------------------------------------------------------------------

Entity::getSTR()

returns the STR attribute of an entity, post modifiers

-------------------------------------------------------------------------------*/

Sint32 Entity::getSTR()
{
	Stat* entitystats;

	if ( (entitystats = this->getStats()) == nullptr )
	{
		return 0;
	}
	return statGetSTR(entitystats, this);
}

Sint32 statGetSTR(Stat* entitystats, Entity* my)
{
	Sint32 STR;
    Creature* myCrtr = (Creature*)my;
	if ( !entitystats )
	{
		return 0;
	}

	STR = entitystats->STR;

	bool cursedItemIsBuff = false;
	bool shapeshifted = false;
	if ( myCrtr && myCrtr->behavior == &actPlayer )
	{
		cursedItemIsBuff = shouldInvertEquipmentBeatitude(entitystats);
		if ( my->effectShapeshift != NOTHING )
		{
			shapeshifted = true;
			if ( my->effectShapeshift == TROLL )
			{
				int bonusSTR = 5;
				STR += bonusSTR;
				if ( STR >= 0 )
				{
					STR *= 1.33;
				}
			}
			else if ( my->effectShapeshift == SPIDER )
			{
				int bonusSTR = 3;
				STR += bonusSTR;
				if ( STR >= 0 )
				{
					STR *= 1.25;
				}
			}
		}
	}

	if ( svFlags & SV_FLAG_HUNGER )
	{
		if ( entitystats->type != AUTOMATON && myCrtr )
		{
			if ( entitystats->HUNGER >= getEntityHungerInterval(-1, myCrtr, entitystats, HUNGER_INTERVAL_OVERSATIATED) )
			{
				STR--;
			}
			if ( entitystats->HUNGER <= getEntityHungerInterval(-1, myCrtr, entitystats, HUNGER_INTERVAL_WEAK) )
			{
				STR--;
			}
			if ( entitystats->HUNGER <= getEntityHungerInterval(-1, myCrtr, entitystats, HUNGER_INTERVAL_STARVING) )
			{
				STR--;
			}
		}
		else if ( entitystats->type == AUTOMATON
			&& entitystats->HUNGER <= getEntityHungerInterval(-1, myCrtr, entitystats, HUNGER_INTERVAL_AUTOMATON_CRITICAL) )
		{
			STR--;
		}
	}
	if ( entitystats->gloves != nullptr )
	{
		if ( entitystats->gloves->type == GAUNTLETS_STRENGTH )
		{
			if ( entitystats->gloves->beatitude >= 0 || cursedItemIsBuff )
			{
				STR++;
			}
			STR += (cursedItemIsBuff ? abs(entitystats->gloves->beatitude) : entitystats->gloves->beatitude);
		}
	}
	if ( entitystats->ring != nullptr )
	{
		if ( entitystats->ring->type == RING_STRENGTH )
		{
			if ( entitystats->ring->beatitude >= 0 || cursedItemIsBuff )
			{
				STR++;
			}
			STR += (cursedItemIsBuff ? abs(entitystats->ring->beatitude) : entitystats->ring->beatitude);
		}
	}
	if ( entitystats->EFFECTS[EFF_SHRINE_RED_BUFF] )
	{
		STR += 8;
	}
	if ( entitystats->EFFECTS[EFF_VAMPIRICAURA] && myCrtr && myCrtr->behavior == &actPlayer )
	{
		if ( entitystats->EFFECTS_TIMERS[EFF_VAMPIRICAURA] == -2 )
		{
			STR += 3; // player cursed vampiric bonus
		}
		else
		{
			STR += (std::max(5, STR / 4));
		}
	}
	if ( entitystats->EFFECTS[EFF_POTION_STR] )
	{
		STR += (std::max(5, STR / 4));
	}
	if ( entitystats->EFFECTS[EFF_DRUNK] )
	{
		switch ( entitystats->type )
		{
			case GOATMAN:
				if ( myCrtr && myCrtr->behavior == &actMonster )
				{
					STR += std::max(10, static_cast<int>(STR * 0.25)); //Goatman love booze.
				}
				else if ( myCrtr && myCrtr->behavior == &actPlayer )
				{
					STR += std::max(4, static_cast<int>(STR * 0.25));
				}
				break;
			default:
				++STR;
				break;
		}
	}
	return STR;
}

/*-------------------------------------------------------------------------------

Entity::getDEX

returns the DEX attribute of an entity, post modifiers

-------------------------------------------------------------------------------*/

Sint32 Entity::getDEX()
{
	Stat* entitystats;

	if ( (entitystats = this->getStats()) == nullptr )
	{
		return 0;
	}
	return statGetDEX(entitystats, this);
}

Sint32 statGetDEX(Stat* entitystats, Entity* my)
{
	Sint32 DEX;
    Creature* myCrtr = (Creature*)my;
	if ( !entitystats )
	{
		return 0;
	}

	// paralyzed
	if ( entitystats->EFFECTS[EFF_PARALYZED] )
	{
		return -10;
	}
	if ( entitystats->EFFECTS[EFF_ASLEEP] )
	{
		return -10;
	}

	DEX = entitystats->DEX;

	bool cursedItemIsBuff = false;
	bool shapeshifted = false;
	if ( myCrtr && myCrtr->behavior == &actPlayer )
	{
		cursedItemIsBuff = shouldInvertEquipmentBeatitude(entitystats);
		if ( my->effectShapeshift != NOTHING )
		{
			shapeshifted = true;
			if ( my->effectShapeshift == TROLL )
			{
				int bonusDEX = -5;
				DEX += bonusDEX;
				if ( DEX >= 0 )
				{
					DEX *= 0.67;
				}
			}
			else if ( my->effectShapeshift == RAT )
			{
				int bonusDEX = 3;
				DEX += bonusDEX;
				if ( DEX >= 0 )
				{
					DEX *= 1.25;
				}
			}
		}
	}

	if ( entitystats->EFFECTS[EFF_VAMPIRICAURA] && !entitystats->EFFECTS[EFF_FAST] && !entitystats->EFFECTS[EFF_SLOW] )
	{
		if ( entitystats->EFFECTS_TIMERS[EFF_VAMPIRICAURA] == -2 )
		{
			DEX += 3; // player cursed vampiric bonus
		}
		else
		{
			DEX += 5;
			if ( myCrtr && entitystats->type == VAMPIRE && myCrtr->behavior == &actMonster )
			{
				DEX += 3; // monster vampires
			}
			if ( myCrtr && myCrtr->behavior == &actPlayer )
			{
				DEX += (std::max(0, DEX / 4));
			}
		}
	}
	else if ( entitystats->EFFECTS[EFF_FAST] && !entitystats->EFFECTS[EFF_SLOW] )
	{
		if ( myCrtr && myCrtr->behavior == &actPlayer )
		{
			DEX += 5;
		}
		else
		{
			DEX += 10;
		}
	}
	if ( entitystats->EFFECTS[EFF_STUNNED] )
	{
		//DEX -= 5;
	}

	if ( my && my->monsterAllyGetPlayerLeader() )
	{
		if ( stats[my->monsterAllyIndex] )
		{
			DEX += 1 + (stats[my->monsterAllyIndex]->PROFICIENCIES[PRO_LEADERSHIP] / 20);
		}
	}

	if ( myCrtr && myCrtr->behavior == &actPlayer && entitystats->type == AUTOMATON )
	{
		real_t ratio = entitystats->MP / static_cast<real_t>(entitystats->MAXMP);
		if ( ratio < 0.1 )
		{
			DEX -= std::max((std::max(0, DEX) / 2), 3);
		}
		else if ( ratio < 0.25 )
		{
			DEX -= std::max((std::max(0, DEX) / 4), 2);
		}

		if ( entitystats->HUNGER == 0 )
		{
			DEX -= 2;
		}
	}
	else if ( svFlags & SV_FLAG_HUNGER && myCrtr )
	{
		if ( entitystats->HUNGER >= getEntityHungerInterval(-1, myCrtr, entitystats, HUNGER_INTERVAL_OVERSATIATED) )
		{
			DEX--;
		}
		if ( entitystats->HUNGER <= getEntityHungerInterval(-1, myCrtr, entitystats, HUNGER_INTERVAL_WEAK) )
		{
			DEX--;
		}
		if ( entitystats->HUNGER <= getEntityHungerInterval(-1, myCrtr, entitystats, HUNGER_INTERVAL_STARVING) )
		{
			DEX--;
		}
	}

	if ( entitystats->EFFECTS[EFF_WEBBED] && !entitystats->EFFECTS[EFF_SLOW] )
	{
		DEX = std::max(std::min(DEX, 2) - 2 * (my ? my->creatureWebbedSlowCount : 0), -4);
	}
	if ( !entitystats->EFFECTS[EFF_FAST] && entitystats->EFFECTS[EFF_SLOW] )
	{
		if ( myCrtr && myCrtr->behavior == &actPlayer )
		{
			DEX = std::max(DEX - 5, -2);
		}
		else
		{
			DEX = std::min(DEX - 3, -2);
		}
	}
	if ( entitystats->shoes != nullptr )
	{
		if ( entitystats->shoes->type == LEATHER_BOOTS_SPEED )
		{
			if ( entitystats->shoes->beatitude >= 0 || cursedItemIsBuff )
			{
				DEX++;
			}
			DEX += (cursedItemIsBuff ? abs(entitystats->shoes->beatitude) : entitystats->shoes->beatitude);
		}
	}
	if ( entitystats->gloves != nullptr )
	{
		if ( entitystats->gloves->type == GLOVES_DEXTERITY )
		{
			if ( entitystats->gloves->beatitude >= 0 || cursedItemIsBuff )
			{
				DEX++;
			}
			DEX += (cursedItemIsBuff ? abs(entitystats->gloves->beatitude) : entitystats->gloves->beatitude);
		}
	}
	if ( entitystats->EFFECTS[EFF_DRUNK] )
	{
		switch ( entitystats->type )
		{
			case GOATMAN:
				DEX -= std::max(2, static_cast<int>(DEX * 0.25));
				break;
			default:
				--DEX;
				break;
		}
	}

	if ( !(svFlags & SV_FLAG_HUNGER) )
	{
		if ( myCrtr && myCrtr->behavior == &actPlayer && entitystats->playerRace == RACE_INSECTOID && entitystats->appearance == 0 )
		{
			int dexDebuff = 0;
			if ( entitystats->MP < (entitystats->MAXMP) / 5 )
			{
				dexDebuff = 2;
			}
			else if ( entitystats->MP < 2 * (entitystats->MAXMP) / 5 )
			{
				dexDebuff = 1;
			}
			DEX -= dexDebuff;
			if ( DEX > 0 )
			{
				DEX -= dexDebuff * (DEX / 4); // -X DEX for every 4 DEX we have.
			}
		}
	}

	if ( entitystats->EFFECTS[EFF_WITHDRAWAL] && !entitystats->EFFECTS[EFF_DRUNK] )
	{
		DEX -= 3; // hungover.
		int minusDex = DEX;
		if ( minusDex > 0 )
		{
			DEX -= (minusDex / 4); // -1 DEX for every 4 DEX we have.
		}
	}
	if ( entitystats->EFFECTS[EFF_SHRINE_GREEN_BUFF] )
	{
		DEX += 8;
	}
	return DEX;
}

/*-------------------------------------------------------------------------------

Entity::getCON

returns the CON attribute of an entity, post modifiers

-------------------------------------------------------------------------------*/

Sint32 Entity::getCON()
{
	Stat* entitystats;

	if ( (entitystats = this->getStats()) == nullptr )
	{
		return 0;
	}
	return statGetCON(entitystats, this);
}

Sint32 statGetCON(Stat* entitystats, Entity* my)
{
	Sint32 CON;
    Creature* myCrtr = (Creature*)my;

	if ( !entitystats )
	{
		return 0;
	}

	CON = entitystats->CON;

	bool cursedItemIsBuff = false;
	bool shapeshifted = false;
	if ( myCrtr && myCrtr->behavior == &actPlayer )
	{
		cursedItemIsBuff = shouldInvertEquipmentBeatitude(entitystats);
		if ( my->effectShapeshift != NOTHING )
		{
			shapeshifted = true;
			if ( my->effectShapeshift == SPIDER )
			{
				int bonusCON = 3;
				CON += bonusCON;
				if ( CON >= 0 )
				{
					CON *= 1.25;
				}
			}
			else if ( my->effectShapeshift == TROLL )
			{
				int bonusCON = 5;
				CON += bonusCON;
				if ( CON >= 0 )
				{
					CON *= 1.33;
				}
			}
		}
	}

	if ( entitystats->ring != nullptr )
	{
		if ( entitystats->ring->type == RING_CONSTITUTION )
		{
			if ( entitystats->ring->beatitude >= 0 || cursedItemIsBuff )
			{
				CON++;
			}
			CON += (cursedItemIsBuff ? abs(entitystats->ring->beatitude) : entitystats->ring->beatitude);
		}
	}
	if ( entitystats->gloves != nullptr )
	{
		if ( entitystats->gloves->type == BRACERS_CONSTITUTION )
		{
			if ( entitystats->gloves->beatitude >= 0 || cursedItemIsBuff )
			{
				CON++;
			}
			CON += (cursedItemIsBuff ? abs(entitystats->gloves->beatitude) : entitystats->gloves->beatitude);
		}
	}
	if ( entitystats->EFFECTS[EFF_SHRINE_RED_BUFF] )
	{
		CON += 8;
	}
	if ( my && entitystats->EFFECTS[EFF_DRUNK] && entitystats->type == GOATMAN )
	{
		CON += std::max(4, static_cast<int>(CON * 0.25));
	}
	return CON;
}

/*-------------------------------------------------------------------------------

Entity::getINT

returns the INT attribute of an entity, post modifiers

-------------------------------------------------------------------------------*/

Sint32 Entity::getINT()
{
	Stat* entitystats;

	if ( (entitystats = this->getStats()) == nullptr )
	{
		return 0;
	}
	return statGetINT(entitystats, this);
}

Sint32 statGetINT(Stat* entitystats, Entity* my)
{
	Sint32 INT;
    Creature* myCrtr = (Creature*)my;
	if ( !entitystats )
	{
		return 0;
	}

	INT = entitystats->INT;

	bool cursedItemIsBuff = false;
	bool shapeshifted = false;
	if ( myCrtr && myCrtr->behavior == &actPlayer )
	{
		cursedItemIsBuff = shouldInvertEquipmentBeatitude(entitystats);
		if ( my->effectShapeshift != NOTHING )
		{
			shapeshifted = true;
			if ( my->effectShapeshift == RAT )
			{
				int bonusINT = 3;
				INT += bonusINT;
				if ( INT >= 0 )
				{
					INT *= 1.25;
				}
			}
			else if ( my->effectShapeshift == CREATURE_IMP )
			{
				int bonusINT = 5;
				INT += bonusINT;
				if ( INT >= 0 )
				{
					INT *= 1.33;
				}
			}
		}
	}

	if ( svFlags & SV_FLAG_HUNGER && myCrtr )
	{
		if ( entitystats->HUNGER <= getEntityHungerInterval(-1, myCrtr, entitystats, HUNGER_INTERVAL_STARVING) )
		{
			INT--;
		}
	}
	if ( entitystats->helmet != nullptr )
	{
		if ( entitystats->helmet->type == HAT_WIZARD )
		{
			if ( entitystats->helmet->beatitude >= 0 || cursedItemIsBuff )
			{
				INT++;
			}
			INT += (cursedItemIsBuff ? abs(entitystats->helmet->beatitude) : entitystats->helmet->beatitude);
		}
		else if ( entitystats->helmet->type == ARTIFACT_HELM )
		{
			if ( entitystats->helmet->beatitude >= 0 || cursedItemIsBuff )
			{
				INT += 8;
			}
			INT += (cursedItemIsBuff ? abs(entitystats->helmet->beatitude) : entitystats->helmet->beatitude);
		}
	}
	if ( entitystats->breastplate != nullptr )
	{
		if ( entitystats->breastplate->type == HEALER_DOUBLET || entitystats->breastplate->type == WIZARD_DOUBLET )
		{
			if ( entitystats->breastplate->beatitude >= 0 || cursedItemIsBuff )
			{
				INT++;
			}
			INT += (cursedItemIsBuff ? abs(entitystats->breastplate->beatitude) : entitystats->breastplate->beatitude);
		}
	}
	if ( entitystats->EFFECTS[EFF_SHRINE_BLUE_BUFF] )
	{
		INT += 8;
	}
	if ( myCrtr && entitystats->EFFECTS[EFF_DRUNK] && myCrtr->behavior == &actPlayer && entitystats->type == GOATMAN )
	{
		INT -= std::max(8, static_cast<int>(INT * 0.25));
	}
	return INT;
}

/*-------------------------------------------------------------------------------

Entity::getPER

returns the PER attribute of an entity, post modifiers

-------------------------------------------------------------------------------*/

Sint32 Entity::getPER()
{
	Stat* entitystats;

	if ( (entitystats = this->getStats()) == nullptr )
	{
		return 0;
	}
	return statGetPER(entitystats, this);
}

Sint32 statGetPER(Stat* entitystats, Entity* my)
{
	Sint32 PER;

	if ( !entitystats )
	{
		return 0;
	}

	PER = entitystats->PER;

	bool cursedItemIsBuff = false;
	bool shapeshifted = false;
    Creature* myCrtr = (Creature*)my;
	if ( myCrtr && myCrtr->behavior == &actPlayer )
	{
		cursedItemIsBuff = shouldInvertEquipmentBeatitude(entitystats);
		if ( my->effectShapeshift != NOTHING )
		{
			shapeshifted = true;
			if ( my->effectShapeshift == SPIDER )
			{
				int bonusPER = 5;
				PER += bonusPER;
				if ( PER >= 0 )
				{
					PER *= 1.33;
				}
			}
			else if ( my->effectShapeshift == CREATURE_IMP )
			{
				int bonusPER = 3;
				PER += bonusPER;
				if ( PER >= 0 )
				{
					PER *= 1.25;
				}
			}
			else if ( my->effectShapeshift == RAT )
			{
				int bonusPER = 3;
				PER += bonusPER;
				if ( PER >= 0 )
				{
					PER *= 1.25;
				}
			}
		}
	}

	if ( svFlags & SV_FLAG_HUNGER )
	{
		if ( entitystats->type != AUTOMATON && myCrtr )
		{
			if ( entitystats->HUNGER <= getEntityHungerInterval(-1, myCrtr, entitystats, HUNGER_INTERVAL_STARVING) )
			{
				PER--;
			}
		}
		else if ( entitystats->type == AUTOMATON
			&& entitystats->HUNGER <= getEntityHungerInterval(-1, myCrtr, entitystats, HUNGER_INTERVAL_AUTOMATON_CRITICAL) )
		{
			PER--;
		}
	}
	if ( entitystats->mask )
	{
		if ( entitystats->mask->type == TOOL_GLASSES
			|| entitystats->mask->type == MONOCLE )
		{
			if ( entitystats->mask->beatitude >= 0 || cursedItemIsBuff )
			{
				PER++;
			}
			PER += (cursedItemIsBuff ? abs(entitystats->mask->beatitude) : entitystats->mask->beatitude);
		}
		else if ( entitystats->mask->type == TOOL_BLINDFOLD
					|| entitystats->mask->type == TOOL_BLINDFOLD_TELEPATHY
					|| entitystats->mask->type == TOOL_BLINDFOLD_FOCUS )
		{
			if ( entitystats->mask->type == TOOL_BLINDFOLD_TELEPATHY
				|| entitystats->mask->type == TOOL_BLINDFOLD_FOCUS )
			{
				PER += 0;
			}
			else
			{
				PER -= 10;
			}
			PER += (cursedItemIsBuff ? abs(entitystats->mask->beatitude) : entitystats->mask->beatitude);
		}
	}
	if ( entitystats->breastplate )
	{
		if ( entitystats->breastplate->type == MACHINIST_APRON )
		{
			if ( entitystats->breastplate->beatitude >= 0 || cursedItemIsBuff )
			{
				PER += 2;
			}
			PER += (cursedItemIsBuff ? abs(entitystats->breastplate->beatitude) : entitystats->breastplate->beatitude);
		}
	}

	if ( !(svFlags & SV_FLAG_HUNGER) )
	{
		if ( myCrtr && myCrtr->behavior == &actPlayer && entitystats->playerRace == RACE_INSECTOID && entitystats->appearance == 0 )
		{
			int perDebuff = 0;
			if ( entitystats->MP < (entitystats->MAXMP) / 5 )
			{
				perDebuff = 2;
			}
			else if ( entitystats->MP < 2 * (entitystats->MAXMP) / 5 )
			{
				perDebuff = 1;
			}
			PER -= perDebuff;
			if ( PER > 0 )
			{
				PER -= perDebuff * (PER / 4); // -X DEX for every 4 DEX we have.
			}
		}
	}

	if ( entitystats->EFFECTS[EFF_SHRINE_GREEN_BUFF] )
	{
		PER += 8;
	}
	if ( entitystats->EFFECTS[EFF_POTION_STR] )
	{
		PER -= std::max(5, PER / 2);
	}
	return PER;
}

/*-------------------------------------------------------------------------------

Entity::getCHR

returns the CHR attribute of an entity, post modifiers

-------------------------------------------------------------------------------*/

Sint32 Entity::getCHR()
{
	Stat* entitystats;

	if ( (entitystats = this->getStats()) == nullptr )
	{
		return 0;
	}
	return statGetCHR(entitystats, this);
}

Sint32 statGetCHR(Stat* entitystats, Entity* my)
{
	Sint32 CHR;
    Creature* myCrtr = (Creature*)my;

	if ( !entitystats )
	{
		return 0;
	}

	CHR = entitystats->CHR;

	bool cursedItemIsBuff = false;
	bool shapeshifted = false;
	if ( myCrtr && myCrtr->behavior == &actPlayer )
	{
		cursedItemIsBuff = shouldInvertEquipmentBeatitude(entitystats);
		if ( my->effectShapeshift != NOTHING )
		{
			shapeshifted = true;
			//if ( my->effectShapeshift == CREATURE_IMP )
			//{
			//	int bonusCHR = (2 + (std::max(0, entitystats->CHR) / 10)); // +2 + 10% base CHR
			//	CHR += bonusCHR;
			//}
		}
	}

	if ( entitystats->helmet != nullptr )
	{
		if ( entitystats->helmet->type == HAT_JESTER )
		{
			if ( entitystats->helmet->beatitude >= 0 || cursedItemIsBuff )
			{
				CHR++;
			}
			CHR += (cursedItemIsBuff ? abs(entitystats->helmet->beatitude) : entitystats->helmet->beatitude);
		}
	}
	if ( entitystats->ring != nullptr )
	{
		if ( entitystats->ring->type == RING_ADORNMENT )
		{
			if ( entitystats->ring->beatitude >= 0 || cursedItemIsBuff )
			{
				CHR++;
			}
			CHR += (cursedItemIsBuff ? abs(entitystats->ring->beatitude) : entitystats->ring->beatitude);
		}
	}
	if ( entitystats->monsterDemonHasBeenExorcised >= 3 )
	{
		CHR += 5;
	}
	if ( myCrtr && entitystats->EFFECTS[EFF_DRUNK] && myCrtr->behavior == &actPlayer && entitystats->type == GOATMAN )
	{
		CHR += std::max(4, static_cast<int>(CHR * .25));
	}
	return CHR;
}

/*-------------------------------------------------------------------------------

Entity::isBlind

returns true if the given entity is blind, and false if it is not

-------------------------------------------------------------------------------*/

bool Entity::isBlind()
{
	Stat* entitystats;
	if ( (entitystats = this->getStats()) == nullptr )
	{
		return false;
	}

	bool shapeshifted = false;
	if ( ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer )
	{
		if ( effectShapeshift != NOTHING )
		{
			shapeshifted = true;
		}
	}

	// being blind
	if ( entitystats->EFFECTS[EFF_BLIND] == true )
	{
		return true;
	}

	// asleep
	if ( entitystats->EFFECTS[EFF_ASLEEP] == true )
	{
		return true;
	}

	// messy face
	if ( entitystats->EFFECTS[EFF_MESSY] == true )
	{
		return true;
	}

	// wearing blindfolds
	if ( entitystats->mask != nullptr && !shapeshifted )
	{
		if ( entitystats->mask->type == TOOL_BLINDFOLD
			|| entitystats->mask->type == TOOL_BLINDFOLD_TELEPATHY
			|| entitystats->mask->type == TOOL_BLINDFOLD_FOCUS )
		{
			return true;
		}
	}

	return false;
}

/*-------------------------------------------------------------------------------

Entity::isInvisible

returns true if the given entity is invisible or else wearing something
that would make it invisible

-------------------------------------------------------------------------------*/

bool Entity::isInvisible() const
{
	if ( intro ) 
	{
		// show up in hi-scores
		return false;
	}
	Stat* entitystats;
	if ( (entitystats = getStats()) == NULL )
	{
		return false;
	}

	// being invisible
	if ( entitystats->EFFECTS[EFF_INVISIBLE] == true )
	{
		return true;
	}

	// wearing invisibility cloaks
	if ( entitystats->cloak != NULL )
	{
		if ( entitystats->cloak->type == CLOAK_INVISIBILITY )
		{
			return true;
		}
	}

	// wearing invisibility ring
	if ( entitystats->ring != NULL )
	{
		if ( entitystats->ring->type == RING_INVISIBILITY )
		{
			return true;
		}
	}

	if ( ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer )
	{
		if ( this->skill[2] >= 0 && this->skill[2] < MAXPLAYERS )
		{
			if ( skillCapstoneUnlockedEntity(PRO_STEALTH) && (stats[this->skill[2]]->sneaking && !stats[this->skill[2]]->defending) )
			{
				if ( this->skill[9] == 0 ) // player attack variable.
				{
					return true;
				}
			}
		}
	}
	/*else if ( skillCapstoneUnlockedEntity(PRO_STEALTH) )
	{
		return true;
	}*/

	return false;
}

/*-------------------------------------------------------------------------------

checkTileForEntity

returns a list of entities that are occupying the map tile specified at
(x, y)

-------------------------------------------------------------------------------*/

list_t* checkTileForEntity(int x, int y)
{
	if ( x < 0 || y < 0 || x > 255 || y > 255 )
	{
		return nullptr; // invalid grid reference!
	}
	return &TileEntityList.gridEntities[x][y];

//	list_t* return_val = NULL;
//
//	//Loop through the list.
//	//If the entity's x and y match the tile's x and y (correcting for the difference in the two x/y systems, of course), then the entity is on the tile.
//	//Traverse map.entities...
//	node_t* node = NULL;
//	node_t* node2 = NULL;
//#ifdef __ARM_NEON__
//	const int32x2_t xy = { x, y };
//#endif
//
//	for ( node = map.entities->first; node != NULL; node = node->next )
//	{
//		if ( node->element )
//		{
//			Entity* entity = (Entity*)node->element;
//			if ( entity ) {
//#ifdef __ARM_NEON__
//				uint32x2_t eqxy = vceq_s32(vcvt_s32_f32(vmul_n_f32(vld1_f32(&entity->x), 1.0f / 16.0f)), xy);
//				if ( eqxy[0] && eqxy[1] )
//#else
//				if ( (int)floor((entity->x / 16)) == x && (int)floor((entity->y / 16)) == y )   //Check if the current entity is on the tile.
//#endif
//				{
//					//Right. So. Create the list if it doesn't exist.
//					if ( !return_val )
//					{
//						return_val = (list_t*)malloc(sizeof(list_t));
//						return_val->first = NULL;
//						return_val->last = NULL;
//					}
//
//					//And add the current entity to it.
//					node2 = list_AddNodeLast(return_val);
//					node2->element = entity;
//					node2->deconstructor = &emptyDeconstructor;
//				}
//			}
//		}
//	}
//
//	return return_val;
}

/*-------------------------------------------------------------------------------

getItemsOnTile

Fills the given list with nodes for every item entity on the given
map tile (x, y)

-------------------------------------------------------------------------------*/

void getItemsOnTile(int x, int y, list_t** list)
{

	//Take the return value of checkTileForEntity() and sort that list for items.
	//if( entity->behavior == &actItem )
	//And then free the list returned by checkTileForEntity.

	//Right. First, grab all the entities on the tile.
	list_t* entities = NULL;
	entities = checkTileForEntity(x, y);

	if ( !entities )
	{
		return;    //No use continuing of got no entities.
	}

	node_t* node = NULL;
	node_t* node2 = NULL;
	//Loop through the list of entities.
	for ( node = entities->first; node != NULL; node = node->next )
	{
		if ( node->element )
		{
			Entity* entity = (Entity*)node->element;
			//Check if the entity is an item.
			if ( entity && entity->behavior == &actItem )
			{
				//If this is the first item found, the list needs to be created.
				if ( !(*list) )
				{
					*list = (list_t*)malloc(sizeof(list_t));
					(*list)->first = NULL;
					(*list)->last = NULL;
				}

				//Add the current entity to it.
				node2 = list_AddNodeLast(*list);
				node2->element = entity;
				node2->deconstructor = &emptyDeconstructor;
			}
		}
	}

	/*if ( entities )
	{
		list_FreeAll(entities);
		free(entities);
	}*/

	//return return_val;
}

/*-------------------------------------------------------------------------------

AC

Returns armor class value from a Stat instance

-------------------------------------------------------------------------------*/

int AC(Stat* stat)
{
	if ( !stat )
	{
		return 0;
	}

	Entity* playerEntity = nullptr;
	for ( int i = 0; i < MAXPLAYERS; ++i )
	{
		if ( stat && stats[i] == stat )
		{
			if ( players[i] && players[i]->entity )
			{
				playerEntity = players[i]->entity;
				break;
			}
		}
	}
	int armor = statGetCON(stat, playerEntity);

	if ( stat->helmet )
	{
		armor += stat->helmet->armorGetAC(stat);
	}
	if ( stat->breastplate )
	{
		armor += stat->breastplate->armorGetAC(stat);
	}
	if ( stat->gloves )
	{
		armor += stat->gloves->armorGetAC(stat);
	}
	if ( stat->shoes )
	{
		armor += stat->shoes->armorGetAC(stat);
	}
	if ( stat->shield )
	{
		armor += stat->shield->armorGetAC(stat);
	}
	if ( stat->cloak )
	{
		armor += stat->cloak->armorGetAC(stat);
	}
	if ( stat->ring )
	{
		armor += stat->ring->armorGetAC(stat);
	}

	if ( stat->type == TROLL || stat->type == RAT || stat->type == SPIDER || stat->type == CREATURE_IMP )
	{
		for ( int i = 0; i < MAXPLAYERS; ++i )
		{
			if ( stat == stats[i] ) // is a player stat pointer.
			{
				return armor; // shapeshifted players do not benefit from shield defense/proficiency.
			}
		}
	}

	if ( stat->shield )
	{
		int shieldskill = stat->getPassiveShieldBonus(true);
		armor += shieldskill;
		if ( stat->defending )
		{
			//messagePlayer(0, "shield up! +%d", 5 + stat->PROFICIENCIES[PRO_SHIELD] / 5);
			armor += stat->getActiveShieldBonus(true);
		}
	}

	return armor;
}

/*-------------------------------------------------------------------------------

Entity::teleportAroundEntity

Teleports the given entity within a radius of a target entity.

-------------------------------------------------------------------------------*/

bool Entity::teleportCoordHasTrap(const int x, const int y)
{
	std::vector<list_t*> entLists = TileEntityList.getEntitiesWithinRadius(x, y, 0);
	for ( auto it = entLists.begin(); it != entLists.end(); ++it )
	{
		list_t* currentList = *it;
		node_t* node;
		for ( node = currentList->first; node != nullptr; node = node->next )
		{
			Entity* entity = (Entity*)node->element;
			if ( !entity ) { continue; }
			if ( entity->behavior == &actSpearTrap )
			{
				int i = static_cast<int>(entity->x) >> 4;
				int j = static_cast<int>(entity->y) >> 4;
				if ( i == x && j == y )
				{
					return true;
				}
			}
		}
	}
	return false;
}

/*-------------------------------------------------------------------------------

Entity::teleporterMove

Teleports the given entity to the given (x, y) location on the map,
in map coordinates. Will not teleport if the destination is an obstacle.

-------------------------------------------------------------------------------*/

bool Entity::teleporterMove(int tele_x, int tele_y, int type)
{
	int player = -1;

	if ( ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer )
	{
		player = skill[2];
	}
	// Can be inside entities?
	//if ( strstr(map.name, "Minotaur") || checkObstacle((tele_x << 4) + 8, (tele_y << 4) + 8, this, NULL) )
	//{
	//	messagePlayer(player, Language::get(707));
	//	return false;
	//}

	// relocate entity
	double oldx = x;
	double oldy = y;
	x = (tele_x << 4) + 8;
	y = (tele_y << 4) + 8;
	/*if ( entityInsideSomething(this) )
	{
		x = oldx;
		y = oldy;
		if ( multiplayer == SERVER && player > 0 )
		{
			messagePlayer(player, MESSAGE_HINT, Language::get(707));
		}
		return false;
	}*/
	if ( multiplayer != CLIENT )
	{
		TileEntityList.updateEntity(*this);
	}
	if ( player > 0 && multiplayer == SERVER && !players[player]->isLocalPlayer() )
	{
		strcpy((char*)net_packet->data, "TELM");
		net_packet->data[4] = tele_x;
		net_packet->data[5] = tele_y;
		net_packet->data[6] = type;
		net_packet->address.host = net_clients[player - 1].host;
		net_packet->address.port = net_clients[player - 1].port;
		net_packet->len = 7;
		sendPacketSafe(net_sock, -1, net_packet, player - 1);
	}

	// play sound effect
	if ( type == 0 || type == 1 )
	{
		playSoundEntityLocal(this, 96, 64);
	}
	else if ( type == 2 )
	{
		playSoundEntityLocal(this, 154, 64);
	}
	return true;
}


void createMonsterEquipment(Stat* stats)
{
	int itemIndex = 0;
	ItemType itemId;
	Status itemStatus;
	int itemBless;
	int itemAppearance = local_rng.rand();
	int itemCount;
	int chance = 1;
	int category = 0;
	bool itemIdentified;
	if ( stats != nullptr )
	{
		for ( itemIndex = 0; itemIndex < 10; ++itemIndex )
		{
			bool generateItem = true;
			category = stats->EDITOR_ITEMS[itemIndex * ITEM_SLOT_NUMPROPERTIES + ITEM_SLOT_CATEGORY];
			if ( category > 0 && stats->EDITOR_ITEMS[itemIndex * ITEM_SLOT_NUMPROPERTIES] == 1 )
			{
				if ( category > 0 && category <= 13 )
				{
					itemId = itemLevelCurve(static_cast<Category>(category - 1), 0, currentlevel);
				}
				else
				{
					int randType = 0;
					if ( category == 14 )
					{
						// equipment
						randType = local_rng.rand() % 2;
						if ( randType == 0 )
						{
							itemId = itemLevelCurve(static_cast<Category>(WEAPON), 0, currentlevel);
						}
						else if ( randType == 1 )
						{
							itemId = itemLevelCurve(static_cast<Category>(ARMOR), 0, currentlevel);
						}
					}
					else if ( category == 15 )
					{
						// jewelry
						randType = local_rng.rand() % 2;
						if ( randType == 0 )
						{
							itemId = itemLevelCurve(static_cast<Category>(AMULET), 0, currentlevel);
						}
						else
						{
							itemId = itemLevelCurve(static_cast<Category>(RING), 0, currentlevel);
						}
					}
					else if ( category == 16 )
					{
						// magical
						randType = local_rng.rand() % 3;
						if ( randType == 0 )
						{
							itemId = itemLevelCurve(static_cast<Category>(SCROLL), 0, currentlevel);
						}
						else if ( randType == 1 )
						{
							itemId = itemLevelCurve(static_cast<Category>(MAGICSTAFF), 0, currentlevel);
						}
						else
						{
							itemId = itemLevelCurve(static_cast<Category>(SPELLBOOK), 0, currentlevel);
						}
					}
				}
			}
			else
			{
				if ( static_cast<ItemType>(stats->EDITOR_ITEMS[itemIndex * ITEM_SLOT_NUMPROPERTIES] - 2) >= 0 )
				{
					itemId = static_cast<ItemType>(stats->EDITOR_ITEMS[itemIndex * ITEM_SLOT_NUMPROPERTIES] - 2);
				}
				else
				{
					itemId = ItemType::WOODEN_SHIELD;
					generateItem = false;
				}
			}

			if ( itemId >= 0 && generateItem )
			{
				itemStatus = static_cast<Status>(stats->EDITOR_ITEMS[itemIndex * ITEM_SLOT_NUMPROPERTIES + 1]);
				if ( itemStatus == 0 )
				{
					itemStatus = static_cast<Status>(DECREPIT + local_rng.rand() % 4);
				}
				else if ( itemStatus > BROKEN )
				{
					itemStatus = static_cast<Status>(itemStatus - 1); // reserved '0' for random, so '1' is decrepit... etc to '5' being excellent.
				}
				itemBless = stats->EDITOR_ITEMS[itemIndex * ITEM_SLOT_NUMPROPERTIES + 2];
				if ( itemBless == 10 )
				{
					itemBless = -2 + local_rng.rand() % 5;
				}
				itemCount = stats->EDITOR_ITEMS[itemIndex * ITEM_SLOT_NUMPROPERTIES + 3];
				if ( stats->EDITOR_ITEMS[itemIndex * ITEM_SLOT_NUMPROPERTIES + 4] == 1 )
				{
					itemIdentified = true;
				}
				else if ( stats->EDITOR_ITEMS[itemIndex * ITEM_SLOT_NUMPROPERTIES + 4] == 2 )
				{
					itemIdentified = local_rng.rand() % 2;
				}
				else
				{
					itemIdentified = false;
				}
				itemAppearance = local_rng.rand();
				chance = stats->EDITOR_ITEMS[itemIndex * ITEM_SLOT_NUMPROPERTIES + 5];

				if ( local_rng.rand() % 100 < chance )
				{
					switch ( itemIndex ) {
						case 0:
							stats->helmet = newItem(itemId, itemStatus, itemBless, itemCount, itemAppearance, itemIdentified, NULL);
							break;
						case 1:
							stats->weapon = newItem(itemId, itemStatus, itemBless, itemCount, itemAppearance, itemIdentified, NULL);
							break;
						case 2:
							stats->shield = newItem(itemId, itemStatus, itemBless, itemCount, itemAppearance, itemIdentified, NULL);
							break;
						case 3:
							stats->breastplate = newItem(itemId, itemStatus, itemBless, itemCount, itemAppearance, itemIdentified, NULL);
							break;
						case 4:
							stats->shoes = newItem(itemId, itemStatus, itemBless, itemCount, itemAppearance, itemIdentified, NULL);
							break;
						case 5:
							stats->ring = newItem(itemId, itemStatus, itemBless, itemCount, itemAppearance, itemIdentified, NULL);
							break;
						case 6:
							stats->amulet = newItem(itemId, itemStatus, itemBless, itemCount, itemAppearance, itemIdentified, NULL);
							break;
						case 7:
							stats->cloak = newItem(itemId, itemStatus, itemBless, itemCount, itemAppearance, itemIdentified, NULL);
							break;
						case 8:
							stats->mask = newItem(itemId, itemStatus, itemBless, itemCount, itemAppearance, itemIdentified, NULL);
							break;
						case 9:
							stats->gloves = newItem(itemId, itemStatus, itemBless, itemCount, itemAppearance, itemIdentified, NULL);
							break;
						default:
							break;
					}
				}
			}
		}
	}
}

int countCustomItems(Stat* stats)
{
	int x = 0;
	int customItemSlotCount = 0;

	for ( x = ITEM_SLOT_INV_1; x <= ITEM_SLOT_INV_6; x = x + ITEM_SLOT_NUMPROPERTIES )
	{
		if ( stats->EDITOR_ITEMS[x] != 1 || (stats->EDITOR_ITEMS[x] == 1 && stats->EDITOR_ITEMS[x + ITEM_SLOT_CATEGORY] != 0) )
		{
			++customItemSlotCount; //found a custom item in inventory
		}
	}

	return customItemSlotCount; //use custom items from editor instead of default generation
}

int countDefaultItems(Stat* stats)
{
	int x = 0;
	int defaultItemSlotCount = 0;

	for ( x = ITEM_SLOT_INV_1; x <= ITEM_SLOT_INV_6; x = x + ITEM_SLOT_NUMPROPERTIES )
	{
		if ( stats->EDITOR_ITEMS[x] == 1 && stats->EDITOR_ITEMS[x + ITEM_SLOT_CATEGORY] == 0 )
		{
			defaultItemSlotCount++; //found a default item in inventory
		}
	}

	return defaultItemSlotCount;
}

void setRandomMonsterStats(Stat* stats)
{
	if ( stats != nullptr )
	{
		//**************************************
		// HEALTH
		//**************************************

		if ( stats->MAXHP == stats->HP )
		{
			stats->MAXHP += local_rng.rand() % (stats->RANDOM_MAXHP + 1);

			if ( stats->RANDOM_MAXHP == stats->RANDOM_HP )
			{
				// if the max hp and normal hp range is the same, hp follows the roll of maxhp.
				stats->HP = stats->MAXHP;
			}
			else
			{
				// roll the current hp
				stats->HP += local_rng.rand() % (stats->RANDOM_HP + 1);
			}
		}
		else
		{
			// roll both ranges independently
			stats->MAXHP += local_rng.rand() % (stats->RANDOM_MAXHP + 1);
			stats->HP += local_rng.rand() % (stats->RANDOM_HP + 1);
		}

		if ( stats->HP > stats->MAXHP )
		{
			// check if hp exceeds maximums
			stats->HP = stats->MAXHP;
		}
		stats->OLDHP = stats->HP;

		//**************************************
		// MANA
		//**************************************

		if ( stats->MAXMP == stats->MP )
		{
			stats->MAXMP += local_rng.rand() % (stats->RANDOM_MAXMP + 1);

			if ( stats->RANDOM_MAXMP == stats->RANDOM_MP )
			{
				// if the max mp and normal mp range is the same, mp follows the roll of maxmp.
				stats->MP = stats->MAXMP;
			}
			else
			{
				// roll the current mp
				stats->MP += local_rng.rand() % (stats->RANDOM_MP + 1);
			}
		}
		else
		{
			// roll both ranges independently
			stats->MAXMP += local_rng.rand() % (stats->RANDOM_MAXMP + 1);
			stats->MP += local_rng.rand() % (stats->RANDOM_MP + 1);
		}

		if ( stats->MP > stats->MAXMP )
		{
			// check if mp exceeds maximums
			stats->MP = stats->MAXMP;
		}

		//**************************************
		// REST OF STATS
		//**************************************

		stats->STR += local_rng.rand() % (stats->RANDOM_STR + 1);
		stats->DEX += local_rng.rand() % (stats->RANDOM_DEX + 1);
		stats->CON += local_rng.rand() % (stats->RANDOM_CON + 1);
		stats->INT += local_rng.rand() % (stats->RANDOM_INT + 1);
		stats->PER += local_rng.rand() % (stats->RANDOM_PER + 1);
		stats->CHR += local_rng.rand() % (stats->RANDOM_CHR + 1);

		stats->LVL += local_rng.rand() % (stats->RANDOM_LVL + 1);
		stats->GOLD += local_rng.rand() % (stats->RANDOM_GOLD + 1);
	}

	// debug print out each monster spawned

	/*messagePlayer(0, "Set stats to: ");
	messagePlayer(0, "MAXHP: %d", stats->MAXHP);
	messagePlayer(0, "HP: %d", stats->HP);
	messagePlayer(0, "MAXMP: %d", stats->MAXMP);
	messagePlayer(0, "MP: %d", stats->MP);
	messagePlayer(0, "Str: %d", stats->STR);
	messagePlayer(0, "Dex: %d", stats->DEX);
	messagePlayer(0, "Con: %d", stats->CON);
	messagePlayer(0, "Int: %d", stats->INT);
	messagePlayer(0, "Per: %d", stats->PER);
	messagePlayer(0, "Chr: %d", stats->CHR);
	messagePlayer(0, "LVL: %d", stats->LVL);
	messagePlayer(0, "GOLD: %d", stats->GOLD);*/


	return;
}


int checkEquipType(const Item *item)
{
	if ( !item )
	{
		return TYPE_NONE;
	}
	if ( itemTypeIsQuiver(item->type) )
	{
		return TYPE_OFFHAND;
	}
	switch ( item->type ) {

		case LEATHER_BOOTS:
		case LEATHER_BOOTS_SPEED:
		case IRON_BOOTS:
		case IRON_BOOTS_WATERWALKING:
		case STEEL_BOOTS:
		case STEEL_BOOTS_LEVITATION:
		case STEEL_BOOTS_FEATHER:
		case CRYSTAL_BOOTS:
		case ARTIFACT_BOOTS:
		case SUEDE_BOOTS:
			return TYPE_BOOTS;
			break;

		case LEATHER_HELM:
		case IRON_HELM:
		case STEEL_HELM:
		case CRYSTAL_HELM:
		case ARTIFACT_HELM:
			return TYPE_HELM;
			break;

		case LEATHER_BREASTPIECE:
		case IRON_BREASTPIECE:
		case STEEL_BREASTPIECE:
		case CRYSTAL_BREASTPIECE:
		case WIZARD_DOUBLET:
		case HEALER_DOUBLET:
		case VAMPIRE_DOUBLET:
		case ARTIFACT_BREASTPIECE:
			return TYPE_BREASTPIECE;
			break;

		case CRYSTAL_SHIELD:
		case WOODEN_SHIELD:
		case BRONZE_SHIELD:
		case IRON_SHIELD:
		case STEEL_SHIELD:
		case STEEL_SHIELD_RESISTANCE:
		case MIRROR_SHIELD:
			return TYPE_SHIELD;
			break;

		case TOOL_TORCH:
		case TOOL_LANTERN:
		case TOOL_CRYSTALSHARD:
			return TYPE_OFFHAND;
			break;

		case CLOAK:
		case CLOAK_MAGICREFLECTION:
		case CLOAK_INVISIBILITY:
		case CLOAK_PROTECTION:
		case ARTIFACT_CLOAK:
		case CLOAK_BLACK:
		case CLOAK_BACKPACK:
		case CLOAK_SILVER:
			return TYPE_CLOAK;
			break;

		case GLOVES:
		case GLOVES_DEXTERITY:
		case GAUNTLETS:
		case GAUNTLETS_STRENGTH:
		case BRACERS:
		case BRACERS_CONSTITUTION:
		case CRYSTAL_GLOVES:
		case ARTIFACT_GLOVES:
		case SPIKED_GAUNTLETS:
		case IRON_KNUCKLES:
		case BRASS_KNUCKLES:
		case SUEDE_GLOVES:
			return TYPE_GLOVES;
			break;

		case HAT_HOOD:
		case HAT_JESTER:
		case HAT_PHRYGIAN:
		case HAT_WIZARD:
		case HAT_FEZ:
		case HAT_HOOD_RED:
		case MASK_SHAMAN:
		case PUNISHER_HOOD:
			return TYPE_HAT;
			break;

		default:
			break;
	}

	return TYPE_NONE;
}

int setGloveSprite(Stat* myStats, Entity* ent, int spriteOffset)
{
	if ( myStats == nullptr )
	{
		return 0;
	}
	if ( myStats->gloves == nullptr )
	{
		return 0;
	}

	if ( myStats->gloves->type == GLOVES || myStats->gloves->type == GLOVES_DEXTERITY ) {
		ent->sprite = 132 + myStats->sex + spriteOffset;
	}
	else if ( myStats->gloves->type == BRACERS || myStats->gloves->type == BRACERS_CONSTITUTION ) {
		ent->sprite = 323 + myStats->sex + spriteOffset;
	}
	else if ( myStats->gloves->type == GAUNTLETS || myStats->gloves->type == GAUNTLETS_STRENGTH ) {
		ent->sprite = 140 + myStats->sex + spriteOffset;
	}
	else if ( myStats->gloves->type == CRYSTAL_GLOVES )
	{
		ent->sprite = 491 + myStats->sex + spriteOffset;
	}
	else if ( myStats->gloves->type == ARTIFACT_GLOVES )
	{
		ent->sprite = 513 + myStats->sex + spriteOffset;
	}
	else if ( myStats->gloves->type == BRASS_KNUCKLES )
	{
		ent->sprite = 531 + myStats->sex + spriteOffset;
	}
	else if ( myStats->gloves->type == IRON_KNUCKLES )
	{
		ent->sprite = 539 + myStats->sex + spriteOffset;
	}
	else if ( myStats->gloves->type == SPIKED_GAUNTLETS )
	{
		ent->sprite = 547 + myStats->sex + spriteOffset;
	}
	else if ( myStats->gloves->type == SUEDE_GLOVES )
	{
		ent->sprite = 804 + (spriteOffset > 0 ? 1 : 0);
	}
	else
	{
		return 0;
	}
	return 1;
}

bool Entity::setBootSprite(Entity* leg, int spriteOffset)
{
	if ( multiplayer == CLIENT )
	{
		return false;
	}

	Stat* myStats;

	if ( ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer )
	{
		myStats = stats[this->skill[2]]; // skill[2] contains the player number.
	}
	else
	{
		myStats = this->getStats();
	}

	if ( myStats == nullptr )
	{
		return false;
	}
	if ( myStats->shoes == nullptr )
	{
		return false;
	}

	switch ( myStats->type )
	{
		case HUMAN:
			if ( myStats->shoes->type == LEATHER_BOOTS || myStats->shoes->type == LEATHER_BOOTS_SPEED )
			{
				leg->sprite = 148 + myStats->sex + spriteOffset;
			}
			else if ( myStats->shoes->type == IRON_BOOTS || myStats->shoes->type == IRON_BOOTS_WATERWALKING )
			{
				leg->sprite = 152 + myStats->sex + spriteOffset;
			}
			else if ( myStats->shoes->type >= STEEL_BOOTS && myStats->shoes->type <= STEEL_BOOTS_FEATHER )
			{
				leg->sprite = 156 + myStats->sex + spriteOffset;
			}
			else if ( myStats->shoes->type == CRYSTAL_BOOTS )
			{
				leg->sprite = 499 + myStats->sex + spriteOffset;
			}
			else if ( myStats->shoes->type == ARTIFACT_BOOTS )
			{
				leg->sprite = 521 + myStats->sex + spriteOffset;
			}
			else if ( myStats->shoes->type == SUEDE_BOOTS )
			{
				leg->sprite = 808 + (spriteOffset > 0 ? 1 : 0);
			}
			else
			{
				return false;
			}
			break;
			// fall throughs below
		case AUTOMATON:
		case GOATMAN:
		case INSECTOID:
		case KOBOLD:
		case GOBLIN:
		case SKELETON:
		case GNOME:
		case SHADOW:
		case INCUBUS:
		case VAMPIRE:
		case SUCCUBUS:
		case SHOPKEEPER:
			if ( myStats->shoes->type == LEATHER_BOOTS || myStats->shoes->type == LEATHER_BOOTS_SPEED )
			{
				leg->sprite = 148 + spriteOffset;
			}
			else if ( myStats->shoes->type == IRON_BOOTS || myStats->shoes->type == IRON_BOOTS_WATERWALKING )
			{
				leg->sprite = 152 + spriteOffset;
			}
			else if ( myStats->shoes->type >= STEEL_BOOTS && myStats->shoes->type <= STEEL_BOOTS_FEATHER )
			{
				leg->sprite = 156 + spriteOffset;
			}
			else if ( myStats->shoes->type == CRYSTAL_BOOTS )
			{
				leg->sprite = 499 + spriteOffset;
			}
			else if ( myStats->shoes->type == ARTIFACT_BOOTS )
			{
				leg->sprite = 521 + spriteOffset;
			}
			else if ( myStats->shoes->type == SUEDE_BOOTS )
			{
				leg->sprite = 808 + (spriteOffset > 0 ? 1 : 0);
			}
			else
			{
				return false;
			}
			break;
		default:
			break;
	}

	return true;
}


/*-------------------------------------------------------------------------------

sLevitating

returns true if the given entity is levitating, or false if it cannot

-------------------------------------------------------------------------------*/

bool isLevitating(Stat* mystats)
{
	if ( mystats == nullptr )
	{
		return false;
	}

	// check levitating value
	bool levitating = false;
	if ( MFLAG_DISABLELEVITATION )
	{
		for ( int i = 0; i < MAXPLAYERS; ++i )
		{
			if ( client_disconnected[i] )
			{
				continue;
			}
			// check if mystats is a player, and levitation flag is disabled.
			if ( players[i] && players[i]->entity )
			{
				if ( players[i]->entity->getStats() == mystats )
				{
					if ( mystats->type == CREATURE_IMP )
					{
						return true;
					}
					return false;
				}
			}
		}
	}
	for ( int i = 0; i < MAXPLAYERS; ++i )
	{
		if ( players[i] && players[i]->entity )
		{
			if ( players[i]->entity->getStats() == mystats )
			{
				if ( players[i]->entity->effectShapeshift == CREATURE_IMP )
				{
					return true;
				}
				break;
			}
		}
	}

	if ( mystats->EFFECTS[EFF_LEVITATING] == true )
	{
		return true;
	}
	else if ( mystats->EFFECTS[EFF_FLUTTER] )
	{
		return true;
	}
	if ( mystats->ring != NULL )
	{
		if ( mystats->ring->type == RING_LEVITATION )
		{
			return true;
		}
	}
	if ( mystats->shoes != NULL )
	{
		if ( mystats->shoes->type == STEEL_BOOTS_LEVITATION )
		{
			return true;
		}
	}
	if ( mystats->cloak != NULL )
	{
		if ( mystats->cloak->type == ARTIFACT_CLOAK )
		{
			return true;
		}
	}

	return false;
}

/*-------------------------------------------------------------------------------

getWeaponSkill

returns the proficiency for the weapon equipped.

-------------------------------------------------------------------------------*/

int getWeaponSkill(Item* weapon)
{
	if ( weapon == NULL )
	{
		return PRO_UNARMED;
	}

	if ( weapon->type == QUARTERSTAFF || weapon->type == IRON_SPEAR || weapon->type == STEEL_HALBERD || weapon->type == ARTIFACT_SPEAR || weapon->type == CRYSTAL_SPEAR )
	{
		return PRO_POLEARM;
	}
	if ( weapon->type == BRONZE_SWORD || weapon->type == IRON_SWORD || weapon->type == STEEL_SWORD || weapon->type == ARTIFACT_SWORD || weapon->type == CRYSTAL_SWORD )
	{
		return PRO_SWORD;
	}
	if ( weapon->type == BRONZE_MACE || weapon->type == IRON_MACE || weapon->type == STEEL_MACE || weapon->type == ARTIFACT_MACE || weapon->type == CRYSTAL_MACE )
	{
		return PRO_MACE;
	}
	if ( weapon->type == BRONZE_AXE || weapon->type == IRON_AXE || weapon->type == STEEL_AXE || weapon->type == ARTIFACT_AXE || weapon->type == CRYSTAL_BATTLEAXE )
	{
		return PRO_AXE;
	}
	if ( isRangedWeapon(*weapon) )
	{
		return PRO_RANGED;
	}
	if ( itemCategory(weapon) == THROWN || itemCategory(weapon) == POTION || itemCategory(weapon) == GEM )
	{
		return PRO_RANGED;
	}
	if ( weapon->type == TOOL_WHIP )
	{
		return PRO_RANGED;
	}
	return -1;
}

/*-------------------------------------------------------------------------------

getStatForProficiency

returns the stat associated with the given proficiency.

-------------------------------------------------------------------------------*/

int getStatForProficiency(int skill)
{
	int statForProficiency = -1;

	switch ( skill )
	{
		case PRO_SWORD:			// base attribute: str
		case PRO_MACE:			// base attribute: str
		case PRO_AXE:			// base attribute: str
		case PRO_POLEARM:		// base attribute: str
		case PRO_UNARMED:
			statForProficiency = STAT_STR;
			break;
		case PRO_STEALTH:		// base attribute: dex
		case PRO_RANGED:        // base attribute: dex
			statForProficiency = STAT_DEX;
			break;
		case PRO_SWIMMING:      // base attribute: con
		case PRO_SHIELD:		// base attribute: con
			statForProficiency = STAT_CON;
			break;
		case PRO_SPELLCASTING:  // base attribute: int
		case PRO_MAGIC:         // base attribute: int
		case PRO_ALCHEMY:       // base attribute: int
			statForProficiency = STAT_INT;
			break;
		case PRO_LOCKPICKING:	// base attribute: per
		case PRO_APPRAISAL:		// base attribute: per
			statForProficiency = STAT_PER;
			break;
		case PRO_TRADING:       // base attribute: chr
		case PRO_LEADERSHIP:    // base attribute: chr
			statForProficiency = STAT_CHR;
			break;
		default:
			statForProficiency = -1;
			break;
	}

	return statForProficiency;
}


int Entity::isEntityPlayer() const
{
	for ( int i = 0; i < MAXPLAYERS; ++i )
	{
		if ( players[i] && this == players[i]->entity )
		{
			return i;
		}
	}

	return -1;
}

int Entity::getReflection() const
{
	Stat *stats = getStats();
	if ( !stats )
	{
		return 0;
	}

	if ( stats->EFFECTS[EFF_MAGICREFLECT] )
	{
		return 3;
	}

	if ( stats->shield )
	{
		if ( stats->shield->type == MIRROR_SHIELD && stats->defending )
		{
			return 3;
		}
	}
	if ( stats->amulet )
	{
		if ( stats->amulet->type == AMULET_MAGICREFLECTION )
		{
			return 2;
		}
	}
	if ( stats->cloak )
	{
		if ( stats->cloak->type == CLOAK_MAGICREFLECTION )
		{
			return 1;
		}
	}
	return 0;
}

bool Entity::hasRangedWeapon() const
{
	Stat *myStats = getStats();
	if ( myStats == nullptr || myStats->weapon == nullptr )
	{
		return false;
	}

	if ( isRangedWeapon(*myStats->weapon) )
	{
		return true;
	}
	else if ( itemCategory(myStats->weapon) == MAGICSTAFF )
	{
		return true;
	}
	else if ( itemCategory(myStats->weapon) == SPELLBOOK )
	{
		return true;
	}
	else if ( itemCategory(myStats->weapon) == THROWN )
	{
		return true;
	}
	else if ( itemCategory(myStats->weapon) == GEM )
	{
		return true;
	}
	else if ( itemCategory(myStats->weapon) == POTION )
	{
		return true;
	}

	return false;
}

Uint32 Entity::getMonsterFootstepSound(int footstepType, int bootSprite)
{
	int sound = -1;

	switch ( footstepType )
	{
		case MONSTER_FOOTSTEP_SKELETON:
			sound = 95;
			break;
		case MONSTER_FOOTSTEP_STOMP:
			sound = 115;
			break;
		case MONSTER_FOOTSTEP_LEATHER:
			sound = local_rng.rand() % 7;
			break;
		case MONSTER_FOOTSTEP_USE_BOOTS:
			if ( bootSprite >= 152 && bootSprite <= 155 ) // iron boots
			{
				sound = 7 + local_rng.rand() % 7;
			}
			else if ( bootSprite >= 156 && bootSprite <= 159 ) // steel boots
			{
				sound = 14 + local_rng.rand() % 7;
			}
			else if ( bootSprite >= 499 && bootSprite <= 502 ) // crystal boots
			{
				sound = 14 + local_rng.rand() % 7;
			}
			else if ( bootSprite >= 521 && bootSprite <= 524 ) // artifact boots
			{
				sound = 14 + local_rng.rand() % 7;
			}
			else
			{
				sound = local_rng.rand() % 7;
			}
			break;
		case MONSTER_FOOTSTEP_NONE:
		default:
			break;
	}
	return static_cast<Uint32>(sound);
}

spell_t* Entity::getActiveMagicEffect(int spellID)
{
	Stat* myStats = getStats();
	if ( !myStats )
	{
		return nullptr;
	}

	spell_t* spell = nullptr;
	spell_t* searchSpell = nullptr;

	for ( node_t *node = myStats->magic_effects.first; node; node = node->next )
	{
		searchSpell = (node->element ? static_cast<spell_t*>(node->element) : nullptr);
		if ( searchSpell && searchSpell->ID == spellID )
		{
			spell = searchSpell;
			break;
		}
	}

	return spell;
}

void actAmbientParticleEffectIdle(Entity* my)
{
	if ( !my )
	{
		return;
	}

	if ( my->particleDuration < 0 )
	{
		list_RemoveNode(my->mynode);
		return;
	}
	else
	{
		if ( my->particleShrink == 1 )
		{
			// shrink the particle.
			my->scalex *= 0.95;
			my->scaley *= 0.95;
			my->scalez *= 0.95;
		}
		--my->particleDuration;
		my->z += my->vel_z;
		my->yaw += 0.1;
		if ( my->yaw > 2 * PI )
		{
			my->yaw = 0;
		}
	}

	return;
}

void actAmbientParticleEffectIdle2(Entity* my)
{
	if ( !my )
	{
		return;
	}

	if ( my->particleDuration < 0 )
	{
		list_RemoveNode(my->mynode);
		return;
	}
	else
	{
		if ( my->particleShrink == 1 )
		{
			// shrink the particle.
			my->scalex *= 0.95;
			my->scaley *= 0.95;
			my->scalez *= 0.95;
		}
		--my->particleDuration;
		my->yaw += 0.1;
		if ( my->yaw > 2 * PI )
		{
			my->yaw = 0;
		}
	}
	return;
}

void Entity::spawnAmbientParticles2(int chance, int particleSprite, int duration, double particleScale, bool shrink)
{
	if ( local_rng.rand() % chance == 0 )
	{
		Entity* spawnParticle = newEntity(particleSprite, 1, map.entities, nullptr); //Particle entity.
		spawnParticle->sizex = 1;
		spawnParticle->sizey = 1;
		spawnParticle->x = x + (-2 + local_rng.rand() % 5);
		spawnParticle->y = y + (-2 + local_rng.rand() % 5);
		spawnParticle->z = z - 2;
		spawnParticle->scalex *= particleScale;
		spawnParticle->scaley *= particleScale;
		spawnParticle->scalez *= particleScale;
		spawnParticle->vel_z = -1;
		spawnParticle->particleDuration = duration;
		if ( shrink )
		{
			spawnParticle->particleShrink = 1;
		}
		else
		{
			spawnParticle->particleShrink = 0;
		}
		spawnParticle->behavior = &actAmbientParticleEffectIdle2;
		spawnParticle->flags[PASSABLE] = true;
		spawnParticle->setUID(-3);
	}
}

void Entity::spawnAmbientParticles(int chance, int particleSprite, int duration, double particleScale, bool shrink)
{
	if ( local_rng.rand() % chance == 0 )
	{
		Entity* spawnParticle = newEntity(particleSprite, 1, map.entities, nullptr); //Particle entity.
		spawnParticle->sizex = 1;
		spawnParticle->sizey = 1;
		spawnParticle->x = x + (-2 + local_rng.rand() % 5);
		spawnParticle->y = y + (-2 + local_rng.rand() % 5);
		spawnParticle->z = 7.5;
		spawnParticle->scalex *= particleScale;
		spawnParticle->scaley *= particleScale;
		spawnParticle->scalez *= particleScale;
		spawnParticle->vel_z = -1;
		spawnParticle->particleDuration = duration;
		if ( shrink )
		{
			spawnParticle->particleShrink = 1;
		}
		else
		{
			spawnParticle->particleShrink = 0;
		}
		spawnParticle->behavior = &actAmbientParticleEffectIdle;
		spawnParticle->flags[PASSABLE] = true;
		spawnParticle->setUID(-3);
	}
}

void Entity::serverUpdateEffectsForEntity(bool guarantee)
{
	if ( multiplayer != SERVER )
	{
		return;
	}

	Stat* myStats = getStats();

	if ( !myStats )
	{
		return;
	}

	for ( int player = 1; player < MAXPLAYERS; ++player )
	{
		if ( client_disconnected[player] || players[player]->isLocalPlayer() )
		{
			continue;
		}

		/*
		* Packet breakdown:
		* [0][1][2][3]: "EFFE"
		* [4][5][6][7]: Entity's UID.
		* [8][9][10][11]: Entity's effects.
		*/

		strcpy((char*)net_packet->data, "EFFE");
		SDLNet_Write32(static_cast<Uint32>(getUID()), &net_packet->data[4]);
		net_packet->data[8] = 0;
		net_packet->data[9] = 0;
		net_packet->data[10] = 0;
		net_packet->data[11] = 0;
		net_packet->data[12] = 0;
		for ( int i = 0; i < NUMEFFECTS; ++i )
		{
			if ( myStats->EFFECTS[i] )
			{
				net_packet->data[8 + i / 8] |= power(2, i - (i / 8) * 8);
			}
		}
		net_packet->address.host = net_clients[player - 1].host;
		net_packet->address.port = net_clients[player - 1].port;
		net_packet->len = 13;
		if ( guarantee )
		{
			sendPacketSafe(net_sock, -1, net_packet, player - 1);
		}
		else
		{
			sendPacket(net_sock, -1, net_packet, player - 1);
		}
		clientsHaveItsStats = true;
	}
}

bool Entity::setEffect(int effect, bool value, int duration, bool updateClients, bool guarantee)
{
	Stat* myStats = getStats();

	if ( !myStats )
	{
		return false;
	}

	if ( value == true )
	{
		switch ( effect )
		{
			case EFF_GREASY:
				if ( myStats->type == GOATMAN )
				{
					return false;
				}
				break;
			case EFF_CONFUSED:
				if ( myStats->type == LICH || myStats->type == DEVIL
					|| myStats->type == LICH_FIRE || myStats->type == LICH_ICE
					|| myStats->type == MINOTAUR )
				{
					return false;
				}
				break;
			case EFF_ASLEEP:
			case EFF_PARALYZED:
			case EFF_PACIFY:
			case EFF_KNOCKBACK:
			case EFF_BLIND:
			case EFF_WEBBED:
				if ( (myStats->type >= LICH && myStats->type < KOBOLD)
					|| myStats->type == COCKATRICE || myStats->type == LICH_FIRE || myStats->type == LICH_ICE )
				{
					if ( !(effect == EFF_PACIFY && myStats->type == SHOPKEEPER) &&
						!(effect == EFF_KNOCKBACK && myStats->type == COCKATRICE) &&
						!(effect == EFF_WEBBED && myStats->type == COCKATRICE) &&
						!(effect == EFF_BLIND && myStats->type == COCKATRICE) &&
						!(effect == EFF_BLIND && myStats->type == SHOPKEEPER) )
					{
						return false;
					}
				}
				break;
			case EFF_DISORIENTED:
				if ( myStats->type == LICH || myStats->type == DEVIL
					|| myStats->type == LICH_FIRE || myStats->type == LICH_ICE
					|| myStats->type == SHADOW || myStats->type == SHOPKEEPER )
				{
					return false;
				}
				break;
			case EFF_FEAR:
				if ( myStats->type == LICH || myStats->type == DEVIL 
					|| myStats->type == LICH_FIRE || myStats->type == LICH_ICE
					|| myStats->type == SHADOW )
				{
					return false;
				}
				break;
			case EFF_POLYMORPH:
				//if ( myStats->EFFECTS[EFF_POLYMORPH] || effectPolymorph != 0 )
				//{
				//	return false;
				//}
				break;
			case EFF_BLEEDING:
				if ( gibtype[(int)myStats->type] == 0 || gibtype[(int)myStats->type] == 5 )
				{
					return false;
				}
				break;
			default:
				break;
		}
	}
	myStats->EFFECTS[effect] = value;
	myStats->EFFECTS_TIMERS[effect] = duration;

	int player = -1;
	for ( int i = 0; i < MAXPLAYERS; ++i )
	{
		if ( players[i] && players[i]->entity == this )
		{
			player = i;
			break;
		}
	}

	if ( multiplayer == SERVER && player > 0 )
	{
		serverUpdateEffects(player);
	}

	if ( updateClients )
	{
		serverUpdateEffectsForEntity(guarantee);
	}
	return true;
}

void Entity::giveClientStats()
{
	if ( !clientStats )
	{
		clientStats = new Stat(0);
	}
    clientsHaveItsStats = true;
}

node_t* Entity::addItemToMonsterInventory(Item* item)
{
	//TODO: Sort into inventory...that is, if an item of this type already exists and they can stack, stack 'em instead of creating a new node.
	if ( !item )
	{
		return nullptr;
	}

	Stat* myStats = getStats();
	if ( !myStats )
	{
		return nullptr;
	}

	item->node = list_AddNodeLast(&myStats->inventory);
	if ( !item->node )
	{
		return nullptr;
	}
	item->node->element = item;
	item->node->deconstructor = &defaultDeconstructor;
	item->node->size = sizeof(Item);

	if ( myStats->type == SHOPKEEPER )
	{
		// sort items into slots
		std::vector<std::pair<int, Item*>> priceAndItems;
		for ( node_t* node = myStats->inventory.first; node != nullptr; node = node->next )
		{
			Item* item = (Item*)node->element;
			if ( !item ) { continue; }

			if ( !item->itemSpecialShopConsumable )
			{
				priceAndItems.push_back(std::make_pair(item->buyValue(clientnum), item));
			}
		}

		std::sort(priceAndItems.begin(), priceAndItems.end(), [](std::pair<int, Item*> lhs, std::pair<int, Item*> rhs) {
			return lhs.first > rhs.first;
		});

		int slotx = 0;
		int sloty = 0;
		for ( auto& v : priceAndItems )
		{
			Item* item = v.second;
			item->x = slotx;
			item->y = sloty;
			++slotx;
			if ( slotx >= Player::ShopGUI_t::MAX_SHOP_X )
			{
				slotx = 0;
				++sloty;
			}
		}
	}

	return item->node;
}

Item* Entity::getBestMeleeWeaponIHave() const
{
	Stat* myStats = getStats();
	if ( !myStats )
	{
		return nullptr;
	}

	Item* currentBest = nullptr;
	if ( myStats->weapon && isMeleeWeapon(*myStats->weapon) )
	{
		currentBest = myStats->weapon;
	}

	//Loop through the creature's inventory & find the best item. //TODO: Make it work on multiplayer clients?
	for ( node_t* node = myStats->inventory.first; node; node = node->next )
	{
		Item* item = static_cast<Item*>(node->element);
		if ( item )
		{
			if ( isMeleeWeapon(*item) && Item::isThisABetterWeapon(*item, currentBest) )
			{
				currentBest = item;
			}
		}
	}

	if ( currentBest && itemIsThrowableTinkerTool(currentBest) )
	{
		currentBest = nullptr;
	}
	/*if ( currentBest )
	{
		messagePlayer(clientnum, "Found best melee weapon: \"%s\"", currentBest->description());
	}*/

	return currentBest;
}

Item* Entity::getBestShieldIHave() const
{
	Stat* myStats = getStats();
	if ( !myStats )
	{
		return nullptr;
	}

	Item* currentBest = nullptr;
	if ( myStats->shield && myStats->shield->isShield() )
	{
		currentBest = myStats->shield;
	}

	//Loop through the creature's inventory & find the best item. //TODO: Make it work on multiplayer clients?
	for ( node_t* node = myStats->inventory.first; node; node = node->next )
	{
		Item* item = static_cast<Item*>(node->element);
		if ( item )
		{
			if ( item->isShield() && Item::isThisABetterArmor(*item, currentBest) )
			{
				currentBest = item;
			}
		}
	}

	/*if ( currentBest )
	{
		messagePlayer(clientnum, "Found best shield: \"%s\"", currentBest->description());
	}*/

	return currentBest;
}

void Entity::removeLightField()
{
	if ( this->light != nullptr )
	{
		list_RemoveNode(this->light->node);
		this->light = nullptr;
	}
}

void Entity::monsterEquipItem(Item& item, Item** slot)
{
	if ( !slot )
	{
		return;
	}

	Stat *myStats = getStats();
	if ( !myStats )
	{
		return;
	}

	dropItemMonster((*slot), this, myStats);

	*slot = &item;
}

bool Entity::monsterHasSpellbook(int spellbookType)
{
	if (spellbookType == SPELL_NONE )
	{
		//messagePlayer(clientnum, "[DEBUG: Entity::monsterHasSpellbook()] skipping SPELL_NONE");
		return false;
	}

	Stat* myStats = getStats();
	if ( !myStats )
	{
		return false;
	}

	if ( myStats->weapon && getSpellIDFromSpellbook(myStats->weapon->type) == spellbookType )
	{
		spell_t *spell = getSpellFromID(getSpellIDFromSpellbook(myStats->weapon->type));
		//messagePlayer(clientnum, "DEBUG: Monster has spell %s.", spell->getSpellName());
		return true;
	}

	for ( node_t* node = myStats->inventory.first; node; node = node->next )
	{
		Item* item = static_cast<Item*>(node->element);
		if ( !item )
		{
			continue;
		}

		if ( getSpellIDFromSpellbook(item->type) == spellbookType )
		{
			spell_t *spell = getSpellFromID(getSpellIDFromSpellbook(item->type));
			//messagePlayer(clientnum, "DEBUG: Monster HAS spell %s.", spell->getSpellName());
			return true;
		}
	}

	return false;
}

void Entity::playerStatIncrease(int playerClass, int chosenStats[3])
{
	std::vector<unsigned int> statWeights = classStatGrowth[playerClass];

	// debug to print which vector values are being used.
	//for ( std::vector<int>::const_iterator i = statWeights.begin(); i != statWeights.end(); ++i )
	//{
	//	messagePlayer(0, "%2d, ", *i);
	//}
	if ( ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer && playerClass == CLASS_SHAMAN && stats[skill[2]] )
	{
		if ( stats[skill[2]]->type == RAT )
		{
			//	            STR	DEX	CON	INT	PER	CHR
			statWeights = { 1,	6,	1,	3,	1,	1 };
		}
		else if ( stats[skill[2]]->type == SPIDER )
		{
			//	            STR	DEX	CON	INT	PER	CHR
			statWeights = { 1,	1,	3,	1,	6,	1 };
		}
		else if ( stats[skill[2]]->type == TROLL )
		{
			//	            STR	DEX	CON	INT	PER	CHR
			statWeights = { 6,	1,	3,	1,	1,	1 };
		}
		else if ( stats[skill[2]]->type == CREATURE_IMP )
		{
			//	            STR	DEX	CON	INT	PER	CHR
			statWeights = { 1,	3,	1,	6,	1,	1 };
		}
	}

	chosenStats[0] = local_rng.rand() % 6; // get first stat randomly.
	statWeights[chosenStats[0]] = 0; // remove the chance of the local stat vector.

	chosenStats[1] = local_rng.discrete(statWeights.data(), statWeights.size()); // get second stat.
	statWeights[chosenStats[1]] = 0; // remove the chance in the local stat vector.

	chosenStats[2] = local_rng.discrete(statWeights.data(), statWeights.size()); // get third stat.

	if ( chosenStats[0] == chosenStats[1] || chosenStats[0] == chosenStats[2] || chosenStats[1] == chosenStats[2] )
	{
		printlog("Err: duplicate stat index chosen on level up of player with class %d!\n", playerClass);
	}

	return;
}

int Entity::getManaringFromEffects(Entity* my, Stat& myStats)
{
	int manaring = 0;
	if ( myStats.EFFECTS[EFF_MP_REGEN] && myStats.type != AUTOMATON )
	{
		manaring += 2;
	}
	return manaring;
}

int Entity::getManaringFromEquipment(Entity* my, Stat& myStats, bool isPlayer)
{
	bool shapeshifted = false;
	if ( isPlayer && myStats.type != HUMAN )
	{
		if ( my && my->effectShapeshift != NOTHING )
		{
			shapeshifted = true;
		}
	}

	int manaring = 0;
	bool cursedItemIsBuff = false;
	if ( isPlayer )
	{
		cursedItemIsBuff = shouldInvertEquipmentBeatitude(&myStats);
	}
	if ( myStats.breastplate != nullptr )
	{
		if ( myStats.breastplate->type == VAMPIRE_DOUBLET )
		{
			if ( myStats.breastplate->beatitude >= 0 || cursedItemIsBuff )
			{
				manaring++;
			}
			else
			{
				manaring--;
			}
		}
	}
	if ( myStats.cloak != nullptr )
	{
		if ( myStats.cloak->type == ARTIFACT_CLOAK )
		{
			if ( myStats.cloak->beatitude >= 0 || cursedItemIsBuff )
			{
				manaring++;
			}
			else
			{
				manaring--;
			}
		}
	}
	if ( myStats.mask != nullptr )
	{
		if ( myStats.mask->type == MASK_SHAMAN && shapeshifted )
		{
			if ( myStats.mask->beatitude >= 0 || cursedItemIsBuff )
			{
				manaring++;
			}
			else
			{
				manaring--;
			}
		}
	}
	return manaring;
}

int Entity::getManaRegenInterval(Entity* my, Stat& myStats, bool isPlayer)
{
	int regenTime = getBaseManaRegen(my, myStats);
	int manaring = 0;
    Creature* myCrtr = (Creature*)my;
	if ( isPlayer && myStats.type != HUMAN )
	{
		if ( myStats.type == SKELETON )
		{
			manaring = -1; // 0.25x regen speed.
		}
	}

	int bonusManaring = 0;
	bonusManaring += Entity::getManaringFromEquipment(my, myStats, true);
	bonusManaring += Entity::getManaringFromEffects(my, myStats);
	manaring += bonusManaring;

	if ( my && bonusManaring >= 2 && ::ticks % TICKS_PER_SECOND == 0 )
	{
		bool oldRegen = myStats.EFFECTS[EFF_MP_REGEN];
		myStats.EFFECTS[EFF_MP_REGEN] = false;
		int bonusManaringNoRegen = Entity::getManaringFromEquipment(my, myStats, true) + Entity::getManaringFromEffects(my, myStats);
		if ( bonusManaringNoRegen >= 2 && myCrtr )
		{
			steamAchievementEntity(myCrtr, "BARONY_ACH_ARCANE_LINK");
		}
		myStats.EFFECTS[EFF_MP_REGEN] = oldRegen;
	}

	if ( manaring > 3 )
	{
		manaring = 3;
	}

	if ( isPlayer && myStats.type == AUTOMATON && myStats.HUNGER <= 300 )
	{
		float floatRegenTime = (60 * regenTime) / (std::max(myStats.MAXMP, 1));
		if ( manaring > 0 )
		{
			return floatRegenTime * (manaring * 2); // lose 1 MP each 12 base seconds - good!
		}
		else if ( manaring < 0 )
		{
			return floatRegenTime / (abs(manaring) * 2); // lose 1 MP each 3 base seconds - bad!
		}
		else if ( manaring == 0 )
		{
			return floatRegenTime;
		}
	}
	else if ( isPlayer && myStats.playerRace == RACE_INSECTOID && myStats.appearance == 0 )
	{
		if ( !(svFlags & SV_FLAG_HUNGER) )
		{
			return -1;
		}

		// how many hunger ticks in seconds from max of 1000.
		float floatRegenTime = (1000.f * (Entity::getHungerTickRate(&myStats, isPlayer, true)) / static_cast<float>(TICKS_PER_SECOND)); 

		floatRegenTime /= (std::max(myStats.MAXMP, 1)); // time for 1 mana in seconds
		floatRegenTime *= TICKS_PER_SECOND; // game ticks for 1 mana
		return floatRegenTime;
	}

	if ( manaring > 0 )
	{
		return regenTime / (manaring * 2); // 1 MP each 6 seconds base
	}
	else if ( manaring < 0 )
	{
		return regenTime * abs(manaring) * 4; // 1 MP each 24 seconds if negative regen
	}
	else if ( manaring == 0 )
	{
		return regenTime;
	}
	return MAGIC_REGEN_TIME;
}

int Entity::getHealringFromEffects(Creature* my, Stat& myStats)
{
	double healring = 0;
	if ( myStats.EFFECTS[EFF_HP_REGEN] )
	{
		if ( my && my->monsterAllyGetPlayerLeader() && my->monsterAllySpecial == ALLY_SPECIAL_CMD_REST && myStats.EFFECTS[EFF_ASLEEP] )
		{
			healring += 1;
		}
		else
		{
			healring += 2;
		}
	}
	if ( myStats.EFFECTS[EFF_TROLLS_BLOOD] )
	{
		healring += 1;
	}
	return (int)healring;
}

int Entity::getHealringFromEquipment(Entity* my, Stat& myStats, bool isPlayer)
{
	double healring = 0;
	bool cursedItemIsBuff = false;
	if ( isPlayer )
	{
		cursedItemIsBuff = shouldInvertEquipmentBeatitude(&myStats);
	}

	if ( myStats.ring != nullptr )
	{
		if ( myStats.ring->type == RING_REGENERATION )
		{
			if ( myStats.ring->beatitude >= 0 || cursedItemIsBuff )
			{
				healring++;
				if ( cursedItemIsBuff )
				{
					healring += std::min(static_cast<int>(abs(myStats.ring->beatitude)), 1);
				}
				else
				{
					healring += std::min(static_cast<int>(myStats.ring->beatitude), 1);
				}
			}
			else
			{
				healring--;
			}
		}
	}
	if ( myStats.breastplate != nullptr )
	{
		if ( myStats.breastplate->type == ARTIFACT_BREASTPIECE )
		{
			if ( myStats.breastplate->beatitude >= 0 || cursedItemIsBuff )
			{
				healring++;
			}
			else
			{
				healring--;
			}
		}
	}
	return healring;
}

int getBaseManaRegen(Entity* my, Stat& myStats)
{
	// reduced time from intelligence and spellcasting ability, 0-200 ticks of 300.
	int profMultiplier = (myStats.PROFICIENCIES[PRO_SPELLCASTING] / 20) + 1; // 1 to 6
	int statMultiplier = std::max(statGetINT(&myStats, my), 0); // get intelligence
	if ( myStats.type == AUTOMATON )
	{
		return MAGIC_REGEN_TIME;
	}

	int multipliedTotal = profMultiplier * statMultiplier;

	if ( myStats.weapon && myStats.weapon->type == ARTIFACT_MACE )
	{
		real_t amount = 0.0;
		getArtifactWeaponEffectChance(myStats.weapon->type, myStats, &amount);
		multipliedTotal += amount;
	}

	// unused - this is never hit by insectoid mana regen, old code
	//if ( behavior == &actPlayer && myStats.playerRace == RACE_INSECTOID && myStats.appearance == 0 )
	//{
	//	int base = MAGIC_REGEN_TIME / 3;
	//	if ( myStats.HUNGER < 50 )
	//	{
	//		base = MAGIC_REGEN_TIME * 3;
	//	}
	//	else if ( myStats.HUNGER < 250 )
	//	{
	//		base = MAGIC_REGEN_TIME;
	//	}
	//	return (base - static_cast<int>(std::min(multipliedTotal, 100))); // return 100-33 ticks, 2-0.67 seconds.
	//}

	return (MAGIC_REGEN_TIME - static_cast<int>(std::min(multipliedTotal, 200))); // return 300-100 ticks, 6-2 seconds.
}

void Entity::setRangedProjectileAttack(Creature& marksman, Stat& myStats, int optionalOverrideForArrowType)
{
	this->arrowSpeed = 7;
	this->arrowShotByWeapon = 0;
	this->arrowQuiverType = 0;

	// get arrow effects.
	if ( myStats.weapon )
	{
		this->arrowShotByWeapon = myStats.weapon->type;
		
		// no longer poisons!
		//if ( myStats.weapon->type == ARTIFACT_BOW )
		//{
		//	// poison arrow
		//	//this->arrowPoisonTime = 540;    // 9 seconds of poison
		//}

		if ( myStats.weapon->type != SLING )
		{
			// get armor pierce chance.
			int statChance = std::min(std::max(marksman.getPER() / 2, 0), 50); // 0 to 50 value.
			if ( myStats.weapon->type == HEAVY_CROSSBOW )
			{
				statChance += 50;
			}
			int chance = local_rng.rand() % 100;
			if ( chance < statChance )
			{
				this->arrowArmorPierce = 1; // pierce half of armor in damage calc.
			}
			else
			{
				this->arrowArmorPierce = 0;
			}
		}

		if ( marksman.behavior == &actPlayer )
		{
			this->setArrowProjectileProperties(this->arrowShotByWeapon);
			this->arrowShotByParent = ARROW_SHOT_BY_PLAYER;
		}
		else if ( marksman.behavior == &actMonster )
		{
			this->arrowSpeed = 7;
			if ( myStats.type == SENTRYBOT )
			{
				this->arrowShotByParent = ARROW_SHOT_BY_TRAP;
			}
			else
			{
				this->setArrowProjectileProperties(this->arrowShotByWeapon);
				this->arrowShotByParent = ARROW_SHOT_BY_MONSTER;
			}
		}
		if ( multiplayer == SERVER )
		{
			skill[2] = -(1000 + arrowShotByWeapon); // invokes actArrow for clients.
		}
	}

	int attack = 0;

	if ( (myStats.shield && rangedWeaponUseQuiverOnAttack(&myStats)) || optionalOverrideForArrowType != WOODEN_SHIELD )
	{
		if ( optionalOverrideForArrowType != WOODEN_SHIELD )
		{
			this->arrowQuiverType = optionalOverrideForArrowType;
			if ( myStats.weapon )
			{
				ItemType oldType = myStats.weapon->type;
				myStats.weapon->type = static_cast<ItemType>(optionalOverrideForArrowType);
				attack += myStats.weapon->weaponGetAttack(&myStats);
				myStats.weapon->type = oldType;
			}
		}
		else
		{
			this->arrowQuiverType = myStats.shield->type;
			attack += myStats.shield->weaponGetAttack(&myStats);
		}
		switch ( arrowQuiverType )
		{
			case QUIVER_SILVER:
				sprite = 924;
				break;
			case QUIVER_PIERCE:
				arrowArmorPierce = 2;
				sprite = 925;
				break;
			case QUIVER_LIGHTWEIGHT:
				sprite = 926;
				break;
			case QUIVER_FIRE:
				sprite = 927;
				break;
			case QUIVER_KNOCKBACK:
				sprite = 928;
				break;
			case QUIVER_CRYSTAL:
				sprite = 929;
				break;
			case QUIVER_HUNTING:
				sprite = 930;
				break;
			default:
				break;
		}
	}

	// get arrow power.
	attack += marksman.getRangedAttack();
	real_t variance = 20;
	real_t baseSkillModifier = 50.0; // 40-60 base
	real_t skillModifier = baseSkillModifier - (variance / 2) + (myStats.PROFICIENCIES[PRO_RANGED] / 2.0);
	skillModifier += (local_rng.rand() % (1 + static_cast<int>(variance)));
	skillModifier /= 100.0;
	skillModifier = std::min(skillModifier, 1.0);
	attack = attack - static_cast<int>((1.0 - skillModifier) * attack);
	this->arrowPower = attack;
}

bool Entity::setArrowProjectileProperties(int weaponType)
{
	if ( weaponType == WOODEN_SHIELD )
	{
		return false;
	}
	if ( multiplayer == CLIENT && weaponType == TOOL_SENTRYBOT )
	{
		// hack for arrow traps.
		this->arrowSpeed = 7;
		this->vel_x = cos(this->yaw) * this->arrowSpeed;
		this->vel_y = sin(this->yaw) * this->arrowSpeed;
		return true;
	}

	if ( weaponType == CROSSBOW || weaponType == SLING || weaponType == HEAVY_CROSSBOW )
	{
		this->vel_z = -0.2;
		this->arrowSpeed = 6;
		this->pitch = -PI / 32;
		this->arrowFallSpeed = 0.1;
		this->arrowBoltDropOffRange = 5; // ticks before projectile starts falling.

		this->vel_x = cos(this->yaw) * this->arrowSpeed;
		this->vel_y = sin(this->yaw) * this->arrowSpeed;
		return true;
	}
	else
	{
		this->vel_z = -0.6;
		this->arrowFallSpeed = 0.08;
		if ( weaponType == SHORTBOW || weaponType == COMPOUND_BOW || weaponType == ARTIFACT_BOW )
		{
			this->arrowSpeed = 7;
			this->vel_z = -0.6;
			this->arrowFallSpeed = 0.08;
		}
		else if ( weaponType == LONGBOW )
		{
			this->arrowSpeed = 8;
			this->vel_z = -0.4;
			this->arrowFallSpeed = 0.04;
		}
		this->pitch = -PI / 32;
		this->arrowBoltDropOffRange = 0;
		this->vel_x = cos(this->yaw) * this->arrowSpeed;
		this->vel_y = sin(this->yaw) * this->arrowSpeed;
		return true;
	}
	return false;
}

/* SetEntityOnFire
 * Attempts to set the Entity on fire. Entities that are not Burnable or are already on fire will return before any processing
 * Entities that do not have Stats (such as furniture) will return after setting the fire time and chance to stop at max
 * Entities with Stats will have their fire time (char_fire) and chance to stop being on fire (chanceToPutOutFire) reduced by their CON
 * Calculations for reductions is outlined in this function
 */
void Entity::SetEntityOnFire(Entity* sourceOfFire)
{
	// Check if the Entity can be set on fire
	if ( this->flags[BURNABLE] )
	{
		if ( ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer )
		{
			Stat* myStats = this->getStats();
			if ( myStats )
			{
				if ( myStats->type == SKELETON )
				{
					return;
				}
				if ( myStats->type == AUTOMATON )
				{
					return;
				}
				if ( myStats->breastplate && myStats->breastplate->type == MACHINIST_APRON )
				{
					return;
				}
			}
		}
		// Check if the Entity is already on fire
		if ( !(this->flags[BURNING]) )
		{
			this->flags[BURNING] = true;
			serverUpdateEntityFlag(this, BURNING);

			/* Set the time the Entity will be on fire, based off their CON
			 * |\_ MAX_TICKS_ON_FIRE is reduced by every 2 points in CON
			 * |
			 * |\_ Fire has a minimum of 4 cycles (120 ticks), and a maximum of 20 cycles (600 ticks), cycles are based off of TICKS_TO_PROCESS_FIRE
			 * |  \_ Constants are defined in entity.hpp: MIN_TICKS_ON_FIRE and MAX_TICKS_ON_FIRE
			 * |
			 *  \_ For every 5 points of CON, the chance to stop being on fire is increased
			 *    \_ The chance to stop being on fire has a minimum of 1 in 10, and a maximum of 1 in 5
			 *      \_ Constants are defined in entity.hpp: MIN_CHANCE_STOP_FIRE and MAX_CHANCE_STOP_FIRE
			 */

			// Set the default time on fire
			this->char_fire = MAX_TICKS_ON_FIRE;
			// Set the default chance of putting out fire
			this->chanceToPutOutFire = MAX_CHANCE_STOP_FIRE;

			// If the Entity is not a Monster, it wont have Stats, end here
			if ( this->getStats() == nullptr )
			{
				return; // The Entity was set on fire, it does not have Stats, so it is on fire for maximum duration
			}

			if ( ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer )
			{
				messagePlayerColor(this->skill[2], MESSAGE_COMBAT, makeColorRGB(255, 0, 0), Language::get(4324));
			}

			// Determine decrease in time on fire based on the Entity's CON
			const Sint32 entityCON = this->getStats()->CON;

			// If the Entity's CON is <= 1 then their time is just MAX_TICKS_ON_FIRE
			if ( entityCON <= 1 )
			{
				return; // The Entity was set on fire, with maximum duration and chance
			}

			// If the Entity's CON is <= 4 then their chance is just MAX_CHANCE_STOP_FIRE
			if ( entityCON <= 4 )
			{
				this->chanceToPutOutFire = MAX_CHANCE_STOP_FIRE;
			}
			else if ( entityCON >= MAX_CON_FOR_STOP_FIRE ) // If the Entity has MAX_CON_FOR_STOP_FIRE (25) or greater CON, then the reduction is equal to or less than MIN_CHANCE_STOP_FIRE
			{
				this->chanceToPutOutFire = MIN_CHANCE_STOP_FIRE;
			}
			else
			{
				this->chanceToPutOutFire -= static_cast<Sint32>(floor(entityCON * 0.2));
			}

			// If the Entity has MAX_CON_FOR_FIRE_TIME (32) or greater CON, then the reduction is equal or less than MIN_TICKS_ON_FIRE
			if ( entityCON >= MAX_CON_FOR_FIRE_TIME )
			{
				this->char_fire = MIN_TICKS_ON_FIRE;
			}
			else
			{
				this->char_fire -= static_cast<Sint32>(floor((entityCON * 0.5) * TICKS_TO_PROCESS_FIRE));
			}

			if ( sourceOfFire && sourceOfFire->behavior == &actArrow )
			{
				if ( ((Creature*)this) && ((Creature*)this)->behavior == &actMonster )
				{
					// monsters shot with arrow burn less, harder for players.
					this->char_fire = std::min(this->char_fire, TICKS_TO_PROCESS_FIRE * 6);
				}
			}

			return; // The Entity was set on fire, with a reduced duration
		}
	}

	return; // The Entity can/should not be set on fire
}

/*-------------------------------------------------------------------------------

messagePlayerMonsterEvent
handles text for monster interaction/damage/obituaries

-------------------------------------------------------------------------------*/

void messagePlayerMonsterEvent(int player, Uint32 color, Stat& monsterStats, const char* msgGeneric, const char* msgNamed, int detailType, Entity* optionalEntity)
{
	if ( player < 0 || player >= MAXPLAYERS )
	{
		return;
	}

	// If true, pretend the monster doesn't have a name and use the generic message "You hit the lesser skeleton!"
	bool namedMonsterAsGeneric = monsterNameIsGeneric(monsterStats);
	int monsterType = monsterStats.type;
    Creature* optionalEntityCrtr = (Creature*)optionalEntity;
	if ( optionalEntityCrtr != nullptr )
	{
		if ( optionalEntityCrtr->behavior == &actPlayer )
		{
			monsterType = optionalEntityCrtr->getMonsterTypeFromSprite();
		}
	}

	//char str[256] = { 0 };
	if ( !strcmp(monsterStats.name, "") )
	{
		// use generic racial name and grammar. "You hit the skeleton"
		switch (detailType)
		{
		case MSG_DESCRIPTION:
			messagePlayerColor(player, MESSAGE_INSPECTION, color, msgGeneric, getMonsterLocalizedName((Monster)monsterType).c_str());
			break;
		case MSG_COMBAT:
			messagePlayerColor(player, MESSAGE_COMBAT, color, msgGeneric, getMonsterLocalizedName((Monster)monsterType).c_str());
			break;
		case MSG_COMBAT_BASIC:
			messagePlayerColor(player, MESSAGE_COMBAT_BASIC, color, msgGeneric, getMonsterLocalizedName((Monster)monsterType).c_str());
			break;
		case MSG_OBITUARY:
			for ( int c = 0; c < MAXPLAYERS; ++c )
			{
				if ( client_disconnected[c] )
				{
					continue;
				}
				if ( c == player )
				{
					messagePlayerColor(c, MESSAGE_OBITUARY, color, msgNamed, getMonsterLocalizedName((Monster)monsterType).c_str(), monsterStats.obituary);
				}
				else if ( multiplayer != SINGLE )
				{
					messagePlayerColor(c, MESSAGE_OBITUARY, color, msgGeneric, stats[player]->name, getMonsterLocalizedName((Monster)monsterType).c_str(), monsterStats.obituary);
				}
			}
			break;
		default:
		case MSG_GENERIC:
			messagePlayerColor(player, MESSAGE_HINT, color, msgGeneric, getMonsterLocalizedName((Monster)monsterType).c_str());
			break;
		case MSG_ATTACKS:
			messagePlayerColor(player, MESSAGE_COMBAT_BASIC, color, msgGeneric,
				getMonsterLocalizedName((Monster)monsterType).c_str(),
				getMonsterLocalizedInjury((Monster)monsterType).c_str());
			break;
		case MSG_STEAL_WEAPON:
			if ( monsterStats.weapon )
			{
				messagePlayerColor(player, MESSAGE_COMBAT, color, msgGeneric, getMonsterLocalizedName((Monster)monsterType).c_str(), monsterStats.weapon->getName());
			}
			break;
		case MSG_TOOL_BOMB:
			if ( optionalEntity && optionalEntity->behavior == &actBomb )
			{
				const int itemType = optionalEntity->skill[21];
				messagePlayerColor(player, MESSAGE_COMBAT, color, msgGeneric, getMonsterLocalizedName((Monster)monsterType).c_str(), items[itemType].getIdentifiedName());
			}
			break;
		}
	}
	else
	{
		// use monster's "name" and pronoun grammar. "You hit Funny Bones!"
		switch (detailType)
		{
		case MSG_DESCRIPTION:
			if ( namedMonsterAsGeneric )
			{
				messagePlayerColor(player, MESSAGE_INSPECTION, color, msgGeneric, monsterStats.name);
			}
			else
			{
				messagePlayerColor(player, MESSAGE_INSPECTION, color, msgNamed, getMonsterLocalizedName((Monster)monsterType).c_str(), monsterStats.name);
			}
			break;
		case MSG_COMBAT:
			if ( namedMonsterAsGeneric )
			{
				messagePlayerColor(player, MESSAGE_COMBAT, color, msgGeneric, monsterStats.name);
			}
			else if ( monsterType < KOBOLD ) //Original monster count
			{
				messagePlayerColor(player, MESSAGE_COMBAT, color, msgNamed, monsterStats.name);
			}
			else if ( monsterType >= KOBOLD ) //New monsters
			{
				messagePlayerColor(player, MESSAGE_COMBAT, color, msgNamed, monsterStats.name);
			}
			break;
		case MSG_COMBAT_BASIC:
			if ( namedMonsterAsGeneric )
			{
				messagePlayerColor(player, MESSAGE_COMBAT_BASIC, color, msgGeneric, monsterStats.name);
			}
			else if ( monsterType < KOBOLD ) //Original monster count
			{
				messagePlayerColor(player, MESSAGE_COMBAT_BASIC, color, msgNamed, monsterStats.name);
			}
			else if ( monsterType >= KOBOLD ) //New monsters
			{
				messagePlayerColor(player, MESSAGE_COMBAT_BASIC, color, msgNamed, monsterStats.name);
			}
			break;
		case MSG_OBITUARY:
			for ( int c = 0; c < MAXPLAYERS; ++c )
			{
				if ( client_disconnected[c] )
				{
					continue;
				}
				if ( namedMonsterAsGeneric )
				{
					if ( c == player )
					{
						messagePlayerColor(c, MESSAGE_OBITUARY, color, msgNamed, monsterStats.name, monsterStats.obituary);
					}
					else if ( multiplayer != SINGLE )
					{
						messagePlayerColor(c, MESSAGE_OBITUARY, color, msgGeneric, stats[player]->name, monsterStats.name, monsterStats.obituary);
					}
				}
				else
				{
					if ( c == player || multiplayer != SINGLE )
					{
						messagePlayerColor(c, MESSAGE_OBITUARY, color, "%s %s", monsterStats.name, monsterStats.obituary);
					}
				}
			}
			break;
		default:
		case MSG_GENERIC:
			if ( namedMonsterAsGeneric || monsterType == HUMAN || (optionalEntityCrtr && optionalEntityCrtr->behavior == &actPlayer) )
			{
				messagePlayerColor(player, MESSAGE_HINT, color, msgGeneric, monsterStats.name);
			}
			else
			{
				messagePlayerColor(player, MESSAGE_HINT, color, msgGeneric, getMonsterLocalizedName((Monster)monsterType).c_str());
			}
			break;
		case MSG_ATTACKS:
			if ( namedMonsterAsGeneric )
			{
				messagePlayerColor(player, MESSAGE_COMBAT_BASIC, color, msgGeneric, monsterStats.name, getMonsterLocalizedInjury((Monster)monsterType).c_str());
			}
			else
			{
				messagePlayerColor(player, MESSAGE_COMBAT_BASIC, color, msgNamed, monsterStats.name, getMonsterLocalizedInjury((Monster)monsterType).c_str());
			}
			break;
		case MSG_STEAL_WEAPON:
			if ( monsterStats.weapon )
			{
				if ( namedMonsterAsGeneric )
				{
					messagePlayerColor(player, MESSAGE_COMBAT, color, msgGeneric, monsterStats.name, monsterStats.weapon->getName());
				}
				else if ( monsterType < KOBOLD ) //Original monster count
				{
					messagePlayerColor(player, MESSAGE_COMBAT, color, msgNamed, monsterStats.name, monsterStats.weapon->getName());
				}
				else if ( monsterType >= KOBOLD ) //New monsters
				{
					messagePlayerColor(player, MESSAGE_COMBAT, color, msgNamed, monsterStats.name, monsterStats.weapon->getName());
				}
			}
			break;
		case MSG_TOOL_BOMB:
			int itemType = WOODEN_SHIELD;
			if ( optionalEntity && optionalEntity->behavior == &actBomb )
			{
				itemType = optionalEntity->skill[21];
				if ( namedMonsterAsGeneric || monsterType == HUMAN )
				{
					messagePlayerColor(player, MESSAGE_COMBAT, color, msgGeneric, monsterStats.name, items[itemType].getIdentifiedName());
				}
				else
				{
					messagePlayerColor(player, MESSAGE_COMBAT, color, msgGeneric, getMonsterLocalizedName((Monster)monsterType).c_str(), items[itemType].getIdentifiedName());
				}
			}
			break;
		}
	}
}

/*-------------------------------------------------------------------------------

playerClassLangEntry
get text string for the different player chosen classes.

-------------------------------------------------------------------------------*/

char const * playerClassLangEntry(int classnum, int playernum)
{
	if ( classnum >= CLASS_BARBARIAN && classnum <= CLASS_JOKER )
	{
		return Language::get(1900 + classnum);
	}
	else if ( classnum >= CLASS_CONJURER )
	{
		return Language::get(3223 + classnum - CLASS_CONJURER);
	}
	else if ( classnum >= CLASS_SEXTON && classnum <= CLASS_MONK )
	{
		return Language::get(2550 + classnum - CLASS_SEXTON);
	}
	else
	{
		return "undefined classname";
	}
}

/*-------------------------------------------------------------------------------

playerClassDescription
get text string for the description of player chosen classes.

-------------------------------------------------------------------------------*/

char const * playerClassDescription(int classnum, int playernum)
{
	if ( classnum >= CLASS_BARBARIAN && classnum <= CLASS_JOKER )
	{
		return Language::get(10 + classnum);
	}
	else if ( classnum >= CLASS_CONJURER )
	{
		return Language::get(3231 + classnum - CLASS_CONJURER);
	}
	else if ( classnum >= CLASS_SEXTON && classnum <= CLASS_MONK )
	{
		return Language::get(2560 + classnum - CLASS_SEXTON);
	}
	else
	{
		return "undefined description";
	}
}

real_t Entity::yawDifferenceFromPlayer(int player)
{
	if ( player >= 0 && players[player] && players[player]->entity )
	{
		real_t targetYaw = this->yaw;
		while ( targetYaw >= 2 * PI )
		{
			targetYaw -= PI * 2;
		}
		while ( targetYaw < 0 )
		{
			targetYaw += PI * 2;
		}
		return (PI - abs(abs(players[player]->entity->yaw - targetYaw) - PI)) * 2;
	}
	return 0.f;
}

Entity* summonChest(long x, long y)
{
	Entity* entity = newEntity(21, 1, map.entities, nullptr); //Chest entity.
	if ( !entity )
	{
		return nullptr;
	}
	setSpriteAttributes(entity, nullptr, nullptr);
	entity->chestLocked = -1;

	// Find a free tile next to the source and then spawn it there.
	if ( multiplayer != CLIENT )
	{
		if ( entityInsideSomething(entity) )
		{
			do
			{
				entity->x = x;
				entity->y = y - 16;
				if (!entityInsideSomething(entity))
				{
					break;    // north
				}
				entity->x = x;
				entity->y = y + 16;
				if (!entityInsideSomething(entity))
				{
					break;    // south
				}
				entity->x = x - 16;
				entity->y = y;
				if (!entityInsideSomething(entity))
				{
					break;    // west
				}
				entity->x = x + 16;
				entity->y = y;
				if (!entityInsideSomething(entity))
				{
					break;    // east
				}
				entity->x = x + 16;
				entity->y = y - 16;
				if (!entityInsideSomething(entity))
				{
					break;    // northeast
				}
				entity->x = x + 16;
				entity->y = y + 16;
				if (!entityInsideSomething(entity))
				{
					break;    // southeast
				}
				entity->x = x - 16;
				entity->y = y - 16;
				if (!entityInsideSomething(entity))
				{
					break;    // northwest
				}
				entity->x = x - 16;
				entity->y = y + 16;
				if (!entityInsideSomething(entity))
				{
					break;    // southwest
				}

				// we can't have monsters in walls...
				list_RemoveNode(entity->mynode);
				entity = nullptr;
				break;
			}
			while (1);
		}
	}

	entity->sizex = 3;
	entity->sizey = 2;
	entity->x = x;
	entity->y = y;
	entity->x += 8;
	entity->y += 8;
	entity->z = 5.5;
	entity->yaw = entity->yaw * (PI / 2); //set to 0 by default in editor, can be set 0-3
	entity->behavior = &actChest;
	entity->sprite = 188;
	//entity->skill[9] = -1; //Set default chest as random category < 0

	Entity* childEntity = newEntity(216, 0, map.entities, nullptr); //Sort-of limb entity.
	if ( !childEntity )
	{
		return nullptr;
	}
	childEntity->parent = entity->getUID();
	entity->parent = childEntity->getUID();
	if ( entity->yaw == 0 ) //EAST FACING
	{
		childEntity->x = entity->x - 3;
		childEntity->y = entity->y;
	}
	else if ( entity->yaw == PI / 2 ) //SOUTH FACING
	{
		childEntity->x = entity->x;
		childEntity->y = entity->y - 3;
	}
	else if ( entity->yaw == PI ) //WEST FACING
	{
		childEntity->x = entity->x + 3;
		childEntity->y = entity->y;
	}
	else if (entity->yaw == 3 * PI/2 ) //NORTH FACING
	{
		childEntity->x = entity->x;
		childEntity->y = entity->y + 3;
	}
	else
	{
		childEntity->x = entity->x;
		childEntity->y = entity->y - 3;
	}
	//printlog("29 Generated entity. Sprite: %d Uid: %d X: %.2f Y: %.2f\n",childEntity->sprite,childEntity->getUID(),childEntity->x,childEntity->y);
	childEntity->z = entity->z - 2.75;
	childEntity->focalx = 3;
	childEntity->focalz = -.75;
	childEntity->yaw = entity->yaw;
	childEntity->sizex = 2;
	childEntity->sizey = 2;
	childEntity->behavior = &actChestLid;
	childEntity->flags[PASSABLE] = true;

	//Chest inventory.
	node_t* tempNode = list_AddNodeFirst(&entity->children);
	tempNode->element = nullptr;
	tempNode->deconstructor = &emptyDeconstructor;

	return entity;
}

void Entity::addToCreatureList(list_t *list)
{
	//printlog("*ATTEMPTING* to add Dennis to creature list.");
	if ( list )
	{
		if ( myCreatureListNode )
		{
			list_RemoveNode(myCreatureListNode);
			myCreatureListNode = nullptr;
		}
		myCreatureListNode = list_AddNodeLast(list);
		myCreatureListNode->element = this;
		myCreatureListNode->deconstructor = &emptyDeconstructor;
		myCreatureListNode->size = sizeof(Entity);
		//printlog("Added dennis to creature list.");
	}
}

void Entity::addToWorldUIList(list_t *list)
{
	//printlog("*ATTEMPTING* to add Dennis to creature list.");
	if ( list )
	{
		if ( myWorldUIListNode )
		{
			list_RemoveNode(myWorldUIListNode);
			myWorldUIListNode = nullptr;
		}
		myWorldUIListNode = list_AddNodeLast(list);
		myWorldUIListNode->element = this;
		myWorldUIListNode->deconstructor = &emptyDeconstructor;
		myWorldUIListNode->size = sizeof(Entity);
		//printlog("Added dennis to creature list.");
	}
}

int Entity::getMagicResistance(Stat* myStats)
{
	int resistance = 0;
	if ( myStats )
	{
		if ( myStats->shield )
		{
			if ( myStats->shield->type == STEEL_SHIELD_RESISTANCE )
			{
				if ( myStats->defending )
				{
					resistance += 2;
				}
				else
				{
					resistance += 1;
				}
			}
		}
		if ( myStats->ring )
		{
			if ( myStats->ring->type == RING_MAGICRESISTANCE )
			{
				resistance += 1;
			}
		}
		if ( myStats->gloves )
		{
			if ( myStats->gloves->type == ARTIFACT_GLOVES )
			{
				resistance += 1;
			}
		}
		if ( myStats->EFFECTS[EFF_MAGICRESIST] )
		{
			resistance += 1;
		}
		if ( myStats->EFFECTS[EFF_SHRINE_BLUE_BUFF] )
		{
			resistance += 1;
		}
	}
	else
	{
		return 0;
	}
	return resistance;
}

void Entity::setHardcoreStats(Stat& stats)
{
	if ( (svFlags & SV_FLAG_HARDCORE) && stats.MISC_FLAGS[STAT_FLAG_MONSTER_DISABLE_HC_SCALING] == 0 )
	{
		// spice up some stats...
		int statIncrease = ((abs(stats.HP) / 20 + 1) * 20); // each 20 HP add 20 random HP
		stats.HP += statIncrease - (local_rng.rand() % (std::max(statIncrease / 5, 1))); // 80%-100% of increased value
		stats.MAXHP = stats.HP;
		stats.OLDHP = stats.HP;

		statIncrease = (abs(stats.STR) / 5 + 1) * 5; // each 5 STR add 5 more STR.
		stats.STR += (statIncrease - (local_rng.rand() % (std::max(statIncrease / 4, 1)))); // 75%-100% of increased value.

		statIncrease = (abs(stats.PER) / 5 + 1) * 5; // each 5 PER add 5 more PER.
		stats.PER += (statIncrease - (local_rng.rand() % (std::max(statIncrease / 4, 1)))); // 75%-100% of increased value.

		statIncrease = std::min((abs(stats.DEX) / 4 + 1) * 1, 8); // each 4 DEX add 1 more DEX, capped at 8.
		stats.DEX += (statIncrease - (local_rng.rand() % (std::max(statIncrease / 2, 1)))); // 50%-100% of increased value.

		statIncrease = (abs(stats.CON) / 5 + 1) * 1; // each 5 CON add 1 more CON.
		stats.CON += (statIncrease - (local_rng.rand() % (std::max(statIncrease / 2, 1)))); // 50%-100% of increased value.

		statIncrease = (abs(stats.INT) / 5 + 1) * 5; // each 5 INT add 5 more INT.
		stats.INT += (statIncrease - (local_rng.rand() % (std::max(statIncrease / 2, 1)))); // 50%-100% of increased value.

		int lvlIncrease = local_rng.rand() % 4;
		lvlIncrease = std::max(0, lvlIncrease - 1);
		stats.LVL += std::max(0, lvlIncrease - 1); // increase by 1 or 2 50%, else stay same.
	}
	//messagePlayer(0, "Set stats to: ");
	//messagePlayer(0, "MAXHP: %d", stats.MAXHP);
	//messagePlayer(0, "HP: %d", stats.HP);
	//messagePlayer(0, "MAXMP: %d", stats.MAXMP);
	//messagePlayer(0, "MP: %d", stats.MP);
	//messagePlayer(0, "Str: %d", stats.STR);
	//messagePlayer(0, "Dex: %d", stats.DEX);
	//messagePlayer(0, "Con: %d", stats.CON);
	//messagePlayer(0, "Int: %d", stats.INT);
	//messagePlayer(0, "Per: %d", stats.PER);
	//messagePlayer(0, "Chr: %d", stats.CHR);
	//messagePlayer(0, "LVL: %d", stats.LVL);
	//messagePlayer(0, "GOLD: %d", stats.GOLD);
}

int playerEntityMatchesUid(Uint32 uid)
{
	for ( int i = 0; i < MAXPLAYERS; ++i )
	{
		if ( players[i] && players[i]->entity && players[i]->entity->getUID() == uid )
		{
			return i;
		}
	}

	return -1;
}

bool monsterNameIsGeneric(Stat& monsterStats)
{
	if ( monsterStats.MISC_FLAGS[STAT_FLAG_MONSTER_NAME_GENERIC] == 1 )
	{
		return true;
	}
	if ( strstr(monsterStats.name, "lesser")
		|| strstr(monsterStats.name, "young")
		|| strstr(monsterStats.name, "enslaved")
		|| strstr(monsterStats.name, "damaged")
		|| strstr(monsterStats.name, "corrupted")
		|| strstr(monsterStats.name, "cultist")
		|| strstr(monsterStats.name, "knight")
		|| strstr(monsterStats.name, "sentinel")
		|| strstr(monsterStats.name, "mage")
		|| strstr(monsterStats.name, "inner")
		|| strstr(monsterStats.name, "training")
		|| strstr(monsterStats.name, "Training")
		|| strstr(monsterStats.name, "Mysterious")
		|| strstr(monsterStats.name, "shaman") )
	{
		// If true, pretend the monster doesn't have a name and use the generic message "You hit the lesser skeleton!"
		return true;
	}
	return false;
}

node_t* TileEntityListHandler::addEntity(Entity& entity)
{
	if ( entity.myTileListNode )
	{
		return nullptr;
	}

	if ( static_cast<Sint32>(entity.getUID()) <= -3 )
	{
		return nullptr;
	}

	int x = (static_cast<int>(entity.x) >> 4);
	int y = (static_cast<int>(entity.y) >> 4);
	if ( x >= 0 && x < kMaxMapDimension && y >= 0 && y < kMaxMapDimension )
	{
		//messagePlayer(0, "added at %d, %d", x, y);
		entity.myTileListNode = list_AddNodeLast(&TileEntityList.gridEntities[x][y]);
		entity.myTileListNode->element = &entity;
		entity.myTileListNode->deconstructor = &emptyDeconstructor;
		entity.myTileListNode->size = sizeof(Entity);
		return entity.myTileListNode;
	}

	return nullptr;
}

node_t* TileEntityListHandler::updateEntity(Entity& entity)
{
	if ( !entity.myTileListNode )
	{
		return nullptr;
	}

	int x = (static_cast<int>(entity.x) >> 4);
	int y = (static_cast<int>(entity.y) >> 4);
	if ( x >= 0 && x < kMaxMapDimension && y >= 0 && y < kMaxMapDimension )
	{
		list_RemoveNode(entity.myTileListNode);
		entity.myTileListNode = list_AddNodeLast(&TileEntityList.gridEntities[x][y]);
		entity.myTileListNode->element = &entity;
		entity.myTileListNode->deconstructor = &emptyDeconstructor;
		entity.myTileListNode->size = sizeof(Entity);
		return entity.myTileListNode;
	}

	return nullptr;
}

void TileEntityListHandler::clearTile(int x, int y)
{
	list_FreeAll(&gridEntities[x][y]);
}

void TileEntityListHandler::emptyGridEntities()
{
	for ( int i = 0; i < kMaxMapDimension; ++i )
	{
		for ( int j = 0; j < kMaxMapDimension; ++j )
		{
			clearTile(i, j);
		}
	}
}

list_t* TileEntityListHandler::getTileList(int x, int y)
{
	if ( x >= 0 && x < kMaxMapDimension && y >= 0 && y < kMaxMapDimension )
	{
		return &gridEntities[x][y];
	}
	return nullptr;
}

/* returns list of entities within a radius, e.g 1 radius is a 3x3 area around given center. */
std::vector<list_t*> TileEntityListHandler::getEntitiesWithinRadius(int u, int v, int radius)
{
	std::vector<list_t*> return_val;

	for ( int i = u - radius; i <= u + radius; ++i )
	{
		for ( int j = v - radius; j <= v + radius; ++j )
		{
			list_t* list = getTileList(i, j);
			if ( list )
			{
				return_val.push_back(list);
			}
		}
	}

	return return_val;
}

/* returns list of entities within a radius around entity, e.g 1 radius is a 3x3 area around entity. */
std::vector<list_t*> TileEntityListHandler::getEntitiesWithinRadiusAroundEntity(Entity* entity, int radius)
{
	int u = static_cast<int>(entity->x) >> 4;
	int v = static_cast<int>(entity->y) >> 4;
	return getEntitiesWithinRadius(u, v, radius);
}

bool Entity::isBossMonster()
{
	Stat* myStats = getStats();
	if ( myStats )
	{
		if ( myStats->type == MINOTAUR
			|| myStats->type == SHOPKEEPER
			|| myStats->type == SHADOW
			|| myStats->type == LICH
			|| myStats->type == LICH_FIRE
			|| myStats->type == LICH_ICE
			|| myStats->type == DEVIL
			|| (myStats->type == VAMPIRE && MonsterData_t::nameMatchesSpecialNPCName(*myStats, "bram kindly"))
			|| (myStats->type == COCKATRICE && !strncmp(map.name, "Cockatrice Lair", 15))
			)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool monsterIsImmobileTurret(Entity* my, Stat* myStats)
{
	if ( myStats )
	{
		if ( myStats->type == SENTRYBOT || myStats->type == SPELLBOT || myStats->type == DUMMYBOT )
		{
			return true;
		}
	}
	else if ( my )
	{
		int race = my->getMonsterTypeFromSprite();
		if ( race == SENTRYBOT || race == SPELLBOT || race == DUMMYBOT )
		{
			return true;
		}
	}

	return false;
}

bool monsterChangesColorWhenAlly(Stat* myStats, Entity* entity)
{
	int race = NOTHING;
	if ( !myStats )
	{
		if ( !entity )
		{
			return true;
		}
		race = entity->getMonsterTypeFromSprite();
	}
	else
	{
		race = myStats->type;
	}
	
	if ( race == HUMAN || race == SENTRYBOT
		|| race == SPELLBOT || race == AUTOMATON || race == GYROBOT || race == DUMMYBOT )
	{
		return false;
	}
	return true;
}

int monsterTinkeringConvertHPToAppearance(Stat* myStats)
{
	if ( myStats )
	{
		if ( myStats->MAXHP == 0 || myStats->HP == 0 )
		{
			return 0;
		}
		if ( myStats->MAXHP == myStats->HP )
		{
			return ITEM_TINKERING_APPEARANCE;
		}
		real_t ratio = (1.0 * myStats->HP) / (myStats->MAXHP);
		if ( ratio >= 0.74 )
		{
			return 3;
		}
		else if ( ratio >= 0.49 )
		{
			return 2;
		}
		else if ( ratio >= 0.24 )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

int monsterTinkeringConvertAppearanceToHP(Stat* myStats, int appearance)
{
	if ( myStats )
	{
		if ( appearance == ITEM_TINKERING_APPEARANCE || (appearance > 0 && appearance % 10 == 0) )
		{
			return myStats->MAXHP;
		}
		int randomHP = std::max(1, myStats->MAXHP / 8);
		randomHP = randomHP + local_rng.rand() % randomHP;
		int convertedAppearance = appearance % 10;
		return std::min(myStats->MAXHP, ((convertedAppearance * myStats->HP) / 4) + randomHP);
	}
	return 0;
}

void Entity::handleQuiverThirdPersonModel(Stat& myStats)
{
	if ( multiplayer == CLIENT )
	{
		return;
	}
	if ( !myStats.breastplate )
	{
		switch ( myStats.type )
		{
			case SKELETON:
			case AUTOMATON:
				sprite += 2; // short strap
				break;
			case KOBOLD:
			case GNOME:
				// no strap.
				break;
			default:
				sprite += 1; // normal strap
				break;
		}
	}
	else
	{
		switch ( myStats.type )
		{
			case SKELETON:
			case AUTOMATON:
				sprite += 2; // short strap
				break;
			default:
				sprite += 3; // shoulderpad-less.
				break;
		}
	}
}

real_t Entity::getDamageTableMultiplier(Entity* my, Stat& myStats, DamageTableType damageType)
{
	real_t damageMultiplier = damagetables[myStats.type][damageType];
	real_t bonus = 0.0;
	if ( myStats.EFFECTS[EFF_SHADOW_TAGGED] )
	{
		if ( myStats.type == LICH || myStats.type == LICH_FIRE || myStats.type == LICH_ICE
			|| myStats.type == DEVIL )
		{
			return 1.5; // rough average.
		}
		else
		{
			return 1.0;
		}
	}
	//messagePlayer(0, "%f", damageMultiplier);
	if ( myStats.type == GOATMAN && myStats.EFFECTS[EFF_DRUNK] )
	{
		bonus = -.2;
	}
	return std::max(0.0, damageMultiplier + bonus);
}

void Entity::createWorldUITooltip()
{
	for ( int i = 0; i < MAXPLAYERS; ++i )
	{
		if ( !players[i]->isLocalPlayerAlive() )
		{
			continue;
		}
		Entity* worldTooltip = nullptr;
#ifndef NINTENDO
		bool failedToAllocate = false;
		try
		{
			worldTooltip = new Entity(-1, 1, map.worldUI, nullptr);
		}
		catch ( std::bad_alloc& ba )
		{
			failedToAllocate = true;
		}

		if ( failedToAllocate || !worldTooltip )
		{
			printlog("failed to allocate memory for new entity!\n");
			exit(1);
		}
#else
		worldTooltip = new Entity(-1, 1, map.worldUI, nullptr);
#endif

		worldTooltip->x = this->x;
		worldTooltip->y = this->y;
		worldTooltip->z = this->z;
		worldTooltip->sizex = 1;
		worldTooltip->sizey = 1;
        worldTooltip->ditheringDisabled = true;
		worldTooltip->flags[NOUPDATE] = true;
		worldTooltip->flags[PASSABLE] = true;
		worldTooltip->flags[SPRITE] = true;
		worldTooltip->flags[UNCLICKABLE] = true;
		worldTooltip->behavior = &actSpriteWorldTooltip;
		worldTooltip->parent = this->getUID();
		worldTooltip->scalex = 0.05;
		worldTooltip->scaley = 0.05;
		worldTooltip->scalez = 0.05;
		worldTooltip->worldTooltipPlayer = i;
		worldTooltip->worldTooltipZ = 1.5;
		players[i]->worldUI.setTooltipDisabled(*worldTooltip);
		//worldTooltip->addToWorldUIList(map.worldUI);

		if ( behavior != &actItem )
		{
			worldTooltip->worldTooltipIgnoreDrawing = 1;
		}

		if ( bEntityTooltipRequiresButtonHeld() )
		{
			worldTooltip->worldTooltipRequiresButtonHeld = 1;
		}
	}
}

bool Entity::bEntityTooltipRequiresButtonHeld() const
{
	if ( behavior == &actPortal || behavior == &actLadder
		|| behavior == &::actMidGamePortal || behavior == &::actExpansionEndGamePortal
		|| behavior == &actWinningPortal || behavior == &actCustomPortal )
	{
		return true;
	}
	return false;
}

bool Entity::bEntityHighlightedForPlayer(const int player) const
{
	if ( player < 0 || player >= MAXPLAYERS )
	{
		return false;
	}
	if ( ((((Creature*)this) && ((Creature*)this)->behavior == &actPlayer) || behavior == &actPlayerLimb)
		&& StatueManager.activeEditing && highlightForUI > 0.001 )
	{
		return true;
	}
	if ( ((Creature*)this) )
	{
		return false;
	}
	if ( players[player]->worldUI.uidForActiveTooltip != 0 )
	{
		if ( players[player]->worldUI.uidForActiveTooltip == getUID() )
		{
			return true;
		}
	}
	return false;
}

int getEntityHungerInterval(int player, Creature* my, Stat* myStats, EntityHungerIntervals hungerInterval)
{
	bool isInsectoidPlayer = false;
	bool isAutomatonPlayer = false;
	if ( player >= 0 )
	{
		if ( stats[player]->type == AUTOMATON )
		{
			isAutomatonPlayer = true;
		}
		else if ( stats[player]->playerRace == RACE_INSECTOID && stats[player]->appearance == 0 )
		{
			isInsectoidPlayer = true;
		}
	}
	else if ( my && my->behavior == &actPlayer && myStats )
	{
		if ( myStats->type == AUTOMATON )
		{
			isAutomatonPlayer = true;
		}
		else if ( myStats->playerRace == RACE_INSECTOID && myStats->appearance == 0 )
		{
			isInsectoidPlayer = true;
		}
	}
	else if ( myStats )
	{
		for ( int i = 0; i < MAXPLAYERS; ++i )
		{
			if ( myStats == stats[i] )
			{
				if ( myStats->type == AUTOMATON )
				{
					isAutomatonPlayer = true;
				}
				else if ( myStats->playerRace == RACE_INSECTOID && myStats->appearance == 0 )
				{
					isInsectoidPlayer = true;
				}
				break;
			}
		}
	}

	if ( isAutomatonPlayer )
	{
		switch ( hungerInterval )
		{
			case HUNGER_INTERVAL_OVERSATIATED:
				return 5000; // unreachable
			case HUNGER_INTERVAL_HUNGRY:
				return -1; // unreachable
			case HUNGER_INTERVAL_WEAK:
				return -1; // unreachable
			case HUNGER_INTERVAL_STARVING:
				return -1; // unreachable
			case HUNGER_INTERVAL_AUTOMATON_CRITICAL:
				return 300;
			case HUNGER_INTERVAL_AUTOMATON_SUPERHEATED:
				return 1200;
			default:
				return 1000;
		}
	}

	switch ( hungerInterval )
	{
		case HUNGER_INTERVAL_OVERSATIATED:
			return 1500;
		case HUNGER_INTERVAL_HUNGRY:
			return !isInsectoidPlayer ? 250 : 100;
		case HUNGER_INTERVAL_WEAK:
			return !isInsectoidPlayer ? 150 : 50;
		case HUNGER_INTERVAL_STARVING:
			return !isInsectoidPlayer ? 50 : 25;
		default:
			break;
	}
	return 1000;
}

/*-------------------------------------------------------------------------------

Creature::setHP

sets the HP of the given entity

-------------------------------------------------------------------------------*/

void Entity::setHP(int amount)
{
	Stat* entitystats = this->getStats();
	if ( !entitystats )
	{
		return;
	}

	int healthDiff = entitystats->HP;

	if ( ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer && godmode )
	{
		amount = entitystats->MAXHP;
	}
	if ( !entitystats || amount == entitystats->HP )
	{
		return;
	}
	entitystats->HP = std::min(std::max(0, amount), entitystats->MAXHP);
	healthDiff -= entitystats->HP;
	strncpy(entitystats->obituary, Language::get(1500), 127);

	if ( ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer && buddhamode && entitystats->HP < 1 )
	{
		entitystats->HP = 1; //Buddhas never die!
	}

	if ( ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer && entitystats->OLDHP >= entitystats->HP )
	{
		inputs.addRumbleForPlayerHPLoss(skill[2], amount);
	}

	if ( multiplayer == SERVER )
	{
		for ( int i = 0; i < MAXPLAYERS; i++ )
		{
			if ( i > 0 && players[i] && this == players[i]->entity && !players[i]->isLocalPlayer() )
			{
				// tell the client its HP changed
				strcpy((char*)net_packet->data, "UPHP");
				SDLNet_Write32((uint32_t)entitystats->HP, &net_packet->data[4]);
				SDLNet_Write32((uint32_t)NOTHING, &net_packet->data[8]);
				net_packet->address.host = net_clients[i - 1].host;
				net_packet->address.port = net_clients[i - 1].port;
				net_packet->len = 12;
				sendPacketSafe(net_sock, -1, net_packet, i - 1);
			}
			if ( ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer && abs(healthDiff) > 0 )
			{
				if ( serverSchedulePlayerHealthUpdate == 0 )
				{
					serverSchedulePlayerHealthUpdate = ::ticks;
				}
			}
		}
		if ( ((Creature*)this) && ((Creature*)this)->behavior == &actMonster )
		{
			if ( this->monsterAllyIndex >= 1 && this->monsterAllyIndex < MAXPLAYERS )
			{
				if ( abs(healthDiff) == 1 || healthDiff == 0 )
				{
					serverUpdateAllyHP(this->monsterAllyIndex, getUID(), entitystats->HP, entitystats->MAXHP, true);
				}
				else
				{
					serverUpdateAllyHP(this->monsterAllyIndex, getUID(), entitystats->HP, entitystats->MAXHP, true);
				}
			}
		}
	}
}

/*-------------------------------------------------------------------------------

Creature::modHP

modifies the HP of the given entity

-------------------------------------------------------------------------------*/

void Entity::modHP(int amount)
{
	Stat* entitystats = this->getStats();

	if ( ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer )
	{
		if ( godmode && amount < 0 )
		{
			amount = 0;
		}
		else if ( entitystats && entitystats->type == AUTOMATON && entitystats->HP <= 0 && ((Creature*)this)->playerAutomatonDeathCounter != 0 )
		{
			return;
		}
	}

	if ( !entitystats || amount == 0 )
	{
		if ( ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer )
		{
			inputs.addRumbleForPlayerHPLoss(skill[2], amount);
		}
		return;
	}

	this->setHP(entitystats->HP + amount);
}

/*-------------------------------------------------------------------------------

Creature::drainMP

Removes this much from MP. Anything over the entity's MP is subtracted from their health. Can be very dangerous.

-------------------------------------------------------------------------------*/

void Entity::drainMP(int amount, bool notifyOverexpend)
{
	//A pointer to the entity's stats.
	Stat* entitystats = this->getStats();

	//Check if no stats found.
	if ( entitystats == NULL || amount == 0 )
	{
		return;
	}

	int overdrawn = 0;
	entitystats->MP -= amount;
	int player = -1;
	for ( int i = 0; i < MAXPLAYERS; ++i )
	{
		if ( players[i] && this == players[i]->entity )
		{
			player = i; //Set the player.
		}
	}

	if ( player >= 0 && entitystats->playerRace == RACE_INSECTOID && entitystats->appearance == 0 )
	{
		if ( svFlags & SV_FLAG_HUNGER )
		{
			// we cast a spell or forcibly reduced our MP. therefore our hunger should reduce to match the MP value.
			if ( amount > 0 )
			{
				int32_t hungerPointPerMana = playerInsectoidHungerValueOfManaPoint(*entitystats);
				int32_t oldHunger = entitystats->HUNGER;
				entitystats->HUNGER -= amount * hungerPointPerMana;
				entitystats->HUNGER = std::max(0, entitystats->HUNGER);
				if ( player > 0 )
				{
					serverUpdateHunger(player);
				}
			}
		}
	}

	if ( entitystats->MP < 0 )
	{
		//Overdrew. Take that extra and flow it over into HP.
		overdrawn = entitystats->MP;
		entitystats->MP = 0;
	}
	if ( multiplayer == SERVER )
	{
		//First check if the entity is the player.
		for ( int i = 1; i < MAXPLAYERS; ++i )
		{
			if ( players[i] && this == players[i]->entity && !players[i]->isLocalPlayer() )
			{
				//It is. Tell the client its MP just changed.
				strcpy((char*)net_packet->data, "UPMP");
				SDLNet_Write32((uint32_t)entitystats->MP, &net_packet->data[4]);
				SDLNet_Write32((uint32_t)stats[i]->type, &net_packet->data[8]);
				net_packet->address.host = net_clients[i - 1].host;
				net_packet->address.port = net_clients[i - 1].port;
				net_packet->len = 12;
				sendPacketSafe(net_sock, -1, net_packet, i - 1);
			}
		}
	}
	else if ( clientnum != 0 && multiplayer == CLIENT )
	{
		if ( this == players[clientnum]->entity )
		{
			//It's the player entity. Tell the server its MP changed.
			strcpy((char*)net_packet->data, "UPMP");
			net_packet->data[4] = clientnum;
			SDLNet_Write32((uint32_t)entitystats->MP, &net_packet->data[5]);
			SDLNet_Write32((uint32_t)stats[clientnum]->type, &net_packet->data[9]);
			net_packet->address.host = net_server.host;
			net_packet->address.port = net_server.port;
			net_packet->len = 13;
			sendPacketSafe(net_sock, -1, net_packet, 0);
		}
	}

	if ( overdrawn < 0 )
	{
		if ( player >= 0 && notifyOverexpend )
		{
			uint32_t color = makeColorRGB(255, 255, 0);
			messagePlayerColor(player, MESSAGE_STATUS, color, Language::get(621));
		}
		this->modHP(overdrawn); //Drain the extra magic from health.
		Stat* tempStats = this->getStats();
		if ( tempStats )
		{
			if ( tempStats->sex == MALE )
			{
				this->setObituary(Language::get(1528));
			}
			else
			{
				this->setObituary(Language::get(1529));
			}
			tempStats->killer = FAILED_INVOCATION;
		}
	}
}

/*-------------------------------------------------------------------------------

Creature::awardXP

Awards XP to the dest (ie killer) entity from the src (ie killed) entity

-------------------------------------------------------------------------------*/

void Entity::awardXP(Entity* src, bool share, bool root)
{
	if ( !src )
	{
		return;
	}
    Creature* srcCrtr = (Creature*)src;

	Stat* destStats = getStats();
	Stat* srcStats = src->getStats();

	if ( !destStats || !srcStats )
	{
		return;
	}

	if ( srcCrtr && srcCrtr->behavior == &actMonster
		&& (srcCrtr->monsterAllySummonRank != 0
			|| srcCrtr->monsterIsTinkeringCreation()) )
	{
		return; // summoned monster, no XP!
	}
	if ( srcStats->type == INCUBUS && !strncmp(srcStats->name, "inner demon", strlen("inner demon")) )
	{
		return;
	}

	int player = -1;
	if ( ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer )
	{
		player = skill[2];
		if ( srcCrtr && srcCrtr->behavior == &actPlayer && root )
		{
			return;
		}
	}

	// calculate XP gain
	int baseXp = 10;
	int xpGain = baseXp + local_rng.rand() % std::max(1, baseXp) + std::max(0, srcStats->LVL - destStats->LVL) * baseXp;
	if ( srcStats->MISC_FLAGS[STAT_FLAG_XP_PERCENT_AWARD] > 0 )
	{
		int value = srcStats->MISC_FLAGS[STAT_FLAG_XP_PERCENT_AWARD] - 1; // offset by 1 since 0 is nothing
		double percent = value / 100.f;
		xpGain = percent * xpGain;
	}
	if ( gameplayCustomManager.inUse() )
	{
		xpGain = (gameplayCustomManager.globalXPPercent / 100.f) * xpGain;
	}

	// save hit struct
	hit_t tempHit;
	tempHit.entity = hit.entity;
	tempHit.mapx = hit.mapx;
	tempHit.mapy = hit.mapy;
	tempHit.side = hit.side;
	tempHit.x = hit.x;
	tempHit.y = hit.y;

	int shareRange = gameplayCustomManager.inUse() ? gameplayCustomManager.xpShareRange : XPSHARERANGE;

	// divide shares
	if ( player >= 0 )
	{
		int numshares = 0;
		Entity* shares[MAXPLAYERS];
		int c;

		for ( c = 0; c < MAXPLAYERS; ++c )
		{
			shares[c] = nullptr;
		}

		// find other players to divide shares with
		node_t * node;
		for ( node = map.creatures->first; node != nullptr; node = node->next ) //Since only looking at players, this should just iterate over players[]
		{
			Creature* creature = (Creature*)node->element;
			if ( creature == this )
			{
				continue;
			}
			if ( creature && creature->behavior == &actPlayer )
			{
				if ( entityDist(this, creature) < shareRange )
				{
					++numshares;
					shares[numshares] = creature;
					if ( numshares == MAXPLAYERS - 1 )
					{
						break;
					}
				}
			}
		}

		// divide value of each share
		if ( numshares )
		{
			xpGain /= numshares;
		}

		// award XP to everyone else in the group
		if ( share )
		{
			for ( c = 0; c < MAXPLAYERS; c++ )
			{
				if ( shares[c] )
				{
					shares[c]->awardXP(src, false, false);
				}
			}

			if ( ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer )
			{
				if ( stats[this->skill[2]] )
				{
					// award XP to player's followers.
					int numFollowers = list_Size(&stats[this->skill[2]]->FOLLOWERS);
					for ( node = stats[this->skill[2]]->FOLLOWERS.first; node != nullptr; node = node->next )
					{
						Creature* follower = nullptr;
						if ( (uint32_t *)node->element )
						{
							follower = uidToCreature(*((uint32_t *)node->element));
						}
						if ( follower && entityDist(this, follower) < shareRange && follower != src )
						{
							if ( follower->monsterIsTinkeringCreation() )
							{
								--numFollowers; // tinkering creation don't penalise XP.
								continue;
							}
							Stat* followerStats = follower->getStats();
							if ( followerStats )
							{
								//int xpDivide = std::min(std::max(1, numFollowers), 4); // 1 - 4 depending on followers.
								if ( follower->monsterAllySummonRank != 0 && numshares > 0 )
								{
									followerStats->EXP += (xpGain * numshares); // summoned monsters aren't penalised XP.
								}
								else
								{
									followerStats->EXP += (xpGain);
								}
								//messagePlayer(0, "monster got %d xp", xpGain);
							}
						}
					}
				}
			}
		}

	}

	// award XP to main victor
	if ( !this->monsterIsTinkeringCreation() )
	{
		destStats->EXP += xpGain;
	}

	if ( (srcStats->type == LICH || srcStats->type == LICH_FIRE || srcStats->type == LICH_ICE) && root
		&& ((Creature*)this) && ((Creature*)this)->behavior == &actMonster )
	{
		if ( destStats->type == CREATURE_IMP
			|| destStats->type == DEMON
			|| (destStats->type == AUTOMATON && !strcmp(destStats->name, "corrupted automaton")) )
		{
			if ( !flags[USERFLAG2] )
			{
				for ( int c = 0; c < MAXPLAYERS; c++ )
				{
					steamAchievementClient(c, "BARONY_ACH_OWN_WORST_ENEMY");
				}
			}
		}
	}

	if ( root ) // global stats
	{
		if ( srcCrtr && srcCrtr->behavior == &actPlayer && ((Creature*)this) && ((Creature*)this)->behavior == &actMonster )
		{
			achievementObserver.updateGlobalStat(getIndexForDeathType(destStats->type));
		}
		else if ( srcCrtr && srcCrtr->behavior == &actMonster && ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer )
		{
			if ( srcStats->type == LICH )
			{
				achievementObserver.updateGlobalStat(STEAM_GSTAT_HERX_SLAIN);
			}
			else if ( srcStats->type == LICH_FIRE )
			{
				achievementObserver.updateGlobalStat(STEAM_GSTAT_TWINSFIRE_SLAIN);
			}
			else if ( srcStats->type == LICH_ICE )
			{
				achievementObserver.updateGlobalStat(STEAM_GSTAT_TWINSICE_SLAIN);
			}
			else if ( srcStats->type == DEVIL )
			{
				achievementObserver.updateGlobalStat(STEAM_GSTAT_BAPHOMET_SLAIN);
			}
			else if ( srcStats->type == MINOTAUR )
			{
				achievementObserver.updateGlobalStat(STEAM_GSTAT_MINOTAURS_SLAIN);
			}
			else if ( srcStats->type == SHOPKEEPER )
			{
				achievementObserver.updateGlobalStat(STEAM_GSTAT_SHOPKEEPERS_SLAIN);
			}
		}
	}


	// award bonus XP and update kill counters
	if ( player >= 0 )
	{
		if ( root == false )
		{
			updateAchievementThankTheTank(player, src, true);
		}
		if ( currentlevel >= 25 && srcStats->type == MINOTAUR )
		{
			for ( int c = 0; c < MAXPLAYERS; c++ )
			{
				steamAchievementClient(c, "BARONY_ACH_REUNITED");
			}
		}
		if ( srcStats->type == SHADOW && root )
		{
			std::basic_string<char> name = "Shadow of ";
			name += stats[player]->name;
			if ( name.compare(srcStats->name) == 0 )
			{
				steamAchievementClient(player, "BARONY_ACH_KNOW_THYSELF");
			}
		}
		if ( srcStats->LVL >= 25 && root
			&& destStats->HP <= 5 && checkEnemy(src) )
		{
			steamAchievementClient(player, "BARONY_ACH_BUT_A_SCRATCH");
		}
		if ( srcStats->EFFECTS[EFF_PARALYZED] )
		{
			serverUpdatePlayerGameplayStats(player, STATISTICS_SITTING_DUCK, 1);
		}
		if ( root )
		{
			achievementObserver.awardAchievementIfActive(player, src, AchievementObserver::BARONY_ACH_TELEFRAG);
			if ( stats[player]->playerRace == RACE_INCUBUS && stats[player]->appearance == 0 )
			{
				achievementObserver.playerAchievements[player].checkTraditionKill(this, src);
			}
			if ( stats[player]->type == SPIDER && srcStats->EFFECTS[EFF_WEBBED] )
			{
				steamStatisticUpdateClient(player, STEAM_STAT_MANY_PEDI_PALP, STEAM_STAT_INT, 1);
			}

			bool guerillaRadio = false;
			if ( srcCrtr && srcCrtr->monsterTarget != 0 )
			{
				Entity* wasTargeting = uidToEntity(srcCrtr->monsterTarget);
				if ( wasTargeting )
				{
					if ( srcCrtr->monsterState == MONSTER_STATE_ATTACK && wasTargeting->getMonsterTypeFromSprite() == DUMMYBOT )
					{
						steamStatisticUpdateClient(player, STEAM_STAT_RAGE_AGAINST, STEAM_STAT_INT, 1);
					}
					else if ( wasTargeting->behavior == &actDecoyBox )
					{
						guerillaRadio = true;
					}
				}
			}
			if ( !guerillaRadio && srcCrtr )
			{//TODO: BIRD -- Recheck logic of this to see if it scans \/
				Entity* noisemaker = uidToEntity(srcCrtr->monsterLastDistractedByNoisemaker);
				if ( noisemaker && noisemaker->behavior == &actDecoyBox
					&& entityDist(noisemaker, src) < TOUCHRANGE )
				{
					guerillaRadio = true;
				}
			}

			if ( guerillaRadio && srcCrtr )
			{
				steamStatisticUpdateClient(player, STEAM_STAT_GUERILLA_RADIO, STEAM_STAT_INT, 1);
				if ( local_rng.rand() % 5 == 0 || (uidToEntity(srcCrtr->monsterTarget) != this && local_rng.rand() % 3 == 0) )
				{
					this->increaseSkill(PRO_LOCKPICKING);
				}
			}
		}

		if ( root && srcStats->type == SHOPKEEPER && srcCrtr )
		{
			ShopkeeperPlayerHostility.onShopkeeperDeath(srcCrtr, srcStats, players[player]->entity);
		}
		if ( player == 0 )
		{
			if ( srcStats->type == LICH )
			{
				kills[LICH] = 1;
			}
			else if ( srcStats->type == LICH_FIRE )
			{
				kills[LICH]++;
			}
			else if ( srcStats->type == LICH_ICE )
			{
				kills[LICH]++;
			}
			else
			{
				kills[srcStats->type]++;
			}
		}
		else if ( multiplayer == SERVER && player > 0 && !players[player]->isLocalPlayer() )
		{
			// inform client of kill
			strcpy((char*)net_packet->data, "MKIL");
			if ( srcStats->type == LICH_FIRE || srcStats->type == LICH_ICE )
			{
				net_packet->data[4] = LICH;
			}
			else
			{
				net_packet->data[4] = srcStats->type;
			}
			net_packet->address.host = net_clients[player - 1].host;
			net_packet->address.port = net_clients[player - 1].port;
			net_packet->len = 5;
			sendPacketSafe(net_sock, -1, net_packet, player - 1);

			// update client attributes
			strcpy((char*)net_packet->data, "ATTR");
			net_packet->data[4] = clientnum;
			net_packet->data[5] = (int8_t)destStats->STR;
			net_packet->data[6] = (int8_t)destStats->DEX;
			net_packet->data[7] = (int8_t)destStats->CON;
			net_packet->data[8] = (int8_t)destStats->INT;
			net_packet->data[9] = (int8_t)destStats->PER;
			net_packet->data[10] = (int8_t)destStats->CHR;
			net_packet->data[11] = (int8_t)destStats->EXP;
			net_packet->data[12] = (int8_t)destStats->LVL;
			SDLNet_Write16((int16_t)destStats->HP, &net_packet->data[13]);
			SDLNet_Write16((int16_t)destStats->MAXHP, &net_packet->data[15]);
			SDLNet_Write16((int16_t)destStats->MP, &net_packet->data[17]);
			SDLNet_Write16((int16_t)destStats->MAXMP, &net_packet->data[19]);
			net_packet->address.host = net_clients[player - 1].host;
			net_packet->address.port = net_clients[player - 1].port;
			net_packet->len = 21;
			sendPacketSafe(net_sock, -1, net_packet, player - 1);
		}
	}
	else
	{
		Creature* leader = nullptr;

		// NPCs with leaders award equal XP to their master (so NPCs don't steal XP gainz)

		if ( (leader = uidToCreature(destStats->leader_uid)) != NULL )
		{
			if ( this->monsterIsTinkeringCreation() )
			{
				if ( local_rng.rand() % 10 == 0 )
				{
					leader->increaseSkill(PRO_LOCKPICKING);
				}
				if ( root && leader->behavior == &actPlayer && srcStats->type == MINOTAUR )
				{
					steamAchievementClient(leader->skill[2], "BARONY_ACH_TIME_TO_PLAN");
				}
			}
			else
			{
				leader->increaseSkill(PRO_LEADERSHIP);
			}
			leader->awardXP(src, true, false);

			if ( leader->behavior == &actPlayer )
			{
				if ( destStats->monsterIsCharmed == 1 )
				{
					// charmed follower killed something.
					steamStatisticUpdateClient(leader->skill[2], STEAM_STAT_KILL_COMMAND, STEAM_STAT_INT, 1);
				}
				if ( destStats->type == INSECTOID )
				{
					if ( leader->getStats()->playerRace == RACE_INSECTOID && leader->getStats()->appearance == 0 )
					{
						steamStatisticUpdateClient(leader->skill[2], STEAM_STAT_MONARCH, STEAM_STAT_INT, 1);
					}
				}
			}
		}
	}

	// restore hit struct
	if ( root )
	{
		hit.entity = tempHit.entity;
		hit.mapx = tempHit.mapx;
		hit.mapy = tempHit.mapy;
		hit.side = tempHit.side;
		hit.x = tempHit.x;
		hit.y = tempHit.y;
	}
}

/*-------------------------------------------------------------------------------

Creature::checkEnemy

Returns true if my and your are enemies, otherwise returns false

-------------------------------------------------------------------------------*/

bool Entity::checkEnemy(Entity* your)
{
	if ( !your )
	{
		return false;
	}

	bool result;

    Creature* yourCrtr = (Creature*)your;
	Stat* myStats = getStats();
	Stat* yourStats = your->getStats();

	if ( !myStats || !yourStats )
	{
		return false;
	}
	if ( everybodyfriendly || intro )   // friendly monsters mode
	{
		return false;
	}

	if ( (yourCrtr && yourCrtr->behavior == &actPlayer || your->behavior == &actPlayerLimb) && (((Creature*)this) && ((Creature*)this)->behavior == &actPlayer || behavior == &actPlayerLimb) )
	{
		return false;
	}

	if ( ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer && yourCrtr && yourCrtr->behavior == &actMonster && yourStats->monsterForceAllegiance != Stat::MONSTER_FORCE_ALLEGIANCE_NONE )
	{
		if ( yourStats->monsterForceAllegiance == Stat::MONSTER_FORCE_PLAYER_ALLY || yourStats->monsterForceAllegiance == Stat::MONSTER_FORCE_PLAYER_RECRUITABLE )
		{
			return false;
		}
		else if ( yourStats->monsterForceAllegiance == Stat::MONSTER_FORCE_PLAYER_ENEMY )
		{
			return true;
		}
	}
	else if ( yourCrtr && yourCrtr->behavior == &actPlayer && ((Creature*)this) && ((Creature*)this)->behavior == &actMonster && myStats->monsterForceAllegiance != Stat::MONSTER_FORCE_ALLEGIANCE_NONE )
	{
		if ( myStats->monsterForceAllegiance == Stat::MONSTER_FORCE_PLAYER_ALLY || myStats->monsterForceAllegiance == Stat::MONSTER_FORCE_PLAYER_RECRUITABLE )
		{
			return false;
		}
		else if ( myStats->monsterForceAllegiance == Stat::MONSTER_FORCE_PLAYER_ENEMY )
		{
			return true;
		}
	}

	if ( myStats->type == GYROBOT )
	{
		return false;
	}

	if ( (myStats->type == SHOPKEEPER && myStats->MISC_FLAGS[STAT_FLAG_MYSTERIOUS_SHOPKEEP] > 0)
		|| (yourStats->type == SHOPKEEPER && yourStats->MISC_FLAGS[STAT_FLAG_MYSTERIOUS_SHOPKEEP] > 0) )
	{
		return false;
	}

	if ( myStats->type == HUMAN && (yourStats->type == AUTOMATON && !strncmp(yourStats->name, "corrupted automaton", 19)) )
	{
		return true;
	}
	else if ( (yourStats->type == HUMAN || (yourCrtr && yourCrtr->behavior == &actPlayer)) && (myStats->type == AUTOMATON && !strncmp(myStats->name, "corrupted automaton", 19)) )
	{
		return true;
	}
	else if ( (yourCrtr && yourCrtr->behavior == &actPlayer) && myStats->type == CREATURE_IMP
		&& (!strncmp(map.name, "Boss", 4) || !strncmp(map.name, "Hell Boss", 9)) )
	{
		if ( this->monsterAllyGetPlayerLeader() )
		{
			return false;
		}
		return true;
	}
	else if ( (yourCrtr && yourCrtr->behavior == &actPlayer) && yourStats->type == CREATURE_IMP
		&& (!strncmp(map.name, "Boss", 4) || !strncmp(map.name, "Hell Boss", 9)) )
	{
		if ( your->monsterAllyGetPlayerLeader() )
		{
			return false;
		}
		return true;
	}
	else if ( (yourCrtr && yourCrtr->behavior == &actPlayer) && myStats->type == VAMPIRE && MonsterData_t::nameMatchesSpecialNPCName(*myStats, "bram kindly") )
	{
		return true;
	}
	else if ( ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer && yourStats->type == VAMPIRE && MonsterData_t::nameMatchesSpecialNPCName(*yourStats, "bram kindly") )
	{
		return true;
	}
	else if ( ((Creature*)this) && ((Creature*)this)->behavior == &actMonster && myStats->type == INCUBUS && !strncmp(myStats->name, "inner demon", strlen("inner demon")) )
	{
		Entity* parentEntity = uidToEntity(this->parent);
		if ( parentEntity != your )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if ( ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer && yourStats->type == INCUBUS && !strncmp(yourStats->name, "inner demon", strlen("inner demon")) )
	{
		Entity* parentEntity = uidToEntity(your->parent);
		if ( parentEntity != this )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if ( ((Creature*)this) && ((Creature*)this)->behavior == &actMonster && (yourCrtr && yourCrtr->behavior == &actMonster) && yourStats->type == INCUBUS && !strncmp(yourStats->name, "inner demon", strlen("inner demon")) )
	{
		Entity* illusionTauntingThisEntity = uidToEntity(static_cast<uint32_t>(((Creature*)your) && ((Creature*)your)->monsterIllusionTauntingThisUid));
		if ( illusionTauntingThisEntity == this )
		{
			return true;
		}
	}

	// if you have a leader, check whether we are enemies instead
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
				return false;
			}
			else
			{
				return checkEnemy(yourLeader);
			}
		}
	}

	// first find out if I have a leader
	Entity* myLeader = NULL;
	if ( myStats->leader_uid )
	{
		myLeader = uidToEntity(myStats->leader_uid);
	}
	if ( myLeader )
	{
		Stat* myLeaderStats = myLeader->getStats();
		if ( myLeaderStats )
		{
			if ( myLeader == your )
			{
				result = false;
			}
			else
			{
				return myLeader->checkEnemy(your);
			}
		}
		else
		{
			// invalid leader, default to allegiance table
			result = swornenemies[myStats->type][yourStats->type];
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
				result = false;
				break;
			}
		}
		if ( !foundFollower )
		{
			// no leader, default to allegiance table
			result = swornenemies[myStats->type][yourStats->type];

			// player exceptions to table go here.
			if ( myStats->type == SHOPKEEPER && ((Creature*)your) && ((Creature*)your)->behavior == &actPlayer )
			{
				result = ShopkeeperPlayerHostility.isPlayerEnemy(your->skill[2]);
			}
			else if ( yourStats->type == SHOPKEEPER && ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer )
			{
				result = ShopkeeperPlayerHostility.isPlayerEnemy(this->skill[2]);
			}
			else if ( ((Creature*)this) && ((Creature*)this)->behavior == &actPlayer && myStats->type != HUMAN )
			{
				result = swornenemies[HUMAN][yourStats->type];
				if ( (yourStats->type == HUMAN || yourStats->type == SHOPKEEPER) && myStats->type != AUTOMATON )
				{
					// enemies.
					result = true;
				}
				else
				{
					switch ( myStats->type )
					{
						case SKELETON:
							if ( yourStats->type == GHOUL )
							{
								result = false;
							}
							break;
						case RAT:
							if ( yourStats->type == RAT )
							{
								result = false;
							}
							break;
						case SPIDER:
							if ( yourStats->type == SPIDER
								|| yourStats->type == SCARAB || yourStats->type == SCORPION )
							{
								result = false;
							}
							break;
						case TROLL:
							if ( yourStats->type == TROLL )
							{
								result = false;
							}
							break;
						case CREATURE_IMP:
							if ( yourStats->type == CREATURE_IMP )
							{
								result = false;
							}
							break;
						case GOBLIN:
							if ( yourStats->type == GOBLIN )
							{
								result = false;
							}
							break;
						case GOATMAN:
							if ( yourStats->type == GOATMAN )
							{
								result = false;
							}
							break;
						case INCUBUS:
						case SUCCUBUS:
							if ( yourStats->type == SUCCUBUS || yourStats->type == INCUBUS )
							{
								result = false;
							}
							break;
						case INSECTOID:
							if ( yourStats->type == SCARAB || yourStats->type == INSECTOID || yourStats->type == SCORPION )
							{
								result = false;
							}
							break;
						case VAMPIRE:
							if ( yourStats->type == VAMPIRE )
							{
								result = false;
							}
							break;
						case AUTOMATON:
							if ( yourStats->type == INCUBUS || yourStats->type == SUCCUBUS )
							{
								result = false;
							}
							break;
						default:
							break;
					}
				}
			}
			else if ( ((Creature*)this) && ((Creature*)this)->behavior == &actMonster && ((Creature*)your) && ((Creature*)your)->behavior == &actPlayer && yourStats->type != HUMAN )
			{
				result = swornenemies[myStats->type][HUMAN];
				if ( (myStats->type == HUMAN || myStats->type == SHOPKEEPER) && yourStats->type != AUTOMATON )
				{
					// enemies.
					result = true;
				}
				else
				{
					switch ( yourStats->type )
					{
						case SKELETON:
							if ( myStats->type == GHOUL )
							{
								result = false;
							}
							break;
						case RAT:
							if ( myStats->type == RAT )
							{
								result = false;
							}
							break;
						case SPIDER:
							if ( myStats->type == SPIDER
								|| myStats->type == SCARAB || myStats->type == SCORPION )
							{
								result = false;
							}
							break;
						case TROLL:
							if ( myStats->type == TROLL )
							{
								result = false;
							}
							break;
						case CREATURE_IMP:
							if ( myStats->type == CREATURE_IMP )
							{
								result = false;
							}
							break;
						case GOBLIN:
							if ( myStats->type == GOBLIN )
							{
								result = false;
							}
							break;
						case GOATMAN:
							if ( myStats->type == GOATMAN )
							{
								result = false;
							}
							break;
						case INCUBUS:
						case SUCCUBUS:
							if ( myStats->type == SUCCUBUS || myStats->type == INCUBUS )
							{
								result = false;
							}
							break;
						case INSECTOID:
							if ( myStats->type == SCARAB
								|| myStats->type == INSECTOID || myStats->type == SCORPION )
							{
								result = false;
							}
							break;
						case VAMPIRE:
							if ( myStats->type == VAMPIRE )
							{
								result = false;
							}
							break;
						case AUTOMATON:
							if ( myStats->type == INCUBUS || myStats->type == SUCCUBUS )
							{
								result = false;
							}
							break;
						default:
							break;
					}
				}
			}
		}
	}

	// confused monsters mistake their allegiances
	if ( myStats->EFFECTS[EFF_CONFUSED] )
	{
		if ( myStats->type == AUTOMATON && yourStats->type == AUTOMATON
			&& !strncmp(myStats->name, "corrupted automaton", strlen("corrupted automaton")) )
		{
			// these guys ignore themselves when confused..
		}
		else
		{
			result = (result == false);
		}
	}

	return result;
}

void Entity::handleHumanoidWeaponLimb(Entity* weaponLimb, Entity* weaponArmLimb)
{
	if ( weaponLimb == nullptr || weaponArmLimb == nullptr )
	{
		return;
	}

	int monsterType = this->getMonsterTypeFromSprite();
	int myAttack = this->monsterAttack;
	bool isPlayer = (Creature*)this && ((Creature*)this)->behavior == &actPlayer;
	if ( isPlayer )
	{
		myAttack = this->skill[9];
	}

	if ( weaponLimb->flags[INVISIBLE] == false ) //TODO: isInvisible()?
	{
		if ( weaponLimb->sprite == items[SHORTBOW].index )
		{
			weaponLimb->x = weaponArmLimb->x - .5 * cos(weaponArmLimb->yaw);
			weaponLimb->y = weaponArmLimb->y - .5 * sin(weaponArmLimb->yaw);
			weaponLimb->z = weaponArmLimb->z + 1;
			weaponLimb->pitch = weaponArmLimb->pitch + .25;
		}
		else if ( weaponLimb->sprite == items[ARTIFACT_BOW].index
		          || weaponLimb->sprite == items[LONGBOW].index
		          || weaponLimb->sprite == items[COMPOUND_BOW].index )
		{
			if ( isPlayer && monsterType == HUMAN )
			{
				weaponLimb->x = weaponArmLimb->x - .5 * cos(weaponArmLimb->yaw);
				weaponLimb->y = weaponArmLimb->y - .5 * sin(weaponArmLimb->yaw);
				weaponLimb->z = weaponArmLimb->z + 1;
				weaponLimb->pitch = weaponArmLimb->pitch + .25;
			}
			else
			{
				weaponLimb->x = weaponArmLimb->x - .5 * cos(weaponArmLimb->yaw);
				weaponLimb->y = weaponArmLimb->y - .5 * sin(weaponArmLimb->yaw);
				weaponLimb->z = weaponArmLimb->z + 1;
				weaponLimb->pitch = weaponArmLimb->pitch + .25;
			}

			if ( weaponLimb->sprite == items[LONGBOW].index )
			{
				weaponLimb->x -= .5 * cos(weaponArmLimb->yaw);
				weaponLimb->y -= .5 * sin(weaponArmLimb->yaw);
			}
			else if ( weaponLimb->sprite == items[COMPOUND_BOW].index )
			{
				weaponLimb->x += .5 * cos(weaponArmLimb->yaw);
				weaponLimb->y += .5 * sin(weaponArmLimb->yaw);
			}
		}
		else if ( weaponLimb->sprite == items[CROSSBOW].index || weaponLimb->sprite == items[HEAVY_CROSSBOW].index )
		{
			weaponLimb->x = weaponArmLimb->x;
			weaponLimb->y = weaponArmLimb->y;
			weaponLimb->z = weaponArmLimb->z + 1;
			weaponLimb->pitch = weaponArmLimb->pitch;
		}
		else if ( weaponLimb->sprite == items[TOOL_LOCKPICK].index )
		{
			weaponLimb->x = weaponArmLimb->x + 1.5 * cos(weaponArmLimb->yaw);
			weaponLimb->y = weaponArmLimb->y + 1.5 * sin(weaponArmLimb->yaw);
			weaponLimb->z = weaponArmLimb->z + 1.5;
			weaponLimb->pitch = weaponArmLimb->pitch + .25;
		}
		else
		{
			/*weaponLimb->focalx = limbs[monsterType][6][0];
			weaponLimb->focalz = limbs[monsterType][6][2];*/
			if ( myAttack == 3 && !isPlayer )
			{
				// poking animation, weapon pointing straight ahead.
				if ( weaponArmLimb->skill[1] < 2 && weaponArmLimb->pitch < PI / 2 )
				{
					// cos(weaponArmLimb->pitch)) * cos(weaponArmLimb->yaw) allows forward/back motion dependent on the arm rotation.
					weaponLimb->x = weaponArmLimb->x + (3 * cos(weaponArmLimb->pitch)) * cos(weaponArmLimb->yaw);
					weaponLimb->y = weaponArmLimb->y + (3 * cos(weaponArmLimb->pitch)) * sin(weaponArmLimb->yaw);

					if ( weaponArmLimb->pitch < PI / 3 )
					{
						// adjust the z point halfway through swing.
						weaponLimb->z = weaponArmLimb->z + 1.5 - 2 * cos(weaponArmLimb->pitch / 2);
						if ( monsterType == INCUBUS || monsterType == SUCCUBUS )
						{
							weaponLimb->z += 2;
						}
					}
					else
					{
						weaponLimb->z = weaponArmLimb->z - .5 * (myAttack == 0);
						if ( weaponLimb->pitch > PI / 2 )
						{
							limbAnimateToLimit(weaponLimb, ANIMATE_PITCH, -0.5, PI * 0.5, false, 0);
						}
						else
						{
							limbAnimateToLimit(weaponLimb, ANIMATE_PITCH, 0.5, PI * 0.5, false, 0);
						}
						if ( monsterType == INCUBUS || monsterType == SUCCUBUS )
						{
							weaponLimb->z += 1.25;
						}
					}
				}
					// hold sword with pitch aligned to arm rotation.
				else
				{
					weaponLimb->x = weaponArmLimb->x + .5 * cos(weaponArmLimb->yaw) * (myAttack == 0);
					weaponLimb->y = weaponArmLimb->y + .5 * sin(weaponArmLimb->yaw) * (myAttack == 0);
					weaponLimb->z = weaponArmLimb->z - .5;
					weaponLimb->pitch = weaponArmLimb->pitch + .25 * (myAttack == 0);
					if ( monsterType == INCUBUS || monsterType == SUCCUBUS )
					{
						weaponLimb->z += 1;
					}
				}
			}
			else
			{
				weaponLimb->x = weaponArmLimb->x + .5 * cos(weaponArmLimb->yaw) * (myAttack == 0);
				weaponLimb->y = weaponArmLimb->y + .5 * sin(weaponArmLimb->yaw) * (myAttack == 0);
				weaponLimb->z = weaponArmLimb->z - .5 * (myAttack == 0);
				weaponLimb->pitch = weaponArmLimb->pitch + .25 * (myAttack == 0);
			}
		}
	}

	weaponLimb->yaw = weaponArmLimb->yaw;
	bool isPotion = false;
	if ( myAttack == MONSTER_POSE_RANGED_WINDUP3 && monsterType == GOATMAN && !isPlayer )
	{
		// specific for potion throwing goatmen.
		limbAnimateToLimit(weaponLimb, ANIMATE_ROLL, 0.25, 1 * PI / 4, false, 0.0);
	}
	else
	{
		weaponLimb->roll = weaponArmLimb->roll;
		if ( isPlayer )
		{
			if ( (weaponLimb->sprite >= 50 && weaponLimb->sprite < 58)
			     || weaponLimb->sprite == 795 )
			{
				weaponLimb->roll += (PI / 2); // potion sprites rotated
				isPotion = true;
			}
			else if ( weaponLimb->sprite == items[BOOMERANG].index )
			{
				weaponLimb->roll += (PI / 2); // sprite rotated
				weaponLimb->pitch -= PI / 8;
				weaponLimb->pitch += .25 * (myAttack != 0); // add 0.25 if attacking
			}
			else if ( weaponLimb->sprite == items[FOOD_CREAMPIE].index )
			{
				weaponLimb->roll += (PI / 2); // sprite rotated
			}
		}
	}

	bool armBended = (!isPlayer && this->monsterArmbended) || (isPlayer && this->skill[11]);
	weaponLimb->scalex = 1.f;
	weaponLimb->scaley = 1.f;
	weaponLimb->scalez = 1.f;
	if ( weaponLimb->sprite == items[TOOL_WHIP].index || weaponLimb->sprite == items[TOOL_WHIP].index + 1 )
	{
		if ( myAttack != 2 )
		{
			weaponLimb->pitch -= PI / 8;
			if ( weaponLimb->sprite == items[TOOL_WHIP].index + 1 )
			{
				weaponLimb->pitch -= PI / 8;
			}
		}
		if ( myAttack == 1 )
		{
			if ( weaponArmLimb->skill[1] == 1 && armBended )
			{
				if ( weaponArmLimb->pitch >= 3 * PI / 2 )
				{
					if ( weaponLimb->sprite == items[TOOL_WHIP].index )
					{
						weaponLimb->sprite += 1;
					}
				}
				else if ( weaponArmLimb->pitch >= PI / 10 )
				{
					if ( weaponLimb->sprite == items[TOOL_WHIP].index + 1 )
					{
						weaponLimb->sprite = items[TOOL_WHIP].index;
					}
				}
			}
			else
			{
				weaponLimb->sprite = items[TOOL_WHIP].index;
			}
		}
		else
		{
			weaponLimb->sprite = items[TOOL_WHIP].index;
		}
	}
	else if ( weaponLimb->sprite == items[TOOL_DECOY].index || weaponLimb->sprite == items[TOOL_DUMMYBOT].index )
	{
		weaponLimb->scalex = 0.8;
		weaponLimb->scaley = 0.8;
		weaponLimb->scalez = 0.8;
	}

	if ( isPlayer && monsterType == CREATURE_IMP )
	{
		weaponLimb->focalx = limbs[monsterType][9][0];
		weaponLimb->focaly = limbs[monsterType][9][1];
		weaponLimb->focalz = limbs[monsterType][9][2];
		weaponLimb->pitch += .5 + limbs[monsterType][10][0];
	}
	else if ( !armBended )
	{
		weaponLimb->focalx = limbs[monsterType][6][0]; // 2.5
		weaponLimb->focaly = limbs[monsterType][6][1]; // 0
		if ( weaponLimb->sprite == items[CROSSBOW].index || weaponLimb->sprite == items[HEAVY_CROSSBOW].index )
		{
			weaponLimb->focalx += 2.1;
			weaponLimb->focaly -= 0.1;
		}
		weaponLimb->focalz = limbs[monsterType][6][2]; // -.5
		if ( isPlayer && isPotion )
		{
			weaponLimb->focalz += 1;
			if ( monsterType == INCUBUS || monsterType == SUCCUBUS )
			{
				weaponLimb->focaly += 1;
				weaponLimb->focalz -= 1.5;
			}
		}
		else if ( weaponLimb->sprite == items[BOOMERANG].index )
		{
			weaponLimb->focalx += 2;
			weaponLimb->focaly += 0.25;
			weaponLimb->focalz += 0;
			weaponLimb->x += -1.2 * cos(weaponArmLimb->yaw + PI / 2) + -.6 * cos(weaponArmLimb->yaw);
			weaponLimb->y += -1.2 * sin(weaponArmLimb->yaw + PI / 2) + -.6 * sin(weaponArmLimb->yaw);
			weaponLimb->z += 0.25;
			switch ( monsterType )
			{
				case SKELETON:
				case AUTOMATON:
				case GOATMAN:
				case INSECTOID:
				case GOBLIN:
					weaponLimb->x += 0.5 * cos(weaponArmLimb->yaw + PI / 2);
					weaponLimb->y += 0.5 * sin(weaponArmLimb->yaw + PI / 2);
					break;
				case INCUBUS:
				case SUCCUBUS:
					weaponLimb->x += 1.75 * cos(weaponArmLimb->yaw + PI / 2) + 0.25 * cos(weaponArmLimb->yaw);
					weaponLimb->y += 1.75 * sin(weaponArmLimb->yaw + PI / 2) + 0.25 * sin(weaponArmLimb->yaw);
					weaponLimb->z += 0;

					weaponLimb->focalx += -0.75;
					weaponLimb->focaly += 1;
					weaponLimb->focalz += 0;
					break;
				default:
					break;
			}
		}
		else if ( weaponLimb->sprite == items[TOOL_WHIP].index || weaponLimb->sprite == items[TOOL_WHIP].index + 1 )
		{
			weaponLimb->focalx += 1;
			weaponLimb->focalz += 1.5;
		}
		else if ( weaponLimb->sprite == items[TOOL_GYROBOT].index )
		{
			weaponLimb->focalz += 1;
		}
		else if ( weaponLimb->sprite >= items[TOOL_BOMB].index && weaponLimb->sprite <= items[TOOL_TELEPORT_BOMB].index )
		{
			weaponLimb->focalz += 2;
			if ( monsterType == SKELETON )
			{
				weaponLimb->focalx -= 0.25;
				weaponLimb->focaly += 0.1;
			}
			else if ( monsterType == AUTOMATON )
			{
				weaponLimb->focaly += 0.5;
				weaponLimb->focalz -= 1;
			}
		}
		else if ( weaponLimb->sprite == items[SHORTBOW].index || weaponLimb->sprite == items[ARTIFACT_BOW].index
		          || weaponLimb->sprite == items[LONGBOW].index || weaponLimb->sprite == items[COMPOUND_BOW].index )
		{
			if ( weaponLimb->sprite == items[SHORTBOW].index )
			{
				switch ( monsterType )
				{
					case HUMAN:
					case VAMPIRE:
					case SHOPKEEPER:
						weaponLimb->x += -.1 * cos(weaponArmLimb->yaw + PI / 2) + 0.25 * cos(weaponArmLimb->yaw);
						weaponLimb->y += -.1 * sin(weaponArmLimb->yaw + PI / 2) + 0.25 * sin(weaponArmLimb->yaw);
						weaponLimb->z += -1.25;
						weaponLimb->focalx += -0.5;
						weaponLimb->focaly += 0;
						weaponLimb->focalz += 1.75;
						break;
					case GOBLIN:
					case GOATMAN:
					case INSECTOID:
					case SUCCUBUS:
					case INCUBUS:
						weaponLimb->x += -.1 * cos(weaponArmLimb->yaw + PI / 2) + 0.25 * cos(weaponArmLimb->yaw);
						weaponLimb->y += -.1 * sin(weaponArmLimb->yaw + PI / 2) + 0.25 * sin(weaponArmLimb->yaw);
						weaponLimb->z += -1;
						weaponLimb->focalx += 0;
						weaponLimb->focaly += 0;
						weaponLimb->focalz += 1.25;
						break;
					case SKELETON:
					case AUTOMATON:
						weaponLimb->x += -.1 * cos(weaponArmLimb->yaw + PI / 2) + 0.5 * cos(weaponArmLimb->yaw);
						weaponLimb->y += -.1 * sin(weaponArmLimb->yaw + PI / 2) + 0.5 * sin(weaponArmLimb->yaw);
						weaponLimb->z += -1;
						weaponLimb->focalx += -0.5;
						weaponLimb->focaly += 0;
						weaponLimb->focalz += 1;
						break;
					default:
						break;
				}
			}
			else if ( weaponLimb->sprite == items[ARTIFACT_BOW].index
			          || weaponLimb->sprite == items[LONGBOW].index )
			{
				switch ( monsterType )
				{
					case HUMAN:
					case VAMPIRE:
					case SHOPKEEPER:
						weaponLimb->x += -.1 * cos(weaponArmLimb->yaw + PI / 2) + 0.75 * cos(weaponArmLimb->yaw);
						weaponLimb->y += -.1 * sin(weaponArmLimb->yaw + PI / 2) + 0.75 * sin(weaponArmLimb->yaw);
						weaponLimb->z += -1.25;
						weaponLimb->focalx += -0.5;
						weaponLimb->focaly += 0;
						weaponLimb->focalz += 1.75;
						if ( weaponLimb->sprite == items[LONGBOW].index )
						{
							weaponLimb->x += -0.25 * cos(weaponArmLimb->yaw);
							weaponLimb->y += -0.25 * sin(weaponArmLimb->yaw);
							weaponLimb->z += 0.25;
							weaponLimb->focalx += 0.5;
							weaponLimb->focaly += 0;
							weaponLimb->focalz += -0.5;
						}
						break;
					case GOBLIN:
					case GOATMAN:
					case INSECTOID:
						weaponLimb->x += -.1 * cos(weaponArmLimb->yaw + PI / 2) + 0.5 * cos(weaponArmLimb->yaw);
						weaponLimb->y += -.1 * sin(weaponArmLimb->yaw + PI / 2) + 0.5 * sin(weaponArmLimb->yaw);
						weaponLimb->z += -1;
						weaponLimb->focalx += 0;
						weaponLimb->focaly += 0;
						weaponLimb->focalz += 1.25;
						break;
					case SUCCUBUS:
					case INCUBUS:
						weaponLimb->x += -.1 * cos(weaponArmLimb->yaw + PI / 2) + 0.25 * cos(weaponArmLimb->yaw);
						weaponLimb->y += -.1 * sin(weaponArmLimb->yaw + PI / 2) + 0.25 * sin(weaponArmLimb->yaw);
						weaponLimb->z += -1;
						weaponLimb->focalx += 0;
						weaponLimb->focaly += 0;
						weaponLimb->focalz += 1.25;
						break;
					case SKELETON:
					case AUTOMATON:
						weaponLimb->x += -.1 * cos(weaponArmLimb->yaw + PI / 2) + 0.25 * cos(weaponArmLimb->yaw);
						weaponLimb->y += -.1 * sin(weaponArmLimb->yaw + PI / 2) + 0.25 * sin(weaponArmLimb->yaw);
						weaponLimb->z += -1.25;
						weaponLimb->focalx += 0;
						weaponLimb->focaly += 0;
						weaponLimb->focalz += 1.25;
						break;
					default:
						break;
				}
				if ( weaponLimb->sprite == items[LONGBOW].index )
				{
					// this applies to all offsets for all monsters.
					weaponLimb->x += -.1 * cos(weaponArmLimb->yaw + PI / 2) + 0.75 * cos(weaponArmLimb->yaw);
					weaponLimb->y += -.1 * sin(weaponArmLimb->yaw + PI / 2) + 0.75 * sin(weaponArmLimb->yaw);
					weaponLimb->z += 0.25;
					weaponLimb->focalx += -.75;
					weaponLimb->focaly += 0;
					weaponLimb->focalz += 0;
				}
			}
			else if ( weaponLimb->sprite == items[COMPOUND_BOW].index )
			{
				switch ( monsterType )
				{
					case HUMAN:
					case VAMPIRE:
					case SHOPKEEPER:
						weaponLimb->x += -.1 * cos(weaponArmLimb->yaw + PI / 2) + 0.f * cos(weaponArmLimb->yaw);
						weaponLimb->y += -.1 * sin(weaponArmLimb->yaw + PI / 2) + 0.f * sin(weaponArmLimb->yaw);
						weaponLimb->z += -1.25;
						weaponLimb->focalx += 0;
						weaponLimb->focaly += 0;
						weaponLimb->focalz += 1.75;
						break;
					case GOBLIN:
					case GOATMAN:
					case INSECTOID:
						weaponLimb->x += -.1 * cos(weaponArmLimb->yaw + PI / 2) + 0.5 * cos(weaponArmLimb->yaw);
						weaponLimb->y += -.1 * sin(weaponArmLimb->yaw + PI / 2) + 0.5 * sin(weaponArmLimb->yaw);
						weaponLimb->z += -1;
						weaponLimb->focalx += 0;
						weaponLimb->focaly += 0;
						weaponLimb->focalz += 1.25;
						break;
					case SUCCUBUS:
					case INCUBUS:
						weaponLimb->x += -.1 * cos(weaponArmLimb->yaw + PI / 2) + 0.25 * cos(weaponArmLimb->yaw);
						weaponLimb->y += -.1 * sin(weaponArmLimb->yaw + PI / 2) + 0.25 * sin(weaponArmLimb->yaw);
						weaponLimb->z += -1;
						weaponLimb->focalx += 0;
						weaponLimb->focaly += 0;
						weaponLimb->focalz += 1.25;
						break;
					case SKELETON:
					case AUTOMATON:
						weaponLimb->x += -.1 * cos(weaponArmLimb->yaw + PI / 2) + 0.25 * cos(weaponArmLimb->yaw);
						weaponLimb->y += -.1 * sin(weaponArmLimb->yaw + PI / 2) + 0.25 * sin(weaponArmLimb->yaw);
						weaponLimb->z += -1.25;
						weaponLimb->focalx += 0;
						weaponLimb->focaly += 0;
						weaponLimb->focalz += 1.25;
						break;
					default:
						break;
				}
			}
			/*weaponLimb->x += limbs[HUMAN][12][0] * cos(weaponArmLimb->yaw + PI / 2) + limbs[HUMAN][12][1] * cos(weaponArmLimb->yaw);
			weaponLimb->y += limbs[HUMAN][12][0] * sin(weaponArmLimb->yaw + PI / 2) + limbs[HUMAN][12][1] * sin(weaponArmLimb->yaw);
			weaponLimb->z += limbs[HUMAN][12][2];
			weaponLimb->focalx += limbs[HUMAN][11][0];
			weaponLimb->focaly += limbs[HUMAN][11][1];
			weaponLimb->focalz += limbs[HUMAN][11][2];*/
		}
		else
		{
			switch ( monsterType )
			{
				case SUCCUBUS:
				case INCUBUS:
				case HUMAN:
				case VAMPIRE:
				case AUTOMATON:
				case INSECTOID:
				case GOBLIN:
					weaponLimb->focaly -= 0.05; // minor z-fighting fix.
					break;
				default:
					break;
			}
		}
	}
	else
	{
		if ( monsterType == INCUBUS || monsterType == SUCCUBUS )
		{
			weaponLimb->focalx = limbs[monsterType][6][0] + 2; // 3.5
			weaponLimb->focaly = limbs[monsterType][6][1]; // 0
			weaponLimb->focalz = limbs[monsterType][6][2] - 3.5; // -2.5
			if ( isPlayer && isPotion )
			{
				weaponLimb->focalz += 4.5;
			}
		}
		else if ( isPlayer && monsterType == HUMAN )
		{
			weaponLimb->focalx = limbs[monsterType][6][0] + 1.5;
			weaponLimb->focaly = limbs[monsterType][6][1];
			weaponLimb->focalz = limbs[monsterType][6][2] - 2;
			if ( isPlayer && isPotion )
			{
				weaponLimb->focalz += 3;
			}
		}
		else
		{
			weaponLimb->focalx = limbs[monsterType][6][0] + 1; // 3.5
			weaponLimb->focaly = limbs[monsterType][6][1]; // 0
			weaponLimb->focalz = limbs[monsterType][6][2] - 2; // -2.5
			if ( isPlayer && isPotion )
			{
				weaponLimb->focalz += 3;
			}
		}

		if ( weaponLimb->sprite == items[BOOMERANG].index )
		{
			weaponLimb->focalx += 2;
			weaponLimb->focaly += 0.25;
			weaponLimb->focalz += 2;
			weaponLimb->x += -1.2 * cos(weaponArmLimb->yaw + PI / 2) + -.1 * cos(weaponArmLimb->yaw);
			weaponLimb->y += -1.2 * sin(weaponArmLimb->yaw + PI / 2) + -.1 * sin(weaponArmLimb->yaw);
			weaponLimb->z += 0.25;
			switch ( monsterType )
			{
				case SKELETON:
				case AUTOMATON:
				case GOATMAN:
				case INSECTOID:
				case GOBLIN:
					weaponLimb->x += 0.5 * cos(weaponArmLimb->yaw + PI / 2);
					weaponLimb->y += 0.5 * sin(weaponArmLimb->yaw + PI / 2);
					break;
				case INCUBUS:
				case SUCCUBUS:
					weaponLimb->x += 1.75 * cos(weaponArmLimb->yaw + PI / 2) + 0.25 * cos(weaponArmLimb->yaw);
					weaponLimb->y += 1.75 * sin(weaponArmLimb->yaw + PI / 2) + 0.25 * sin(weaponArmLimb->yaw);
					weaponLimb->z += 0;

					weaponLimb->focalx += -0.75;
					weaponLimb->focaly += 1;
					weaponLimb->focalz += 1.5;
					break;
				default:
					break;
			}
		}
		else if ( weaponLimb->sprite == items[TOOL_WHIP].index + 1 )
		{
			weaponLimb->focalx += 5.5;
			weaponLimb->focalz += 3.5;
		}
		else if ( weaponLimb->sprite == items[TOOL_WHIP].index )
		{
			weaponLimb->focalx += 1.5;
			weaponLimb->focalz += 2.5;
		}
		else if ( weaponLimb->sprite == items[TOOL_GYROBOT].index )
		{
			weaponLimb->focalz += 1;
		}
		else if ( weaponLimb->sprite >= items[TOOL_BOMB].index && weaponLimb->sprite <= items[TOOL_TELEPORT_BOMB].index )
		{
			weaponLimb->focalz += 2;
		}

		weaponLimb->yaw -= sin(weaponArmLimb->roll) * PI / 2;
		weaponLimb->pitch += cos(weaponArmLimb->roll) * PI / 2;
	}

	return;
}

int32_t Entity::playerInsectoidHungerValueOfManaPoint(Stat& myStats)
{
	float manaPointPercentage = 1 / static_cast<float>(myStats.MAXMP);
	return static_cast<int32_t>(1000 * manaPointPercentage);
}

void Entity::setHelmetLimbOffsetWithMask(Entity* helm, Entity* mask)
{
	if ( !helm || !mask )
	{
		return;
	}

	if ( !mask->flags[INVISIBLE] && !helm->flags[INVISIBLE] )
	{
		helm->scalex = 1.01;
		helm->scaley = 1.01;
		helm->scalez = 1.01;
	}
	else
	{
		mask->scalex = 1.01;
		mask->scaley = 1.01;
		mask->scalez = 1.01;
		return;
	}

	if ( helm->sprite == items[LEATHER_HELM].index
		|| helm->sprite == items[IRON_HELM].index
		|| (helm->sprite >= items[HAT_HOOD].index && helm->sprite < items[HAT_HOOD].index + items[HAT_HOOD].variations)
		|| helm->sprite == items[HAT_HOOD_RED].index
		|| helm->sprite == items[HAT_HOOD_SILVER].index
		|| helm->sprite == items[PUNISHER_HOOD].index )
	{
		helm->scalex = 1.05;
		helm->scaley = 1.05;
		helm->scalez = 1.05;
		if ( helm->sprite == items[PUNISHER_HOOD].index )
		{
			/*helm->scalex += limbs[HUMAN][11][0];
			helm->scaley += limbs[HUMAN][11][1];
			helm->scalez += limbs[HUMAN][11][2];*/
		}
	}

	mask->scalex = 1.01;
	mask->scaley = 1.01;
	mask->scalez = 1.01;

	int monster = getMonsterTypeFromSprite();
	switch ( monster )
	{
		case HUMAN:
		case VAMPIRE:
		case SHOPKEEPER:
			if ( helm->sprite == items[LEATHER_HELM].index )
			{
				helm->focalz -= 0.2;
			}
			break;
		case GOBLIN:
			if ( helm->sprite == items[LEATHER_HELM].index
				|| helm->sprite == items[IRON_HELM].index )
			{
				helm->focalz -= 0.2;
			}
			else if ( helm->sprite == (items[HAT_HOOD].index + 2) )
			{
				// black hood
				helm->focalx += 0.25;
			}
			break;
		case GOATMAN:
			if ( helm->sprite == items[LEATHER_HELM].index
				|| helm->sprite == items[IRON_HELM].index )
			{
				helm->focalz -= 0.25;
			}
			else if ( helm->sprite == items[PUNISHER_HOOD].index )
			{
				helm->scaley += 0.05;
			}
			break;
		case INCUBUS:
			if ( helm->sprite == items[LEATHER_HELM].index
				|| helm->sprite == items[IRON_HELM].index )
			{
				helm->focalx += 0.4;
				helm->focalz -= 0.2;
			}
			else if ( helm->sprite == items[PUNISHER_HOOD].index )
			{
				mask->focalx -= 0.1;
			}
			break;
		case SUCCUBUS:
			if ( helm->sprite == items[LEATHER_HELM].index
				|| helm->sprite == items[IRON_HELM].index )
			{
				helm->focalz -= 0.2;
			}
			break;
		case AUTOMATON:
			if ( helm->sprite == items[LEATHER_HELM].index
				|| helm->sprite == items[IRON_HELM].index )
			{
				mask->focalx -= 0.2;
			}
			else if ( helm->sprite == items[PUNISHER_HOOD].index )
			{
				mask->focalx -= 0.2;
			}
			break;
		case INSECTOID:
			if ( helm->sprite == items[LEATHER_HELM].index )
			{
				helm->focalz -= 0.2;
			}
			else if ( helm->sprite == (items[HAT_HOOD].index + 2) )
			{
				// black hood
				helm->focalx += 0.25;
				helm->focaly += 0.06;
			}
			else if ( helm->sprite == items[PUNISHER_HOOD].index )
			{
				mask->focalx -= 0.2;
			}
			break;
		case SKELETON:
			break;
		default:
			break;
	}
}

/*-------------------------------------------------------------------------------

setHelmetLimbOffset
Adjusts helmet offsets for all monsters, depending on the type of headwear.

-------------------------------------------------------------------------------*/

void Entity::setHelmetLimbOffset(Entity* helm)
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

void Entity::setHumanoidLimbOffset(Entity* limb, Monster race, int limbType)
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

/*void Creature::returnWeaponarmToNeutral(Entity* weaponarm, Entity* rightbody)
{
weaponarm->skill[0] = rightbody->skill[0];
monsterWeaponYaw = 0;
weaponarm->pitch = rightbody->pitch;
weaponarm->roll = 0;
monsterArmbended = 0;
monsterAttack = 0;
}*/

void Entity::handleWeaponArmAttack(Entity* weaponarm)
{
	if ( weaponarm == nullptr )
	{
		return;
	}

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

	// vertical chop windup
	if ( monsterAttack == MONSTER_POSE_MELEE_WINDUP1 )
	{
		if ( monsterAttackTime == 0 )
		{
			// init rotations
			weaponarm->pitch = 0;
			this->monsterArmbended = 0;
			this->monsterWeaponYaw = 0;
			weaponarm->roll = 0;
			weaponarm->skill[1] = 0;
		}

		limbAnimateToLimit(weaponarm, ANIMATE_PITCH, -0.25, 5 * PI / 4, false, 0.0);

		if ( monsterAttackTime >= ANIMATE_DURATION_WINDUP / (monsterGlobalAnimationMultiplier / 10.0) )
		{
			if ( multiplayer != CLIENT && ((Creature*)this) )
			{
                ((Creature*)this)->attack(1, 0, nullptr);
			}
		}
	}
	// vertical chop attack
	else if ( monsterAttack == 1 )
	{
		if ( weaponarm->pitch >= 3 * PI / 2 )
		{
			this->monsterArmbended = 1;
		}

		if ( weaponarm->skill[1] == 0 )
		{
			// chop forwards
			if ( limbAnimateToLimit(weaponarm, ANIMATE_PITCH, 0.4, PI / 3, false, 0.0) )
			{
				weaponarm->skill[1] = 1;
			}
		}
		else if ( weaponarm->skill[1] >= 1 )
		{
			if ( limbAnimateToLimit(weaponarm, ANIMATE_PITCH, -0.25, 7 * PI / 4, false, 0.0) )
			{
				weaponarm->skill[0] = rightbody->skill[0];
				this->monsterWeaponYaw = 0;
				weaponarm->pitch = rightbody->pitch;
				weaponarm->roll = 0;
				this->monsterArmbended = 0;
				monsterAttack = 0;
				//returnWeaponarmToNeutral(weaponarm, rightbody);
			}
		}
	}
	// horizontal chop windup
	else if ( monsterAttack == MONSTER_POSE_MELEE_WINDUP2 )
	{
		if ( monsterAttackTime == 0 )
		{
			// init rotations
			weaponarm->pitch = PI / 4;
			weaponarm->roll = 0;
			this->monsterArmbended = 1;
			weaponarm->skill[1] = 0;
			this->monsterWeaponYaw = 6 * PI / 4;
		}

		limbAnimateToLimit(weaponarm, ANIMATE_ROLL, -0.2, 3 * PI / 2, false, 0.0);
		limbAnimateToLimit(weaponarm, ANIMATE_PITCH, -0.2, 0, false, 0.0);


		if ( monsterAttackTime >= ANIMATE_DURATION_WINDUP / (monsterGlobalAnimationMultiplier / 10.0) )
		{
            if ( multiplayer != CLIENT && ((Creature*)this) )
            {
                ((Creature*)this)->attack(2, 0, nullptr);
            }
		}
	}
	// horizontal chop attack
	else if ( monsterAttack == 2 )
	{
		if ( weaponarm->skill[1] == 0 )
		{
			// swing
			// this->weaponyaw is OK to change for clients, as server doesn't update it for them.
			if ( limbAnimateToLimit(this, ANIMATE_WEAPON_YAW, 0.3, 2 * PI / 8, false, 0.0) )
			{
				weaponarm->skill[1] = 1;
			}
		}
		else if ( weaponarm->skill[1] >= 1 )
		{
			// post-swing return to normal weapon yaw
			if ( limbAnimateToLimit(this, ANIMATE_WEAPON_YAW, -0.5, 0, false, 0.0) )
			{
				// restore pitch and roll after yaw is set
				if ( limbAnimateToLimit(weaponarm, ANIMATE_ROLL, 0.4, 0, false, 0.0)
					&& limbAnimateToLimit(weaponarm, ANIMATE_PITCH, -0.4, 7 * PI / 4, false, 0.0) )
				{
					weaponarm->skill[0] = rightbody->skill[0];
					this->monsterWeaponYaw = 0;
					weaponarm->pitch = rightbody->pitch;
					weaponarm->roll = 0;
					this->monsterArmbended = 0;
					monsterAttack = 0;
				}
			}
		}
	}
	// stab windup
	else if ( monsterAttack == MONSTER_POSE_MELEE_WINDUP3 )
	{
		if ( monsterAttackTime == 0 )
		{
			// init rotations
			this->monsterArmbended = 0;
			this->monsterWeaponYaw = 0;
			weaponarm->roll = 0;
			weaponarm->pitch = 0;
			weaponarm->skill[1] = 0;
		}

		limbAnimateToLimit(weaponarm, ANIMATE_PITCH, 0.5, 2 * PI / 3, false, 0.0);

		if ( monsterAttackTime >= ANIMATE_DURATION_WINDUP / (monsterGlobalAnimationMultiplier / 10.0) )
		{
            if ( multiplayer != CLIENT && ((Creature*)this) )
            {
                ((Creature*)this)->attack(3, 0, nullptr);
            }
		}
	}
	// stab attack - refer to weapon limb code for additional animation
	else if ( monsterAttack == 3 )
	{
		if ( weaponarm->skill[1] == 0 )
		{
			if ( limbAnimateToLimit(weaponarm, ANIMATE_PITCH, -0.3, 0, false, 0.0) )
			{
				weaponarm->skill[1] = 1;
			}
		}
		else if ( weaponarm->skill[1] == 1 )
		{
			if ( limbAnimateToLimit(weaponarm, ANIMATE_PITCH, 0.3, 2 * PI / 3, false, 0.0) )
			{
				weaponarm->skill[1] = 2;
			}
		}
		else if ( weaponarm->skill[1] >= 2 )
		{
			// return to neutral
			if ( limbAnimateToLimit(weaponarm, ANIMATE_PITCH, -0.2, 0, false, 0.0) )
			{
				weaponarm->skill[0] = rightbody->skill[0];
				this->monsterWeaponYaw = 0;
				weaponarm->pitch = rightbody->pitch;
				weaponarm->roll = 0;
				this->monsterArmbended = 0;
				monsterAttack = 0;
			}
		}
	}
	// ranged weapons
	else if ( monsterAttack == MONSTER_POSE_RANGED_WINDUP1 )
	{
		// crossbow
		if ( monsterAttackTime == 0 )
		{
			// init rotations
			this->monsterArmbended = 0;
			this->monsterWeaponYaw = 0;
			weaponarm->roll = 0;
			weaponarm->skill[1] = 0;
		}

		// draw the crossbow level... slowly
		if ( weaponarm->pitch > PI || weaponarm->pitch < 0 )
		{
			limbAnimateToLimit(weaponarm, ANIMATE_PITCH, 0.1, 0, false, 0.0);
		}
		else
		{
			limbAnimateToLimit(weaponarm, ANIMATE_PITCH, -0.1, 0, false, 0.0);
		}

		if ( monsterAttackTime >= ANIMATE_DURATION_WINDUP / (monsterGlobalAnimationMultiplier / 10.0) )
		{
            if ( multiplayer != CLIENT && ((Creature*)this) )
            {
                ((Creature*)this)->attack(MONSTER_POSE_RANGED_SHOOT1, 0, nullptr);
            }
		}
	}
	// shoot crossbow
	else if ( monsterAttack == MONSTER_POSE_RANGED_SHOOT1 )
	{
		// recoil upwards
		if ( weaponarm->skill[1] == 0 )
		{
			if ( limbAnimateToLimit(weaponarm, ANIMATE_PITCH, -0.2, 15 * PI / 8, false, 0.0) )
			{
				weaponarm->skill[1] = 1;
			}
		}
		// recoil downwards
		else if ( weaponarm->skill[1] == 1 )
		{
			if ( limbAnimateToLimit(weaponarm, ANIMATE_PITCH, 0.1, PI / 3, false, 0.0) )
			{
				weaponarm->skill[1] = 2;
			}
		}
		else if ( weaponarm->skill[1] >= 2 )
		{
			// limbAngleWithinRange cuts off animation early so it doesn't snap too far back to position.
			if ( limbAnimateToLimit(weaponarm, ANIMATE_PITCH, -0.2, 0, false, 0.0) || limbAngleWithinRange(weaponarm->pitch, -0.2, rightbody->pitch) )
			{
				weaponarm->skill[0] = rightbody->skill[0];
				this->monsterWeaponYaw = 0;
				//if ( this->hasRangedWeapon() && this->monsterState == MONSTER_STATE_ATTACK )
				//{
				//	// don't move ranged weapons so far if ready to attack
				//	weaponarm->pitch = rightbody->pitch * 0.25;
				//}
				//else
				//{
				//	weaponarm->pitch = rightbody->pitch;
				//}
				weaponarm->roll = 0;
				this->monsterArmbended = 0;
				monsterAttack = 0;
			}
		}
	}
	// shortbow/sling
	else if ( monsterAttack == MONSTER_POSE_RANGED_WINDUP2 )
	{
		if ( monsterAttackTime == 0 )
		{
			// init rotations
			this->monsterArmbended = 0;
			this->monsterWeaponYaw = 0;
			weaponarm->roll = 0;
			weaponarm->skill[1] = 0;
		}

		// draw the weapon level... slowly and shake
		if ( weaponarm->pitch > PI || weaponarm->pitch < 0 )
		{
			limbAnimateToLimit(weaponarm, ANIMATE_PITCH, 0.1, 0, true, 0.1);
		}
		else
		{
			limbAnimateToLimit(weaponarm, ANIMATE_PITCH, -0.1, 0, true, 0.1);
		}

		if ( monsterAttackTime >= ANIMATE_DURATION_WINDUP / (monsterGlobalAnimationMultiplier / 10.0) )
		{
            if ( multiplayer != CLIENT && ((Creature*)this) )
            {
                ((Creature*)this)->attack(MONSTER_POSE_RANGED_SHOOT2, 0, nullptr);
            }
		}
	}
	// shoot shortbow/sling
	else if ( monsterAttack == MONSTER_POSE_RANGED_SHOOT2 )
	{
		// recoil upwards
		if ( weaponarm->skill[1] == 0 )
		{
			if ( limbAnimateToLimit(weaponarm, ANIMATE_PITCH, -0.2, 14 * PI / 8, false, 0.0) )
			{
				weaponarm->skill[1] = 1;
			}
		}
		// recoil downwards
		else if ( weaponarm->skill[1] == 1 )
		{
			if ( limbAnimateToLimit(weaponarm, ANIMATE_PITCH, 0.1, 1 * PI / 3, false, 0.0) )
			{
				weaponarm->skill[1] = 2;
			}
		}
		else if ( weaponarm->skill[1] >= 2 )
		{
			// limbAngleWithinRange cuts off animation early so it doesn't snap too far back to position.
			if ( limbAnimateToLimit(weaponarm, ANIMATE_PITCH, -0.2, 0, false, 0.0) || limbAngleWithinRange(weaponarm->pitch, -0.2, rightbody->pitch) )
			{
				weaponarm->skill[0] = rightbody->skill[0];
				this->monsterWeaponYaw = 0;
				weaponarm->pitch = rightbody->pitch;
				weaponarm->roll = 0;
				this->monsterArmbended = 0;
				monsterAttack = 0;
				// play draw arrow sound
				playSoundEntityLocal(this, 246, 16);
			}
		}
	}
	else if ( monsterAttack == MONSTER_POSE_MAGIC_WINDUP1 )
	{
		// magic wiggle hands
		if ( monsterAttackTime == 0 )
		{
			// init rotations
			this->monsterArmbended = 0;
			this->monsterWeaponYaw = 0;
			weaponarm->roll = 0;
			weaponarm->pitch = 0;
			weaponarm->yaw = this->yaw;
			weaponarm->skill[1] = 0;
			// casting particles
			createParticleDot(this);
			// play casting sound
			playSoundEntityLocal(this, 170, 32);
		}

		double animationYawSetpoint = 0.f;
		double animationYawEndpoint = 0.f;
		double armSwingRate = 0.f;
		double animationPitchSetpoint = 0.f;
		double animationPitchEndpoint = 0.f;

		switch ( this->monsterSpellAnimation )
		{
			case MONSTER_SPELLCAST_NONE:
				break;
			case MONSTER_SPELLCAST_SMALL_HUMANOID:
				// smaller models so arms can wave in a larger radius and faster.
				animationYawSetpoint = normaliseAngle2PI(this->yaw + 2 * PI / 8);
				animationYawEndpoint = normaliseAngle2PI(this->yaw - 2 * PI / 8);
				animationPitchSetpoint = 2 * PI / 8;
				animationPitchEndpoint = 14 * PI / 8;
				armSwingRate = 0.3;
				if ( monsterAttackTime == 0 )
				{
					weaponarm->yaw = this->yaw - PI / 8;
				}
				break;
			case MONSTER_SPELLCAST_HUMANOID:
				animationYawSetpoint = normaliseAngle2PI(this->yaw + 1 * PI / 8);
				animationYawEndpoint = normaliseAngle2PI(this->yaw - 1 * PI / 8);
				animationPitchSetpoint = 1 * PI / 8;
				animationPitchEndpoint = 15 * PI / 8;
				armSwingRate = 0.15;
				break;
			default:
				break;
		}

		if ( weaponarm->skill[1] == 0 )
		{
			if ( limbAnimateToLimit(weaponarm, ANIMATE_PITCH, armSwingRate, animationPitchSetpoint, false, 0.0) )
			{
				if ( limbAnimateToLimit(weaponarm, ANIMATE_YAW, armSwingRate, animationYawSetpoint, false, 0.0) )
				{
					weaponarm->skill[1] = 1;
				}
			}
		}
		else
		{
			if ( limbAnimateToLimit(weaponarm, ANIMATE_PITCH, -armSwingRate, animationPitchEndpoint, false, 0.0) )
			{
				if ( limbAnimateToLimit(weaponarm, ANIMATE_YAW, -armSwingRate, animationYawEndpoint, false, 0.0) )
				{
					weaponarm->skill[1] = 0;
				}
			}
		}

		if ( monsterAttackTime >= 2 * ANIMATE_DURATION_WINDUP / (monsterGlobalAnimationMultiplier / 10.0) )
		{
            if ( multiplayer != CLIENT && ((Creature*)this) )
            {
                // swing the arm after we prepped the spell
                ((Creature*)this)->attack(MONSTER_POSE_MAGIC_WINDUP2, 0, nullptr);
            }
		}
	}
	// swing arm to cast spell
	else if ( monsterAttack == MONSTER_POSE_MAGIC_WINDUP2 )
	{
		if ( monsterAttackTime == 0 )
		{
			// init rotations
			weaponarm->pitch = 0;
			this->monsterArmbended = 0;
			this->monsterWeaponYaw = 0;
			weaponarm->roll = 0;
			weaponarm->skill[1] = 0;
		}

		if ( limbAnimateToLimit(weaponarm, ANIMATE_PITCH, -0.3, 5 * PI / 4, false, 0.0) )
		{
			if ( multiplayer != CLIENT && ((Creature*)this) )
			{
				Stat* stats = this->getStats();
				if ( stats && stats->type == SHADOW )
				{
                    ((Creature*)this)->attack(MONSTER_POSE_MAGIC_CAST1, 0, nullptr);
				}
				else
				{
                    ((Creature*)this)->attack(1, 0, nullptr);
				}
			}
		}
	}

	return;
}

void Entity::humanoidAnimateWalk(Entity* limb, node_t* bodypartNode, int bodypart, double walkSpeed, double dist, double distForFootstepSound)
{
	if ( bodypart == LIMB_HUMANOID_RIGHTLEG || bodypart == LIMB_HUMANOID_LEFTARM )
	{
		Entity* rightbody = nullptr;
		// set rightbody to left leg.
		node_t* rightbodyNode = list_Node(&this->children, LIMB_HUMANOID_LEFTLEG);
		if ( rightbodyNode )
		{
			rightbody = (Entity*)rightbodyNode->element;
		}
		else
		{
			return;
		}

		node_t* shieldNode = list_Node(&this->children, 8);
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
									if ( ((Creature*)this) && ((Creature*)this)->monsterFootstepType == MONSTER_FOOTSTEP_USE_BOOTS )
									{
										node_t* tempNode = list_Node(&this->children, 3);
										if ( tempNode )
										{
											Entity* foot = (Entity*)tempNode->element;
											playSoundEntityLocal(this, getMonsterFootstepSound( ((Creature*)this)->monsterFootstepType, foot->sprite), 32);
										}
									}
									else
									{
										playSoundEntityLocal(this, getMonsterFootstepSound( ((Creature*)this)->monsterFootstepType, 0), 32);
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
									if ( ((Creature*)this)->monsterFootstepType == MONSTER_FOOTSTEP_USE_BOOTS )
									{
										node_t* tempNode = list_Node(&this->children, 3);
										if ( tempNode )
										{
											Entity* foot = (Entity*)tempNode->element;
											playSoundEntityLocal(this, getMonsterFootstepSound(((Creature*)this)->monsterFootstepType, foot->sprite), 32);
										}
									}
									else
									{
										playSoundEntityLocal(this, getMonsterFootstepSound(((Creature*)this)->monsterFootstepType, 0), 32);
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
				if ( bodypart == LIMB_HUMANOID_RIGHTARM && this->hasRangedWeapon() && ((Creature*)this) && ((Creature*)this)->monsterState == MONSTER_STATE_ATTACK )
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