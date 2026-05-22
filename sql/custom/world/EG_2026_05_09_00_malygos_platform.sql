-- EoE: make the Nexus Raid Platform (GO 193070) immune to HP-based destruction
UPDATE gameobject_template SET Data0 = 0, Data5 = 0 WHERE entry = 193070;
