# 10/26/2026: RHEL 10 is the default build. Use makefile_rhel8 for older RHEL builds.
all:
	make PROJECT=wxappfw FINAL=1 64BITCFG=1 RHEL=10 LIBRARIES="-lXxf86vm -lSM -lICE" -f ../../env/linux.mak

install:
	make PROJECT=wxappfw FINAL=1 64BITCFG=1 RHEL=10 -f ../../env/linux.mak install

clean:
	make PROJECT=wxappfw FINAL=1 64BITCFG=1 RHEL=10 -f ../../env/linux.mak clean
