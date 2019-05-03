#!/bin/bash

IP="127.0.0.1"
PORT=63210
NC_TIMEOUT=1
STIMULUS="THIS IS A TEST"
EXPECTED="THIS IS A TEST"

# echo in green
echo_good ()
{
	echo -e "\033[0;32m$1\033[0m"
}

# echo in red
echo_bad ()
{
	echo -e "\033[0;31m$1\033[0m"
}

# check if port is available
if lsof -i :$PORT > /dev/null
then
    echo_bad "Server port $PORT is not available."
    exit 1
fi

# start server
./server $PORT &
SERVER_PID=$!

# wait a second
sleep 1s

# check if server started successfully and is running
if ! ps -p $SERVER_PID > /dev/null
then
    echo_bad "Server failed to start."
    exit 1
fi

# send a string to the server
REPLY=`echo "$STIMULUS" | nc -w $NC_TIMEOUT $IP $PORT`

# print the server reply
echo "$REPLY"

# check the server reply
PASSED=0
if [ "$REPLY" = "$EXPECTED" ]; then
	PASSED=1
fi

# close the server
kill -9 $SERVER_PID

# return 1 if test did not pass
if [ $PASSED -eq 0 ]; then
	echo_bad "FAILED"
	exit 1
fi

echo_good "PASSED"

