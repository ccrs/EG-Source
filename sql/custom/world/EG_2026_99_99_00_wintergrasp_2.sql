--
-- Battlefield raid commands
DELETE FROM `command` WHERE `name` IN ('bf raid', 'bf raid open', 'bf raid release', 'bf raid add', 'bf raid remove', 'bf raid list');
INSERT INTO `command` (`name`, `help`) VALUES
('bf raid', 'Syntax: .bf raid $subcommand\nType .bf raid to see the list of possible subcommands.\nBattlefield raid management.'),
('bf raid release', 'Syntax: .bf raid release #battleid\nConverts the tracked battlefield raids into stored raids and stops tracking them.'),
('bf raid add', 'Syntax: .bf raid add #battleid [$playername]\nPuts the player (or selection, or yourself) into a battlefield raid of their faction, leaving their current group.'),
('bf raid remove', 'Syntax: .bf raid remove #battleid [$playername]\nRemoves the player (or selection, or yourself) from their battlefield raid.'),
('bf raid list', 'Syntax: .bf raid list #battleid\nLists the tracked battlefield raids of both factions.');

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
