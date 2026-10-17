--
UPDATE `creature_template` SET `AIName` = 'SmartAI' WHERE `entry` IN (2678, 8615, 8836, 9556, 10718, 11200, 11636, 11637, 12473, 15720, 15901, 15935, 17706, 23192, 23320, 24529, 26238, 27936, 29337, 30568, 34264, 34265, 34266, 38763) AND `AIName` = '' AND `ScriptName` = '';

UPDATE `creature_template` SET `flags_extra` = `flags_extra` & ~2 WHERE `entry` = 15720; -- Timbermaw Ancestor: drop CREATURE_FLAG_EXTRA_CIVILIAN

DELETE FROM `smart_scripts` WHERE `source_type` = 0 AND `entryorguid` IN (8615, 9556, 12473, 15720, 17706, 23320, 24529, 30568);
INSERT INTO `smart_scripts` (`entryorguid`, `source_type`, `id`, `link`, `event_type`, `event_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action_type`, `action_param1`, `action_param2`, `action_param3`, `action_param4`, `action_param5`, `action_param6`, `target_type`, `target_param1`, `target_param2`, `target_param3`, `target_x`, `target_y`, `target_z`, `target_o`, `comment`) VALUES
(12473, 0, 0, 0, 0, 0, 100, 0, 5000, 8000, 9000, 13000, 11, 9658, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Arcanite Dragonling - In Combat - Cast ''Flame Buffet'''),
(12473, 0, 1, 0, 0, 0, 100, 0, 6000, 11000, 11000, 23000, 11, 8873, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Arcanite Dragonling - In Combat - Cast ''Flame Breath'''),
(8615, 0, 0, 0, 0, 0, 100, 0, 7000, 11000, 12000, 27000, 11, 8873, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Mithril Dragonling - In Combat - Cast ''Flame Breath'''),
(30568, 0, 0, 0, 4, 0, 100, 0, 0, 0, 0, 0, 11, 6268, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Phantom Boar - On Aggro - Cast ''Rushing Charge'''),
(30568, 0, 1, 0, 0, 0, 100, 0, 5000, 11000, 5000, 11000, 11, 6268, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Phantom Boar - In Combat - Cast ''Rushing Charge'''),
(17706, 0, 0, 0, 4, 0, 100, 0, 0, 0, 0, 0, 11, 6268, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Felsteel Boar - On Aggro - Cast ''Rushing Charge'''),
(17706, 0, 1, 0, 0, 0, 100, 0, 5000, 11000, 5000, 11000, 11, 6268, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 'Felsteel Boar - In Combat - Cast ''Rushing Charge'''),
(9556, 0, 0, 0, 0, 0, 100, 0, 2000, 4000, 8000, 12000, 11, 2691, 64, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Felhound Minion - In Combat - Cast ''Mana Burn'''),
(23320, 0, 0, 0, 0, 0, 100, 0, 1000, 3000, 5000, 7000, 11, 25055, 64, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Netherwing Ally - In Combat - Cast ''Arcane Bolt'''),
(24529, 0, 0, 0, 0, 0, 100, 0, 1000, 3000, 6000, 8000, 11, 19874, 64, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Voodoo Servant - In Combat - Cast ''Lightning Bolt'''),
(15720, 0, 0, 0, 0, 0, 100, 0, 1000, 3000, 7000, 9000, 11, 20295, 64, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 'Timbermaw Ancestor - In Combat - Cast ''Lightning Bolt''');
