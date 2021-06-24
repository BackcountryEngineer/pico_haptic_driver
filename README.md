# pico_haptic_driver

## Connect to pico through swd

Easiest is to use a second pico for this.

One of the pico runs the picoprobe firmware 
`openocd -f interface/picoprobe.cfg -f target/rp2040.cfg -s tcl`

To load and debug programs  
```
gdb-multiarch
target remote localhost:3333
load some_program.elf
monitor reset init
continue
quit
```



## Running a ros2 node

Run the micro ros agent in a docker container  
`docker run -it --rm -v /dev:/dev --privileged --net=host microros/micro-ros-agent:galactic serial --dev /dev/ttyACM0 -b 115200`