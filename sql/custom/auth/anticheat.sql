-- run if dropping and creating the tables for the first time or wanting to start fresh on anticheat stats
CREATE TABLE IF NOT EXISTS `account_anticheat_reports` (
  `id` int unsigned NOT NULL DEFAULT 0 COMMENT 'Account id',
  `realmid` int unsigned NOT NULL DEFAULT 0,
  `guid` int unsigned NOT NULL DEFAULT 0,
  `time` int unsigned NOT NULL DEFAULT 0,
  `creation_time` int unsigned NOT NULL DEFAULT 0,
  `average` float NOT NULL DEFAULT 0,
  `total_reports` bigint unsigned NOT NULL DEFAULT 0,
  `speed_reports` bigint unsigned NOT NULL DEFAULT 0,
  `fly_reports` bigint unsigned NOT NULL DEFAULT 0,
  `jump_reports` bigint unsigned NOT NULL DEFAULT 0,
  `waterwalk_reports` bigint unsigned NOT NULL DEFAULT 0,
  `teleportplane_reports` bigint unsigned NOT NULL DEFAULT 0,
  `climb_reports` bigint unsigned NOT NULL DEFAULT 0,
  `teleport_reports` bigint unsigned NOT NULL DEFAULT 0,
  `ignorecontrol_reports` bigint unsigned NOT NULL DEFAULT 0,
  `zaxis_reports` bigint unsigned NOT NULL DEFAULT 0,
  `antiswim_reports` bigint unsigned NOT NULL DEFAULT 0,
  `gravity_reports` bigint unsigned NOT NULL DEFAULT 0,
  `antiknockback_reports` bigint unsigned NOT NULL DEFAULT 0,
  `no_fall_damage_reports` bigint unsigned NOT NULL DEFAULT 0,
  `op_ack_hack_reports` bigint unsigned NOT NULL DEFAULT 0,
  `counter_measures_reports` bigint unsigned NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`,`realmid`,`guid`,`time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS `account_anticheat_lua` (
  `id` int unsigned NOT NULL DEFAULT 0 COMMENT 'Account id',
  `realmid` int unsigned NOT NULL DEFAULT 0,
  `guid` int unsigned NOT NULL DEFAULT 0,
  `macro` varchar(255) NOT NULL DEFAULT '',
    PRIMARY KEY (`id`,`realmid`,`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
