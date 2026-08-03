-- add hardcore command info
DELETE FROM `command` WHERE `name`='settings hardcore';
INSERT INTO `command` (`name`, `help`) VALUES
('settings hardcore', "Usage: .settings hardcore [confirm]
Enables Hardcore mode on the current character. Only available at level 1.
WARNING: This setting is PERMANENT and can NEVER be disabled:
 - Any death is permanent: the character remains a ghost forever and can never be resurrected by any means.
 - All other character settings are wiped and permanently disabled.
Run without arguments to read the full warning; run with 'confirm' to activate.");

DELETE FROM `trinity_string` WHERE `entry` IN (20014, 20015, 20016);
INSERT INTO `trinity_string` (`entry`, `content_default`) VALUES
(20014, '|cffff0000[HARDCORE]|r %s (level %u) has been slain by %s. Their journey ends here.'),
(20015, '|cffff0000[HARDCORE]|r %s (level %u) has fallen to %s. Their journey ends here.'),
(20016, '|cffff0000[HARDCORE]|r %s (level %u) has died. Their journey ends here.');
