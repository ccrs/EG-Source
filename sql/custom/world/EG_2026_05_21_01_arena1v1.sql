--
DELETE FROM `creature_template` WHERE `entry` = 60004;
INSERT INTO `creature_template` (`entry`, `modelid1`, `modelid2`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `scale`, `rank`, `dmgschool`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(60004, 27339, 0, 'Arena Duelmaster', '1v1 Arena', '', 0, 80, 80, 2, 35, 1, 1, 0, 0, 2000, 0, 1, 0, 0, 138936390, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, 0, 'EG_npc_arena1v1');

DELETE FROM `creature_template_movement` WHERE `CreatureId` = 60004;
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`, `Chase`, `Random`) VALUES
(60004, 1, 0, 0, 0, 0, 0);
