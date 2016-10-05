SECONDS=0

# Create the symbolic link we need.
if [ ! -f $COMPILATION_DATABASE ]; then
  ln -s build/compile_commands.json
fi

echo Reformatting...
clang-format -i walls-of-doom/*.[hc]
clang-format -i tests/*.[hc]

echo Analyzing...
clang-tidy walls-of-doom/*.[c]
clang-tidy tests/*.[c]

echo "Done after $SECONDS seconds."
