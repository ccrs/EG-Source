--
DELETE FROM `spell_area` WHERE `area` = 4197;
INSERT INTO `spell_area` (`area`, `spell`, `quest_start`, `quest_start_status`, `quest_end`, `quest_end_status`, `autocast`, `aura_spell`, `racemask`, `gender`) VALUES
(4197, 55164, 0, 64, 0, 11, 1, 8326, 65527, 2),
(4197, 55773, 0, 64, 0, 11, 1, 0, 0, 2),
(4197, 55774, 0, 64, 0, 11, 1, 0, 0, 2),
(4197, 58045, 0, 64, 0, 11, 1, 0, 0, 2),
(4197, 58730, 0, 64, 0, 11, 1, 0, 0, 2),
(4197, 74411, 0, 64, 0, 11, 1, 0, 0, 2);

-- Bridge Guards
DELETE FROM `creature` WHERE `guid` IN (131281, 131280);
DELETE FROM `creature_addon` WHERE `guid` IN (131281, 131280);
