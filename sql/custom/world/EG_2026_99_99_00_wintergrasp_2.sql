--
-- Battlefield raid commands
DELETE FROM `command` WHERE `name` IN ('bf raid', 'bf raid open', 'bf raid release', 'bf raid add', 'bf raid remove', 'bf raid list');
INSERT INTO `command` (`name`, `help`) VALUES
('bf raid', 'Syntax: .bf raid $subcommand\nType .bf raid to see the list of possible subcommands.\nBattlefield raid management.'),
('bf raid release', 'Syntax: .bf raid release #battleid\nConverts the tracked battlefield raids into stored raids and stops tracking them.'),
('bf raid add', 'Syntax: .bf raid add #battleid [$playername]\nPuts the player (or selection, or yourself) into a battlefield raid of their faction, leaving their current group.'),
('bf raid remove', 'Syntax: .bf raid remove #battleid [$playername]\nRemoves the player (or selection, or yourself) from their battlefield raid.'),
('bf raid list', 'Syntax: .bf raid list #battleid\nLists the tracked battlefield raids of both factions.');

-- Battlefield battle commands
DELETE FROM `command` WHERE `name` IN ('bf start', 'bf stop', 'bf timer');
INSERT INTO `command` (`name`, `help`) VALUES
('bf start', 'Syntax: .bf start #battleid\nStarts the battle now.'),
('bf stop', 'Syntax: .bf stop #battleid\nEnds the battle with the result set so far; none by default (no control change, no counters, no rewards).'),
('bf timer', 'Syntax: .bf timer #battleid #seconds\nMoves the end of the running battle, or the start of the next battle, to #seconds from now. The current phase cannot last longer than one battle cycle (start to start), or the preparation duration during preparation.');

-- Graveyard teleports
DELETE FROM `spell_target_position` WHERE `ID` IN (59760, 59762, 59763, 59765, 59766, 59767, 59769);
INSERT INTO `spell_target_position` (`ID`, `EffectIndex`, `MapID`, `PositionX`, `PositionY`, `PositionZ`, `Orientation`, `VerifiedBuild`) VALUES
(59760, 0, 571, 5538.03, 2893.1, 517.055, 4.764749, 12340),
(59762, 0, 571, 5100.76, 2299.47, 368.485, 0.45684, 12340),
(59763, 0, 571, 5096.62, 3467.84, 368.485, 6.108653, 12340),
(59765, 0, 571, 5032.45, 3711.38, 372.468, 3.97162, 0),
(59766, 0, 571, 4335.95, 3232.7, 390.249, 0.252549, 12340),
(59767, 0, 571, 4317.06, 2404.3, 392.504, 6.26813, 0), -- orientation not sniffed
(59769, 0, 571, 5140.79, 2179.12, 390.95, 1.97222, 0);

-- Building flags
UPDATE `gameobject_template_addon` SET `flags` = 32 WHERE `entry` IN (190220, 190221, 190358, 190369, 190370, 190373, 190375, 190376, 191804, 191805, 191809);

-- Workshop banners
DELETE FROM `gameobject` WHERE `guid` IN (71385, 71386, 71387, 71388, 200045, 200046, 200047, 200048);
DELETE FROM `gameobject_addon` WHERE `guid` IN (71385, 71386, 71387, 71388, 200045, 200046, 200047, 200048);
INSERT INTO `gameobject` (`guid`, `id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `VerifiedBuild`) VALUES
(71385, 190475, 571, 4197, 4538, 1, 16, 4938.364, 2454.5986, 320.39178, 3.769911, 0, 0, -0.9510565, 0.30901712, 0, 0, 0, 12340),
(200045, 192626, 571, 4197, 4538, 1, 32, 4938.364, 2454.5986, 320.39178, 3.769911, 0, 0, -0.9510565, 0.30901712, 0, 0, 0, 12340),
(71386, 190487, 571, 4197, 4539, 1, 16, 4939.4854, 3339.6526, 376.87494, 0.890117, 0, 0, 0.43051052, 0.90258557, 0, 0, 0, 12340),
(200046, 192627, 571, 4197, 4539, 1, 32, 4939.4854, 3339.6526, 376.87494, 0.890117, 0, 0, 0.43051052, 0.90258557, 0, 0, 0, 12340),
(200047, 194960, 571, 4197, 4612, 1, 16, 4394.2964, 2357.411, 376.2978, 3.769911, 0, 0, -0.9510565, 0.30901712, 0, 0, 0, 12340),
(71387, 194959, 571, 4197, 4612, 1, 32, 4394.2964, 2357.411, 376.2978, 3.769911, 0, 0, -0.9510565, 0.30901712, 0, 0, 0, 12340),
(200048, 194963, 571, 4197, 4611, 1, 16, 4392.1494, 3300.9258, 372.42868, 0.890117, 0, 0, 0.43051052, 0.90258557, 0, 0, 0, 12340),
(71388, 194962, 571, 4197, 4611, 1, 32, 4392.1494, 3300.9258, 372.42868, 0.890117, 0, 0, 0.43051052, 0.90258557, 0, 0, 0, 12340);

-- Attacker banners at Eastspark and Westspark
UPDATE `gameobject` SET `phaseMask` = 64 WHERE `guid` IN (67242, 67252);

-- Factory phases in The Chilled Quagmire
DELETE FROM `spell_area` WHERE `spell` IN (56617, 56618) AND `area` = 4589;
INSERT INTO `spell_area` (`area`, `spell`, `quest_start`, `quest_start_status`, `quest_end`, `quest_end_status`, `autocast`, `aura_spell`, `racemask`, `gender`) VALUES
(4589, 56617, 0, 64, 0, 11, 1, 0, 0, 2),
(4589, 56618, 0, 64, 0, 11, 1, 0, 0, 2);
