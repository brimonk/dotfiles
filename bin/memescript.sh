#!/usr/bin/env bash
# Brian Chrzanowski
# 2021-01-19 13:40:56
#
# Meme Maker with imgflip
#
# NOTE (Brian)
# jq is cool, but different than like, every other shell utility, rip

set -e

# usage : prints usage info
function usage
{
	echo "memescript.sh --meme 'template name' --top 'top text here' --bottom 'bottom text here'"
}

if [ "$#" -eq 0 ]; then
	usage
fi

MEME=""
TEXT0=""
TEXT1=""

MEMES=$(curl -s https://api.imgflip.com/get_memes)

echo "$MEMES" | jq ".data.memes[] | { name: .name }"

while (( "$#" ))
do
	cmd="$1"
	arg="$2"

	case "$cmd" in
		"--meme")
			MEME=$(echo "$arg" | tr A-Z a-z)
			shift
		;;
		"--top")
			TEXT0="$arg"
			shift
		;;
		"--bottom")
			TEXT1="$arg"
			shift
		;;
		*)
			usage
		;;
	esac

	shift
done

MEMEID=$(echo "$MEMES" | jq ".data.memes[] | { id: .id, name: .name } | \
	with_entries(if .key == \"name\" then .value |= ascii_downcase else . end) | \
	select(.name | contains(\"$MEME\")) | .id | tonumber")

curl -s \
	-d template_id=$MEMEID \
	-d username=$IMGUSER \
	-d password=$IMGPASS \
	-d text0="$TEXT0" \
	-d text1="$TEXT1" \
	"https://api.imgflip.com/caption_image" | jq -r ".data.url"

