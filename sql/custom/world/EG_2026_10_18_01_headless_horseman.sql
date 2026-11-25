--
DELETE FROM `gossip_menu_option` WHERE `MenuID` = 8891 AND `OptionID` = 0;
UPDATE `gameobject_template` SET `ScriptName` = '' WHERE `entry` = 186267;

--
UPDATE `item_loot_template` SET `Item` = 49126 WHERE `Entry` = 54516 AND `Item` = 33808;
UPDATE `item_loot_template` SET `Item` = 49128 WHERE `Entry` = 54516 AND `Item` = 38175;
UPDATE `item_loot_template` SET `Chance` = 1 WHERE `Entry` = 54516 AND `Item` = 37012; -- The Horseman's Reins
