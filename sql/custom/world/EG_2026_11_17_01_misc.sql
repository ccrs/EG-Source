--
DELETE FROM `creature_loot_template` WHERE `Entry` IN (35305, 35307, 35309) AND `Item` IN (1, 2, 3, 4, 5, 6, 8);
INSERT INTO `creature_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(35305, 1, 35063, 40, 0, 1, 0, 1, 1, 'Argent Monk - (ReferenceTable)'),
(35305, 2, 35063, 40, 0, 1, 0, 1, 1, 'Argent Monk - (ReferenceTable)'),
(35305, 3, 35064, 20, 0, 1, 0, 1, 1, 'Argent Monk - (ReferenceTable)'),
(35305, 4, 35065, 7, 0, 1, 0, 1, 1, 'Argent Monk - (ReferenceTable)'),
(35305, 5, 35066, 10, 0, 1, 0, 1, 1, 'Argent Monk - (ReferenceTable)'),
(35305, 6, 35067, 3, 0, 1, 0, 1, 1, 'Argent Monk - (ReferenceTable)'),
(35305, 8, 35074, 1, 0, 1, 0, 1, 1, 'Argent Monk - (ReferenceTable)'),
(35307, 1, 35063, 40, 0, 1, 0, 1, 1, 'Argent Priestess - (ReferenceTable)'),
(35307, 2, 35063, 40, 0, 1, 0, 1, 1, 'Argent Priestess - (ReferenceTable)'),
(35307, 3, 35064, 20, 0, 1, 0, 1, 1, 'Argent Priestess - (ReferenceTable)'),
(35307, 4, 35065, 7, 0, 1, 0, 1, 1, 'Argent Priestess - (ReferenceTable)'),
(35307, 5, 35066, 10, 0, 1, 0, 1, 1, 'Argent Priestess - (ReferenceTable)'),
(35307, 6, 35067, 3, 0, 1, 0, 1, 1, 'Argent Priestess - (ReferenceTable)'),
(35307, 8, 35074, 1, 0, 1, 0, 1, 1, 'Argent Priestess - (ReferenceTable)'),
(35309, 1, 35063, 40, 0, 1, 0, 1, 1, 'Argent Lightwielder - (ReferenceTable)'),
(35309, 2, 35063, 40, 0, 1, 0, 1, 1, 'Argent Lightwielder - (ReferenceTable)'),
(35309, 3, 35064, 20, 0, 1, 0, 1, 1, 'Argent Lightwielder - (ReferenceTable)'),
(35309, 4, 35065, 7, 0, 1, 0, 1, 1, 'Argent Lightwielder - (ReferenceTable)'),
(35309, 5, 35066, 10, 0, 1, 0, 1, 1, 'Argent Lightwielder - (ReferenceTable)'),
(35309, 6, 35067, 3, 0, 1, 0, 1, 1, 'Argent Lightwielder - (ReferenceTable)'),
(35309, 8, 35074, 1, 0, 1, 0, 1, 1, 'Argent Lightwielder - (ReferenceTable)');

DELETE FROM `reference_loot_template` WHERE `Entry` = 34170 AND `Item` IN (47229, 47560);
INSERT INTO `reference_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(34170, 47229, 0, 0, 0, 1, 1, 1, 1, 'Girdle of Arrogant Downfall');

DELETE FROM `reference_loot_template` WHERE `Entry` = 12025 AND `Item` IN (47197, 47497);
INSERT INTO `reference_loot_template` (`Entry`, `Item`, `Reference`, `Chance`, `QuestRequired`, `LootMode`, `GroupId`, `MinCount`, `MaxCount`, `Comment`) VALUES
(12025, 47497, 0, 0, 0, 1, 1, 1, 1, 'Helm of the Crestfallen Challenger');

UPDATE `creature_template` SET `mechanic_immune_mask` = 785 WHERE `entry` IN (35305, 35306, 35307, 35308, 35309, 35310);
UPDATE `creature_template` SET `mechanic_immune_mask` = 612439889 WHERE `entry` IN (34657, 34701, 34702, 34703, 34705, 35314, 35323, 35325, 35326, 35327, 35328, 35329, 35330, 35331, 35332, 35569, 35570, 35571, 35572, 35617, 36082, 36083, 36084, 36085, 36086, 36087, 36088, 36089, 36090, 36091);
UPDATE `creature_template` SET `mechanic_immune_mask` = 617299839 WHERE `entry` IN (34928, 34942, 35028, 35029, 35030, 35031, 35032, 35033, 35034, 35036, 35037, 35038, 35039, 35040, 35041, 35042, 35043, 35044, 35045, 35046, 35047, 35048, 35049, 35050, 35051, 35052, 35119, 35451, 35490, 35517, 35518, 35519, 35520, 35521, 35522, 35523, 35524, 35525, 35527, 35528, 35529, 35530, 35531, 35532, 35533, 35534, 35535, 35536, 35537, 35538, 35539, 35540, 35541, 35542, 35543, 35544);
