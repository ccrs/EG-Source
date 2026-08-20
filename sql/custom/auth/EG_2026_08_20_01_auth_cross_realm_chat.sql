--
DROP TABLE IF EXISTS `realm_chat_queue`;
CREATE TABLE `realm_chat_queue` (
  `id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  `realmid` INT UNSIGNED NOT NULL COMMENT 'Destination realm, the one that consumes and deletes the row',
  `sourcerealmid` INT UNSIGNED NOT NULL,
  `sourcerealm` VARCHAR(32) NOT NULL DEFAULT '' COMMENT 'Display name of the origin realm, written by the publisher',
  `channel` VARCHAR(32) NOT NULL DEFAULT '',
  `sender` VARCHAR(12) NOT NULL DEFAULT '',
  `text` VARCHAR(512) NOT NULL DEFAULT '',
  `postdate` INT UNSIGNED NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`),
  KEY `inbox` (`realmid`,`id`),
  KEY `postdate` (`postdate`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
