# Kingpin: Life of Crime Multi Mod
Kingpin Life Of Crime Multi Mod v0.01

# GeoIP setup

- updated to kpded2 implimentation of geoip v2

# Log

- bots now aim for feet when using the rocken luncher

- aim lower for crouched players

# v0.24

- bot will aim at next target sooner once old target dies. speed is x4 of skill

- added display of stats in dedicated console at endgame

- stoped connecting nodes while in noclip

- stoped bots shooting while on a ladder

- made bots not connect paths when play does RJ etc. delete old rout files if they look up at a target and spin. 

- routs should not connect through fences anymore

- added geoip MH:

- added admin command. "mapvote". will let you chose next map

- botdebug. added command "localnode on". when debugging, nodes that are within your location will be shown. path lines will be disabled

- botdebug. changed viewed paths to be shorter. less overflows

- botdebug. bound keys 1-10. key8 gets closest node and sets the info to bound key9. for easy relocation

- added some bad spawn locations to be moved. 8mile, 420dm1

# v0.25

- removed print "connected" for bots

- "votemap" changed to now allow non admin to call a vote. all maps are now playable. even in main.

- "endmap" admin command added. will end map early and allow all players to vote

- "commands" only print available options. non admin list is shorter

- added glass/alpha to find enemy calculations

- bot now select weapon when picked up. it used to switch when only when attacking

- introduced some random best weapons, 3 main groups. top weapon being tommy, hmg and rl.

- stoped bot only shooting at closest player. if old target exists. it will keep shooting them

- bots now look more to the side for items. instead of just in front

- fixed a bug where ammo in weapon was not counted for selectable guns

- bots with RL now try to move backwards when to close to an enemy. instead of selecting another weapon

- made shotgun reload 3 bullets at once. without animations.

- added MH: fix for bad spawn locations

**note you may need to reduce skill in this version**


# v0.26

- "nodefinal" debug command. mark .nod file to never be updated. good for distributing custom rout tables. regardless of "sv_botpath"

- "sv_botjump" 0 off default. to disable the auto creation of nodes when a player jumps. bunnyhop will make maps a mess.

- found missing code not commented in acebots. for jump nodes. code updates map with many node when bunny hoping, so i created sv_botjump default 0.

- fixed shared armour issue. was not checking light/heavy properly

- bots now die better. they would still continue there movement actions

- trace for slopes was not calculated with bounding box. causing wandering bot to get stuck in crates etc.

- trace for slope has also been shortened 

- added some code to U-turn bot if its stuck making the same decision

- fixed issue with bots accurecy. now they miss alot more depending on sv_botskill value.

- bug. bots will be stuck and stil have "velocity". velocity is calculated from last frame

- when a bot climbed the back of a ladder it would get stuck and die, now uses the 180 u-turn 

- made bots try to route around players when trying to get to a goal

- decrease bot accuracy if on fire upto 20 units.

- lower bot skill's also decreses the angle a bot will detect an enamy, FOV, making them easyer.

- votemap now shows map name your are voting for

- players with 4 or more frags will be hunted down by bots

- stop players auto switching to crow bar when picked up

- new cvar sv_keeppistol. if a player selected the pistol after picking up a weapon. it wont switch

# v0.27

- added 1 more check on keeping pistol

- random taunts for bots when attacking

- changed random weps for bots. hmg will get a boost in percent as fav wep

- added some check for shooting boozooka above small railings. will aim for head instead of legs

- bots with crowbar will now chase you. not stand back like using pistol etc.

- bot more accurate. fixed a bug that would calculate bots shot 0.1 sec late. if player strafed bots would miss

- fixed votemap issue with more than 1 player

- dm players that crashed/dissconected can now reconnect and resume where there score was for DM games

- stoped view bob when in spec

- bots now pickup droped weapons bassed on ammo count. not wep inventory anymore

- when going to spec. you will keep your current position, instead of respawning.

- maps with team spawns in dm will get used if every one of them have style set.

- pause at player intermision for a few seconds. then allow free roam

- added 100ms compensation to antilag.

- antilag disabled for bots.

- connecting bots now print "bot007 (BOT) connected. BestWep = "Heavy machinegun""

- antispawn camp shows gun red while immortal, then shows green when player can be shot. updated times

- decrease bot accuracy if on fire. increased to 40 units.

- node links are only joined if player is within 92 units from a node. max node dist is 384

# v0.28

- fixed teleporter issue. caused by velocity not being reset and crashing into floor

- mapxxx.cfg is now allways used. no longer using old local game style setup.

