--
DELETE FROM `creature_formations` WHERE `leaderGUID` IN (128070, 128072, 128067, 128074, 128073, 128068, 128078, 128085, 128087, 128089) OR `memberGUID` IN (128070, 62036, 62037, 128071, 128072, 62784, 62038, 128084, 128067, 62785, 62039, 128075, 128074, 62034, 128073, 62033, 128068, 128069, 128078, 128079, 128085, 62744, 62746, 128086, 62035, 128091, 62745, 128094, 128095, 128096, 128087, 62244, 62743, 128088, 128092, 62273, 128097, 128098, 128099, 128089, 62041, 62242, 128090, 128093, 62228, 128100, 128101, 128102);
INSERT INTO `creature_formations` (`leaderGUID`, `memberGUID`, `dist`, `angle`, `groupAI`, `point_1`, `point_2`) VALUES
(128070, 128070, 0, 0, 3, 0, 0), -- Patchwerk approach golem pack - Patchwork Golem (leader)
(128070, 62036, 0, 0, 3, 0, 0), -- Patchwerk approach golem pack - Patchwork Golem (25 only)
(128070, 62037, 0, 0, 3, 0, 0), -- Patchwerk approach golem pack - Patchwork Golem (25 only)
(128070, 128071, 0, 0, 3, 0, 0), -- Patchwerk approach golem pack - Patchwork Golem
(128072, 128072, 0, 0, 3, 0, 0), -- Patchwerk approach golem/retcher pack (west) - Patchwork Golem (leader)
(128072, 62784, 0, 0, 3, 0, 0), -- Patchwerk approach golem/retcher pack (west) - Patchwork Golem (25 only)
(128072, 62038, 0, 0, 3, 0, 0), -- Patchwerk approach golem/retcher pack (west) - Bile Retcher (25 only)
(128072, 128084, 0, 0, 3, 0, 0), -- Patchwerk approach golem/retcher pack (west) - Bile Retcher (10 only)
(128067, 128067, 0, 0, 3, 0, 0), -- Patchwerk approach golem/retcher pack (east) - Patchwork Golem (leader)
(128067, 62785, 0, 0, 3, 0, 0), -- Patchwerk approach golem/retcher pack (east) - Patchwork Golem (25 only)
(128067, 62039, 0, 0, 3, 0, 0), -- Patchwerk approach golem/retcher pack (east) - Bile Retcher (25 only)
(128067, 128075, 0, 0, 3, 0, 0), -- Patchwerk approach golem/retcher pack (east) - Bile Retcher (10 only)
(128074, 128074, 0, 0, 3, 0, 0), -- Hub ramp golem pair (north) - Patchwork Golem (leader)
(128074, 62034, 0, 0, 3, 0, 0), -- Hub ramp golem pair (north) - Patchwork Golem (25 only)
(128073, 128073, 0, 0, 3, 0, 0), -- Hub ramp golem pair (south) - Patchwork Golem (leader)
(128073, 62033, 0, 0, 3, 0, 0), -- Hub ramp golem pair (south) - Patchwork Golem (25 only)
(128068, 128068, 0, 0, 3, 0, 0), -- West hall golem pair - Patchwork Golem (leader)
(128068, 128069, 0, 0, 3, 0, 0), -- West hall golem pair - Patchwork Golem
(128078, 128078, 0, 0, 3, 0, 0), -- West hall retcher pair - Bile Retcher (leader)
(128078, 128079, 0, 0, 3, 0, 0), -- West hall retcher pair - Bile Retcher
(128085, 128085, 0, 0, 3, 0, 0), -- West hall surgery pack (south) - Mad Scientist (leader)
(128085, 62744, 0, 0, 3, 0, 0), -- West hall surgery pack (south) - Mad Scientist (25 only)
(128085, 62746, 0, 0, 3, 0, 0), -- West hall surgery pack (south) - Mad Scientist (25 only)
(128085, 128086, 0, 0, 3, 0, 0), -- West hall surgery pack (south) - Mad Scientist
(128085, 62035, 0, 0, 3, 0, 0), -- West hall surgery pack (south) - Bile Retcher (25 only)
(128085, 128091, 0, 0, 3, 0, 0), -- West hall surgery pack (south) - Living Monstrosity
(128085, 62745, 0, 0, 3, 0, 0), -- West hall surgery pack (south) - Surgical Assistant (25 only)
(128085, 128094, 0, 0, 3, 0, 0), -- West hall surgery pack (south) - Surgical Assistant
(128085, 128095, 0, 0, 3, 0, 0), -- West hall surgery pack (south) - Surgical Assistant
(128085, 128096, 0, 0, 3, 0, 0), -- West hall surgery pack (south) - Surgical Assistant
(128087, 128087, 0, 0, 3, 0, 0), -- West hall surgery pack (middle) - Mad Scientist (leader)
(128087, 62244, 0, 0, 3, 0, 0), -- West hall surgery pack (middle) - Mad Scientist (25 only)
(128087, 62743, 0, 0, 3, 0, 0), -- West hall surgery pack (middle) - Mad Scientist (25 only)
(128087, 128088, 0, 0, 3, 0, 0), -- West hall surgery pack (middle) - Mad Scientist
(128087, 128092, 0, 0, 3, 0, 0), -- West hall surgery pack (middle) - Living Monstrosity
(128087, 62273, 0, 0, 3, 0, 0), -- West hall surgery pack (middle) - Surgical Assistant (25 only)
(128087, 128097, 0, 0, 3, 0, 0), -- West hall surgery pack (middle) - Surgical Assistant
(128087, 128098, 0, 0, 3, 0, 0), -- West hall surgery pack (middle) - Surgical Assistant
(128087, 128099, 0, 0, 3, 0, 0), -- West hall surgery pack (middle) - Surgical Assistant
(128089, 128089, 0, 0, 3, 0, 0), -- West hall surgery pack (north) - Mad Scientist (leader)
(128089, 62041, 0, 0, 3, 0, 0), -- West hall surgery pack (north) - Mad Scientist (25 only)
(128089, 62242, 0, 0, 3, 0, 0), -- West hall surgery pack (north) - Mad Scientist (25 only)
(128089, 128090, 0, 0, 3, 0, 0), -- West hall surgery pack (north) - Mad Scientist
(128089, 128093, 0, 0, 3, 0, 0), -- West hall surgery pack (north) - Living Monstrosity
(128089, 62228, 0, 0, 3, 0, 0), -- West hall surgery pack (north) - Surgical Assistant (25 only)
(128089, 128100, 0, 0, 3, 0, 0), -- West hall surgery pack (north) - Surgical Assistant
(128089, 128101, 0, 0, 3, 0, 0), -- West hall surgery pack (north) - Surgical Assistant
(128089, 128102, 0, 0, 3, 0, 0); -- West hall surgery pack (north) - Surgical Assistant

UPDATE `creature` SET `StringId` = 'PatchwerkAssist' WHERE `guid` IN (97718, 97736, 97747, 128080, 128082, 128083, 62040, 128078, 128079, 128068, 128069, 62035, 62744, 62746, 128085, 128086, 128091, 62745, 128094, 128095, 128096, 62244, 62743, 128087, 128088, 128092, 62273, 128097, 128098, 128099, 62041, 62242, 128089, 128090, 128093, 62228, 128100, 128101, 128102);
