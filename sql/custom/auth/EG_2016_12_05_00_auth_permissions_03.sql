DELETE FROM `rbac_linked_permissions` WHERE `id` IN (193, 197);
INSERT INTO `rbac_linked_permissions` VALUES
(193,  14), -- Skip character creation team mask check
(193,  15), -- Skip character creation class mask check
(193,  16), -- Skip character creation race mask check
(193,  17), -- Skip character creation reserved name check
(193,  19), -- Skip needed requirements to use channel check
(193,  20), -- Skip disable map check
(193,  22), -- Skip spam chat check
(193,  23), -- Skip over-speed ping check
(193,  30), -- Save character without delay with .save command
(193,  31), -- Use params with .unstuck command
(193,  35), -- See all security levels with who command
(193,  46), -- Change channel settings without being channel moderator
(193,  48), -- Enable IP, Last Login and EMail output in pinfo
(193,  51), -- Allow trading between factions
(193, 194), -- Role: Sec Level Moderator
(193, 197), -- Role: Gamemaster Commands
--
(197, 217), -- Command: account
(197, 221), -- Command: account lock
(197, 222), -- Command: account lock country
(197, 223), -- Command: account lock ip
(197, 230), -- Command: achievement
(197, 231), -- Command: achievement add
(197, 233), -- Command: arena captain
(197, 234), -- Command: arena create
(197, 235), -- Command: arena disband
(197, 238), -- Command: arena rename
(197, 239), -- Command: ban
(197, 240), -- Command: ban account
(197, 241), -- Command: ban character
(197, 242), -- Command: ban ip
(197, 243), -- Command: ban playeraccount
(197, 244), -- Command: baninfo
(197, 245), -- Command: baninfo account
(197, 246), -- Command: baninfo character
(197, 247), -- Command: baninfo ip
(197, 248), -- Command: banlist
(197, 249), -- Command: banlist account
(197, 250), -- Command: banlist character
(197, 251), -- Command: banlist ip
(197, 252), -- Command: unban
(197, 253), -- Command: unban account
(197, 254), -- Command: unban character
(197, 255), -- Command: unban ip
(197, 256), -- Command: unban playeraccount
(197, 267), -- Command: cast
(197, 268), -- Command: cast back
(197, 269), -- Command: cast dist
(197, 270), -- Command: cast self
(197, 271), -- Command: cast target
(197, 272), -- Command: cast dest
(197, 273), -- Command: character
(197, 274), -- Command: character customize
(197, 275), -- Command: character changefaction
(197, 276), -- Command: character changerace
(197, 284), -- Command: character rename
(197, 285), -- Command: character reputation
(197, 286), -- Command: character titles
(197, 301), -- Command: debug anim
(197, 311), -- Command: debug lootrecipient
(197, 373), -- Command: gm fly
(197, 388), -- Command: gobject activate
(197, 390), -- Command: gobject add temp
(197, 401), -- Command: guild
(197, 402), -- Command: guild create
(197, 403), -- Command: guild delete
(197, 404), -- Command: guild invite
(197, 405), -- Command: guild uninvite
(197, 406), -- Command: guild rank
(197, 407), -- Command: guild rename
(197, 408), -- Command: honor
(197, 409), -- Command: honor add
(197, 410), -- Command: honor add kill
(197, 411), -- Command: honor update
(197, 414), -- Command: instance unbind
(197, 416), -- Command: instance savedata
(197, 417), -- Command: learn
(197, 418), -- Command: learn all
(197, 419), -- Command: learn all my
(197, 420), -- Command: learn all my class
(197, 421), -- Command: learn all my pettalents
(197, 422), -- Command: learn all my spells
(197, 423), -- Command: learn all my talents
(197, 424), -- Command: learn all gm
(197, 425), -- Command: learn all crafts
(197, 426), -- Command: learn all default
(197, 427), -- Command: learn all lang
(197, 428), -- Command: learn all recipes
(197, 463), -- Command: channel
(197, 464), -- Command: channel set
(197, 465), -- Command: channel set ownership
(197, 479), -- Command: pet
(197, 480), -- Command: pet create
(197, 481), -- Command: pet learn
(197, 482), -- Command: pet unlearn
(197, 483), -- Command: send
(197, 484), -- Command: send items
(197, 485), -- Command: send mail
(197, 486), -- Command: send message
(197, 488), -- Command: additem
(197, 489), -- Command: additemset
(197, 498), -- Command: damage
(197, 500), -- Command: die
(197, 506), -- Command: guid
(197, 508), -- Command: hidearea
(197, 509), -- Command: itemmove
(197, 510), -- Command: kick
(197, 513), -- Command: maxskill
(197, 514), -- Command: movegens
(197, 516), -- Command: neargrave
(197, 526), -- Command: setskill
(197, 542), -- Command: morph
(197, 543), -- Command: demorph
(197, 545), -- Command: modify arenapoints
(197, 550), -- Command: modify gender
(197, 551), -- Command: modify honor
(197, 552), -- Command: modify hp
(197, 553), -- Command: modify mana
(197, 556), -- Command: modify phase
(197, 558), -- Command: modify reputation
(197, 575), -- Command: npc add temp
(197, 593), -- Command: npc info
(197, 594), -- Command: npc near
(197, 596), -- Command: npc playemote
(197, 601), -- Command: npc tame
(197, 743), -- Command: ticket assign
(197, 750), -- Command: ticket escalatedlist
(197, 758), -- Command: ticket unassign
(197, 761), -- Command: titles
(197, 762), -- Command: titles add
(197, 763), -- Command: titles current
(197, 764), -- Command: titles remove
(197, 765), -- Command: titles set
(197, 766), -- Command: titles set mask
(197, 767), -- Command: wp
(197, 774), -- Command: wp show
(197, 778), -- Command: ahbot
(197, 793), -- Command: ahbot status
(197, 836), -- Command: debug boundary
(197, 841), -- Command: debug neargraveyard
(197, 856), -- Command: npc spawngroup
(197, 857), -- Command: npc despawngroup
(197, 858), -- Command: gobject spawngroup
(197, 859), -- Command: gobject despawngroup
(197, 866); -- Command: list spawnpoints