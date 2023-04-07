#!/bin/sh
# This file can be used as a service script for non systemd 
# based Linux distrubutions and some BSD based systems with 
# Linux compatibility layer enabled

DISPLAY=:0
LOOP_DELAY=0.5

start() {
  export DISPLAY
  pscircle --daemonize=true --loop-delay=${LOOP_DELAY}
}

stop() {
  pkill pscircle
}

restart() {
  stop
  start
}

case $1 in
start)
  start
  ;;
stop)
  stop
  ;;
restart)
  restart
  ;;
esac