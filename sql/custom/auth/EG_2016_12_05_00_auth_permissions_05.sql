DELETE FROM `rbac_default_permissions`;
DELETE FROM `rbac_permissions` WHERE `id` IN (2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007);
INSERT INTO `rbac_permissions` (`id`, `name`) VALUES
(2000, 'Role: Player'),
(2001, 'Role: Moderator'),
(2002, 'Role: Game Master'),
(2003, 'Role: Senior Game Master'),
(2004, 'Role: Developer'),
(2005, 'Role: Administrator'),
(2006, 'Role: PTR Player'),
(2007, 'Role: PTR Tester');

INSERT INTO `rbac_default_permissions` (`secId`, `permissionId`, `realmId`) VALUES
(0, 2000, -1),
(1, 2001, -1),
(2, 2002, -1),
(3, 2003, -1),
(4, 2005, -1);

DELETE FROM `rbac_linked_permissions` WHERE `id` IN (2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007);
INSERT INTO `rbac_linked_permissions` VALUES
-- Player
(2000,  195), -- Role: Sec Level Player
(2000, 1000), -- Command: Custom - Character settings
-- Moderator
(2001,  194), -- Role: Sec Level Moderator
(2001, 2000), -- Role: Player
(2001, 1002), -- Skip being charged for using transmogrification.
-- Game Master
(2002,  193), -- Role: Sec Level Gamemaster
(2002, 2001), -- Role: Player
-- Senior Game Master
(2003, 2002), -- Role: Game Master
-- Developer
(2004, 2002), -- Role: Game Master
-- Administrator
(2005,  192), -- Role: Sec Level Administrator
(2005, 2003), -- Role: Senior Game Master
-- PTR Player
(2006, 2001), -- Role: Moderator
-- PTR Tester
(2007, 2002); -- Role: Game Master
