--
DELETE FROM `trinity_string` WHERE `entry` IN (20019, 20020);
INSERT INTO `trinity_string` (`entry`, `content_default`) VALUES
(20019, '|cff00FF96Realm %s is now connected to cross-realm chat.|r'),
(20020, '|cffFF6060Realm %s has disconnected from cross-realm chat.|r');
