--
DELETE FROM `creature_text` WHERE `CreatureID` = 33579;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(33579, 0, 0, 14, 0, 100, 0, 0, 0, 33671, 0, 'Brann intro 1 - Pentarus you heard'),
(33579, 1, 0, 12, 0, 100, 0, 0, 0, 33684, 0, 'Brann intro 3 - Let us move out'),
(33579, 2, 0, 14, 0, 100, 0, 0, 0, 33687, 0, 'Brann intro 5 - Bring down the shield');

DELETE FROM `creature_text` WHERE `CreatureID` = 33624;
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(33624, 0, 0, 14, 0, 100, 0, 0, 0, 33673, 0, 'Pentarus intro 2 - Of course Brann'),
(33624, 1, 0, 14, 0, 100, 0, 0, 0, 33677, 0, 'Pentarus intro 4 - Mages of the Kirin Tor');

DELETE FROM `creature_text` WHERE `CreatureID` = 33686 AND `GroupID` IN (5,6);
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(33686, 5, 0, 12, 0, 100, 0, 0, 0, 34422, 0, 'Norgannon hardmode confirm - Security override permitted'),
(33686, 6, 0, 12, 0, 100, 0, 0, 0, 33808, 0, 'Norgannon hardmode confirm - Deactivating');

DELETE FROM `creature_text` WHERE `CreatureID` = 34054 AND `GroupID` IN (3,4);
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `TextRange`, `comment`) VALUES
(34054, 3, 0, 12, 0, 100, 0, 0, 0, 34146, 0, 'Brann Radio hardmode warning - generators coming online'),
(34054, 4, 0, 12, 0, 100, 0, 0, 0, 34147, 0, 'Brann Radio hardmode warning - four generators sabotage');

DELETE FROM `creature_text` WHERE `CreatureID` = 33701 AND `GroupID` IN (7,8);
INSERT INTO `creature_text` (`CreatureID`, `GroupID`, `ID`, `Type`, `Language`, `Probability`, `Emote`, `Duration`, `Sound`, `BroadcastTextId`, `Text`, `TextRange`, `comment`) VALUES
(33701, 7, 0, 14, 0, 100, 0, 0, 0, 0, 'What... What did you just do, $n?! Brann! Braaaaannn!', 0, 'Dellorah hardmode reaction yell'),
(33701, 8, 0, 14, 0, 100, 0, 0, 0, 0, 'Brann! $n just activated the orbital defense system! If we don''t get out of here soon, we''re going to be toast!', 0, 'Dellorah hardmode warning yell about orbital defense');

DELETE FROM `smart_scripts` WHERE `entryorguid` = 33701 AND `source_type` = 0 AND `id` = 1;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `event_param5`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(33701, 0, 1, 0, 38, 0, 100, 0, 99, 99, 0, 0, 0, 80, 9999999, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'High Explorer Dellorah - On Data Set 99 99 - CALL_TIMED_ACTIONLIST(id=9999999, timerType=0, allowOverride=1)');

DELETE FROM `creature` WHERE `id` = 33062 AND `map` = 603 AND `spawnMask` = 2;
INSERT INTO `creature` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `wander_distance`, `currentwaypoint`, `curhealth`, `curmana`, `MovementType`, `npcflag`, `unit_flags`, `dynamicflags`) VALUES
(33062, 603, 4273, 4273, 2, 1, 0, 0, -718.45, -112.61, 430.23, 0.087266, 180, 0, 0, 1, 0, 0, 0, 0, 0),
(33062, 603, 4273, 4273, 2, 1, 0, 0, -718.45, -100.00, 430.10, 0.122173, 180, 0, 0, 1, 0, 0, 0, 0, 0),
(33062, 603, 4273, 4273, 2, 1, 0, 0, -759.09, -206.58, 429.84, 1.832596, 180, 0, 0, 1, 0, 0, 0, 0, 0);
