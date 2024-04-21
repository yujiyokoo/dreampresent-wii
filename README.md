# Dreampresent-Wii

This is the port of [Dreampresent](https://github.com/yujiyokoo/dreampresent) to Nintendo Wii.
It is still a Work-In-Progress, and not many features have been implemented.

## Prerequisites

* Wii or some form of emulator
  * If a real Wii unit is used, some way to run your code on it (there are lots of exploits available, with links to be added here soon)
* [mruby](https://github.com/mruby/mruby) (recent versions - as of October 2023, build config for Nintendo Wii is included in mruby's official distribution) installed
* [devkitPPC](https://wiibrew.org/wiki/DevkitPPC) installed

## Build

This will be fixed soon, but currently, you have to first edit Makefile. There are paths like "/media/psf/Home/Repos/parallels-ubuntu/mruby/build/wii/lib/libmruby_core.a" and "/opt/devkitpro/portlibs/ppc" which you have to replace with the paths that suit your environment.

Then run this command:
```
make clean && cd src/ && mrbc -g -Bprogram -o program.c page_data.rb platform_lib.rb presentation.rb dreampresent.rb start.rb && cd .. && make
```

If everything goes well, you should see a file called `dreampresent-wii.dol`. You can run this on emulators, or Wii if you can get your program to run on it (more details soon).

## Test

Testing is WIP, but you should be able to run some tests if you run:

```shell
bundle install
bundle exec rake test
```

Please note this assumes you have a working Ruby installation (not mruby, but "regular" Ruby where you have bundler, rake, minitest etc available) in your environment.
