//
// Created by remex on 10/5/23.
//

#pragma once
#include "entity.hpp"
#include "main.hpp"
#include "game.hpp"
#include "stat.hpp"
#include "light.hpp"
#include "monster.hpp"
#include "entity_base.hpp"
//#include "scores.hpp" //Breaks a lot of stuff

//extern Player* players[MAXPLAYERS];

class Creature : public Entity {

public:
    Creature(Sint32 in_sprite, Uint32 pos, list_t* entlist, list_t* creaturelist);
    ~Creature();
    void initMonster(int mySprite);

//--PUBLIC MONSTER SKILLS--
    Sint32& monsterState; //skill[0]
    Sint32& monsterTarget; //skill[1]
    real_t& monsterTargetX; //fskill[2]
    real_t& monsterTargetY; //fskill[3]
    Sint32& monsterSpecialTimer; //skill[29]
//Only used by goatman.
    Sint32& monsterSpecialState; //skill[33]
    Sint32& monsterFootstepType; //skill[31]
    Sint32& monsterLookTime; //skill[32]
    Sint32& monsterMoveTime; //skill[4]
    Sint32& monsterHitTime; //skill[8]
    Sint32& monsterPathBoundaryXStart; //skill[9]
    Sint32& monsterPathBoundaryYStart; //skill[10]
    Sint32& monsterPathBoundaryXEnd; //fskill[5]
    Sint32& monsterPathBoundaryYEnd; //skill[6]
    Sint32& monsterStoreType; //skill[7]
    Sint32& monsterStrafeDirection; //skill[14]
    Sint32& monsterPathCount; //skill[15]
    real_t& monsterLookDir; //skill[16]
    Sint32& monsterAllyState; //skill[17]
    Sint32& monsterAllyPickupItems; //skill[18]
    Sint32& monsterAllyInteractTarget; //skill[39]
    Sint32& monsterAllyClass; //skill[38]
    Sint32& monsterDefend; //fskill[4]
    Sint32& monsterAllySpecial; //skill[41]
    Sint32& monsterAllySpecialCooldown; //skill[42] If monster is an ally of a player, assign number 0-3 to it for the players to track on the map.
    Sint32& monsterAllySummonRank; //skill[43]
    real_t& monsterKnockbackVelocity; //skill[44]

    Sint32& monsterKnockbackUID; //skill[51]
    Sint32& monsterFearfulOfUid; //skill[53]

    Sint32& monsterIllusionTauntingThisUid; //skill[55]
    Sint32& monsterLastDistractedByNoisemaker; //skill[55] shared with above as above only is for inner demons.
    Sint32& monsterExtraReflexTick; //skill[56]
    real_t& monsterSentrybotLookDir; //fskill[10]
    real_t& monsterKnockbackTangentDir; //fskill[11]
    real_t& playerStrafeVelocity; //fskill[12]
    real_t& playerStrafeDir; //fskill[13]
    real_t& monsterSpecialAttackUnequipSafeguard; //fskill[14]

    //--PUBLIC PLAYER SKILLS--
    Sint32& playerLevelEntrySpeech; //skill[18]
    Sint32& playerAliveTime; //skill[12]
    Sint32& playerVampireCurse; //skill[51]
    Sint32& playerAutomatonDeathCounter; //skill[15] - 0 if unused, > 0 if counting to death
    Sint32& playerCreatedDeathCam; //skill[16] - if we triggered actDeathCam already.
//--PUBLIC MONSTER SHADOW SKILLS--
    Sint32& monsterShadowInitialMimic; //skill[34]. 0 = false, 1 = true.
    Sint32& monsterShadowDontChangeName; //skill[35]. 0 = false, 1 = true. Doesn't change name in its mimic if = 1.
//--PUBLIC MONSTER LICH SKILLS--
    Sint32& monsterLichFireMeleeSeq; //skill[34]
    Sint32& monsterLichFireMeleePrev; //skill[35]
    Sint32& monsterLichIceCastSeq; //skill[34]
    Sint32& monsterLichIceCastPrev; //skill[35]
    Sint32& monsterLichMagicCastCount; //skill[37] count the basic spell attacks in the seq and switch things up if too many in a row.
    Sint32& monsterLichMeleeSwingCount; //skill[38] count the 'regular' attacks in the seq and switch things up if too many in a row.
    Sint32& monsterLichBattleState; //skill[27] used to track hp/battle progress
    Sint32& monsterLichTeleportTimer; //skill[40] used to track conditions to teleport away.
    Sint32& monsterLichAllyStatus; //skill[18] used to track if allies are alive.
    Sint32& monsterLichAllyUID; //skill[17] used to track lich ally uid.
//--PLAYER SPAWN POINT--
    Sint32& playerStartDir; //skill[1]

