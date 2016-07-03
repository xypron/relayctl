# relayctl
Simple command-line control of SainSmart USB relay boards based on FTDI
chips using bitbang mode.

## Permissions

Per default, only root is allowed to use devices directly, therefor the SiS-PM
also only works as root.

To allow group relayctl access create file /lib/udev/rules.d/60-relayctl.rules
with the following content

    SUBSYSTEM=="usb", ATTR{idVendor}=="0403", ATTR{idProduct}=="6001", GROUP="relayctl", MODE="660"

Then reload the udev rules with

    udevadm control --reload-rules
