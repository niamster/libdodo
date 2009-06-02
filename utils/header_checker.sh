#!/bin/bash

src=(trunk/include/libdodo trunk/src)

##
## Check for header description and #ifndef/#define file protector mark
## 
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

##
## Check for files that do not include directives.h
##
pushd trunk/src
wo_direcites=$(uniq -u < <(sort <(grep -l directives.h *cc *inline) <(ls -1)))
if [[ -n "$wo_direcites" ]]
then
	echo "Source files that do not contain directives.h inclusion:"
	echo $wo_direcites
fi
popd
