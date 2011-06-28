#!/bin/sh

if [ $# -lt 1 ]
then	echo "Usage: $0 <executable> [0x... [0x... ] ]"
	exit 1
fi

NM="nm -t d -n"

executable=$1
shift

while [ $# -gt 0 ]
do	dec="`printf %d $1`"
	$NM $executable | grep ' [Tt] ' | awk '
	  {  if($1 > '$dec') { printf "%s+0x%x\n", name, '$dec'-offset; exit }
	     name=$3; offset=$1
	  }'
	shift
done
