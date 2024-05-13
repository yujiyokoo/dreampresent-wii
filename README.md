# Dreampresent-Wii

This is the cross-platform version of [Dreampresent](https://github.com/yujiyokoo/dreampresent) to run on both Sega Dremcast and Nintendo Wii.
The binaries would have to be built separately but most of the main logic are in the shared directory (src/common).
It is still a Work-In-Progress.

## Prerequisites (for Dreamcast)

For Dreamcast, there is a Docker-based build process. Please install Docker for your environment such as Docker Destkop for Mac.

## Build (for Dreamcast)

The build requires an extra step of copying audio files at the moment (to be fixed soon).
To build, run:

```
rm -f romdisk/*.raw
cp sound/*.wav romdisk/
docker run -i -t -v $(pwd):/mnt yujiyokoo/mruby-kos-dc bash -c 'cd /mnt && . /opt/toolchains/dc/kos/environ.sh && make --file=Makefile.dc'
```

This should build a CDI image called `dreampresent.cdi` which you can use on your emulator or write to a CD-R to run on a real Dreamcast unit.

## Prerequisites (for Wii)

* Wii or some form of emulator
  * If a real Wii unit is used, some way to run your code on it (there are lots of exploits available, with links to be added here soon)
* [mruby](https://github.com/mruby/mruby) (recent versions - as of October 2023, build config for Nintendo Wii is included in mruby's official distribution) installed
* [devkitPPC](https://wiibrew.org/wiki/DevkitPPC) and [GRRLIB](https://github.com/GRRLIB/GRRLIB) are installed

## Build (for Wii)

This will be fixed soon, but currently, you have to first edit Makefile. There are paths like "/media/psf/Home/Repos/parallels-ubuntu/mruby/build/wii/lib/libmruby_core.a" and "/opt/devkitpro/portlibs/ppc" which you have to replace with the paths that suit your environment.

Also, the build requires an extra step of copying audio files at the moment (to be fixed soon).

After editing the Makefile, run:
```
rm -f romdisk/*.wav
cp sound/*.raw romdisk/
make -f Makefile.wii clean && cd src/ && mrbc -g -Bprogram -o program.c  common/page_data.rb wii/platform_lib.rb common/presentation.rb common/dreampresent.rb common/start.rb  && cd .. && make -f Makefile.wii
```

If everything goes well, you should see a file called `dreampresent-wii.dol`. You can run this on emulators, or Wii if you can get your program to run on it (more details soon).

## Test

Testing is currently broken, and also work in progress, but some tests have been written in Ruby. To run:

```shell
bundle install
bundle exec rake test
```

Please note this assumes you have a working Ruby installation (not mruby, but "regular" CRuby where you have bundler, rake, minitest etc available) in your environment.