- added votebotadd.  	"votebotadd 1" will add a bot to team "1" dragons

- added votebotremove. 	"votebotremove thugbot_1" will remove bot to dragons

- added votebotskill. 	"votebotskill 4" will make all bots maximum skill of 4

- fixed alot of the bots walking of the edge on floating maps. map must have sky underneath
fix above. also includes a lava check

- added more random strafing when attacking

- found bot aim issue. pmove calculates movement then shoot dir, causing off target hits when strafing

- 	-fix is to predict where they will be and set view angle. causing slight off centre but still within target

- 	-this also effects clients but less noticable because of framerate. bot are 10fps


- bots that have just spawned will look for a better weapon, if in range. even in skill 4. stops pistol spamming

- "sv_bot_allow_add".   add cvar to allow voteing

- "sv_bot_allow_skill". add cvar to allow voteing

- "sv_bot_max_players". when set, if a player enters the server. a bot wil be removed if total players+bots are above this value

- 	-only one bot will be added/removed when a client leaves/enteres at a time. 

- 	-if this value is lower than the number of bots in <mapname>.cfg the value wil never be	as low unless bots are removed manualy

- 	-will allow added bots above value untill a new player enters then one will be removed

- 	-does not check count if only 1 player in server. 

- 	-map change resets bots


- "sv_bot_max"	 default 8. total bots allowed to be voted into to server

- "kick_flamehack" default is now 1. can be set in server config file

- "anti_spawncamp" default is now 1  can be set in server config file



- **note** auto routing to higher places are disabled. noticed on jump pads not linking. need to manualy add the link
disabled because player may have used rocketjump to get there. N/A.. yet :)


# v0.29

- previous version?. fixed jump pads. bot now stops directional movements while in air

- added client command "menu" for easy voteing

- stoped player moving on func_plate when going up

- bot now times out by comparing last origin not velocity

# v0.30

- added hitmen option to comp.ini. enable_hitmen. includes hitmen.ini for setup

- fixed bot angle for corps

- bot should now die if stuck on a ladder

- stopNodeUpdate now stops any tempory edits to map routs

- new command "clearnode #" that wipes all paths used to node number

- crate_time reduced for smaller jumps

- fixed a rand() issue for bot dodge

- added a check for jump nodes to make sure they are not allready airborn

# v0.31

- updated to mm2.0

- made bots more accuret when target is closer

- bots will now shoot sooner if attacked

- fixed multiple issue with func_plat

- node file updated to ver 4. this will delet all old routs

- fixed some more short/int inconsistancies in node usage

# v0.32

- links "to" teleporters now link through solid walls (work around for bad maps. solid brush over teleporters. cause issues??)

- jump pads no longer create a path. player can steer in the air, causing routs to go all over the place

- jump pads and teleportes are used as a short term goal. makes bots use more of map (though bots cant wander through solid brushes over teleporters)

- fixed esc bug while spectating

- added some checks to water movement. jumping out of pool, item picked up allready.

- pathmap timeout not reset after level change. causing nodes not to be created(this is major bug)

# v0.33

- added cmd < sv removebot single >. will remove a single bot. the last one added

- made bot random aim via bullets not viewport

- doors are set to auto open

# v0.34

- fixed bot name truncated to scoreboard size

- frag typo on hud

- match bot vote to M's webpage 0-10

- hitman weapon timer correct at match start

- fixed game start count down timer for shorter pregame

# v0.35

- added moded pistol with silencer for hitmen

- 100hp each bullet for pistol in hitmen

- fix crouch bug again!!!!

- adde taunt animations


# v0.36

- added cvar hitman

- bot skill shown on scoreboard

- anti spawn camp timer reduced to 1.5 sec

- "menu" now shows botskill 0-10 web value

# v0.37

- fixed bots shooting through some trans surfaces

- changed player names from 13 char to 15.

- added "hitman 1" to cvar. enable hitmen through console. comp.ini still working and efects this cvar.
    also shows in server browser info. requires map change to take effect.


- added "sv_hook" to cvar. enable hook for all game modes. default=0

- added "sv_pretime" to cvar. allows setting the count down to game start, default=25 sec

- added "sv_pretimebm" to cvar. allows setting the count down to game start on team games, default=15 sec

- fixed countdown "time" shown in client browser

- additional fix to all scoreboards to truncate to 13 chars. player real name is still 15 chars

- added "sv_pretimebm" temaply will use a shorter start time than dm

- bug in hitmen causing weapon lag (previous call from shotty/ GL sets RDF_NOLERP)

# v0.37.M1 

- Bots will jump off ladders if stuck.

