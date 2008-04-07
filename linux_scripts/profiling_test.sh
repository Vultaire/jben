#!/bin/sh

cmd=../bin/linux/profile/jben

for ((i=1; i<=5; i++)); do
	sleep 5
	$cmd
	mv gmon.out gmon.out.$((i))
	gprof $cmd gmon.out.$((i)) > gmon.txt.$((i))
done
