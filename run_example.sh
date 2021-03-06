#!/bin/sh
docker kill $(docker ps -q)
for i in 1 2 3 4 5
do
    docker run -v $(pwd)/:/home/mapreduce/distributed_map_reduce -itd milishchuk/mapreduce:latest /bin/bash
done
echo 'cd distributed_map_reduce && \
      rm -rf build; mkdir build && cd build && \
      cmake .. && make -j4 && make install && \
      cd ../example/word_counter/ && \
      rm -rf build; mkdir build && cd build && \
      cmake .. && make -j4 && \
      export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd) && \
      ./example' | docker run -v $(pwd)/:/home/mapreduce/distributed_map_reduce -a stdin -a stdout -a stderr -i milishchuk/mapreduce:latest
