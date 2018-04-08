SECONDS=0

# Create the symbolic link we need.
if [ ! -f $COMPILATION_DATABASE ]; then
  ln -s build/compile_commands.json
fi

echo Reformatting...

clang-format -i sources/*.cpp
clang-format -i sources/*.hpp

clang-format -i tests/*.cpp

echo "Done after $SECONDS seconds."
