cd .. && nohup ./bin/lbsrv > /dev/null 2>&1 & 
sleep 1
cd .. && nohup ./bin/proxy > /dev/null 2>&1 & 
sleep 1
cd .. && nohup ./bin/gamesrv > /dev/null 2>&1 & 
sleep 1
cd .. && nohup ./bin/gateway > /dev/null 2>&1 & 