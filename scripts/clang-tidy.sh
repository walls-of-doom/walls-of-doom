SECONDS=0

# Create the symbolic link we need.
if [ ! -f $COMPILATION_DATABASE ]; then
  ln -s build/compile_commands.json
fi

echo Analyzing...
clang-tidy --quiet --fix walls-of-doom/*.[c]
clang-tidy --quiet --fix tests/*.[c]

echo "Done after $SECONDS seconds."
