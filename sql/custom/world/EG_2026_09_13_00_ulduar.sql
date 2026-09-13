--
DELETE FROM `gameobject_loot_template` WHERE `Entry` = 27069 AND `Item` IN (1, 2, 45624);

DELETE FROM `gameobject_loot_template` WHERE `Entry` = 26950 AND `Item` IN (1, 2, 3, 45038, 45087, 45624, 45457, 45459, 45460, 45461, 45462, 45612);
INSERT INTO `gameobject_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`) VALUES
(26950, 45457, 0, 0, 0, 1, 1, 1, 1),
(26950, 45459, 0, 0, 0, 1, 1, 1, 1),
(26950, 45460, 0, 0, 0, 1, 1, 1, 1),
(26950, 45461, 0, 0, 0, 1, 1, 1, 1),
(26950, 45462, 0, 0, 0, 1, 1, 1, 1),
(26950, 45612, 0, 0, 0, 1, 1, 1, 1);
