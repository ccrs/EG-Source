--
DELETE FROM `spelldifficulty_dbc` WHERE `id` IN (62207, 62450, 62217, 62451);
INSERT INTO `spelldifficulty_dbc` (`id`, `spellid0`, `spellid1`, `spellid2`, `spellid3`) VALUES
(62207, 62207, 62921, 0, 0),
(62450, 62450, 62868, 0, 0),
(62217, 62217, 62922, 0, 0),
(62451, 62451, 62865, 0, 0);
