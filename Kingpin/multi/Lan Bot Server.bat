cd..
start "Kingpin DM Lan Bot Server. (port 31510)" /B kingpin.exe +set sv_lan_play 1 +set game multi +set deathmatch 1 +set port 31510 +exec bot_setup.cfg +exec lan_server.cfg