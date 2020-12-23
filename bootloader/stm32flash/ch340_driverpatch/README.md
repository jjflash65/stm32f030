#CH341#

The Ubuntu standard driver for CH341 USB to Serial converter seems to have some parity bit problem.

This is the patched file applying [1] to [2]
[1]: https://github.com/karlp/ch341-linux/blob/master/0001-usb-serial-ch341-Add-parity-support.patch
[2]: https://git.kernel.org/cgit/linux/kernel/git/torvalds/linux.git/plain/drivers/usb/serial/ch341.c?id=refs/tags/v4.5-rc3

## USAGE:##

sudo rmmod ch341

make

sudo insmod ./ch341.ko

sudo cp ./ch341.ko /lib/modules/$(uname -r)/kernel/drivers/usb/serial/
