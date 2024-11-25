#!/bin/bash

which -s gh > /dev/null 2> /dev/null
if [[ $? -ne 0 ]]; then
	echo "gh cli not found, using \"refs/heads/main\" to pull ekutils"
	CURRENT_COMMIT=refs/heads/main
else
	CURRENT_COMMIT=$(gh browse -R wyatt-radkiewicz/ekutils -n -c | awk -F '/' '{ print $7 }')
fi

HEADER=test/.ek_h_$CURRENT_COMMIT
SOURCE=test/.ek_c_$CURRENT_COMMIT

function exists()
{
	file -E $1 > /dev/null 2> /dev/null
	return $?
}

if exists $HEADER && exists $SOURCE; then
	echo "ekutils up to date"
else
	rm -f test/.ek_* test/ek*
	touch $HEADER
	touch $SOURCE
	wget https://github.com/wyatt-radkiewicz/ekutils/raw/$CURRENT_COMMIT/src/ek.h -O test/ek.h
	wget https://github.com/wyatt-radkiewicz/ekutils/raw/$CURRENT_COMMIT/src/ek.c -O test/ek.c
	echo "updated ekutils to commit $CURRENT_COMMIT"
fi

