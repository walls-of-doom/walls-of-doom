SECONDS=0

# Create the symbolic link we need.
ln -s build/compile_commands.json

echo Reformatting...
clang-format -i walls-of-doom/*.[hc]

echo Analyzing...
clang-tidy walls-of-doom/*.[c]

# Clean up.
rm compile_commands.json

echo "Done after $SECONDS seconds."
