SECONDS=0

# Create the symbolic link we need.
if [ ! -f $COMPILATION_DATABASE ]; then
  ln -s build/compile_commands.json
fi

echo Analyzing...
clang-tidy --quiet --fix sources/*.cpp
clang-tidy --quiet --fix tests/*.cpp

echo "Done after $SECONDS seconds."
