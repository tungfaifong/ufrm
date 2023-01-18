srvs="lbsrv \
    proxy \
    dbsrv \
    commonsrv \
    gamesrv \
    gateway"

for srv in ${srvs}
do
    pgrep -l ${srv}
    if [ $? != 0 ]
    then
        echo "${srv} is not running."
    fi
done