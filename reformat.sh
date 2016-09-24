SECONDS=0

echo Reformatting...
clang-format -i walls-of-doom/*.[hc]

echo Analyzing...
clang-tidy walls-of-doom/*.[c]

echo "Done after $SECONDS seconds."
