--
DELETE FROM `rbac_linked_permissions` WHERE `id` IN (194, 198);
INSERT INTO `rbac_linked_permissions` VALUES
(194,   1), -- Instant logout
(194,   2), -- Skip Queue
(194,   9), -- Cannot earn realm first achievements
(194,  11), -- Log GM trades
(194,  13), -- Skip Instance required bosses check
(194,  18), -- Skip character creation death knight min level check
(194,  25), -- Allow say chat between factions
(194,  26), -- Allow channel chat between factions
(194,  28), -- See two side who list
(194,  27), -- Two side mail interaction
(194,  29), -- Add friends of other faction
(194,  32), -- Can be assigned tickets with .assign ticket command
(194,  33), -- Notify if a command was not found
(194,  34), -- Check if should appear in list using .gm ingame command
(194,  36), -- Filter whispers
(194,  37), -- Use staff badge in chat
(194,  38), -- Resurrect with full Health Points
(194,  39), -- Restore saved gm setting states
(194,  40), -- Allows to add a gm to friend list
(194,  41), -- Use Config option START_GM_LEVEL to assign new character level
(194,  44), -- Receive global GM messages/texts
(194,  45), -- Join channels without announce
(194,  47), -- Can ignore non-strong lower security checks if it\'s disabled in config
(194,  52), -- No battleground deserter debuff
(194,  53), -- Can be AFK on the battleground
(194, 195), -- Role: Sec Level Player
(194, 198), -- Role: Moderator Commands
--
(198, 232), -- Command: arena
(198, 236), -- Command: arena info
(198, 237), -- Command: arena lookup
(198, 291), -- Command: cheat
(198, 292), -- Command: cheat casttime
(198, 293), -- Command: cheat cooldown
(198, 294), -- Command: cheat explore
(198, 295), -- Command: cheat god
(198, 296), -- Command: cheat power
(198, 297), -- Command: cheat status
(198, 298), -- Command: cheat taxi
(198, 299), -- Command: cheat waterwalk
(198, 300), -- Command: debug
(198, 302), -- Command: debug areatriggers
(198, 309), -- Command: debug combat
(198, 312), -- Command: debug los
(198, 315), -- Command: debug play
(198, 316), -- Command: debug play cinematics
(198, 317), -- Command: debug play movie
(198, 318), -- Command: debug play sound
(198, 337), -- Command: debug threat
(198, 341), -- Command: deserter
(198, 342), -- Command: deserter bg
(198, 343), -- Command: deserter bg add
(198, 344), -- Command: deserter bg remove
(198, 345), -- Command: deserter instance
(198, 346), -- Command: deserter instance add
(198, 347), -- Command: deserter instance remove
(198, 367), -- Command: event
(198, 368), -- Command: event activelist
(198, 371), -- Command: gm
(198, 372), -- Command: gm chat
(198, 374), -- Command: gm ingame
(198, 375), -- Command: gm list
(198, 376), -- Command: gm visible
(198, 377), -- Command: go
(198, 387), -- Command: gobject
(198, 392), -- Command: gobject info
(198, 394), -- Command: gobject near
(198, 398), -- Command: gobject target
(198, 412), -- Command: instance
(198, 413), -- Command: instance listbinds
(198, 415), -- Command: instance stats
(198, 430), -- Command: lfg
(198, 431), -- Command: lfg player
(198, 432), -- Command: lfg group
(198, 433), -- Command: lfg queue
(198, 434), -- Command: lfg clean
(198, 436), -- Command: list
(198, 437), -- Command: list creature
(198, 438), -- Command: list item
(198, 439), -- Command: list object
(198, 440), -- Command: list auras
(198, 441), -- Command: list mail
(198, 442), -- Command: lookup
(198, 443), -- Command: lookup area
(198, 444), -- Command: lookup creature
(198, 445), -- Command: lookup event
(198, 446), -- Command: lookup faction
(198, 447), -- Command: lookup item
(198, 448), -- Command: lookup itemset
(198, 449), -- Command: lookup object
(198, 450), -- Command: lookup quest
(198, 451), -- Command: lookup player
(198, 452), -- Command: lookup player ip
(198, 453), -- Command: lookup player account
(198, 454), -- Command: lookup player email
(198, 455), -- Command: lookup skill
(198, 456), -- Command: lookup spell
(198, 457), -- Command: lookup spell id
(198, 458), -- Command: lookup taxinode
(198, 459), -- Command: lookup tele
(198, 461), -- Command: lookup map
(198, 462), -- Command: announce
(198, 466), -- Command: gmannounce
(198, 467), -- Command: gmnameannounce
(198, 468), -- Command: gmnotify
(198, 469), -- Command: nameannounce
(198, 471), -- Command: whispers
(198, 472), -- Command: group
(198, 473), -- Command: group leader
(198, 474), -- Command: group disband
(198, 475), -- Command: group remove
(198, 476), -- Command: group join
(198, 477), -- Command: group list
(198, 478), -- Command: group summon
(198, 490), -- Command: appear
(198, 492), -- Command: bank
(198, 494), -- Command: combatstop
(198, 497), -- Command: cooldown
(198, 501), -- Command: dismount
(198, 502), -- Command: distance
(198, 504), -- Command: freeze
(198, 505), -- Command: gps
(198, 512), -- Command: listfreeze
(198, 515), -- Command: mute
(198, 517), -- Command: pinfo
(198, 520), -- Command: recall
(198, 521), -- Command: repairitems
(198, 522), -- Command: respawn
(198, 523), -- Command: revive
(198, 524), -- Command: saveall
(198, 525), -- Command: save
(198, 528), -- Command: summon
(198, 531), -- Command: unfreeze
(198, 532), -- Command: unmute
(198, 534), -- Command: unstuck
(198, 544), -- Command: modify
(198, 547), -- Command: modify drunk
(198, 548), -- Command: modify energy
(198, 549), -- Command: modify faction
(198, 555), -- Command: modify mount
(198, 557), -- Command: modify rage
(198, 559), -- Command: modify runicpower
(198, 560), -- Command: modify scale
(198, 561), -- Command: modify speed
(198, 562), -- Command: modify speed all
(198, 563), -- Command: modify speed backwalk
(198, 564), -- Command: modify speed fly
(198, 565), -- Command: modify speed walk
(198, 566), -- Command: modify speed swim
(198, 568), -- Command: modify standstate
(198, 570), -- Command: npc
(198, 597), -- Command: npc say
(198, 598), -- Command: npc textemote
(198, 599), -- Command: npc whisper
(198, 600), -- Command: npc yell
(198, 632), -- Command: mutehistory
(198, 718), -- Command: server
(198, 725), -- Command: server info
(198, 737), -- Command: tele
(198, 740), -- Command: tele name
(198, 741), -- Command: tele group
(198, 742), -- Command: ticket
(198, 744), -- Command: ticket close
(198, 745), -- Command: ticket closedlist
(198, 746), -- Command: ticket comment
(198, 747), -- Command: ticket complete
(198, 749), -- Command: ticket escalate
(198, 751), -- Command: ticket list
(198, 752), -- Command: ticket onlinelist
(198, 754), -- Command: ticket response
(198, 755), -- Command: ticket response append
(198, 756), -- Command: ticket response appendln
(198, 759), -- Command: ticket viewid
(198, 760), -- Command: ticket viewname
(198, 794), -- Command: .guild info
(198, 797), -- Command: pvpstats
(198, 855), -- Command: debug play music
(198, 865), -- Command: npc showloot
(198, 860), -- Command: list respawns
(198, 861), -- Command: group set
(198, 862), -- Command: group set assistant
(198, 863), -- Command: group set maintank
(198, 864), -- Command: group set mainassist
(198, 870), -- Command: debug threatinfo
(198, 875), -- Command: lookup map id
(198, 876), -- Command: lookup item id
(198, 877); -- Command: lookup quest id