    void effectTimes();

    void succubusChooseWeapon(const Entity* target, double dist);

    void lichFireSetNextAttack(Stat& myStats);

    bool goblinCanWieldItem(const Item& item) const;

    void increaseSkill(int skill, bool notify = true) override;

	Stat* getStats() const override;

    void (*behavior)(class Creature* my);

	//Lets monsters swap out weapons.
	void inline chooseWeapon(const Entity* target, double dist)
	{
	    Stat* myStats = getStats();
	    if ( !myStats )
	    {
	        return;
	    }

	    if ( myStats->EFFECTS[EFF_FEAR] )
	    {
	        return; // don't change weapons while feared.
	    }

	    switch ( myStats->type )
	    {
	        case GOATMAN:
	            goatmanChooseWeapon(target, dist);
	            break;
	        case INSECTOID:
	            insectoidChooseWeapon(target, dist);
	            break;
	        case INCUBUS:
	            incubusChooseWeapon(target, dist);
	            break;
	        case VAMPIRE:
	            vampireChooseWeapon(target, dist);
	            break;
	        case SHADOW:
	            shadowChooseWeapon(target, dist);
	            break;
	        case SUCCUBUS:
	            succubusChooseWeapon(target, dist);
	            break;
	        case SHOPKEEPER:
	            if ( target )
	            {
	                if ( Stat* targetStats = target->getStats() )
	                {
	                    if ( targetStats->type == SHOPKEEPER && myStats->weapon && myStats->weapon->type == SPELLBOOK_DRAIN_SOUL )
	                    {
	                        // gentlemans agreement to shoot bleed
	                        myStats->weapon->type = SPELLBOOK_BLEED;
	                    }
	                }
	            }
	            break;
	        default:
	            break;
	    }
	}

	//Lich effects
	void lichFireTeleport();

    void lichFireSummonMonster(Monster creature);

    void lichIceSetNextAttack(Stat& myStats);

    void vampireChooseWeapon(const Entity* target, double dist);

    bool automatonCanWieldItem(const Item& item) const;

    bool shadowCanWieldItem(const Item& item) const;

    void lichIceTeleport();

	// automaton specific function
	void automatonRecycleItem();

    void shadowSpecialAbility(bool initialMimic);

    void lichIceCreateCannon();

    Entity* lichThrowProjectile(real_t angle);

    void incubusChooseWeapon(const Entity* target, double dist);

    void lichIceSummonMonster(Monster creature);

    void goatmanChooseWeapon(const Entity* target, double dist);

    bool insectoidCanWieldItem(const Item& item) const;

	// incubus teleport spells
	void incubusTeleportToTarget(const Entity* target);

    void insectoidChooseWeapon(const Entity* target, double dist);

    bool shadowCanMimickSpell(int spellID);

    void incubusTeleportRandom();

	//Shadow teleport spells.
	void shadowTeleportToTarget(const Entity* target, int range);

    void shadowChooseWeapon(const Entity* target, double dist);

    bool goatmanCanWieldItem(const Item& item) const;

    bool humanCanWieldItem(const Item& item) const;

    void humanSetLimbsClient(int bodypart);

	//--*CheckBetterEquipment functions--
	void checkBetterEquipment(Stat* myStats);

    static void monsterRollLevelUpStats(int increasestat[3]);

    void handleEffects(Stat* myStats);

    bool isMobile();

    void attack(int pose, int charge, Entity* target);

    void playerLevelEntrySpeechSecond();

    static bool isPlayerHeadSprite(const int sprite);

    bool isPlayerHeadSprite() const;

    Monster getMonsterFromPlayerRace(int playerRace);

    void monsterMoveBackwardsAndPath();

    bool monsterHasLeader();

    void monsterAllySendCommand(int command, int destX, int destY, Uint32 uid = 0);

    bool teleport(int x, int y);

    bool monsterAllySetInteract();

    bool monsterSetPathToLocation(int destX, int destY, int adjacentTilesToCheck, int pathingType, bool tryRandomSpot = false);

