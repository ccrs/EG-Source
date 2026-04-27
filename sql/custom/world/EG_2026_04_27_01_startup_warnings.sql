--
-- Fix SmartAI unused param warnings
--
UPDATE `smart_scripts` SET `action_param2` = 0 WHERE `entryorguid` = -122985 AND `source_type` = 0 AND `id` = 6;
UPDATE `smart_scripts` SET `action_param2` = 0 WHERE `entryorguid` = -122985 AND `source_type` = 0 AND `id` = 7;
UPDATE `smart_scripts` SET `action_param2` = 0 WHERE `entryorguid` = -122953 AND `source_type` = 0 AND `id` = 7;
UPDATE `smart_scripts` SET `action_param2` = 0 WHERE `entryorguid` = -122777 AND `source_type` = 0 AND `id` = 7;
UPDATE `smart_scripts` SET `action_param3` = 0 WHERE `entryorguid` = 3569    AND `source_type` = 0 AND `id` = 1;
UPDATE `smart_scripts` SET `event_param1` = 0, `event_param2` = 0 WHERE `entryorguid` = 6268  AND `source_type` = 0 AND `id` = 2;
UPDATE `smart_scripts` SET `event_param1` = 0, `event_param2` = 0 WHERE `entryorguid` = 14912 AND `source_type` = 0 AND `id` = 1;
UPDATE `smart_scripts` SET `event_param1` = 0 WHERE `entryorguid` = 17824 AND `source_type` = 0 AND `id` = 3;
UPDATE `smart_scripts` SET `action_param3` = 0 WHERE `entryorguid` = 25969 AND `source_type` = 0 AND `id` = 5;
UPDATE `smart_scripts` SET `target_param1` = 0 WHERE `entryorguid` = 27328 AND `source_type` = 0 AND `id` = 32;
UPDATE `smart_scripts` SET `event_param1` = 0, `event_param2` = 0 WHERE `entryorguid` = 37764 AND `source_type` = 0 AND `id` = 5;
UPDATE `smart_scripts` SET `event_param3` = 0 WHERE `entryorguid` = 37764 AND `source_type` = 0 AND `id` = 6;
UPDATE `smart_scripts` SET `action_param6` = 0 WHERE `entryorguid` = 1755100 AND `source_type` = 9 AND `id` = 4;

--
-- Fix missing help text for commands added to code without DB entries
--
DELETE FROM `command` WHERE `name` IN ('additem to', 'list auras id', 'list auras name', 'lookup item id', 'lookup map id', 'lookup quest id');
INSERT INTO `command` (`name`, `help`) VALUES
('additem to',      'Syntax: .additem to $playerName $itemID|[$itemName] [$itemCount]\r\n\r\nAdds the specified item to the target player by name. Works like .additem but targets a player by name instead of selection.'),
('list auras id',   'Syntax: .list auras id $spellId\r\n\r\nLists active auras on the selected unit filtered by spell ID.'),
('list auras name', 'Syntax: .list auras name $namePart\r\n\r\nLists active auras on the selected unit whose spell name contains the given string.'),
('lookup item id',  'Syntax: .lookup item id $itemId\r\n\r\nLooks up an item by exact numeric ID and prints its name.'),
('lookup map id',   'Syntax: .lookup map id $mapId\r\n\r\nLooks up a map by exact numeric ID and prints its name.'),
('lookup quest id', 'Syntax: .lookup quest id $questId\r\n\r\nLooks up a quest by exact numeric ID and prints its title.');
