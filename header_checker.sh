#!/bin/bash

src=(trunk/include/libdodo trunk/src)

for i in ${src[*]}
do
	pushd $i

	for j in *h *cc 
	do
		if [[ ! -f $j ]]
		then
			continue
		fi

		file=$(sed -n "2p;2q" $j)
		grep -q "$j" < <(echo $file)
		if [[ $? -ne 0 ]]
		then 
			echo "${j}: header file do not match: $file"
		fi

		if [[ "$j" == "${j%.cc}" ]]
		then
			directive=$(echo ${j//./_} | awk '{ print "_" toupper($1) "_"}')
			grep -q $directive $j
			if [[ $? -ne 0 ]]
			then 
				echo "${j}: header file do not match: $directive"
			fi
		fi
	done

	popd
done
