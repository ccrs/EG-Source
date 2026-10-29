--
-- Tier 1 raids: lift the average equipped item level cap
UPDATE `access_requirement` SET `item_level_max` = 0 WHERE `mapid` IN (533,615,616,624);
