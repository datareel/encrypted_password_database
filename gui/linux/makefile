# 10/26/2026: RHEL 10 is the default build. Use makefile_rhel8 or
# makefile_rhel9 for older RHEL builds.
all:
	make PROJECT=encrypted_password_database FINAL=1 64BITCFG=1 RHEL=10 LIBRARIES="-lXxf86vm -lSM -lICE" -f ../../env/linux.mak

install:
	./install.sh

clean:
	make PROJECT=encrypted_password_database FINAL=1 64BITCFG=1 RHEL=10 -f ../../env/linux.mak clean

