-- update hardcore command info: a fallen run past level 40 can be abandoned
DELETE FROM `command` WHERE `name`='settings hardcore';
INSERT INTO `command` (`name`, `help`) VALUES
('settings hardcore', "Usage: .settings hardcore [confirm]
Enables Hardcore mode on the current character. Only available at level 1.
WARNING: This setting is PERMANENT and can never be switched back on once it ends:
 - A death at level 40 or below is permanent: the character remains a ghost forever and can never be resurrected by any means.
 - A death past level 40 can be abandoned with this same command: Hardcore switches off, the character resurrects normally and keeps a permanent record of the fall.
 - All other character settings are wiped and stay disabled while Hardcore is on.
Run without arguments to read the full warning; run with 'confirm' to act on it.");
