# uio-master-joeba

Testing topology:

(Client) 192.168.1.11 enp0s8 <------> enp0s8 192.168.1.1 (Router) 192.168.2.2 enp0s9 <---( delay )---> enp0s8 192.168.2.22 (Server)

## Config of router
### install
    Ubuntu Server (22 - Kernel >5.15) 
#### apt
    update
    upgrade
    build-essential
    ifupdown
    net-tools
    ---------
    virtualbox-guest-utils (for shared folder)

### config
    sudo adduser joe vboxsf
    sudo vi /etc/sysctl.conf ? (for router)
        enable ip_forward