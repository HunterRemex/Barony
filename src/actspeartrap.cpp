/*-------------------------------------------------------------------------------

	BARONY
	File: actspeartrap.cpp
	Desc: implements spear trap code

	Copyright 2013-2016 (c) Turning Wheel LLC, all rights reserved.
	See LICENSE for details.

-------------------------------------------------------------------------------*/

#include "main.hpp"
#include "game.hpp"
#include "stat.hpp"
#include "entity.hpp"
#include "engine/audio/sound.hpp"
#include "net.hpp"
#include "collision.hpp"
#include "player.hpp"

/*-------------------------------------------------------------------------------

	act*

	The following function describes an entity behavior. The function
	takes a pointer to the entity that uses it as an argument.

-------------------------------------------------------------------------------*/

#define SPEARTRAP_INIT my->skill[0]
#define SPEARTRAP_AMBIENCE my->skill[1]
#define SPEARTRAP_STATUS my->skill[3]
#define SPEARTRAP_OUTTIME my->skill[4]
#define SPEARTRAP_STARTHEIGHT my->fskill[0]
#define SPEARTRAP_VELZ my->vel_z

void actSpearTrap(Entity* my)
{
	SPEARTRAP_AMBIENCE--;
	if ( SPEARTRAP_AMBIENCE <= 0 )
	{
		SPEARTRAP_AMBIENCE = TICKS_PER_SECOND * 30;
		playSoundEntityLocal( my, 149, 64 );
	}

	if ( multiplayer != CLIENT )
	{
		if (!my->skill[28])
		{
			return;
		}

		if (my->skill[28] == 2)
		{
			// shoot out the spears
			if (!SPEARTRAP_STATUS )
			{
				SPEARTRAP_STATUS = 1;
				SPEARTRAP_OUTTIME = 0;
				playSoundEntity(my, 82, 64);
				serverUpdateEntitySkill(my, 3);
				serverUpdateEntitySkill(my, 4);
			}
		}
		else
		{
			// retract the spears
			if ( SPEARTRAP_STATUS )
			{
				SPEARTRAP_STATUS = 0;
				if ( SPEARTRAP_OUTTIME <= 60 )
				{
					playSoundEntity(my, 82, 64);
				}
				SPEARTRAP_OUTTIME = 0;
				serverUpdateEntitySkill(my, 3);
				serverUpdateEntitySkill(my, 4);
			}
		}
	}
	else
	{
		my->flags[NOUPDATE] = true;
	}
	if ( !SPEARTRAP_INIT )
	{
		SPEARTRAP_INIT = 1;
		SPEARTRAP_STARTHEIGHT = my->z;
	}

	if ( !SPEARTRAP_STATUS || SPEARTRAP_OUTTIME > 60 )
	{
		// retract spears
		if ( my->z < SPEARTRAP_STARTHEIGHT )
		{
			SPEARTRAP_VELZ += .25;
			my->z = std::min(SPEARTRAP_STARTHEIGHT, my->z + SPEARTRAP_VELZ);
		}
		else
		{
			SPEARTRAP_VELZ = 0;
		}
	}
	else
	{
		// shoot out spears
		my->z = fmax(SPEARTRAP_STARTHEIGHT - 20, my->z - 4);
		if ( multiplayer != CLIENT )
		{
			SPEARTRAP_OUTTIME++;
			if ( SPEARTRAP_OUTTIME > 60 )
			{
				playSoundEntity(my, 82, 64);
				serverUpdateEntitySkill(my, 4);
			}
			else if ( SPEARTRAP_OUTTIME == 1 )
			{
				node_t* node;
				for ( node = map.creatures->first; node != nullptr; node = node->next ) //Searching explicitly for players and monsters, so search only creature list, not map.entities.
				{
					Creature* creature = (Creature*)node->element;
					if ( creature->behavior == &actPlayer || creature->behavior == &actMonster )
					{
						Stat* stats = creature->getStats();
						if ( stats )
						{
							if ( !creature->flags[PASSABLE] && entityInsideEntity(my, creature) )
							{
								// do damage!
								if ( creature->behavior == &actPlayer )
								{
									Uint32 color = makeColorRGB(255, 0, 0);
									messagePlayerColor(creature->skill[2], MESSAGE_STATUS, color, Language::get(586));
									if ( players[creature->skill[2]]->isLocalPlayer() )
									{
										cameravars[creature->skill[2]].shakex += .1;
										cameravars[creature->skill[2]].shakey += 10;
									}
									else
									{
										if ( creature->skill[2] > 0 && !players[creature->skill[2]]->isLocalPlayer() )
										{
											strcpy((char*)net_packet->data, "SHAK");
											net_packet->data[4] = 10; // turns into .1
											net_packet->data[5] = 10;
											net_packet->address.host = net_clients[creature->skill[2] - 1].host;
											net_packet->address.port = net_clients[creature->skill[2] - 1].port;
											net_packet->len = 6;
											sendPacketSafe(net_sock, -1, net_packet, creature->skill[2] - 1);
										}
									}
								}
								playSoundEntity(creature, 28, 64);
								spawnGib(creature);
								creature->modHP(-50);
								if ( stats->HP <= 0 )
								{
									if ( stats->type == AUTOMATON )
									{
										creature->playerAutomatonDeathCounter = TICKS_PER_SECOND * 5; // set the death timer to immediately pop for players.
									}
								}
								// set obituary
								creature->setObituary(Language::get(1507));
						        stats->killer = KilledBy::TRAP_SPIKE;
							}
						}
					}
				}
			}
		}
	}
}
