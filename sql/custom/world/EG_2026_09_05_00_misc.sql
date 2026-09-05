--
DELETE FROM `creature_onkill_reputation` WHERE `creature_id` IN (17856,17857,17866,17877,17924,17925,18139,18140,18358,18382,18912,22085,22814);
INSERT INTO `creature_onkill_reputation` (`creature_id`, `RewOnKillRepFaction1`, `RewOnKillRepFaction2`, `MaxStanding1`, `IsTeamAward1`, `RewOnKillRepValue1`, `MaxStanding2`, `IsTeamAward2`, `RewOnKillRepValue2`, `TeamDependent`) VALUES
(17856, 970, 0, 7, 0, -75, 0, 0, 0, 0),
(17857, 970, 0, 7, 0, -75, 0, 0, 0, 0),
(17866, 970, 0, 7, 0, -75, 0, 0, 0, 0),
(17877, 970, 0, 7, 0, -75, 0, 0, 0, 0),
(17924, 970, 0, 7, 0, -75, 0, 0, 0, 0),
(17925, 970, 0, 7, 0, -75, 0, 0, 0, 0),
(18139, 970, 0, 7, 0, -25, 0, 0, 0, 0),
(18140, 970, 0, 7, 0, -25, 0, 0, 0, 0),
(18358, 970, 0, 7, 0, -25, 0, 0, 0, 0),
(18382, 970, 0, 7, 0, -75, 0, 0, 0, 0),
(18912, 970, 0, 7, 0, -25, 0, 0, 0, 0),
(22085, 970, 0, 7, 0, -25, 0, 0, 0, 0),
(22814, 970, 0, 7, 0, -25, 0, 0, 0, 0);
