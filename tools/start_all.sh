cd .. && nohup ./bin/lbsrv /config/lbsrv.toml > /dev/null 2>&1 & 
sleep 1
cd .. && nohup ./bin/proxy /config/proxy.toml > /dev/null 2>&1 & 
sleep 1
cd .. && nohup ./bin/gamesrv /config/gamesrv.toml > /dev/null 2>&1 & 
sleep 1
cd .. && nohup ./bin/gateway /config/gateway.toml > /dev/null 2>&1 & 