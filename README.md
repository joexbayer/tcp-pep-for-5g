# uio-master-joeba

Inital testing topology:
```
  (Client)                                (Router)                              (Server)
192.168.1.11                     192.168.1.1 - 192.168.2.2                    192.168.2.22
   enp0s8 <---( 100ms delay) ---> enp0s8           enp0s9 <---( 20ms delay )---> enp0s8
                    ?                                          10Mb htb rate
                    ?                                           25000 Bfifo
```
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

### Findings
Using 100ms delay, 10mb hbt rate & 25000 bfifo rate on the link between pep and server,
and 598304 recv / snd buffer sizes the client finishes sending the 6mb file in 7s, while the server after 10s.

Changing bfifo to 250000 mid transfer reduced the server time to 8s.

Using 100ms delay, 10mb hbt rate & 25000 bfifo rate without PEP used 46s for client and 48s for server.
With pep: 7s for client, 10s for server.


### SSH
    client:
        ssh joe@localhost -p 5555 -i .ssh/.id_rsa_virt -o StrictHostKeyChecking=no "cd /media/sf_uio-master-joeba; CMDs"
    server:
        ssh joe@localhost -p 5556 -i .ssh/.id_rsa_virt -o StrictHostKeyChecking=no "cd /media/sf_uio-master-joeba; CMDs"
    router:
        ssh joe@localhost -p 5557 -i .ssh/.id_rsa_virt -o StrictHostKeyChecking=no "cd /media/sf_uio-master-joeba; CMDs"
        

#### Public key
ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABgQDvSXOoDuLKw0ioTIW9tdQpUK0r4KGRgkmZkhwDm7kUGgGtmMjbQo81A74kA7na9ueexbSV/482ZmZcosg3Nwe+isjUINCBCFSXvmhqovv/ynnhn+kBTF03Bxpqciq4LNYViubLj3SnrCle6HU4aoDOx0L3MNyCVInaQTualeLKloA4ZKo/fv7kbzWXAKvClNH6v6axtQl58wfIt1ABEROcl0myY22D0Mn6yQCm28qqb5mqstVGWjTVxJPhfjtVp9NVXyz9ZPfnca/Tdbj/XRdZDtE3fsxaKAUdPs+VRwqNSAzkXH40Wz1t1dUMK/ahxZ5SCZkyBtqq4qE8e/A0SwUfuixH48Z2uVOjPNkNy0s+tu9NJjenLzLIFczRU4K8Il8U3roUsE7/m0mHqld74M/Ti3/U4SmKgcK9p28XR7LOJmY3sdRwt4THxOuQa3J6xJZkPAiE+3Oq3qME6mp2s08/pB55NY+6v3pXY/xVOSCkbkQU1CHA4YylZfT9kYbwwd8= joe@regolith
