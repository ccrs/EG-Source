-- Recruit-A-Friend reward queue, owned and populated by the website
CREATE TABLE IF NOT EXISTS `account_recruit_rewards` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `realmid` INT UNSIGNED NOT NULL,
  `recruiter` INT UNSIGNED NOT NULL,
  `recruited` INT UNSIGNED NOT NULL,
  `guid` INT UNSIGNED NOT NULL,
  `name` VARCHAR(12) NOT NULL DEFAULT '',
  `item_entry` MEDIUMINT UNSIGNED NOT NULL,
  `item_count` INT UNSIGNED NOT NULL DEFAULT 1,
  `mail_subject` VARCHAR(255) NOT NULL DEFAULT '',
  `mail_body` TEXT NOT NULL,
  `status` TINYINT UNSIGNED NOT NULL DEFAULT 0,
  `last_error` VARCHAR(255) DEFAULT NULL,
  `queuedate` INT UNSIGNED DEFAULT NULL,
  `processdate` INT UNSIGNED DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `recruiter` (`recruiter`,`recruited`),
  KEY `realmid` (`realmid`,`status`,`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
