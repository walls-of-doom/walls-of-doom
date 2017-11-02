SECONDS=0

# Create the symbolic link we need.
if [ ! -f $COMPILATION_DATABASE ]; then
  ln -s build/compile_commands.json
fi

echo Analyzing...
clang-check -analyze walls-of-doom/*.[c]
clang-check -analyze tests/*.[c]

echo "Done after $SECONDS seconds."
