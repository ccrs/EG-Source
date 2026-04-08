--
UPDATE `creature_template` SET `flags_extra` = 0 WHERE `entry` IN (26668,30810);

 -- Eye of Dominion smart ai
SET @ENTRY := 193058;
UPDATE `gameobject_template` SET `AIName` = 'SmartGameObjectAI' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 1 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 1, 0, 1, 62, 0, 100, 0, 10028, 0, 0, 0, 85, 58524, 2, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On gossip action 0 from menu 10028 selected - Gossip player: Cast spell  Control Eidolon Watcher (58524) with flags triggered on self'),
(@ENTRY, 1, 1, 2, 61, 0, 100, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On gossip action 0 from menu 10028 selected - Gossip player: Close gossip'),
(@ENTRY, 1, 2, 0, 61, 0, 100, 0, 0, 0, 0, 0, 15, 13168, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On gossip action 0 from menu 10028 selected - Gossip player: Call quest Parting Gifts (13168) group event happened');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 193058 AND `SourceId` = 1;

 -- Eidolon Watcher smart ai
SET @ENTRY := 31110;
UPDATE `creature_template` SET `AIName` = 'SmartAI', `ScriptName` = 'EG_npc_eidolon_watcher' WHERE `entry` = @ENTRY;
DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryOrGuid` = @ENTRY;
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(@ENTRY, 0, 0, 0, 54, 0, 100, 512, 0, 0, 0, 0, 11, 58548, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On just summoned - Self: Cast spell  Ethereal (58548) with flags triggered on Self'),
(@ENTRY, 0, 1, 0, 27, 0, 100, 513, 0, 0, 0, 0, 11, 58548, 2, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On passenger boarded (vehicle) - Self: Cast spell  Ethereal (58548) with flags triggered on Passenger'),
(@ENTRY, 0, 2, 0, 28, 0, 100, 513, 0, 0, 0, 0, 28, 58548, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 'On passenger removed (vehicle) - Passenger: Remove aura due to spell  Ethereal (58548)'),
(@ENTRY, 0, 3, 0, 25, 0, 100, 512, 0, 0, 0, 0, 11, 58548, 2, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'On reset - Self: Cast spell  Ethereal (58548) with flags triggered on Self');


DELETE FROM `conditions` WHERE `SourceTypeOrReferenceId` = 22 AND `SourceEntry` = 31110 AND `SourceId` = 0;
