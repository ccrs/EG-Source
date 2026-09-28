--
ALTER TABLE `access_requirement`
    ADD COLUMN `item_level_max` smallint(5) unsigned NOT NULL DEFAULT 0 COMMENT 'max average equipped item level, 0 = no cap';

-- Tier 1 raids: average equipped item level may not exceed 226
UPDATE `access_requirement` SET `item_level_max` = 226 WHERE `mapid` IN (533,615,616,624);

DELETE FROM `trinity_string` WHERE `entry` = 20025;
INSERT INTO `trinity_string` (`entry`, `content_default`) VALUES
(20025, 'Requires an average equipped item level of %u or lower (yours is %u).');
