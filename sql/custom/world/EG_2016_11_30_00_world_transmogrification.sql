DELETE FROM `creature_template` WHERE `entry` IN (60000, 60001);
INSERT INTO `creature_template` (`entry`, `modelid1`, `modelid2`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `exp`, `faction`, `npcflag`, `scale`, `rank`, `dmgschool`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `AIName`, `MovementType`, `HoverHeight`, `RacialLeader`, `movementId`, `RegenHealth`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES
(60000, 20985, 0, 'Warpweaver Hashom', 'Transmogrifier', '', 0, 80, 80, 2, 35, 1, 1, 0, 0, 2000, 0, 1, 0, 0, 138936390, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, 0, 'EG_transmogrification_creature'),
(60001, 20985, 0, 'Warpweaver Dushar', 'Transmogrifier', '', 0, 80, 80, 2, 35, 1, 1, 0, 0, 2000, 0, 1, 0, 0, 138936390, 0, 0, 0, '', 0, 1, 0, 0, 1, 0, 0, 'EG_transmogrification_creature');

DELETE FROM `creature_template_movement` WHERE `CreatureId` IN (60000, 60001);
INSERT INTO `creature_template_movement` (`CreatureId`, `Ground`, `Swim`, `Flight`, `Rooted`, `Chase`, `Random`) VALUES
('60000', '1', '0', '0', '0', '0', '0'),
('60001', '1', '0', '0', '0', '0', '0');


DELETE FROM `creature` WHERE `guid` IN (300100, 300101);
INSERT INTO `creature` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`, `VerifiedBuild`) VALUES
(300100,60000,0,0,0,1,1,0,0,-8700.15,851.796,105.065,3.79383,300,0,0,12600,0,0,0,0,0),
(300101,60001,1,0,0,1,1,0,0,1792.59,-4555.24,23.0066,5.62405,300,0,0,12600,0,0,0,0,0);

SET @TEXT_ID := 60000;
DELETE FROM `npc_text` WHERE `ID` IN (@TEXT_ID, @TEXT_ID+1);
INSERT INTO `npc_text` (`ID`, `text0_0`) VALUES
(@TEXT_ID+0, 'Transmogrification allows you to change how your items look like without changing the stats of the items.\r\nItems used in transmogrification are no longer refundable, tradeable and are bound to you.\r\nUpdating a menu updates the view and prices.\r\n\r\nNot everything can be transmogrified with eachother.\r\nRestrictions include but are not limited to:\r\nOnly armor and weapons can be transmogrified\r\nGuns, bows and crossbows can be transmogrified with eachother\r\nFishing poles can not be transmogrified\r\nYou must be able to equip both items used in the process.\r\n\r\nTransmogrifications stay on your items as long as you own them.\r\nIf you try to put the item in guild bank or mail it to someone else, the transmogrification is stripped.\r\n\r\nYou can also remove transmogrifications for free at the transmogrifier.'),
(@TEXT_ID+1, "You can save your own transmogrification sets.\r\n\r\nTo save, first you must transmogrify your equipped items.\r\nThen when you go to the set management menu and go to save set menu,\r\nall items you have transmogrified are displayed so you see what you are saving.\r\nIf you think the set is fine, you can click to save the set and name it as you wish.\r\n\r\nTo use a set you can click the saved set in the set management menu and then select use set.\r\nIf the set has a transmogrification for an item that is already transmogrified, the old transmogrification is lost.\r\nNote that same transmogrification restrictions apply when trying to use a set as in normal transmogrification.\r\n\r\nTo delete a set you can go to the set\'s menu and select delete set.");

SET @STRING_ENTRY := 20001;
DELETE FROM `trinity_string` WHERE `entry` BETWEEN @STRING_ENTRY AND @STRING_ENTRY+9;
INSERT INTO `trinity_string` (`entry`, `content_default`) VALUES
(@STRING_ENTRY+0, 'Item transmogrified'),
(@STRING_ENTRY+1, 'Equipment slot is empty'),
(@STRING_ENTRY+2, 'Invalid source item selected'),
(@STRING_ENTRY+3, 'Source item does not exist'),
(@STRING_ENTRY+4, 'Destination item does not exist'),
(@STRING_ENTRY+5, 'Selected items are invalid'),
(@STRING_ENTRY+6, 'Not enough money'),
(@STRING_ENTRY+7, "You don\'t have enough tokens"),
(@STRING_ENTRY+8, 'Transmogrifications removed'),
(@STRING_ENTRY+9, 'There are no transmogrifications');
