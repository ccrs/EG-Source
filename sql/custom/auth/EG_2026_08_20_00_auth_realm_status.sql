--
DROP TABLE IF EXISTS `realm_status`;
CREATE TABLE `realm_status` (
  `realmid` INT UNSIGNED NOT NULL,
  `realmname` VARCHAR(32) NOT NULL DEFAULT '',
  `capabilities` INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Bitmask of cross-realm features this realm advertises',
  `heartbeat` INT UNSIGNED NOT NULL DEFAULT 0 COMMENT 'Refreshed by the realm itself, a stale value means down',
  PRIMARY KEY (`realmid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
