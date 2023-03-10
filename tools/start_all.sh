srvs="lbsrv:lbsrv.toml \
	proxy:proxy.toml \
	dbsrv:dbsrv.toml \
	commonsrv:iosrv.toml \
	gamesrv:gamesrv.toml \
	gateway:gateway.toml"

for srv in ${srvs}
do
	k=${srv%:*}
	v=${srv#*:}
	cd .. && nohup ./bin/${k} /config/${v} > /dev/null 2>&1 &
	sleep 0.01
done

./check_all.sh
