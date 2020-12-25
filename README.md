# Goobers: Super-Old, VGA Version
I posted this code so that students in my OS and other classes could take a peak into the past.
You'll need to use Turbo C++ 3.2 (might work on older versions) to compile it due to DOS-specific dependencies.
I've started a branch using DJGPP, which allows cross-compiling in Linux, but many calls would need to be updated,
as DJGPP can only run in 32-bit (extended) mode.

## Native Compiling
The best (only?) way to compile the code as written is using Turbo C++, which will compile in real mode and allow the
interrupt and port reading / writing to function as originally written all those years ago!

The easiest way to build is in DOSBox or another "real" DOS environment. DOSEmu on Linux may work, but I haven't tried.
To build, you'll need the Turbo C++ binaries in your path; then switch to the source root and run `make`. The project
is built into the "Build" directory.

## Cross-Compiling
I began a DJGPP branch that is incomplete if you'd like to try it. It does compile, but it will not run due to the real
mode calls used for interrupts to work directly with video and input hardware. A cross-compiler can be used on most
modern Linux distributions. Note that compiler versions in the download commands (wget) may need to be updated as new
versions of GCC and DJGPP come out. You can build the cross-compiler yourself; here, instructions for installing the
binary bundles are included for common distros below.

### Debian-Based Distributions (Including Ubuntu)
The project was build using the WSL Ubuntu 20.04 distribution; mileage of other versions may vary.

To compile on Ubuntu, install the dependency packages first:  
`sudo apt install fakeroot alien wget`

Next, grab the prebuilt-binaries and convert them into DEB files:  
~~~
wget http://www.mirrorservice.org/sites/ftp.delorie.com/pub/djgpp/rpms/djcr{oss-{binutils-2.34,gcc-10.2.0/djcross-gcc-{,{c++,info,tools}-}10.2.0}-1ap,x-2.05-5}.x86_64.rpm`  
fakeroot alien dj*.rpm
~~~

Finally, reformat the bundles as DEB files and install them:
`sudo dpkg -i dj*.deb`

### RPM-Based Distributions (Fedora, RedHat, CentOS)
The instructions for RPM distributions have not been tested, but should work, as the toolchains were originally build in RPM.

In principle, installation on RPM-based distributions should be quite simple. First, grab the RPM bundles:  
~~~
wget http://www.mirrorservice.org/sites/ftp.delorie.com/pub/djgpp/rpms/djcr{oss-{binutils-2.34,gcc-10.2.0/djcross-gcc-{,{c++,info,tools}-}10.2.0}-1ap,x-2.05-5}.x86_64.rpm
~~~

Then, install the bundles:  
`dnf install *.rpm`
or
`yum localinstall *.rpm`

## Acknowledgements
Thanks to Joel Yliluoma (@RealBisqwit) for his clever one line command for downloading all of these packages:  
https://twitter.com/realbisqwit/status/951903018080972802
