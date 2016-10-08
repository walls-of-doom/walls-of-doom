if [ $# -ne 2 ]
    then
        echo "Pass two arguments: the new tag and a short release message."
    else
        old_tag=$(git describe --abbrev=0)
        git commit -a -m "$(printf "Release $1\n\n$2")"
        git tag -a $1 -m "$2"
	git push origin HEAD --follow-tags
	git push upstream HEAD --follow-tags
fi