- Made bots line up with node X/Y origin on ladders to stop them getting stuck. 

- Node's in map might need to be moved/relocated if bot still gets stuck.

- Bots that die from being stuck does not effects their score anymore. they cant help it being dumb(blame me:)

- set node distance back to 128. hitting max nodes.

- fixed bots not shooting through alpha surfaces.

- bots now run in the opisite direction to rockets/nads. unless they are looking at the enemy, then they will strafe.

- bots should not jump up stairs anymore. they do get slightly stuck sometimes though

- fixed a crouch issues. where bot would think it can jump instead.

- added check for crates while LRG is set. when a move node gets placed on top of a creat the bot wont jump but time out.

- changed links to be created upto crates for non jump nodes.(needs more testing to confirm most maps are compatable) 

- added "clearallpaths" to remove all node links in map. keep nodes but removes links.

- bots will wander when shooting at an enemy. this prevents bots change direction to resume old goal.

- after a bot shoots an enemy and looses sight, a new goal will be set to try follow them for a short time. 

- bots dodge/strafe event is now dependent on skill. they will stand stil more with lower skill levels. 

- custum route tables for kpdm1-4

# v0.37.M2
fixed crash with a long map file name. 

# v0.37.M3

- bots now jumps when slowed down by a short edge. kpdm1 fix

- fixed a ladder bug that could posibly send bot into void

- tweeked bots skill. 0 was previously to hard

- stop bots avoiding rockets/nads at lower skill levels. 

- Flamer is now using skill properly. viewport also shows flame dir(unlike bullets)

- stats on spec player now shows the current chase players score

- Fixed SPistol bug. if you pickup a pistol mod. you have to wait for the complete silencer attatch animations. 20 frames

- bot no longer miss at skill 4

- fixed aim at feet when not nessary

# v0.38

- added hook suport for bots. max 50 nodes allowed

- fixed typo in vector flat distance

- fixed console bug in bot_print

- added some new mm features. like 3rd person death camera

- bots on ladders are still going into void. added additional checks

- per bot cfg skill setting. 0.0 to 2.0 multiplier

- updated geoIP to use new version in kpded2

- ** latest kpded 2 now has an option to disable the ESC bug fix. allowing client command 'menu' to work properly

- bots predict rockets and shoot were player will be.

### todo
- prevent bots jumping down from great heights. esp when wandering
end game bot comment

- MAKE more rout tables

- if wep pistol. serch longer for a gun. random..

- reloading makes bot serch for weps? WEAPON_RELOADING code not working on bot

- bots climbing when "next" to a ladder, like in thin air(fixed?)

- node pathes will be recalulated every time its visited.

- endmap stats to sort...

- bots attacking with pistol and a better wep right next to them. make bot pick it up (not recently spawned)

- predict rl direction to shoot

- "hook" 4 bots. implimented but needs testing. posible to have bad posi if allowed to be autoroute

- hitmen "showmotd"

- hitman scores

- bots move slow on trig push

- teleporter. bots cant use teleporters as a goal while wandering if surrounded by a brush solid

- teleporter. bots will try avoid(turn) walking into teleporter if surrounded by a solid brush (goal set)

- bots cant use secret doors eg.. kpq1dm6_final

- add topplayer to rl/gl so dodge worke better.
------------------------------
### maps to test
### Ladders
-----------
- toomuchblood
- fragndie3 4
- nycdm3_kp
- fucked
- sym //stairs

### Other 
---------
- bigcube_v3
- nwth
- dm_cw
- downhere
- kp_biodm_v3
- kptrdm05a (stuck on walls+lift+ladder)

### Teleporters 
---------------
- kp_biodm_v3	= issue with solid wall above teleporter
- stdm5		= ok. trig push speed upped 4 bot use
- facility_2016	= ok
- kpq1dm6_final = issue with solid brush surounding centre of teleporter
- 420dm1 	= fixed lift in version2

- kpdm11
- sonik_e1m7
- panzer_tly
- dm_fis_b1
- team_float

### Lava 
--------
- brutal
- sinister 
- kpq2dm4
- team_crossfire
- chrome

### Crash 
---------
- parkgarage
- ddkp1

# Legal
----------
Koogle Bot is a modification of the ACE Bot, and is therefore
in NO WAY supported by Steve Yeager.

The ACE Bot is a product of Steve Yeager, and is available from
the ACE Bot homepage, at http://www.axionfx.com/ace.

The ACE Bot MUST NOT be sold in ANY form. If you have paid for 
The ACE Bot product, you should contact Steve Yeager immediately, via
the ACE Bot homepage.
