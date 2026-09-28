--
ALTER TABLE `character_extended`
    ADD COLUMN `originRealmId` tinyint(3) unsigned NOT NULL DEFAULT 0 COMMENT 'realm id the character was created on, 0 until the server first writes it';
