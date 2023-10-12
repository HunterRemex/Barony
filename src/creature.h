//
// Created by remex on 10/5/23.
//

#pragma once
#include "entity.hpp"
//#include "scores.hpp" //Breaks a lot of stuff

//extern Player* players[MAXPLAYERS];

class Creature : public Entity {

public:
    Creature(Sint32 in_sprite, Uint32 pos, list_t* entlist, list_t* creaturelist);
    ~Creature();
    void initMonster(int mySprite);

//--PUBLIC MONSTER SKILLS--
    Sint32& monsterState;
    Sint32& monsterTarget;
    real_t& monsterTargetX;
    real_t& monsterTargetY;
    Sint32& monsterSpecialTimer;
//Only used by goatman.
    Sint32& monsterSpecialState;
    Sint32& monsterFootstepType;
    Sint32& monsterLookTime;
    Sint32& monsterMoveTime;
    Sint32& monsterHitTime;
    Sint32& monsterPathBoundaryXStart;
    Sint32& monsterPathBoundaryYStart;
    Sint32& monsterPathBoundaryXEnd;
    Sint32& monsterPathBoundaryYEnd;
    Sint32& monsterStoreType;
    Sint32& monsterStrafeDirection;
    Sint32& monsterPathCount;
    real_t& monsterLookDir;
    Sint32& monsterAllyState;
    Sint32& monsterAllyPickupItems;
    Sint32& monsterAllyInteractTarget;
    Sint32& monsterAllyClass;
    Sint32& monsterDefend;
    Sint32& monsterAllySpecial;
    Sint32& monsterAllySpecialCooldown;
    Sint32& monsterAllySummonRank;
    real_t& monsterKnockbackVelocity;

    Sint32& monsterKnockbackUID;
    Sint32& monsterFearfulOfUid;

    Sint32& monsterIllusionTauntingThisUid;
    Sint32& monsterLastDistractedByNoisemaker;
    Sint32& monsterExtraReflexTick;
    real_t& monsterSentrybotLookDir;
    real_t& monsterKnockbackTangentDir;
    real_t& playerStrafeVelocity;
    real_t& playerStrafeDir;
    real_t& monsterSpecialAttackUnequipSafeguard;

    //--PUBLIC PLAYER SKILLS--
    Sint32& playerLevelEntrySpeech;
    Sint32& playerAliveTime;
    Sint32& playerVampireCurse;
    Sint32& playerAutomatonDeathCounter;
    Sint32& playerCreatedDeathCam;
//--PUBLIC MONSTER SHADOW SKILLS--
    Sint32& monsterShadowInitialMimic;
    Sint32& monsterShadowDontChangeName;
//--PUBLIC MONSTER LICH SKILLS--
    Sint32& monsterLichFireMeleeSeq;
    Sint32& monsterLichFireMeleePrev;
    Sint32& monsterLichIceCastSeq;
    Sint32& monsterLichIceCastPrev;
    Sint32& monsterLichMagicCastCount;
    Sint32& monsterLichMeleeSwingCount;
    Sint32& monsterLichBattleState;
    Sint32& monsterLichTeleportTimer;
    Sint32& monsterLichAllyStatus;
    Sint32& monsterLichAllyUID;
//--PLAYER SPAWN POINT--
    Sint32& playerStartDir;

    void effectTimes();

    void succubusChooseWeapon(const Entity* target, double dist);

    void lichFireSetNextAttack(Stat& myStats);

    bool goblinCanWieldItem(const Item& item) const;

    void increaseSkill(int skill, bool notify = true) override;

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

    int shouldMonsterDefend(Stat& myStats, const Entity& target, const Stat& targetStats, int targetDist, bool hasrangedweapon);

    bool monsterConsumeFoodEntity(Entity* food, Stat* myStats);

    bool monsterAllyEquipmentInClass(const Item& item) const;

    void monsterHandleKnockbackVelocity(real_t monsterFacingTangent, real_t weightratio);

    int monsterGetDexterityForMovement();

    void monsterGenerateQuiverItem(Stat* myStats, bool lesserMonster = false);

    bool checkFriend(Entity* your);

    bool isFollowerFreeToPathToPlayer(Stat* myStats);

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

    bool shouldMonsterEquipThisWeapon(const Item& itemToEquip) const;

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

    int getMonsterEffectiveDistanceOfRangedWeapon(Item *weapon);

    static int getHealthRegenInterval(Entity* my, Stat& myStats, bool isPlayer);

    bool handleMonsterSpecialAttack(Stat* myStats, Entity* target, double dist, bool forceDeinit);

    int getAttackPose() const;

    void humanoidAnimateWalk(Entity* limb, node_t* bodypartNode, int bodypart, double walkSpeed, double dist,
                             double distForFootstepSound);

    void setHelmetLimbOffset(Entity* helm);

    void setHumanoidLimbOffset(Entity* limb, Monster race, int limbType);

    void handleMonsterAttack(Stat* myStats, Entity* target, double dist);
};