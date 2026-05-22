--
DELETE FROM `spawn_group` WHERE `spawnType` = 1 AND `spawnId` = 30029;
INSERT INTO `spawn_group` (`groupId`, `spawnType`, `spawnId`) VALUES
(1, 1, 30029);

UPDATE `item_template` SET `spelltrigger_1` = 0 WHERE `entry` = 31134;

DELETE FROM `creature_text` WHERE `CreatureID` IN (12936, 12937, 12938);
INSERT INTO `creature_text` (`CreatureID`,`GroupID`,`ID`,`Text`,`Type`,`Language`,`Probability`,`Emote`,`Duration`,`Sound`,`BroadcastTextId`,`TextRange`,`comment`) VALUES
(12936,0,0,"I'm saved! Thank you, doctor!",12,0,100,0,0,0,8355,0,""),
(12937,0,0,"I'm saved! Thank you, doctor!",12,0,100,0,0,0,8355,0,""),
(12938,0,0,"I'm saved! Thank you, doctor!",12,0,100,0,0,0,8355,0,"");

UPDATE `gameobject` SET `spawntimesecs` = -1 WHERE `guid` IN (11757, 60069, 60080, 60095);

DELETE FROM `spawn_group` WHERE `spawnType` = 1 AND `spawnId` IN (11757, 60069, 60080, 60095);
INSERT INTO `spawn_group` (`groupId`, `spawnType`, `spawnId`) VALUES
(1, 1, 11757),
(1, 1, 60069),
(1, 1, 60080),
(1, 1, 60095);
