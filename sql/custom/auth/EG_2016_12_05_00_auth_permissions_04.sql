--
DELETE FROM `rbac_linked_permissions` WHERE `id` IN (192, 196);
INSERT INTO `rbac_linked_permissions` VALUES
(192,  21), -- Skip reset talents when used more than allowed check
(192,  42), -- Allows to use CMSG_WORLD_TELEPORT opcode
(192,  43), -- Allows to use CMSG_WHOIS opcode
(192, 193), -- Role: Sec Level Gamemaster
(192, 196), -- Role: Administrator Commands
--
(196, 200), -- Command: rbac
(196, 201), -- Command: rbac account
(196, 202), -- Command: rbac account list
(196, 203), -- Command: rbac account grant
(196, 204), -- Command: rbac account deny
(196, 205), -- Command: rbac account revoke
(196, 206), -- Command: rbac list
(196, 218), -- Command: account addon
(196, 225), -- Command: account password
(196, 226), -- Command: account set
(196, 227), -- Command: account set addon
(196, 257), -- Command: bf
(196, 258), -- Command: bf start
(196, 259), -- Command: bf stop
(196, 260), -- Command: bf switch
(196, 261), -- Command: bf timer
(196, 262), -- Command: bf enable
(196, 263), -- Command: account email
(196, 264), -- Command: account set sec
(196, 265), -- Command: account set sec email
(196, 266), -- Command: account set sec regmail
(196, 277), -- Command: character deleted
(196, 279), -- Command: character deleted list
(196, 280), -- Command: character deleted restore
(196, 283), -- Command: character level
(196, 287), -- Command: levelup
(196, 288), -- Command: pdump
(196, 289), -- Command: pdump load
(196, 290), -- Command: pdump write
(196, 303), -- Command: debug arena
(196, 304), -- Command: debug bg
(196, 305), -- Command: debug entervehicle
(196, 306), -- Command: debug getitemstate
(196, 307), -- Command: debug getitemvalue
(196, 308), -- Command: debug getvalue
(196, 310), -- Command: debug itemexpire
(196, 313), -- Command: debug mod32value
(196, 314), -- Command: debug moveflags
(196, 319), -- Command: debug send
(196, 320), -- Command: debug send buyerror
(196, 321), -- Command: debug send channelnotify
(196, 322), -- Command: debug send chatmessage
(196, 323), -- Command: debug send equiperror
(196, 324), -- Command: debug send largepacket
(196, 325), -- Command: debug send opcode
(196, 326), -- Command: debug send qinvalidmsg
(196, 327), -- Command: debug send qpartymsg
(196, 328), -- Command: debug send sellerror
(196, 329), -- Command: debug send setphaseshift
(196, 330), -- Command: debug send spellfail
(196, 331), -- Command: debug setaurastate
(196, 332), -- Command: debug setbit
(196, 333), -- Command: debug setitemvalue
(196, 334), -- Command: debug setvalue
(196, 335), -- Command: debug setvid
(196, 336), -- Command: debug spawnvehicle
(196, 338), -- Command: debug update
(196, 339), -- Command: debug worldstate
(196, 340), -- Command: wpgps
(196, 348), -- Command: disable
(196, 349), -- Command: disable add
(196, 350), -- Command: disable add achievement_criteria
(196, 351), -- Command: disable add battleground
(196, 352), -- Command: disable add map
(196, 353), -- Command: disable add mmap
(196, 354), -- Command: disable add outdoorpvp
(196, 355), -- Command: disable add quest
(196, 356), -- Command: disable add spell
(196, 357), -- Command: disable add vmap
(196, 358), -- Command: disable remove
(196, 359), -- Command: disable remove achievement_criteria
(196, 360), -- Command: disable remove battleground
(196, 361), -- Command: disable remove map
(196, 362), -- Command: disable remove mmap
(196, 363), -- Command: disable remove outdoorpvp
(196, 364), -- Command: disable remove quest
(196, 365), -- Command: disable remove spell
(196, 366), -- Command: disable remove vmap
(196, 369), -- Command: event start
(196, 370), -- Command: event stop
(196, 378), -- Command: account 2fa
(196, 379), -- Command: account 2fa setup
(196, 380), -- Command: account 2fa remove
(196, 389), -- Command: gobject add
(196, 391), -- Command: gobject delete
(196, 393), -- Command: gobject move
(196, 394), -- Command: gobject near
(196, 395), -- Command: gobject set
(196, 396), -- Command: gobject set phase
(196, 397), -- Command: gobject set state
(196, 399), -- Command: gobject turn
(196, 400), -- Command: debug transport
(196, 429), -- Command: unlearn
(196, 435), -- Command: lfg options
(196, 470), -- Command: notify
(196, 487), -- Command: send money
(196, 491), -- Command: aura
(196, 493), -- Command: bindsight
(196, 495), -- Command: cometome
(196, 499), -- Command: dev
(196, 503), -- Command: flusharenapoints
(196, 511), -- Command: linkgrave
(196, 518), -- Command: playall
(196, 519), -- Command: possess
(196, 527), -- Command: showarea
(196, 529), -- Command: unaura
(196, 530), -- Command: unbindsight
(196, 533), -- Command: unpossess
(196, 535), -- Command: wchange
(196, 536), -- Command: mmap
(196, 537), -- Command: mmap loadedtiles
(196, 538), -- Command: mmap loc
(196, 539), -- Command: mmap path
(196, 540), -- Command: mmap stats
(196, 541), -- Command: mmap testarea
(196, 546), -- Command: modify bit
(196, 554), -- Command: modify money
(196, 567), -- Command: modify spell
(196, 569), -- Command: modify talentpoints
(196, 571), -- Command: npc add
(196, 572), -- Command: npc add formation
(196, 573), -- Command: npc add item
(196, 574), -- Command: npc add move
(196, 576), -- Command: npc add delete
(196, 577), -- Command: npc add delete item
(196, 578), -- Command: npc add follow
(196, 579), -- Command: npc add follow stop
(196, 580), -- Command: npc set
(196, 581), -- Command: npc set allowmove
(196, 582), -- Command: npc set entry
(196, 583), -- Command: npc set factionid
(196, 584), -- Command: npc set flag
(196, 585), -- Command: npc set level
(196, 586), -- Command: npc set link
(196, 587), -- Command: npc set model
(196, 588), -- Command: npc set movetype
(196, 589), -- Command: npc set phase
(196, 590), -- Command: npc set spawndist
(196, 591), -- Command: npc set spawntime
(196, 592), -- Command: npc set data
(196, 595), -- Command: npc move
(196, 602), -- Command: quest
(196, 603), -- Command: quest add
(196, 604), -- Command: quest complete
(196, 605), -- Command: quest remove
(196, 606), -- Command: quest reward
(196, 607), -- Command: reload
(196, 608), -- Command: reload access_requirement
(196, 609), -- Command: reload achievement_criteria_data
(196, 610), -- Command: reload achievement_reward
(196, 611), -- Command: reload all
(196, 612), -- Command: reload all achievement
(196, 613), -- Command: reload all area
(196, 614), -- Command: broadcast_text
(196, 615), -- Command: reload all gossips
(196, 616), -- Command: reload all item
(196, 617), -- Command: reload all locales
(196, 618), -- Command: reload all loot
(196, 619), -- Command: reload all npc
(196, 620), -- Command: reload all quest
(196, 621), -- Command: reload all scripts
(196, 622), -- Command: reload all spell
(196, 623), -- Command: reload areatrigger_involvedrelation
(196, 624), -- Command: reload areatrigger_tavern
(196, 625), -- Command: reload areatrigger_teleport
(196, 626), -- Command: reload auctions
(196, 627), -- Command: reload autobroadcast
(196, 628), -- Command: reload command
(196, 629), -- Command: reload conditions
(196, 630), -- Command: reload config
(196, 631), -- Command: reload battleground_template
(196, 633), -- Command: reload creature_linked_respawn
(196, 634), -- Command: reload creature_loot_template
(196, 635), -- Command: reload creature_onkill_reputation
(196, 636), -- Command: reload creature_questender
(196, 637), -- Command: reload creature_queststarter
(196, 638), -- Command: reload creature_summon_groups
(196, 639), -- Command: reload creature_template
(196, 640), -- Command: reload creature_text
(196, 641), -- Command: reload disables
(196, 642), -- Command: reload disenchant_loot_template
(196, 643), -- Command: reload event_scripts
(196, 644), -- Command: reload fishing_loot_template
(196, 645), -- Command: reload graveyard_zone
(196, 646), -- Command: reload game_tele
(196, 647), -- Command: reload gameobject_questender
(196, 648), -- Command: reload gameobject_loot_template
(196, 649), -- Command: reload gameobject_queststarter
(196, 650), -- Command: reload gm_tickets
(196, 651), -- Command: reload gossip_menu
(196, 652), -- Command: reload gossip_menu_option
(196, 653), -- Command: reload item_enchantment_template
(196, 654), -- Command: reload item_loot_template
(196, 655), -- Command: reload item_set_names
(196, 656), -- Command: reload lfg_dungeon_rewards
(196, 657), -- Command: reload locales_achievement_reward
(196, 658), -- Command: reload locales_creature
(196, 659), -- Command: reload locales_creature_text
(196, 660), -- Command: reload locales_gameobject
(196, 661), -- Command: reload locales_gossip_menu_option
(196, 662), -- Command: reload locales_item
(196, 663), -- Command: reload locales_item_set_name
(196, 664), -- Command: reload locales_npc_text
(196, 665), -- Command: reload locales_page_text
(196, 666), -- Command: reload locales_points_of_interest
(196, 667), -- Command: reload locales_quest
(196, 668), -- Command: reload mail_level_reward
(196, 669), -- Command: reload mail_loot_template
(196, 670), -- Command: reload milling_loot_template
(196, 671), -- Command: reload npc_spellclick_spells
(196, 672), -- Command: reload trainer
(196, 673), -- Command: reload npc_vendor
(196, 674), -- Command: reload page_text
(196, 675), -- Command: reload pickpocketing_loot_template
(196, 676), -- Command: reload points_of_interest
(196, 677), -- Command: reload prospecting_loot_template
(196, 678), -- Command: reload quest_poi
(196, 679), -- Command: reload quest_template
(196, 680), -- Command: reload rbac
(196, 681), -- Command: reload reference_loot_template
(196, 682), -- Command: reload reserved_name
(196, 683), -- Command: reload reputation_reward_rate
(196, 684), -- Command: reload reputation_spillover_template
(196, 685), -- Command: reload skill_discovery_template
(196, 686), -- Command: reload skill_extra_item_template
(196, 687), -- Command: reload skill_fishing_base_level
(196, 688), -- Command: reload skinning_loot_template
(196, 689), -- Command: reload smart_scripts
(196, 690), -- Command: reload spell_required
(196, 691), -- Command: reload spell_area
(196, 692), -- Command: reload spell_bonus_data
(196, 693), -- Command: reload spell_group
(196, 694), -- Command: reload spell_learn_spell
(196, 695), -- Command: reload spell_loot_template
(196, 696), -- Command: reload spell_linked_spell
(196, 697), -- Command: reload spell_pet_auras
(196, 698), -- Command: character changeaccount
(196, 699), -- Command: reload spell_proc
(196, 700), -- Command: reload spell_scripts
(196, 701), -- Command: reload spell_target_position
(196, 702), -- Command: reload spell_threats
(196, 703), -- Command: reload spell_group_stack_rules
(196, 704), -- Command: reload trinity_string
(196, 706), -- Command: reload waypoint_scripts
(196, 707), -- Command: reload waypoint_data
(196, 708), -- Command: reload vehicle_accessory
(196, 709), -- Command: reload vehicle_template_accessory
(196, 710), -- Command: reset
(196, 711), -- Command: reset achievements
(196, 712), -- Command: reset honor
(196, 713), -- Command: reset level
(196, 714), -- Command: reset spells
(196, 715), -- Command: reset stats
(196, 716), -- Command: reset talents
(196, 717), -- Command: reset all
(196, 719), -- Command: server corpses
(196, 721), -- Command: server idlerestart
(196, 722), -- Command: server idlerestart cancel
(196, 723), -- Command: server idleshutdown
(196, 724), -- Command: server idleshutdown cancel
(196, 726), -- Command: server plimit
(196, 727), -- Command: server restart
(196, 728), -- Command: server restart cancel
(196, 729), -- Command: server set
(196, 730), -- Command: server set closed
(196, 733), -- Command: server set motd
(196, 734), -- Command: server shutdown
(196, 735), -- Command: server shutdown cancel
(196, 736), -- Command: server motd
(196, 738), -- Command: tele add
(196, 739), -- Command: tele del
(196, 748), -- Command: ticket delete
(196, 753), -- Command: ticket reset
(196, 757), -- Command: ticket togglesystem
(196, 768), -- Command: wp add
(196, 769), -- Command: wp event
(196, 770), -- Command: wp load
(196, 771), -- Command: wp modify
(196, 772), -- Command: wp unload
(196, 773), -- Command: wp reload
(196, 777), -- Command: mailbox
(196, 779), -- Command: ahbot items
(196, 780), -- Command: ahbot items gray
(196, 781), -- Command: ahbot items white
(196, 782), -- Command: ahbot items green
(196, 783), -- Command: ahbot items blue
(196, 784), -- Command: ahbot items purple
(196, 785), -- Command: ahbot items orange
(196, 786), -- Command: ahbot items yellow
(196, 787), -- Command: ahbot ratio
(196, 788), -- Command: ahbot ratio alliance
(196, 789), -- Command: ahbot ratio horde
(196, 790), -- Command: ahbot ratio neutral
(196, 791), -- Command: ahbot rebuild
(196, 792), -- Command: ahbot reload
(196, 795), -- Command: instance setbossstate
(196, 796), -- Command: instance getbossstate
(196, 798), -- Command: mod xp
(196, 835), -- Command: debug loadcells
(196, 836), -- Command: debug boundary
(196, 837), -- Command: npc evade
(196, 838), -- Command: pet level
(196, 839), -- Command: server shutdown force
(196, 840), -- Command: server restart force
(196, 843), -- Command: reload quest_greeting
(196, 852), -- Command: debug dummy
(196, 867), -- Command: reload quest_greeting_locale
(196, 871), -- Command: debug instancespawn
(196, 872), -- Command: server debug
(196, 873), -- Command: reload creature_movement_override
(196, 878), -- Command: debug questreset
(196, 879), -- Command: debug poolstatus
(196, 881), -- Command: reload vehicle_template
(196, 884), -- Command: bg start
(196, 885); -- Command: bg stop
