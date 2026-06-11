--
DELETE FROM `rbac_permissions` WHERE `id` IN (1003, 1004);
INSERT INTO `rbac_permissions` (`id`, `name`) VALUES
(1003, 'Allow crossfaction arena team interaction'),
(1004, 'Command: tournament');

DELETE FROM `rbac_linked_permissions` WHERE `id` IN (195, 199);
INSERT INTO `rbac_linked_permissions` VALUES
(195,   3), -- Join Normal Battleground
(195,   4), -- Join Random Battleground
(195,   5), -- Join Arenas
(195,   6), -- Join Dungeon Finder
(195,  24), -- Two side faction characters on the same account
(195,  25), -- Allow say chat between factions
(195,  26), -- Allow channel chat between factions
(195,  27), -- Two side mail interaction
(195,  28), -- See two side who list
(195,  29), -- Add friends of other faction
(195,  40), -- Allows to add a gm to friend list
(195,  49), -- Forces to enter the email for confirmation on password change
(195,  51), -- Allow trading between factions
(195, 199), -- Role: Player Commands
(195, 1003), -- Allow crossfaction arena team interaction
--
(199, 496), -- Command: commands
(199, 507); -- Command: help
