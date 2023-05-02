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
Using the default 20ms config between PEP and Receiver:
Sender -----> PEP -- 20ms --> Receiver.
NO PEP:
Client: 8-11s
Server: 9-12s
PEP:
Client: 3.8s
Server: 5s

Using default 20ms config between client -> router and router -> server:
Sender -- 20ms --> PEP -- 20ms --> Receiver.
NO PEP:
Client: 5.3s
Server: 5.6s
PEP:
Client: 5.0s, 5.1s
Server: 5.4s, 5.5s

Sender -- 8ms (100mbs) --> PEP -- 20ms (10mbs) --> Receiver.
6mb
NO PEP
Client: 5.4s
Server: 5,6s

WITH PEP:
Client: 3.5s
Server: 6.3s

32mb
NO PEP
Client: 29s
Server: 29s

WITH PEP:
Client: 50s
Server: 52s

200mb
NO PEP:
Client: 230
Server: 230

WITH PEP:
Client: 303
Server: 303

#### IF the pep has enough buffe to store entire 32mb
WITH PEP:
Client: 4.8s
Server: 28s


Sender -- 100ms (100mbs) --> PEP -- 20ms (10mbs) --> Receiver.
32mb no pep
CLient: 79s
Server 80s

6mb no pep
Client: 14.5s
Server: 15s
6mb pep
Client: 9s
Server 10s

### SSH
    client:
        ssh joe@localhost -p 5555 -i .ssh/.id_rsa_virt -o StrictHostKeyChecking=no "cd /media/sf_uio-master-joeba; CMDs"
    server:
        ssh joe@localhost -p 5556 -i .ssh/.id_rsa_virt -o StrictHostKeyChecking=no "cd /media/sf_uio-master-joeba; CMDs"
    router:
        ssh joe@localhost -p 5557 -i .ssh/.id_rsa_virt -o StrictHostKeyChecking=no "cd /media/sf_uio-master-joeba; CMDs"
        

#### Public key
ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABgQDvSXOoDuLKw0ioTIW9tdQpUK0r4KGRgkmZkhwDm7kUGgGtmMjbQo81A74kA7na9ueexbSV/482ZmZcosg3Nwe+isjUINCBCFSXvmhqovv/ynnhn+kBTF03Bxpqciq4LNYViubLj3SnrCle6HU4aoDOx0L3MNyCVInaQTualeLKloA4ZKo/fv7kbzWXAKvClNH6v6axtQl58wfIt1ABEROcl0myY22D0Mn6yQCm28qqb5mqstVGWjTVxJPhfjtVp9NVXyz9ZPfnca/Tdbj/XRdZDtE3fsxaKAUdPs+VRwqNSAzkXH40Wz1t1dUMK/ahxZ5SCZkyBtqq4qE8e/A0SwUfuixH48Z2uVOjPNkNy0s+tu9NJjenLzLIFczRU4K8Il8U3roUsE7/m0mHqld74M/Ti3/U4SmKgcK9p28XR7LOJmY3sdRwt4THxOuQa3J6xJZkPAiE+3Oq3qME6mp2s08/pB55NY+6v3pXY/xVOSCkbkQU1CHA4YylZfT9kYbwwd8= joe@regolith
