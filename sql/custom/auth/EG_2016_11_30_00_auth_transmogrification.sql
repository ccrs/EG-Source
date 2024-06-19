DELETE FROM `rbac_permissions` WHERE `id` IN (1000,1002);
INSERT INTO `rbac_permissions` (`id`, `name`) VALUES
(1000, 'Command: Custom - Character settings.'),
(1002, 'Skip being charged for using transmogrification.');
