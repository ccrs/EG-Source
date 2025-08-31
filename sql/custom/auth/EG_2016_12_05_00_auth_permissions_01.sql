--
DELETE FROM `rbac_linked_permissions` WHERE `id` IN (195, 199);
INSERT INTO `rbac_linked_permissions` VALUES
(195,   3), -- Join Normal Battleground
(195,   4), -- Join Random Battleground
(195,   5), -- Join Arenas
(195,   6), -- Join Dungeon Finder
(195,  24), -- Two side faction characters on the same account
(195,  49), -- Forces to enter the email for confirmation on password change
(195, 199), -- Role: Player Commands
--
(199, 496), -- Command: commands
(199, 507); -- Command: help