    bool teleportRandom();

    bool gyrobotSetPathToReturnLocation(int destX, int destY, int adjacentTilesToCheck, bool tryRandomSpot = false);

    // teleport entity to a target, within a radius dist (range in whole tile lengths)
    bool teleportAroundEntity(Entity* target, int dist, int effectType = 0);

    void handleNPCInteractDialogue(Stat& myStats, AllyNPCChatter event);

    bool checkFriend(Entity* your);

    void lookAtEntity(Entity& target);

	void handleEffectsClient();

    /*
     * @param state: required to let the entity know if it should enter MONSTER_STATE_PATH, MONSTER_STATE_ATTACK, etc.
     * @param monsterWasHit: monster is retaliating to an attack as opposed to finding an enemy. to set reaction time accordingly in hardcore
     */
    void monsterAcquireAttackTarget(const Entity& target, Sint32 state, bool monsterWasHit = false);

    /*
     * Attempts to set the target to 0.
     * May refuses to do so and consequently return false in cases such as the shadow, which cannot lose its target until it's dead.
     * Returns true otherwise, if successfully zero-d out target.
     */
    bool monsterReleaseAttackTarget(bool force = false);

    void checkGroundForItems();

    bool canWieldItem(const Item& item) const;

// check for nearby items to add to monster's inventory, returns true if picked up item
    bool monsterAddNearbyItemToInventory(Stat* myStats, int rangeToFind, int maxInventoryItems, Entity* forcePickupItem = nullptr);

    bool shouldMonsterEquipThisWeapon(const Item& itemToEquip) const; //TODO: Look @ proficiencies.
    Item** shouldMonsterEquipThisArmor(const Item& item) const;
    int shouldMonsterDefend(Stat& myStats, const Entity& target, const Stat& targetStats, int targetDist, bool hasrangedweapon);
    bool monsterConsumeFoodEntity(Entity* food, Stat* myStats);
    void monsterHandleKnockbackVelocity(real_t monsterFacingTangent, real_t weightratio);
    int monsterGetDexterityForMovement();
    void monsterGenerateQuiverItem(Stat* myStats, bool lesserMonster = false);
    int getMonsterEffectiveDistanceOfRangedWeapon(Item *weapon);
    bool isFollowerFreeToPathToPlayer(Stat* myStats);

	// monster special attack handler, returns true if monster should attack after calling this function.
	bool handleMonsterSpecialAttack(Stat* myStats, Entity* target, double dist, bool forceDeinit);

    bool monsterWantsItem(const Item& item, Item**& shouldEquip, node_t*& replaceInventoryItem) const;

    double monsterRotate();

    // degrade chosen armor piece by 1 on entity, update clients.
    void degradeArmor(Stat& hitstats, Item& armor, int armornum);

	// check stats if monster should "retreat" in actMonster
	bool shouldRetreat(Stat& myStats);

	// check if monster should retreat or stand still when less than given distance
	bool backupWithRangedWeapon(Stat& myStats, int dist, int hasrangedweapon);

    void createPathBoundariesNPC(int maxTileDistance = -1);

	//bool monsterKnowsSpell(int spellID); //TODO: Should monsters use the spell item instead of spellbooks?
	node_t* chooseAttackSpellbookFromInventory();

    void handleHumanoidShieldLimb(Entity* shieldLimb, Entity* shieldArmLimb);

    void handleKnockbackDamage(Stat& myStats, Entity* knockedInto);

    Sint32 playerInsectoidExpectedManaFromHunger(Stat& myStats);

    void alertAlliesOnBeingHit(Creature *attacker, std::unordered_set<Entity*>* skipEntitiesToAlert = nullptr);


    static int getHealthRegenInterval(Entity* my, Stat& myStats, bool isPlayer);

	// monster attack pose, return the animation to use based on weapon.
    int getAttackPose() const;

    void humanoidAnimateWalk(Entity* limb, node_t* bodypartNode, int bodypart, double walkSpeed, double dist,
                             double distForFootstepSound) override;

    void handleMonsterAttack(Stat* myStats, Entity* target, double dist);


	bool monsterAllyEquipmentInClass(const Item& item) const;

	void setMP(int amount, bool updateClients = true);

	void modMP(int amount, bool updateClients = true);

	bool safeConsumeMP(int amount);
};