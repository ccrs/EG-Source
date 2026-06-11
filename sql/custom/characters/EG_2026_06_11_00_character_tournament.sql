--
DROP TABLE IF EXISTS `tournament`;
CREATE TABLE `tournament` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(100) NOT NULL,
  `state` tinyint(3) unsigned NOT NULL DEFAULT 0 COMMENT '0 draft, 1 registration, 2 locked, 3 running, 4 ended',
  `difficulty` tinyint(3) unsigned NOT NULL DEFAULT 1 COMMENT '0 normal, 1 heroic',
  `ilvlCap` smallint(5) unsigned NOT NULL DEFAULT 213 COMMENT 'max allowed equipped item level',
  `startTime` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'unix time the tournament went running',
  `endTime` int(10) unsigned NOT NULL DEFAULT 0,
  `createdBy` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'admin character guidLow',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `tournament_dungeon`;
CREATE TABLE `tournament_dungeon` (
  `tournamentId` int(10) unsigned NOT NULL,
  `slot` tinyint(3) unsigned NOT NULL COMMENT '1..5',
  `mapId` smallint(5) unsigned NOT NULL,
  `difficulty` tinyint(3) unsigned NOT NULL DEFAULT 1 COMMENT '0 normal, 1 heroic',
  `revealed` tinyint(3) unsigned NOT NULL DEFAULT 0 COMMENT 'hidden until tournament start',
  PRIMARY KEY (`tournamentId`,`slot`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `tournament_team`;
CREATE TABLE `tournament_team` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `tournamentId` int(10) unsigned NOT NULL,
  `name` varchar(100) NOT NULL,
  `status` tinyint(3) unsigned NOT NULL DEFAULT 0 COMMENT '0 active, 1 disqualified',
  `dqReason` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `idx_tournament` (`tournamentId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `tournament_team_member`;
CREATE TABLE `tournament_team_member` (
  `teamId` int(10) unsigned NOT NULL,
  `charGuid` int(10) unsigned NOT NULL COMMENT 'character guidLow',
  `accountId` int(10) unsigned NOT NULL DEFAULT 0,
  `role` tinyint(3) unsigned NOT NULL DEFAULT 0 COMMENT '0 tank, 1 healer, 2 dps',
  PRIMARY KEY (`teamId`,`charGuid`),
  KEY `idx_char` (`charGuid`) COMMENT 'uniqueness is per tournament, enforced by the core'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `tournament_run`;
CREATE TABLE `tournament_run` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `teamId` int(10) unsigned NOT NULL,
  `dungeonSlot` tinyint(3) unsigned NOT NULL,
  `mapId` smallint(5) unsigned NOT NULL,
  `instanceId` int(10) unsigned NOT NULL DEFAULT 0,
  `state` tinyint(3) unsigned NOT NULL DEFAULT 0 COMMENT '0 pending, 1 active, 2 completed, 3 void, 4 rejected',
  `combatStart` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'unix time, first player in combat',
  `bossFinish` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'unix time, end boss dead with all encounters done',
  `durationMs` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'millisecond-precise run time',
  `rejectReason` varchar(255) NOT NULL DEFAULT '',
  `verifiedBy` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'staff character guidLow',
  `created` int(10) unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`),
  KEY `idx_team` (`teamId`),
  KEY `idx_slot` (`dungeonSlot`),
  KEY `idx_instance` (`instanceId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `tournament_run_event`;
CREATE TABLE `tournament_run_event` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `runId` int(10) unsigned NOT NULL,
  `ts` int(10) unsigned NOT NULL DEFAULT 0 COMMENT 'unix time',
  `type` tinyint(3) unsigned NOT NULL DEFAULT 0 COMMENT '0 enter, 1 combatStart, 2 bossKill, 3 wipe, 4 gearViolation, 5 finish, 6 void, 7 rejected',
  `detail` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`id`),
  KEY `idx_run` (`runId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
